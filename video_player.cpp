#include "controller.h"
#include "video_player.h"
#include <QMessageBox>
#include <QDebug>
#include <cmath> // Include cmath for pow function


VideoPlayer::VideoPlayer(QWidget *parent)
    : QMainWindow(parent), isPaused(false), isPlaying(false),
    controller(nullptr) { // Initialize shouldResize
    setWindowTitle("Imaging Instruments - VideoProcessor");
    setupUI();
    setFixedSize(1200, 800);

}

void VideoPlayer::setController(ImagingInstrumentsController *ctrl) {
    controller = ctrl; // Set the controller

}


void VideoPlayer::setupUI() {
    setupVideoView();
    setupControls();
    setupLayout();

    setupConnections();
    // Apply theme from the controller
    if (controller) {
        applyTheme(controller->getTheme()); // Use the correct method
    } else {
        applyTheme("Dark"); // Fallback if controller is null
    }

}

void VideoPlayer::setupVideoView() {
    videoView = new QGraphicsView(this);
    videoView->setFixedSize(900, 800);
    videoView->setStyleSheet("background-color: #e0e0e0; border: 2px dashed #ccc;");
    videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    videoScene = new QGraphicsScene(this);
    videoItem = new QGraphicsPixmapItem();
    videoScene->addItem(videoItem);
    videoView->setScene(videoScene);
    videoView->setRenderHint(QPainter::Antialiasing);
    videoView->setAlignment(Qt::AlignCenter);
}


void VideoPlayer::setupControls() {

    if (!controlsLayout) {
        controlsLayout = new QVBoxLayout(); // Initialize controlsLayout
    } else {
        qWarning() << "controlsLayout already exists. Reinitializing.";
        qDeleteAll(controlsLayout->children()); // Clear existing widgets
        controlsLayout->deleteLater(); // Schedule for deletion
        controlsLayout = new QVBoxLayout(); // Create a new layout
    }

    playButton = createButton("Process");
    pauseButton = createButton("Pause");
    stopButton = createButton("Stop");
    returnButton = createButton("Return");

    controlsLayout = new QVBoxLayout(this); // Set this as the parent

    controlsLayout->addWidget(playButton);
    controlsLayout->addWidget(pauseButton);
    controlsLayout->addWidget(stopButton);
    controlsLayout->addWidget(returnButton);

    // Set sliderLayout as a child of controlsLayout
    QVBoxLayout *sliderLayout = new QVBoxLayout(); // No parent here; will be added to controlsLayout

    // Initialize the color adjustment labels and sliders
    redLabel = new QLabel("Red", this);
    greenLabel = new QLabel("Green", this);
    blueLabel = new QLabel("Blue", this);

    // Ensure sliders are initialized
    redSlider = createSlider(-255, 255);
    greenSlider = createSlider(-255, 255);
    blueSlider = createSlider(-255, 255);

    // Add color adjustment labels and sliders to the layout
    sliderLayout->addWidget(redLabel);
    sliderLayout->addWidget(redSlider);
    sliderLayout->addSpacing(15); // Space

    sliderLayout->addWidget(greenLabel);
    sliderLayout->addWidget(greenSlider);
    sliderLayout->addSpacing(15); // Space

    sliderLayout->addWidget(blueLabel);
    sliderLayout->addWidget(blueSlider);
    sliderLayout->addSpacing(15); // Space

    controlsLayout->addLayout(sliderLayout); // Now sliderLayout is managed by controlsLayout
    controlsLayout->addStretch(); // Push the time label to the bottom

    // Time label
    timeLabel = new QLabel("00:00 / 00:00", this);
    timeLabel->setAlignment(Qt::AlignCenter);
    controlsLayout->addWidget(timeLabel);

    // Status label
    statusLabel = new QLabel();
    statusLabel->setStyleSheet("color: white; font-size: 24px; background: transparent; padding: 10px;");
    statusLabel->setVisible(false);
    QGraphicsProxyWidget *proxy = videoScene->addWidget(statusLabel);
    proxy->setPos(700, 400);
}


QPushButton* VideoPlayer::createButton(const QString &text) {
    QPushButton *button = new QPushButton(text, this);
    button->setStyleSheet("QPushButton { border: 2px solid black; background-color: transparent; }");
    return button;
}


