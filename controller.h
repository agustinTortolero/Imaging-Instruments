#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QApplication>
#include <QObject>
#include <QMainWindow>
#include <QDir>
#include <QDebug>
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
#include <QSplashScreen>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>

#include "opencv2/opencv.hpp"
#include <cuda_runtime.h>
#include <QVBoxLayout>
#include <QDialog>
#include <chrono>

#include "plugin_interface.h"
#include "gpu_filtering.h"
#include "vector_filtering_lib.h"

#include "model.h"
#include "mainwindow.h"
#include "image_view.h"
#include "video_player.h"


enum MessageType {
    ERROR_MSG,
    STATUS_MSG
};


class ImagingInstrumentsView;
class ImageView;
class VideoPlayer;

class ImagingInstrumentsController : public QObject
{
    Q_OBJECT

public:
    explicit ImagingInstrumentsController(QObject *parent = nullptr);
    ~ImagingInstrumentsController();

    ImagingInstrumentsModel* model;
    ImageView* image_view;
    ImagingInstrumentsView* main_window;
    VideoPlayer* video_player;
    QString currentTheme;
    float noise_density;

    void switchToDragDropView();
    void logMessage(const QString &message, MessageType type);

    void setTheme(const QString &theme);
    QString getTheme() const;

    void setDragging(bool enabled);
    bool isDraggingEnabled() const;

    void loadPlugins(QMenu *menu);
    void applyCustomInstrument(PluginInstrument* instrument);
    QList<PluginInstrument*> getCustomInstruments() const;

public slots:
    void loadImage(const QString &fileName);
    void saveImage(const QImage &image);
    void onImageDropped(const QImage &image);
    void onVideoDropped(const QString &videoPath); // Slot for video drop

    void enterImpulseNoiseMode();
    void exitImpulseNoiseMode();
    bool isImpulseNoiseMode() const;

    void enterDrawingMode();
    void exitDrawingMode();
    bool isDrawingMode() const;
    void updateDrawingImage(const QImage &drawing);


    void applyImpulseNoise();
    void applyVectorFilter();
    void applyColorEnhancement();

    ImagingInstrumentsModel* getModel() const {
        return model;
    }

    void clearLogFile();

    void applySobelFilter();
    void applyBlur();
    void applyDeBlur();
    void applyBinarization(int threshold = 128);
    void applyErosion(int erosionSize = 3);
    void applyDilation(int dilationSize = 3);
    void applyOpening(int openingSize = 3);
    void applyClosing(int closingSize = 3);

signals:
    void impulseNoiseRequested();
    void themeChanged(const QString &theme);

private:
    bool impulseNoiseMode = false;
    bool drawingMode = false;
    bool isDragging;

    bool vectorFilterEnabled = false;
    bool colorEnhancementEnabled = false;
    bool saveEnabled = false;

    void setupVideoPlayer();
    bool checkCUDA();

    QString outputPath;
    QCheckBox *saveCheckBox;

    const QString pluginPathInstruments = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/INSTRUMENTS_HERE");
    QList<PluginInstrument*> customInstruments;
    void print_instruments();


public slots:
    void onFileDropped(const QString &filePath);

public:
    void setVectorFilterEnabled(bool enabled);
    void setColorEnhancementEnabled(bool enabled);
    void setSaveEnabled(bool enabled);
    bool isVectorFilterEnabled() const;
    bool isColorEnhancementEnabled() const;
    bool isSaveEnabled() const;

    void addCustomInstrumentActions(QMenu *menu);

   bool hasCustomInstruments() const;
};

#endif
