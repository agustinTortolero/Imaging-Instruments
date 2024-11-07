#include <QApplication>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>
#include <QFile>
#include <QLockFile>
#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication Imaging_Instruments(argc, argv);

    // Define a unique lock file path
    QString lockFilePath = QDir::temp().absoluteFilePath("ImagingInstruments.lock");

    // Use QLockFile to prevent multiple instances
    QLockFile lockFile(lockFilePath);
    if (!lockFile.tryLock(100)) {  // Wait for 100ms to acquire the lock
        QMessageBox::warning(nullptr, "Warning", "An instance of Imaging Instruments is already running.");
        return 0;  // Exit if another instance is running
    }

    // Set the global style
    Imaging_Instruments.setStyleSheet(
        "QWidget { background-color: black; color: white; }"
        "QLineEdit { background-color: black; color: white; }"
        "QLineEdit:disabled { background-color: gray; color: darkgray; }"
        "QPushButton { background-color: black; color: white; }"
        "QPushButton:disabled { background-color: gray; color: darkgray; }"
        );

    // Set the application directory and icon
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    Imaging_Instruments.setWindowIcon(QIcon(":/resources/icon_camera4.ico"));

    // Create the system tray icon and set its parent to the application
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(&Imaging_Instruments);
    trayIcon->setIcon(QIcon(":/resources/icon_camera4.ico"));
    trayIcon->setVisible(true);

    // Create a context menu for the tray icon with an exit action
    QMenu *trayMenu = new QMenu();
    QAction *exitAction = trayMenu->addAction("Exit");
    QObject::connect(exitAction, &QAction::triggered, &Imaging_Instruments, &QApplication::quit);
    trayIcon->setContextMenu(trayMenu);

    // Show the tray icon
    trayIcon->show();

    // Initialize the main controller
    ImagingInstrumentsController controller;

    // Start the application event loop
    int result = Imaging_Instruments.exec();

    // Release the lock file on exit
    lockFile.unlock();

    return result;
}