void VideoPlayer::setupSliders(QVBoxLayout *sliderLayout) {
    sliderLayout->setContentsMargins(5, 1, 5, 1);
    sliderLayout->setSpacing(1); // Adjusted spacing

    redSlider = createSlider(-255, 255);
    greenSlider = createSlider(-255, 255);
    blueSlider = createSlider(-255, 255);
    //gammaSlider = createSlider(1, 300); // Gamma range

    // Add Red Adjustment
    QLabel *redLabel = new QLabel("Red");
    redLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sliderLayout->addWidget(redLabel);
    sliderLayout->addWidget(redSlider);
    sliderLayout->addSpacing(15); // Space

    // Add Green Adjustment
    QLabel *greenLabel = new QLabel("Green");
    greenLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sliderLayout->addWidget(greenLabel);
    sliderLayout->addWidget(greenSlider);
    sliderLayout->addSpacing(15); // Space

    // Add Blue Adjustment
    QLabel *blueLabel = new QLabel("Blue");
    blueLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sliderLayout->addWidget(blueLabel);
    sliderLayout->addWidget(blueSlider);
    sliderLayout->addSpacing(15); // Space

    connectSliders();
}




QSlider* VideoPlayer::createSlider(int min, int max) {
    QSlider *slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min, max);

    // Set default style; this can be overridden later based on the theme
    slider->setStyleSheet("QSlider {"
                          "background: #333;"  // Default background for slider
                          "border: 1px solid white;" // Default border
                          "}");

    return slider;
}


void VideoPlayer::connectSliders() {
    connect(blueSlider, &QSlider::valueChanged, this, [this](int value) { adjustBrightness(value, 0); });
    connect(greenSlider, &QSlider::valueChanged, this, [this](int value) { adjustBrightness(value, 1); });
    connect(redSlider, &QSlider::valueChanged, this, [this](int value) { adjustBrightness(value, 2); });
    //connect(gammaSlider, &QSlider::valueChanged, this, [this](int value) { adjustGamma(value); });
}

void VideoPlayer::setupLabels() {
    // Time label
    timeLabel = new QLabel("00:00 / 00:00", this);
    timeLabel->setAlignment(Qt::AlignCenter);
    controlsLayout->addWidget(timeLabel);

    // Status label
    statusLabel = new QLabel(); // Do not set parent to this
    statusLabel->setStyleSheet("QLabel { color: white; font-size: 24px; background-color: rgba(0, 0, 0, 0); padding: 10px; }");
    statusLabel->setText(""); // Initially empty
    statusLabel->setVisible(false); // Initially hidden

    // Create a proxy widget to add the label to the graphics scene
    QGraphicsProxyWidget *proxy = videoScene->addWidget(statusLabel);
    proxy->setPos(700, 400); // Adjust this for desired position over the video item
}




void VideoPlayer::setupLayout() {
    if (centralWidget() && centralWidget()->layout()) {
        qWarning() << "A layout is already set. Clearing existing layout.";
        // Optionally clear the existing layout
        delete centralWidget()->layout(); // Clean up the existing layout
    }


    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(controlsLayout);
    mainLayout->addWidget(videoView);

    QWidget *container = new QWidget;
    container->setLayout(mainLayout);
    setCentralWidget(container);
}

void VideoPlayer::setupConnections() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoPlayer::processFrame);
    connect(playButton, &QPushButton::clicked, this, &VideoPlayer::playVideo);
    connect(pauseButton, &QPushButton::clicked, this, &VideoPlayer::pauseVideo);
    connect(stopButton, &QPushButton::clicked, this, &VideoPlayer::stopVideo);

    // Modified connection for the return button
    connect(returnButton, &QPushButton::clicked, this, [this]() {
        qDebug() << "Return button clicked!";
        onReturnButtonClicked();
    });
}

