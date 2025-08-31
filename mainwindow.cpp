#include "mainwindow.h"
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QRegularExpression>
#include <QLabel>
#include <QDir>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("ADB Qt6 GUI Wrapper");
    resize(600, 400);

    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // IP input and connect button
    QHBoxLayout *ipLayout = new QHBoxLayout();
    ipEdit = new QLineEdit(this);
    ipEdit->setPlaceholderText("Enter device IP (e.g., 192.168.1.100)");
    connectButton = new QPushButton("Connect", this);
    ipLayout->addWidget(new QLabel("Device IP:"));
    ipLayout->addWidget(ipEdit);
    ipLayout->addWidget(connectButton);
    mainLayout->addLayout(ipLayout);

    // List devices button
    listDevicesButton = new QPushButton("List Devices", this);
    mainLayout->addWidget(listDevicesButton);

    // Devices list
    devicesList = new QListWidget(this);
    mainLayout->addWidget(devicesList);

    // APK input and install
    QHBoxLayout *apkLayout = new QHBoxLayout();
    apkEdit = new QLineEdit(this);
    apkEdit->setPlaceholderText("Select APK file");
    browseApkButton = new QPushButton("Browse", this);
    installButton = new QPushButton("Install", this);
    apkLayout->addWidget(new QLabel("APK Path:"));
    apkLayout->addWidget(apkEdit);
    apkLayout->addWidget(browseApkButton);
    apkLayout->addWidget(installButton);
    mainLayout->addLayout(apkLayout);

    // Package name and uninstall/launch
    QHBoxLayout *packageLayout = new QHBoxLayout();
    packageEdit = new QLineEdit(this);
    packageEdit->setPlaceholderText("Enter package name");
    uninstallButton = new QPushButton("Uninstall", this);
    getPackageButton = new QPushButton("Get Package", this);
    launchButton = new QPushButton("Launch App", this);
    packageLayout->addWidget(new QLabel("Package:"));
    packageLayout->addWidget(packageEdit);
    packageLayout->addWidget(uninstallButton);
    packageLayout->addWidget(getPackageButton);
    packageLayout->addWidget(launchButton);
    mainLayout->addLayout(packageLayout);

    // Output text
    outputText = new QTextEdit(this);
    outputText->setReadOnly(true);
    mainLayout->addWidget(outputText);

    setCentralWidget(centralWidget);

    // Connect signals to slots
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::connectDevice);
    connect(listDevicesButton, &QPushButton::clicked, this, &MainWindow::listDevices);
    connect(browseApkButton, &QPushButton::clicked, this, &MainWindow::browseApk);
    connect(installButton, &QPushButton::clicked, this, &MainWindow::installApk);
    connect(uninstallButton, &QPushButton::clicked, this, &MainWindow::uninstallPackage);
    connect(getPackageButton, &QPushButton::clicked, this, &MainWindow::getPackageName);
    connect(launchButton, &QPushButton::clicked, this, &MainWindow::launchApp);
}

QString MainWindow::runCommand(const QString &command) {
    QProcess process;
    process.start("cmd.exe", QStringList() << "/c" << command);
    process.waitForFinished(-1);
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();
    return error.isEmpty() ? output : output + "\nError: " + error;
}

void MainWindow::connectDevice() {
    QString ip = ipEdit->text().trimmed();
    if (ip.isEmpty()) {
        outputText->setText("Enter IP address.");
        return;
    }
    savedIp = ip; // Save IP
    QString command = QString("%1 connect %2:5555").arg(adbPath, ip);
    outputText->setText(runCommand(command));
}

void MainWindow::listDevices() {
    QString command = QString("%1 devices").arg(adbPath);
    QString result = runCommand(command);
    devicesList->clear();
    QStringList lines = result.split("\n", Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (!line.contains("List of devices attached"))
            devicesList->addItem(line.trimmed());
    }
    outputText->setText(result);
}

void MainWindow::browseApk() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select APK", "", "APK Files (*.apk)");
    if (!filePath.isEmpty()) {
        apkEdit->setText(QDir::toNativeSeparators(filePath));
    }
}

void MainWindow::installApk() {
    QString apkPath = QDir::toNativeSeparators(apkEdit->text().trimmed());
    if (apkPath.isEmpty() || !QFile::exists(apkPath)) {
        outputText->setText("Invalid APK path.");
        return;
    }
    QString command = QString("%1 install \"%2\"").arg(adbPath, apkPath);
    outputText->setText(runCommand(command));
}

void MainWindow::uninstallPackage() {
    QString packageName = packageEdit->text().trimmed();
    if (packageName.isEmpty()) {
        outputText->setText("Enter package name.");
        return;
    }
    QString command = QString("%1 uninstall %2").arg(adbPath, packageName);
    outputText->setText(runCommand(command));
}

void MainWindow::getPackageName() {
    QString apkPath = QDir::toNativeSeparators(apkEdit->text().trimmed());
    if (apkPath.isEmpty() || !QFile::exists(apkPath)) {
        outputText->setText("Invalid APK path: " + apkPath);
        return;
    }
    QFileInfo aaptFile(aaptPath);
    if (!aaptFile.exists()) {
        outputText->setText("AAPT tool not found at: " + aaptPath);
        return;
    }
    // Properly format command with escaped quotes for cmd.exe
    QString command = QString("cmd.exe /c \"\"%1\" dump badging \"%2\" | findstr \"package\"\"")
                          .arg(QDir::toNativeSeparators(aaptPath), apkPath);
    QString result = runCommand(command);
    QRegularExpression regex("package: name='([^']+)'");
    QRegularExpressionMatch match = regex.match(result);
    if (match.hasMatch()) {
        QString packageName = match.captured(1);
        packageEdit->setText(packageName);
        outputText->setText("Package name extracted: " + packageName);
    } else {
        outputText->setText("Failed to extract package name:\n" + result);
    }
}

void MainWindow::launchApp() {
    QString packageName = packageEdit->text().trimmed();
    if (packageName.isEmpty()) {
        outputText->setText("Enter package name.");
        return;
    }
    QString command = QString("%1 shell monkey -p %2 -c android.intent.category.LAUNCHER 1").arg(adbPath, packageName);
    outputText->setText(runCommand(command));
}
