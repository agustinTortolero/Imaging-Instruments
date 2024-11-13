#include "controller.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

ImagingInstrumentsView::ImagingInstrumentsView(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), controller(nullptr)
{
    ui->setupUi(this);
    setWindowTitle("Imaging Instruments");

    dragDropLabel = new QLabel("Drop an Image", this);
    dragDropLabel->setAlignment(Qt::AlignCenter);
    QFont font("Verdana", 24);
    dragDropLabel->setFont(font);
    dragDropLabel->setStyleSheet("color: #A9A9A9;");
    setCentralWidget(dragDropLabel);
    setAcceptDrops(true);

    // Raise the window to the front
    raise();
    activateWindow();
}

ImagingInstrumentsView::~ImagingInstrumentsView()
{
    delete ui;
}

void ImagingInstrumentsView::setController(ImagingInstrumentsController *controller)
{
    this->controller = controller;
    connect(this, &ImagingInstrumentsView::imageDropped, controller, &ImagingInstrumentsController::onImageDropped);
    connect(controller, &ImagingInstrumentsController::themeChanged, this, &ImagingInstrumentsView::applyTheme);
}

void ImagingInstrumentsView::dragEnterEvent(QDragEnterEvent *event) {
    qDebug()<<"dragEnterEvent in ImagingInstrumentsView ";
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImagingInstrumentsView::dropEvent(QDropEvent *event) {
        qDebug()<<"dropEvent in ImagingInstrumentsView ";
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            QImage image(filePath);
            if (!image.isNull()) {
                emit imageDropped(image);  // Emit the signal with the QImage
            } else {
                qWarning() << "Failed to load image from file:" << filePath;
            }
        }
    }
    event->acceptProposedAction();
}

void ImagingInstrumentsView::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "Context menu event triggered!";

    QMenu contextMenu(this);

    QAction *loadImageAction = contextMenu.addAction("Load Image"); // New action for loading an image
    QAction *aboutAction = contextMenu.addAction("About");
    QAction *hardwareInfoAction = contextMenu.addAction("Show Hardware Info");


    // Create a submenu for the themes
    QMenu *themeMenu = contextMenu.addMenu("Theme");
    QAction *lightAction = themeMenu->addAction("Light");
    QAction *darkAction = themeMenu->addAction("Dark");
    QAction *cyberpunkAction = themeMenu->addAction("Cyberpunk");

    QAction *quitAction = contextMenu.addAction("Quit");

    connect(aboutAction, &QAction::triggered, this, &ImagingInstrumentsView::showAbout);
    connect(hardwareInfoAction, &QAction::triggered, this, &ImagingInstrumentsView::showHardwareInfo);
    connect(loadImageAction, &QAction::triggered, this, &ImagingInstrumentsView::loadImageFromFile); // Connect to the new slot
    connect(quitAction, &QAction::triggered, this, &ImagingInstrumentsView::quitApp);
    connect(lightAction, &QAction::triggered, this, &ImagingInstrumentsView::setLightTheme);
    connect(darkAction, &QAction::triggered, this, &ImagingInstrumentsView::setDarkTheme);
    connect(cyberpunkAction, &QAction::triggered, this, &ImagingInstrumentsView::setCyberpunkTheme);

    contextMenu.exec(event->globalPos());
    qDebug() << "Context menu executed.";
}


void ImagingInstrumentsView::loadImageFromFile()
{
    // Get the path to the user's Pictures directory
    QString picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    QString filePath = QFileDialog::getOpenFileName(this, "Load Image", picturesPath, "Images (*.png *.jpg *.jpeg *.bmp)");

    if (!filePath.isEmpty()) {
        QImage image(filePath);
        if (image.isNull()) {
            QMessageBox::warning(this, "Load Image", "Failed to load image.");
            return;
        }

        emit imageDropped(image);
    }
}
void ImagingInstrumentsView::showAbout()
{
    QDialog aboutDialog(this);
    aboutDialog.setWindowTitle("About Imaging Instruments");

    QVBoxLayout *layout = setupLayout(&aboutDialog);
    addTitle(layout);
    addVersionInfo(layout);
    addDescription(layout);
    addLogos(layout);
    addCloseButton(layout, &aboutDialog);

    aboutDialog.exec();
}

