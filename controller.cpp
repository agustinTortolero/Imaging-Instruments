#include "controller.h"




ImagingInstrumentsController::ImagingInstrumentsController(QObject *parent)
    : QObject(parent),
    model(new ImagingInstrumentsModel(this)),
    image_view(new ImageView(nullptr)),
    main_window(new ImagingInstrumentsView(nullptr)),
    // video_player(new VideoPlayer(this)),
    currentTheme("Cyberpunk"),
    noise_density(0.0f)
{
    clearLogFile();
    logMessage("Imaging Instruments Controller initialized.", STATUS_MSG);

    connect(main_window, &ImagingInstrumentsView::imageDropped, this, &ImagingInstrumentsController::onImageDropped);
    connect(image_view, &ImageView::imageDropped, this, &ImagingInstrumentsController::onImageDropped);

    connect(image_view, &ImageView::impulseNoiseRequested, this, &ImagingInstrumentsController::applyImpulseNoise);

    QPixmap pixmap(400, 200);
    pixmap.fill(Qt::black);
    QSplashScreen splash(pixmap);
    splash.setFont(QFont("Verdana", 24, QFont::Bold));
    splash.showMessage("Imaging Instruments", Qt::AlignCenter | Qt::AlignTop, Qt::white);
    splash.show();
    std::this_thread::sleep_for(std::chrono::seconds(2));

    splash.finish(main_window);
    print_instruments();


    main_window->setController(this);
    image_view->setController(this);
    main_window->applyTheme(getTheme());


    connect(this, &ImagingInstrumentsController::themeChanged, main_window, &ImagingInstrumentsView::applyTheme);
    connect(this, &ImagingInstrumentsController::themeChanged, image_view, &ImageView::applyTheme);

    main_window->show();
    main_window->raise();
    main_window->activateWindow();
}

ImagingInstrumentsController::~ImagingInstrumentsController() {
    delete model;
    delete main_window;
    //delete video_player; // Clean up VideoPlayer
}


void ImagingInstrumentsController::switchToDragDropView() {
    model->inputImage.release();
    model->noisyImage.release();

    if (image_view) {
        image_view->clearImage();
        image_view->hide();
    }

    /*
    // Hide the VideoPlayer view
    if (video_player) { // Ensure videoPlayer exists
        video_player->hide(); // Hide the VideoPlayer
    }
    */

    main_window->show();
}

bool ImagingInstrumentsController::checkCUDA() {
    int deviceCount = 0;
    cudaError_t error = cudaGetDeviceCount(&deviceCount);

    if (error != cudaSuccess) {
        QString errorMsg = "CUDA error: " + QString(cudaGetErrorString(error));
        logMessage(errorMsg, ERROR_MSG);

        return false;
    }

    if (deviceCount == 0) {
        logMessage("No CUDA-capable device found.", STATUS_MSG);
        return false;
    }
    logMessage("CUDA-capable device count:", STATUS_MSG);

    return true;
}

void ImagingInstrumentsController::onFileDropped(const QString &filePath) {
    QStringList imageExtensions = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tiff"};
    QString fileExtension = QFileInfo(filePath).suffix().toLower();

    if (imageExtensions.contains("." + fileExtension)) {
        QImage image(filePath);
        if (!image.isNull()) {
            onImageDropped(image);
        } else {
            QMessageBox::warning(main_window, tr("Invalid Image"), tr("The dropped file is not a valid image."));
        }
    } else {
        QMessageBox::warning(main_window, tr("Invalid File"), tr("The dropped file is neither a valid image nor a video."));
    }
}