void VideoPlayer::onReturnButtonClicked() {
    if (isPlaying) {
        stopVideo(); // Stop the video if it's playing
    }

    // Clear the video item and reset everything
    videoItem->setPixmap(QPixmap());
    redAdjustment = 0;
    greenAdjustment = 0;
    blueAdjustment = 0;
    //gammaAdjustment = 1.0;

    redSlider->setValue(0);
    greenSlider->setValue(0);
    blueSlider->setValue(0);
    //gammaSlider->setValue(100);

    videoItem->resetTransform();

    // Switch view if controller exists
    if (controller) {
        qDebug() << "Switching to DragDropView.";
        controller->switchToDragDropView();
    } else {
        qDebug() << "Controller is null!";
    }
}


void VideoPlayer::adjustBrightness(int value, int channel) {
    switch (channel) {
    case 2: redAdjustment = value; break;   // Red channel adjustment
    case 1: greenAdjustment = value; break; // Green channel adjustment
    case 0: blueAdjustment = value; break;  // Blue channel adjustment

    }
}


void VideoPlayer::adjustGamma(int value) {
    gammaAdjustment = value / 100.0; // Scale to a range of 1.0 to 3.0
}

void VideoPlayer::loadVideo(const QString &filePath) {
    qDebug() << "Loading video from:" << filePath;

    cap.open(filePath.toStdString());
    if (!cap.isOpened()) {
        QMessageBox::warning(this, "Error", "Could not open video file.");
        qDebug() << "Failed to open video file.";
        return;
    }

    frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    // Get total time of the video in seconds
    double fps = cap.get(cv::CAP_PROP_FPS);
    double frameCount = cap.get(cv::CAP_PROP_FRAME_COUNT);
    totalTime = frameCount / fps; // Total time in seconds

    // Format total time for display
    int totalMinutes = static_cast<int>(totalTime) / 60;
    int totalSeconds = static_cast<int>(totalTime) % 60;
    timeLabel->setText(QString("00:00 / %1:%2")
                           .arg(totalMinutes, 2, 10, QChar('0'))
                           .arg(totalSeconds, 2, 10, QChar('0')));

    isPlaying = false;

    if (controller->isSaveEnabled()){
        initializeVideoWriter();
        qDebug() << "called initializeVideoWriter in VideoPlayer::loadVideo";
    }

    qDebug() << "Video loaded successfully.";
}


void VideoPlayer::playVideo() {
    qDebug() << "Attempting to play video...";
    if (!isPlaying && !isPaused) {
        isPlaying = true;
        if (timer) {
            timer->start(30); // Start the timer to process frames
            if (statusLabel) {
                updateStatusLabel("Processing");
            } else {
                qDebug() << "Error: statusLabel is null.";
            }
            qDebug() << "Playback started.";
        } else {
            qDebug() << "Timer is not initialized.";
        }
    }
}


void VideoPlayer::pauseVideo() {
    if (isPlaying) {
        isPlaying = false; // Mark as not playing
        timer->stop(); // Stop the timer
        updateStatusLabel("Paused");
        qDebug() << "Playback paused.";
    }
}

void VideoPlayer::stopVideo() {
    isPlaying = false; // Ensure not playing
    isPaused = false; // Reset the paused state
    timer->stop(); // Stop the timer
    cap.set(cv::CAP_PROP_POS_FRAMES, 0); // Reset to the first frame
    videoItem->setPixmap(QPixmap()); // Clear the current frame display

    // Reset elapsed time to 0
    timeLabel->setText("00:00 / 00:00");

    // Hide status label
    updateStatusLabel("");

    qDebug() << "Playback stopped.";
}

//TODO:mover a controller

