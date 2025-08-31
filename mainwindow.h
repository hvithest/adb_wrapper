#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

class QLineEdit;
class QPushButton;
class QTextEdit;
class QListWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void connectDevice();
    void listDevices();
    void browseApk();
    void installApk();
    void uninstallPackage();
    void getPackageName();
    void launchApp();

private:
    QString runCommand(const QString &command);
    QString adbPath = "adb"; // Update to full path if not in PATH, e.g., "C:/path/to/adb.exe"
    QString aaptPath = "aapt"; // Update to full path if not in PATH
    QString savedIp; // Store IP in memory

    QLineEdit *ipEdit;
    QLineEdit *apkEdit;
    QLineEdit *packageEdit;
    QPushButton *connectButton;
    QPushButton *listDevicesButton;
    QPushButton *browseApkButton;
    QPushButton *installButton;
    QPushButton *uninstallButton;
    QPushButton *getPackageButton;
    QPushButton *launchButton;
    QListWidget *devicesList;
    QTextEdit *outputText;
};

#endif // MAINWINDOW_H