void ImagingInstrumentsController::onImageDropped(const QImage &image) {
    image_view->scene->clear();

    // Convert the image to RGB888 format for processing
    QImage originalImage = image.convertToFormat(QImage::Format_RGB888);

    // Convert to cv::Mat for processing
    cv::Mat matImage(originalImage.height(), originalImage.width(), CV_8UC3, originalImage.bits(), originalImage.bytesPerLine());

    // Update the model with the original image
    model->inputImage = matImage.clone();
    model->setOriginalInputImage(matImage);

    main_window->close();

    // Apply theme and display the original image without resizing
    image_view->applyTheme(currentTheme);
    image_view->displayImage(originalImage);

    // Set the scene rect to the image size and fit it in the view
    image_view->scene->setSceneRect(0, 0, originalImage.width(), originalImage.height());
    image_view->graphicsView->fitInView(image_view->scene->sceneRect(), Qt::KeepAspectRatio);

    image_view->activateWindow();
    image_view->raise();
    image_view->setWindowState((image_view->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);

    image_view->show();
}



void ImagingInstrumentsController::onVideoDropped(const QString &videoPath) {
    QString logMsg = "onVideoDropped called with path: " + videoPath;
    logMessage(logMsg, STATUS_MSG);

    if (QFileInfo::exists(videoPath)) {
        VideoSettings settingsDialog;

        settingsDialog.applyTheme(currentTheme);

        logMsg = "currentTheme: " + currentTheme;
        logMessage(logMsg, STATUS_MSG);

        if (settingsDialog.exec() == QDialog::Accepted) {
            QString outputPath = settingsDialog.getOutputPath();
            QString selectedFormat = settingsDialog.getSelectedFormat();

            logMsg = "Output Path: " + outputPath + ", Selected Format: " + selectedFormat;
            logMessage(logMsg, STATUS_MSG);

            // Set checkbox states
            setVectorFilterEnabled(settingsDialog.isVectorFilterEnabled());
            setColorEnhancementEnabled(settingsDialog.isColorEnhancementEnabled());

            logMsg = "Save checkbox state: " + QString::number(settingsDialog.isSaveEnabled());
            logMessage(logMsg, STATUS_MSG);
            setSaveEnabled(settingsDialog.isSaveEnabled());
            logMsg = "Controller save state after update: " + QString::number(isSaveEnabled());
            logMessage(logMsg, STATUS_MSG);
        }
    } else {
        QMessageBox::warning(nullptr, "File Error", "The video file does not exist.");
        logMessage("File Error: the video file does not exist.", ERROR_MSG);
    }
}



void ImagingInstrumentsController::loadImage(const QString &fileName)
{
    cv::Mat image = cv::imread(fileName.toStdString());

    if (image.empty()) {
        QString logMsg = "Failed to load image from file: " + fileName;
        logMessage(logMsg, ERROR_MSG); // Log the error if the image could not be loaded
        return; // Early return if image loading fails
    }

    model->inputImage = image;

    // Log successful image loading
    QString logMsg = "Image loaded successfully from file: " + fileName;
    logMessage(logMsg, STATUS_MSG);

    QImage qImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
}

void ImagingInstrumentsController::saveImage(const QImage &image)
{
    QString filePath = QFileDialog::getSaveFileName(image_view, tr("Save Image"), "", tr("Images (*.png *.xpm *.jpg)"));
    if (filePath.isEmpty()) {
        logMessage("Image save operation canceled by the user.", STATUS_MSG); // Log cancelation
        return;
    }

    if (image.save(filePath)) {
        QMessageBox::information(image_view, tr("Save Image"), tr("Image saved successfully."));
        QString logMsg = "Image saved successfully to file: " + filePath;
        logMessage(logMsg, STATUS_MSG); // Log successful save
    } else {
        QMessageBox::warning(image_view, tr("Save Image"), tr("Failed to save image."));
        QString logMsg = "Failed to save image to file: " + filePath;
        logMessage(logMsg, ERROR_MSG); // Log failure to save
    }
}

void ImagingInstrumentsController::enterImpulseNoiseMode()
{
    impulseNoiseMode = true;
    QString logMsg = "Impulse Noise Mode activated with noise_density set to: " + QString::number(noise_density);
    logMessage(logMsg, STATUS_MSG); // Log the activation of impulse noise mode
}

void ImagingInstrumentsController::exitImpulseNoiseMode()
{
    impulseNoiseMode = false;
    QString logMsg = "Impulse Noise Mode deactivated";
    logMessage(logMsg, STATUS_MSG);

    if (!model->noisyImage.empty()) {
        model->inputImage = model->noisyImage.clone();
        logMsg = "Current image updated with the noisy image.";
        logMessage(logMsg, STATUS_MSG); // Log the image update
    } else {
        logMsg = "No noisy image available to update.";
        logMessage(logMsg, ERROR_MSG);
    }
}

bool ImagingInstrumentsController::isImpulseNoiseMode() const {
    return impulseNoiseMode;
}
void ImagingInstrumentsController::applyImpulseNoise()
{
    // Log the attempt to apply impulse noise
    QString logMsg = "Attempting to apply impulse noise...";
    logMessage(logMsg, STATUS_MSG);

    if (!impulseNoiseMode) {
        logMessage("Impulse Noise Mode is not active. Skipping filter application.", ERROR_MSG); // Log the error
        return;
    }

    if (model->inputImage.empty()) {
        logMessage("No image loaded.", ERROR_MSG);
        QMessageBox::warning(image_view, tr("Warning"), tr("No image loaded."));
        return;
    }

    if (model->inputImage.type() != CV_8UC3) {
        logMessage("Input image is not an unsigned 8-bit color image. Converting...", STATUS_MSG); // Log the conversion attempt

        if (model->inputImage.type() == CV_32FC3) {
            model->inputImage.convertTo(model->inputImage, CV_8UC3, 255.0);
            logMessage("Input image converted from CV_32FC3 to CV_8UC3.", STATUS_MSG); // Log conversion
        } else {
            logMessage("Unsupported image format. Please provide a valid image.", ERROR_MSG); // Log error
            QMessageBox::warning(image_view, tr("Warning"), tr("Input image must be an unsigned 8-bit color image."));
            return;
        }
    }

    QString pluginPath = QCoreApplication::applicationDirPath() + "/libs/impulse_noise2.dll";
    logMessage("Plugin Path: " + pluginPath, STATUS_MSG); // Log the plugin path

    QPluginLoader pluginLoader(pluginPath);
    if (!pluginLoader.instance()) {
        logMessage("Failed to load plugin: " + pluginLoader.errorString(), ERROR_MSG); // Log the error
        QMessageBox::warning(image_view, tr("Warning"), tr("Failed to load the impulse noise plugin."));
        return;
    }

    PluginInterfaceNoise *plugin = dynamic_cast<PluginInterfaceNoise*>(pluginLoader.instance());
    if (!plugin) {
        logMessage("Plugin does not implement PluginInterfaceNoise", ERROR_MSG); // Log the error
        QMessageBox::warning(image_view, tr("Warning"), tr("The plugin does not implement PluginInterfaceNoise."));
        return;
    }

    cv::Mat outputImage1;
    logMessage("Executing ImpulseNoise processing...", STATUS_MSG); // Log the processing attempt
    plugin->processImage(model->inputImage, outputImage1, noise_density);

    QImage currentImage(outputImage1.data, outputImage1.cols, outputImage1.rows, outputImage1.step, QImage::Format_RGB888);
    QImage currentImageCopy = currentImage.copy();

    if (currentImageCopy.isNull()) {
        logMessage("Failed to create QImage from processed output.", ERROR_MSG); // Log the error
        QMessageBox::warning(image_view, tr("Warning"), tr("Failed to create QImage from the processed image."));
        return;
    }

    logMessage("Current image dimensions: " + QString::number(currentImageCopy.width()) + "x" + QString::number(currentImageCopy.height()) +
                   " Type: " + QString::number(currentImageCopy.format()), STATUS_MSG); // Log dimensions

    model->noisyImage = outputImage1.clone();
    image_view->displayImage(currentImageCopy);
}



/*
  Gpu		.161  sec
  cpu		1.846  sec
  speedUp = 1.846/.161  = 11.44X


laptop hd
gpu .12
cpu muy lento 20 seg
  */


void ImagingInstrumentsController::applyVectorFilter()
{
    if (model->inputImage.empty()) {
        logMessage("No image loaded.",ERROR_MSG);  // Log message
        QMessageBox::warning(image_view, tr("Warning"), tr("No image loaded."));
        return;
    }

    if (model->inputImage.type() != CV_8UC3) {
        logMessage("Input image is not an unsigned 8-bit color image. Converting",STATUS_MSG);  // Log message
        if (model->inputImage.type() == CV_32FC3) {
            model->inputImage.convertTo(model->inputImage, CV_8UC3, 255.0);
        } else {
            logMessage("Unsupported image format. Please provide a valid image.",ERROR_MSG);  // Log message
            QMessageBox::warning(image_view, tr("Warning"), tr("Input image must be an unsigned 8-bit color image."));
            return;
        }
    }

    size_t Y = model->inputImage.rows;
    size_t X = model->inputImage.cols;

    cv::Mat img_noisy_float;
    cv::Mat img_clean_float;

    auto start = std::chrono::high_resolution_clock::now();
    if (checkCUDA()) {
        logMessage("GPU filtering.",STATUS_MSG);

        model->inputImage.convertTo(img_noisy_float, CV_32FC3);

        img_clean_float = cv::Mat(Y, X, CV_32FC3);

        try {
            // Call the CUDA kernel to filter the image
            run_gpu_filter(img_clean_float.ptr<float>(), img_noisy_float.ptr<float>(), Y, X); // Pass the output pointer
        } catch (const std::exception &e) {
            logMessage("CUDA filter execution failed: " + QString::fromStdString(e.what()),ERROR_MSG);  // Log message
            QMessageBox::warning(image_view, tr("Warning"), tr("CUDA filter execution failed."));
            return;
        }

        img_clean_float.convertTo(model->outputImage, CV_8UC3);  // Store the result in model->outputImage
    } else {
        //CPU EXEC
        logMessage("CPU Filtering.",STATUS_MSG);

        ImageFiltering filter;

        model->outputImage = model->inputImage.clone(); // Initialize outputImage
        try {
            // Call the DLL function to filter the image
            filter.run_filter(model->inputImage, model->outputImage); // Using the function from your DLL
        } catch (const std::exception &e) {
            logMessage("DLL filter execution failed: " + QString::fromStdString(e.what()), ERROR_MSG);  // Log message
            QMessageBox::warning(image_view, tr("Warning"), tr("DLL filter execution failed."));
            return;
        }
    }

    if (model->outputImage.empty()) {
        logMessage("Output image is empty!",ERROR_MSG);  // Log message
        QMessageBox::warning(image_view, tr("Warning"), tr("Failed to process the image."));
        return;
    }
    if (model->outputImage.type() != CV_8UC3) {
        logMessage("Output image is not CV_8UC3. Converting",STATUS_MSG);  // Log message
        if (model->outputImage.type() == CV_32FC3) {
            model->outputImage.convertTo(model->outputImage, CV_8UC3);  // Convert float to 8-bit color
        } else {
            logMessage("Unsupported output image format!", ERROR_MSG);  // Log message
            QMessageBox::warning(image_view, tr("Warning"), tr("Output image has an unsupported format."));
            return;
        }
    }
    auto end = std::chrono::high_resolution_clock::now(); // End timing
    std::chrono::duration<double> elapsed = end - start;

    if (checkCUDA()) {
        logMessage("Time taken for GPU processing: " + QString::number(elapsed.count()) + " seconds", STATUS_MSG);  // Log message
    } else {
        logMessage("Time taken for CPU processing: " + QString::number(elapsed.count()) + " seconds", STATUS_MSG);  // Log message
    }

    QImage currentImage(model->outputImage.data, model->outputImage.cols, model->outputImage.rows, model->outputImage.step, QImage::Format_RGB888);
    QImage currentImageCopy = currentImage.copy();

    if (currentImageCopy.isNull()) {
        logMessage("Failed to create QImage from processed output", ERROR_MSG);  // Log message
        QMessageBox::warning(image_view, tr("Warning"), tr("Failed to create QImage from the processed image."));
        return;
    }

    // Store the filtered image in the model for further processing
    model->inputImage = model->outputImage.clone(); // Update inputImage with the result

    // Display the image
    image_view->displayImage(currentImageCopy);
}


void ImagingInstrumentsController::applyColorEnhancement() {
    // Log start of function
    logMessage("Applying color enhancement",STATUS_MSG);

    if (model->inputImage.empty()) {

        logMessage("No image loaded.", ERROR_MSG);
        QMessageBox::warning(image_view, tr("Warning"), tr("No image loaded."));
        return;
    }

    if (model->inputImage.type() != CV_8UC3) {
        logMessage("Input image is not an unsigned 8-bit color image. Converting", STATUS_MSG);
        if (model->inputImage.type() == CV_32FC3) {
            model->inputImage.convertTo(model->inputImage, CV_8UC3, 255.0);
            logMessage("Input image converted to unsigned 8-bit color format.", STATUS_MSG);
        } else {
            logMessage("Unsupported image format. Please provide a valid image.",ERROR_MSG);
            QMessageBox::warning(image_view, tr("Warning"), tr("Input image must be an unsigned 8-bit color image."));
            return;
        }
    }

    size_t Y = model->inputImage.rows;
    size_t X = model->inputImage.cols;
    cv::Mat outputImage;

#ifdef _WIN32
    QString pluginPath = QCoreApplication::applicationDirPath() + "/libs/color_enhancement.dll";
    logMessage("Plugin Path: " + pluginPath, STATUS_MSG);
#else
    QString pluginPath = qApp->applicationDirPath() + FILTERS_SUBFOLDER + "libcolor_enhancement_plugin.so";
#endif

    QPluginLoader pluginLoader(pluginPath);
    if (!pluginLoader.instance()) {
        logMessage("Failed to load plugin: " + pluginLoader.errorString(),ERROR_MSG);
        QMessageBox::warning(image_view, tr("Warning"), tr("Failed to load the vector filter plugin."));
        return;
    }

    PluginInterfaceColorEnhancement *plugin = dynamic_cast<PluginInterfaceColorEnhancement*>(pluginLoader.instance());
    if (!plugin) {
        logMessage("Plugin does not implement PluginInterfaceFiltering",ERROR_MSG);
        QMessageBox::warning(image_view, tr("Warning"), tr("The plugin does not implement PluginInterfaceFiltering."));
        return;
    }

    outputImage = model->inputImage.clone();

    plugin->processImage(model->inputImage, outputImage, "histAdaptive");
    logMessage("Image processed using method: histAdaptive",STATUS_MSG);

    QImage currentImage(outputImage.data, outputImage.cols, outputImage.rows, outputImage.step, QImage::Format_RGB888);
    QImage currentImageCopy = currentImage.copy();

    if (currentImageCopy.isNull()) {
        logMessage("Failed to create QImage from processed output",ERROR_MSG);

        QMessageBox::warning(image_view, tr("Warning"), tr("Failed to create QImage from the processed image."));
        return;
    }

    model->inputImage = outputImage.clone();

    image_view->displayImage(currentImageCopy);
    logMessage("Color enhancement applied successfully and image displayed.", STATUS_MSG);
}


void ImagingInstrumentsController::applySobelFilter()
{
    if (model) {
        logMessage("Applying Sobel Edge Detection...", STATUS_MSG); // Log the start of Sobel filter application
        model->applySobelEdgeDetection();

        if (image_view) {
            cv::Mat rgbImage;
            cv::cvtColor(model->outputImage, rgbImage, cv::COLOR_BGR2RGB);
            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();

            image_view->displayImage(img);
            logMessage("Sobel filter applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply Sobel filter.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyBlur()
{
    if (model) {
        logMessage("Applying Blur...", STATUS_MSG); // Log the start of blur application
        model->applyBlur();

        if (image_view) {
            cv::Mat rgbImage;
            cv::cvtColor(model->outputImage, rgbImage, cv::COLOR_BGR2RGB);
            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();

            image_view->displayImage(img);
            logMessage("Blur applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply blur.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyDeBlur()
{
    if (model) {
        logMessage("Applying De-Blur...", STATUS_MSG); // Log the start of de-blur application
        model->applyDeBlur();

        if (image_view) {
            cv::Mat rgbImage;
            cv::cvtColor(model->outputImage, rgbImage, cv::COLOR_BGR2RGB);
            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();

            image_view->displayImage(img);
            logMessage("De-Blur applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply de-blur.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyBinarization(int threshold)
{
    if (model) {
        logMessage("Applying Binarization with threshold: " + QString::number(threshold), STATUS_MSG); // Log the start of binarization
        model->applyBinarization(threshold);

        if (image_view) {
            cv::Mat binaryImage = model->outputImage;

            cv::Mat rgbImage;
            cv::cvtColor(binaryImage, rgbImage, cv::COLOR_BGR2RGB);

            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();

            image_view->displayImage(img);
            logMessage("Binarization applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply binarization.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyErosion(int erosionSize)
{
    if (model) {
        logMessage("Applying Erosion with size: " + QString::number(erosionSize), STATUS_MSG); // Log the start of erosion
        model->applyErosion(erosionSize);

        if (image_view) {
            cv::Mat erodedImage = model->outputImage;

            cv::Mat rgbImage;
            cv::cvtColor(erodedImage, rgbImage, cv::COLOR_BGR2RGB);

            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();
            image_view->displayImage(img);
            logMessage("Erosion applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply erosion.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyDilation(int dilationSize)
{
    if (model) {
        logMessage("Applying Dilation with size: " + QString::number(dilationSize), STATUS_MSG); // Log the start of dilation
        model->applyDilation(dilationSize);

        if (image_view) {
            cv::Mat dilatedImage = model->outputImage;

            cv::Mat rgbImage;
            cv::cvtColor(dilatedImage, rgbImage, cv::COLOR_BGR2RGB);

            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();
            image_view->displayImage(img);
            logMessage("Dilation applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply dilation.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyOpening(int openingSize)
{
    if (model) {
        logMessage("Applying Opening with size: " + QString::number(openingSize), STATUS_MSG); // Log the start of opening
        model->applyOpening(openingSize);

        if (image_view) {
            cv::Mat openedImage = model->outputImage;

            cv::Mat rgbImage;
            cv::cvtColor(openedImage, rgbImage, cv::COLOR_BGR2RGB);

            model->inputImage = rgbImage.clone();
            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();
            image_view->displayImage(img);
            logMessage("Opening applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply opening.", ERROR_MSG); // Log error if model is null
    }
}

void ImagingInstrumentsController::applyClosing(int closingSize)
{
    if (model) {
        logMessage("Applying Closing with size: " + QString::number(closingSize), STATUS_MSG); // Log the start of closing
        model->applyClosing(closingSize);

        if (image_view) {
            cv::Mat closedImage = model->outputImage;

            cv::Mat rgbImage;
            cv::cvtColor(closedImage, rgbImage, cv::COLOR_BGR2RGB);

            model->inputImage = rgbImage.clone();

            QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
            img = img.copy();
            image_view->displayImage(img);
            logMessage("Closing applied and image displayed.", STATUS_MSG); // Log successful application
        }
    } else {
        logMessage("Model is null, cannot apply closing.", ERROR_MSG); // Log error if model is null
    }
}



void ImagingInstrumentsController::logMessage(const QString &message, MessageType type)
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString logFilePath = documentsPath + "/hello_log.txt";

    qDebug() << message;

    QFile file(logFilePath);

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        // Determine the type of message
        QString messageType = (type == ERROR_MSG) ? "ERROR" : "STATUS";

        out << currentDateTime << " - " << messageType << ": " << message << "\n";
        file.close();
    } else {
        qWarning("Could not open log file for writing.");
    }
}

void ImagingInstrumentsController::clearLogFile()
{
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString logFilePath = documentsPath + "/hello_log.txt";

    QFile file(logFilePath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.resize(0); // Clear the file content
        file.close(); // Close the file
    } else {
        qWarning("Could not open log file for clearing.");
    }
}



void ImagingInstrumentsController::setTheme(const QString &theme) {
    currentTheme = theme;
    emit themeChanged(theme);
    logMessage("Theme changed to: " + theme, STATUS_MSG); // Log theme change
}

QString ImagingInstrumentsController::getTheme() const
{
    return currentTheme;
}

void ImagingInstrumentsController::enterDrawingMode() {
    drawingMode = true;
    setDragging(false);

    QString logMsg = "Drawing Mode activated";
    logMessage(logMsg, STATUS_MSG); // Log activation of drawing mode
}

void ImagingInstrumentsController::exitDrawingMode() {
    drawingMode = false;
    setDragging(true);

    QString logMsg = "Drawing Mode deactivated";
    logMessage(logMsg, STATUS_MSG); // Log deactivation of drawing mode

    if (!model->noisyImage.empty()) {
        model->inputImage = model->noisyImage.clone(); // Update the model with the noisy image
        logMsg = "Current image updated with Doodles";
        logMessage(logMsg, STATUS_MSG); // Log the image update
    } else {
        logMsg = "No image available to update in exitDrawingMode()";
        logMessage(logMsg, ERROR_MSG); // Log the absence of a noisy image
    }
}

bool ImagingInstrumentsController::isDrawingMode() const {
    return drawingMode;
}

void ImagingInstrumentsController::updateDrawingImage(const QImage &drawing) {
    cv::Mat drawingMat;
    drawingMat = cv::Mat(drawing.height(), drawing.width(), CV_8UC4, (void*)drawing.bits(), drawing.bytesPerLine());

    model->noisyImage = drawingMat;
    logMessage("Drawing image updated in model.", STATUS_MSG); // Log drawing image update
}

void ImagingInstrumentsController::setDragging(bool enabled) {
    isDragging = enabled;
    image_view->setDragging(isDragging);
    logMessage("Dragging set to: " + QString(enabled ? "enabled" : "disabled"), STATUS_MSG); // Log dragging state change
}

bool ImagingInstrumentsController::isDraggingEnabled() const {
    return isDragging;
}

void ImagingInstrumentsController::setVectorFilterEnabled(bool enabled) {
    vectorFilterEnabled = enabled;
    logMessage("Vector filter set to: " + QString(enabled ? "enabled" : "disabled"), STATUS_MSG); // Log vector filter state change
}

void ImagingInstrumentsController::setColorEnhancementEnabled(bool enabled) {
    colorEnhancementEnabled = enabled;
    logMessage("Color enhancement set to: " + QString(enabled ? "enabled" : "disabled"), STATUS_MSG); // Log color enhancement state change
}

void ImagingInstrumentsController::setSaveEnabled(bool enabled) {
    saveEnabled = enabled;
    logMessage("Save functionality set to: " + QString(enabled ? "enabled" : "disabled"), STATUS_MSG); // Log save functionality state change
}

bool ImagingInstrumentsController::isVectorFilterEnabled() const {
    return vectorFilterEnabled;
}

bool ImagingInstrumentsController::isColorEnhancementEnabled() const {
    return colorEnhancementEnabled;
}

bool ImagingInstrumentsController::isSaveEnabled() const {
    return saveEnabled;
}



void ImagingInstrumentsController::loadPlugins(QMenu *customInstrumentMenu)
{

    QDir pluginsDir(pluginPathInstruments);

    if (!pluginsDir.exists()) {
        qWarning() << "Plugin directory does not exist:" << pluginPathInstruments;
        return;
    }

    qDebug() << "Loading plugins from directory:" << pluginPathInstruments;

    // Clear the current list of custom instruments before adding new ones
    customInstruments.clear();

    // Iterate through all files in the plugin directory
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        if (!fileName.endsWith(".dll", Qt::CaseInsensitive)) {
            continue;  // Skip non-DLL files
        }

        QString filePath = pluginsDir.absoluteFilePath(fileName);
        qDebug() << "Attempting to load plugin:" << fileName;

        QPluginLoader loader(filePath);
        QObject *plugin = loader.instance();

        if (!plugin) {
            qWarning() << "Failed to load plugin:" << fileName << loader.errorString();
            continue;
        }

        // Attempt to cast the loaded plugin to PluginInstrument
        PluginInstrument *pluginInstrument = qobject_cast<PluginInstrument*>(plugin);
        if (pluginInstrument) {
            qDebug() << "Plugin" << fileName << "implements PluginInstrument.";

            customInstruments.append(pluginInstrument);  // Add to custom instruments list

            QString menuEntry = pluginInstrument->context_menu_str();  // Get the menu entry string from the plugin
            qDebug() << "Plugin context menu entry:" << menuEntry;

            /*
            if (!menuEntry.isEmpty()) {
                QAction *pluginAction = new QAction(menuEntry, customInstrumentMenu);
                connect(pluginAction, &QAction::triggered, [this, pluginInstrument]() {
                    this->applyCustomInstrument(pluginInstrument);  // Apply the instrument action when triggered
                });
                customInstrumentMenu->addAction(pluginAction);  // Add the action to the menu
                qDebug() << "Added plugin action to customInstrumentMenu:" << menuEntry;
            } else {
                qWarning() << "Empty menu entry string for plugin:" << fileName;
            }
            */
        } else {
            qWarning() << "Plugin" << fileName << "does not implement PluginInstrument.";
        }
    }

    // Now you can check if there are custom instruments available
    if (hasCustomInstruments()) {
        qDebug() << "Custom instruments are available!";
    } else {
        qDebug() << "No custom instruments found.";
    }
}

void ImagingInstrumentsController::applyCustomInstrument(PluginInstrument* instrument)
{
    if (model) {
        qDebug() << "Applying custom instrument plugin";

        // Check if inputImage is valid (i.e., not empty) before proceeding
        if (model->inputImage.empty()) {
            qWarning() << "Input image is empty!";
            return;  // Exit if no valid image is provided
        }

        // Apply the custom instrument's image processing function
        instrument->processImage(model->inputImage, model->outputImage);

        // Check if outputImage is valid after processing
        if (model->outputImage.empty()) {
            qWarning() << "Output image is empty after plugin processing!";
            return;
        }

        // Convert the output image from BGR to RGB for display
        cv::Mat rgbImage;
        cv::cvtColor(model->outputImage, rgbImage, cv::COLOR_BGR2RGB);
        model->inputImage = rgbImage.clone();  // Update inputImage with processed result

        // Convert the processed image to QImage for display in UI
        QImage img = QImage(rgbImage.data, rgbImage.cols, rgbImage.rows, rgbImage.step[0], QImage::Format_RGB888);
        img = img.copy();  // Ensure the QImage is copied, as original data may not be valid after the method call

        // Display the image in the UI
        if (image_view) {
            image_view->displayImage(img);
            qDebug() << "Custom instrument applied and image displayed.";
        }

    } else {
        qWarning() << "Model is null, cannot apply custom instrument plugin!";
    }
}



QList<PluginInstrument*> ImagingInstrumentsController::getCustomInstruments() const
{
    return customInstruments;  // Assuming customInstruments is a member of the class
}

void ImagingInstrumentsController::addCustomInstrumentActions(QMenu *menu)
{
    foreach (PluginInstrument *plugin, customInstruments) {
        QString menuEntry = plugin->context_menu_str();  // Get the menu string from the plugin
        if (!menuEntry.isEmpty()) {
            QAction *pluginAction = new QAction(menuEntry, menu);
            connect(pluginAction, &QAction::triggered, [this, plugin]() {
                this->applyCustomInstrument(plugin);  // Apply the custom instrument when selected
            });
            menu->addAction(pluginAction);
            qDebug() << "Added custom instrument action:" << menuEntry;
        } else {
            qWarning() << "Empty menu entry string for plugin:" << plugin->plugin_name();
        }
    }
}

bool ImagingInstrumentsController::hasCustomInstruments() const
{
    return !customInstruments.isEmpty();  // customInstruments should be a list or container holding loaded plugins
}


void ImagingInstrumentsController::print_instruments()
{
const QString pluginPathInstruments = QDir::cleanPath(QCoreApplication::applicationDirPath() + "/INSTRUMENTS_HERE");
    QDir pluginsDir(pluginPathInstruments);

    if (!pluginsDir.exists()) {
        qWarning() << "Plugin directory does not exist:" << pluginPathInstruments;
        return;
    }

    qDebug() << "Printing detected plugins from directory:" << pluginPathInstruments;

    // Iterate through all files in the plugin directory
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        // Check if the file has a .dll extension
        if (!fileName.endsWith(".dll", Qt::CaseInsensitive)) {
            continue; // Skip non-DLL files
        }

        // Print out the detected DLL plugin file names
        qDebug() << "Detected plugin:" << fileName;

        // Load the plugin dynamically
        QString filePath = pluginsDir.absoluteFilePath(fileName);
        QPluginLoader loader(filePath);
        QObject *plugin = loader.instance();

        if (!plugin) {
            qWarning() << "Failed to load plugin:" << fileName << loader.errorString();
            continue;
        }

        // Check if the plugin implements PluginInstrument
        PluginInstrument *pluginInstrument = qobject_cast<PluginInstrument*>(plugin);
        if (pluginInstrument) {
            qDebug() << "Plugin" << fileName << "implements PluginInstrument.";
        } else {
            qDebug() << "Plugin" << fileName << "does not implement PluginInstrument.";
        }
    }
}