QVBoxLayout* ImagingInstrumentsView::setupLayout(QDialog *dialog)
{
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    dialog->setMinimumSize(400, 500); // Set a minimum size for the dialog
    return layout;
}

void ImagingInstrumentsView::addTitle(QVBoxLayout *layout)
{
    QLabel *titleLabel = new QLabel("Imaging Instruments");
    titleLabel->setFont(QFont("Verdana", 16, QFont::Bold));
    layout->addWidget(titleLabel);
}


void ImagingInstrumentsView::addVersionInfo(QVBoxLayout *layout)
{
    // Get the current date and time in the formats "yyyy-MM-dd" and "hh:mm"
    QString compilationDate = QDate::currentDate().toString("yyyy-MM-dd");
    QString compilationTime = QTime::currentTime().toString("hh:mm");

    // Construct the version string with the compilation date and time
    QString version = QString("Version 0.8 (Built on %1 at %2)").arg(compilationDate, compilationTime);

    // Create and configure the label
    QLabel *versionLabel = new QLabel(version);
    versionLabel->setFont(QFont("Verdana", 12)); // Set font to Verdana

    // Add the label to the layout
    layout->addWidget(versionLabel);
}

QString ImagingInstrumentsView::readBuildNumber()
{
    QString buildNumber;
    QFile buildFile("build_number.txt");
    if (buildFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&buildFile);
        buildNumber = in.readLine(); // Read the first line
        buildFile.close();
    } else {
        buildNumber = "Unknown"; // Handle case where file is not found
    }
    return buildNumber;
}

void ImagingInstrumentsView::addDescription(QVBoxLayout *layout)
{
    QString description =
        "<p><b>Imaging Instruments</b> is a high-performance image processing application crafted for efficiency and designed to deliver powerful and reliable image manipulation capabilities.<br>"
        "Developed using the <b>Model-View-Controller (MVC)</b> architecture, <b>Imaging Instruments</b> harnesses the full potential of modern technologies, combining <b>C++</b>, <b>Qt6</b>, <b>CUDA-C</b>, and <b>OpenCV</b> to deliver a powerful and seamless experience for both professionals and enthusiasts.<br>"
        "With multi-threading support via <b>OpenMP</b> and GPU acceleration through <b>CUDA-C</b>, <b>Imaging Instruments</b> provides lightning-fast image processing, unlocking the full capabilities of modern hardware.</p>"
        "<p>Built and designed by Agustin Tortolero. Fueled by copious amounts of yerba mate.</p>"
        "<p><b>Contact</b>: agustin.tortolero@proton.me</p>"
        "<p><b>Source code</b>: <a href='https://github.com/agustinTortolero/Imaging-Instruments'>GitHub</a></p>";



    QTextEdit *descriptionEdit = new QTextEdit();
    descriptionEdit->setHtml(description); // Use setHtml to interpret HTML content
    descriptionEdit->setReadOnly(true);
    descriptionEdit->setStyleSheet("border: none;"); // Remove border for a cleaner look
    descriptionEdit->setFont(QFont("Verdana", 10)); // Set font to Verdana
    layout->addWidget(descriptionEdit);
}
void ImagingInstrumentsView::addLogos(QVBoxLayout *layout)
{
    QHBoxLayout *logoLayout = new QHBoxLayout();

    // List of logo paths
    QStringList logoPaths = {
        ":/resources/Cpp_logo.png",
        ":/resources/Nvidia_logo.png",
        ":/resources/OpenCV_logo.png",
        ":/resources/OpenMP_logo.png",
        ":/resources/Qt_logo.png"
    };

    bool allLogosLoaded = true;

    for (const QString &path : logoPaths) {
        QLabel *logoLabel = new QLabel();
        QPixmap pixmap(path);

        if (pixmap.isNull()) {
            controller->logMessage(QString("Failed to load logo: %1").arg(path), MessageType::ERROR_MSG);
            allLogosLoaded = false;
        } else {
            logoLabel->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            logoLayout->addWidget(logoLabel);
        }
    }

    if (allLogosLoaded) {
        controller->logMessage("All logos loaded successfully", MessageType::STATUS_MSG);
    }
    layout->addLayout(logoLayout);
}

