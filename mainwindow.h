#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPluginLoader>
#include <QFileInfoList>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QAction>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QUrl>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <QImageReader>
#include <QFont>
#include <QPalette>
#include <QTextEdit>
#include <QMessageBox>
#include <QTextEdit>
#include <QSysInfo>
#include <QStandardPaths>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>

#include <QVBoxLayout>


#ifdef USE_CUDA
#include <cuda_runtime.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#include <cuda_runtime.h>
#endif

#ifdef __linux__
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cuda_runtime.h>
#endif

#include <thread>

#include "opencv2/opencv.hpp"

class ImagingInstrumentsController;

namespace Ui {
class MainWindow;
}

class ImagingInstrumentsView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImagingInstrumentsView(QWidget *parent = nullptr);
    ~ImagingInstrumentsView();
    void displayImage(const QImage &image);
    void setController(ImagingInstrumentsController *controller);


signals:
    void imageLoaded(const QImage &image);
    void fileDropped(const QString &filePath);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    Ui::MainWindow *ui;
    ImagingInstrumentsController *controller;
    QLabel *dragDropLabel;
    QImage currentImage;

    QVBoxLayout* setupLayout(QDialog *dialog);
    void addTitle(QVBoxLayout *layout);
    void addVersionInfo(QVBoxLayout *layout);
    QString readBuildNumber();
    void addDescription(QVBoxLayout *layout);
    void addLogos(QVBoxLayout *layout);
    void addCloseButton(QVBoxLayout *layout, QDialog *dialog);


    void showHardwareInfo();
    QString getCpuInfo();
    QString getGpuInfo();
    bool videoDroppedOnce;
    void onLoadImageTriggered();



signals:
    void imageDropped(const QImage &image);
    void videoDropped(const QString &videoPath);
    void loadImage(const QString &fileName);

public slots:
    void setLightTheme();
    void setDarkTheme();
    void setCyberpunkTheme();

    void applyTheme(const QString &theme); // Add this line

    void loadImageFromFile();

private slots:
    void showAbout();
    void quitApp();
};

#endif