void VideoPlayer::processFrame() {
    if (!isPlaying || isPaused) return;

    cv::Mat frame;
    if (cap.read(frame)) {
        // Log frame reading status
        controller->logMessage("Frame read successfully.", STATUS_MSG);

        // Get the original frame
        cv::Mat originalFrame = frame.clone();

        // Update the controller's inputImage with the current frame
        controller->getModel()->inputImage = frame.clone();

        // Apply filters based on checkbox states in the controller
        if (controller->isVectorFilterEnabled()) {
            controller->applyVectorFilter();
            controller->logMessage("Vector filter applied.", STATUS_MSG);
        }
        if (controller->isColorEnhancementEnabled()) {
            controller->applyColorEnhancement();
            controller->logMessage("Color enhancement applied.", STATUS_MSG);
        }

        // Get the processed image from the model (still at original size)
        cv::Mat processedFrame = controller->getModel()->inputImage.clone();

        // Adjust the channels
        std::vector<cv::Mat> channels(3);
        cv::split(processedFrame, channels);

        channels[2].convertTo(channels[2], CV_8UC1, 1, redAdjustment);   // Red channel
        channels[1].convertTo(channels[1], CV_8UC1, 1, greenAdjustment); // Green channel
        channels[0].convertTo(channels[0], CV_8UC1, 1, blueAdjustment);  // Blue channel

        // Clamp values to the range [0, 255]
        for (int i = 0; i < 3; ++i) {
            cv::threshold(channels[i], channels[i], 0, 255, cv::THRESH_TOZERO);
            cv::threshold(channels[i], channels[i], 255, 255, cv::THRESH_TRUNC);
        }

        // Merge channels back
        cv::merge(channels, processedFrame);

        // Use the processed frame directly without resizing
        cv::Mat resizedFrame = processedFrame.clone();

        // Check if saving is enabled in the controller
        if (controller->isSaveEnabled() && videoWriter.isOpened()) {
            controller->logMessage("Writing frame to disk.", STATUS_MSG);
            videoWriter.write(resizedFrame);  // Write the processed frame
        } else {
            controller->logMessage("Video writer not opened or save not enabled.", STATUS_MSG);
        }

        // Display the processed frame
        cv::cvtColor(resizedFrame, resizedFrame, cv::COLOR_BGR2RGB);
        QImage img = QImage(resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step[0], QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img);

        QSize viewSize = videoView->size();
        QSize pixmapSize = pixmap.size();
        qreal scaleX = static_cast<qreal>(viewSize.width()) / pixmapSize.width();
        qreal scaleY = static_cast<qreal>(viewSize.height()) / pixmapSize.height();
        qreal scale = qMin(scaleX, scaleY);

        videoItem->setPixmap(pixmap.scaled(pixmapSize * scale, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // Update time label
        double currentTime = cap.get(cv::CAP_PROP_POS_MSEC) / 1000.0;
        int elapsedSeconds = static_cast<int>(currentTime);
        int totalSeconds = static_cast<int>(totalTime);

        timeLabel->setText(QString("%1:%2 / %3:%4")
                               .arg(elapsedSeconds / 60, 2, 10, QChar('0'))
                               .arg(elapsedSeconds % 60, 2, 10, QChar('0'))
                               .arg(totalSeconds / 60, 2, 10, QChar('0'))
                               .arg(totalSeconds % 60, 2, 10, QChar('0')));
    } else {
        controller->logMessage("No more frames to read. Stopping video.", STATUS_MSG);
        stopVideo();
    }
}




void VideoPlayer::updateStatusLabel(const QString &text) {
    if (statusLabel) {
        statusLabel->setText(text);
        statusLabel->setVisible(!text.isEmpty());
    } else {
        qDebug() << "Error: statusLabel is null.";
    }
}

void VideoPlayer::setOutputPath(const QString &path) {
    outputPath = path;
    frameCounter = 0; // Reset the frame counter when setting a new path
}

void VideoPlayer::initializeVideoWriter() {

    controller->logMessage("Output path for video writer: " + outputPath, STATUS_MSG);

    // Set codec and other parameters
    int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'); // Use MJPG codec
    double fps = cap.get(cv::CAP_PROP_FPS); // Get the frame rate from the input video

    // Get the original frame dimensions
    int originalWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int originalHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    // Attempt to open the video writer with the original size
    videoWriter.open(outputPath.toStdString(), codec, fps, cv::Size(originalWidth, originalHeight), true);

    if (!videoWriter.isOpened()) {
        controller->logMessage("Error: Could not open the video writer!", ERROR_MSG);
    } else {
        controller->logMessage("Video writer opened successfully.", STATUS_MSG);
    }
}



void VideoPlayer::showOutputPathDialog() {
    /*
    OutputPathDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString path = dialog.getOutputPath();
        QString format = dialog.getSelectedFormat();

        // Set the output path and adjust the extension based on user choice
        if (format == "AVI") {
            path += ".avi";
        } else if (format == "MKV") {
            path += ".mkv";
        }

        setOutputPath(path); // This is your existing function
    }
*/
}

void VideoPlayer::closeEvent(QCloseEvent *event) {
    if (isPlaying) {
        stopVideo(); // Stop the video if it's still playing
    }

    if (timer->isActive()) {
        timer->stop(); // Stop the QTimer
    }

    cap.release(); // Release the video capture resource


    controller->logMessage("Window is closing, video processing stopped." + outputPath, STATUS_MSG);

    event->accept(); // Accept the close event to close the window
}
void VideoPlayer::applyTheme(const QString &theme) {
    qDebug() << "Applying theme:" << theme;

    QString backgroundColor, buttonColor, textColor, hoverColor, pressedColor, borderColor, sliderBackground;

    if (theme == "Light") {
        backgroundColor = "white";
        buttonColor = "transparent";
        textColor = "black";
        hoverColor = "rgba(0, 0, 0, 0.1)";
        pressedColor = "rgba(0, 0, 0, 0.2)";
        borderColor = "black";
        sliderBackground = "#e0e0e0"; // Light slider background
    } else if (theme == "Dark") {
        backgroundColor = "black";
        buttonColor = "transparent";
        textColor = "white";
        hoverColor = "#666";
        pressedColor = "#444";
        borderColor = "white";
        sliderBackground = "#444"; // Dark slider background
    } else if (theme == "Cyberpunk") {
        backgroundColor = "#1E1E2F";  // Cyberpunk background color
        buttonColor = "transparent";
        textColor = "#00FF7F";        // Cyberpunk text color
        hoverColor = "rgba(0, 255, 127, 0.2)"; // Light green hover
        pressedColor = "#00FF7F";     // Same color as text for pressed
        borderColor = "#00FF7F";       // Border color for buttons
        sliderBackground = "#333";     // Slider background for Cyberpunk
    } else {
        qDebug() << "No theme at applyTheme ";
        return;
    }

    setStyleSheet(QString("background-color: %1;").arg(backgroundColor));
    videoView->setStyleSheet(QString("background-color: #333; border: 2px dashed #555;"));
    statusLabel->setStyleSheet(QString("color: %1; font-size: 24px; background: transparent; padding: 10px;").arg(textColor));

    // Set colors for the labels
    QString labelColor = (theme == "Light") ? "black" : (theme == "Dark" ? "white" : "#00FF7F"); // Set label colors based on the theme
    redLabel->setStyleSheet(QString("color: %1;").arg(labelColor));
    greenLabel->setStyleSheet(QString("color: %1;").arg(labelColor));
    blueLabel->setStyleSheet(QString("color: %1;").arg(labelColor));

    QString buttonStyle = QString(
                              "QPushButton { background-color: %1; color: %2; border: 2px solid %3; }"
                              "QPushButton:hover { background-color: %4; }"
                              "QPushButton:pressed { background-color: %5; }"
                              ).arg(buttonColor, textColor, borderColor, hoverColor, pressedColor);

    playButton->setStyleSheet(buttonStyle);
    pauseButton->setStyleSheet(buttonStyle);
    stopButton->setStyleSheet(buttonStyle);
    returnButton->setStyleSheet(buttonStyle);

    // Apply styles to sliders based on the theme
    QString sliderStyle = QString(
        "QSlider::groove:horizontal {"
        "background: %1;"
        "height: 5px;"
        "}"
        "QSlider::handle:horizontal {"
        "background: %2;"
        "border: 1px solid %3;"
        "width: 20px;"
        "height: 20px;"
        "border-radius: 10px;"
        "margin: -5px 0;"
        "}"
        );

    QString sliderHandleColor = (theme == "Light") ? "black" : (theme == "Dark") ? "white" : "#00FF7F"; // Handle color based on theme
    redSlider->setStyleSheet(sliderStyle.arg(sliderBackground, sliderHandleColor, borderColor));
    greenSlider->setStyleSheet(sliderStyle.arg(sliderBackground, sliderHandleColor, borderColor));
    blueSlider->setStyleSheet(sliderStyle.arg(sliderBackground, sliderHandleColor, borderColor));
}