void ImagingInstrumentsView::addCloseButton(QVBoxLayout *layout, QDialog *dialog)
{
    QPushButton *closeButton = new QPushButton("Close");
    closeButton->setFont(QFont("Verdana", 10)); // Set font to Verdana
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(closeButton);
}



void ImagingInstrumentsView::showHardwareInfo()
{
    // Get CPU core count using std::thread
    int cpuCores = std::thread::hardware_concurrency();
    QString cpuInfo = QString("CPU Cores: %1").arg(cpuCores);

    // Get CPU name based on platform
    QString cpuName;

#ifdef _WIN32
    // Windows: Get CPU name from the registry
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        TCHAR buffer[256];
        DWORD bufferSize = sizeof(buffer);
        if (RegQueryValueEx(hKey, TEXT("ProcessorNameString"), NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            cpuName = QString::fromWCharArray(buffer);
        }
        RegCloseKey(hKey);
    } else {
        cpuName = "Unknown CPU";
    }
#elif __linux__
    // Linux: Get CPU name from /proc/cpuinfo
    std::ifstream cpuInfoFile("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuInfoFile, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                cpuName = QString::fromStdString(line.substr(pos + 1));
                break;
            }
        }
    }
    if (cpuName.isEmpty()) {
        cpuName = "Unknown CPU";
    }
#endif

    cpuInfo = QString("CPU: %1\nCores: %2").arg(cpuName).arg(cpuCores);

    // Get GPU info if CUDA is available
    QString gpuInfo;
#ifdef USE_CUDA
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount > 0) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, 0);

        gpuInfo = QString("GPU: %1\nArchitecture: %2.%3")
                      .arg(prop.name)
                      .arg(prop.major)
                      .arg(prop.minor);
    } else {
        gpuInfo = "No CUDA-compatible GPU found.";
    }
#else
    gpuInfo = "CUDA not enabled.";
#endif

    // Combine all the info and show it in a message box
    QString info = cpuInfo + "\n" + gpuInfo;
    QMessageBox::information(this, "Hardware Info", info);
}

QString ImagingInstrumentsView::getCpuInfo()
{
    QProcess process;
    process.start("wmic", QStringList() << "cpu" << "get" << "name");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.trimmed();
}

QString ImagingInstrumentsView::getGpuInfo()
{
    QProcess process;
    process.start("wmic", QStringList() << "path" << "win32_VideoController" << "get" << "name");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.trimmed();
}

void ImagingInstrumentsView::setLightTheme()
{
    controller->setTheme("Light");
}

void ImagingInstrumentsView::setDarkTheme()
{
    controller->setTheme("Dark");
}

void ImagingInstrumentsView::setCyberpunkTheme()
{
    controller->setTheme("Cyberpunk");

}


void ImagingInstrumentsView::applyTheme(const QString &theme)
{
    if (theme == "Light") {
        setStyleSheet("background-color: white; color: black;");
        dragDropLabel->setStyleSheet("color: black;");
    } else if (theme == "Dark") {
        setStyleSheet("background-color: black; color: white;");
        dragDropLabel->setStyleSheet("color: white;");
    } else if (theme == "Cyberpunk") {
        setStyleSheet("background-color: #1E1E2F; color: #00FF7F;"); // Dark background with neon green text
        dragDropLabel->setStyleSheet("color: #FF00FF;"); // Neon pink text for the label

    }

    controller->logMessage("Applied theme:", MessageType::STATUS_MSG);
}


void ImagingInstrumentsView::quitApp()
{
    qApp->quit();
}
