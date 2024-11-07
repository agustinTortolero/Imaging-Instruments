#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSlider>
#include <QGraphicsProxyWidget>
#include <QCheckBox>
#include <QCloseEvent>

#include <opencv2/opencv.hpp>

//#include "controller.h"
#include "video_settings.h"


class ImagingInstrumentsController; // Forward declaration

class VideoPlayer : public QMainWindow {
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    void loadVideo(const QString &filePath);

    void setController(ImagingInstrumentsController *ctrl);

     void setOutputPath(const QString &path);
      void initializeVideoWriter(const std::string& outputPath, const cv::Size& frameSize);


private:
    // Add this member variable
    ImagingInstrumentsController *controller; // Pointer to the controller

    // UI Components
    QGraphicsView *videoView;
    QGraphicsScene *videoScene;
    QGraphicsPixmapItem *videoItem;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *returnButton; // Return button
    QSlider *redSlider;
    QSlider *greenSlider;
    QSlider *blueSlider;
    QSlider *gammaSlider;
    QLabel *timeLabel;
    QLabel *statusLabel;

    // Layouts
    QVBoxLayout *controlsLayout;
    QGraphicsTextItem *statusLabelItem;

    // Video processing variables
    cv::VideoCapture cap;
    QTimer *timer;
    bool isPlaying;
    bool isPaused;
    double totalTime;
    int redAdjustment;
    int greenAdjustment;
    int blueAdjustment;
    double gammaAdjustment;

    QCheckBox *vectorFilterCheckbox;
    QCheckBox *colorEnhancementCheckbox;
    QCheckBox *previewModeCheckbox;
    QString outputPath;

    QCheckBox *saveCheckBox;
    int frameCounter;

    void initializeVideoWriter();
    cv::VideoWriter videoWriter;
    int frameWidth;              // Declare frameWidth
    int frameHeight;
    void showOutputPathDialog();
    bool shouldResize;
    void resetVideoDropFlag();
    void applyButtonStyles(const QString &theme);

    QLabel *redLabel;
    QLabel *greenLabel;
    QLabel *blueLabel;

public slots:
    void applyTheme(const QString &theme);

private slots:
    // Slot function declarations
    void onReturnButtonClicked();
    void setupUI();
    void setupVideoView();
    void setupControls();
    void setupLayout();
    void setupConnections();
    void setupLabels();
    void setupSliders(QVBoxLayout *sliderLayout);

    void updateStatusLabel(const QString &text);
    QPushButton* createButton(const QString &text);
    QSlider* createSlider(int min = 0, int max = 100);
    void connectSliders();

    void playVideo();
    void pauseVideo();
    void stopVideo();
    void processFrame();
    void adjustBrightness(int value, int channel);
    void adjustGamma(int value);

protected:
    void closeEvent(QCloseEvent *event) override;

};

#endif // VIDEO_PLAYER_H
