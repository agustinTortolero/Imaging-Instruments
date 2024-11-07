
#include "controller.h"
#include "image_view.h"

ImageView::ImageView(QWidget *parent)
    : QMainWindow(parent),
    scene(new QGraphicsScene(this)),
    graphicsView(new QGraphicsView(this)),
    controller(nullptr),
    isDragging(false),
    overlayTextItem(nullptr)
{
    controller->logMessage("Initializing ImageView...", MessageType::STATUS_MSG);

    graphicsView->setScene(scene);
    setCentralWidget(graphicsView);

    setWindowTitle("Imaging Instruments");
    controller->logMessage("Window title set", MessageType::STATUS_MSG);

    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    controller->logMessage("GraphicsView settings applied", MessageType::STATUS_MSG);

    this->resize(800, 800);
    controller->logMessage("Window resized to 800x800", MessageType::STATUS_MSG);

    setAcceptDrops(true);
    controller->logMessage("Drag and drop enabled", MessageType::STATUS_MSG);
}




ImageView::~ImageView()
{
    delete scene;
    delete graphicsView;
    controller->logMessage("ImageView is cleaned ", MessageType::STATUS_MSG);
}

void ImageView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        controller->logMessage("Drag enter event accepted", MessageType::STATUS_MSG);
    } else {
        event->ignore();
        controller->logMessage("Drag enter event ignored", MessageType::STATUS_MSG);
    }
}

void ImageView::dropEvent(QDropEvent *event) {
    controller->logMessage("dropEvent called in ImageView", MessageType::STATUS_MSG);

    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            QImage image(filePath);
            if (!image.isNull()) {
                emit imageDropped(image);
                controller->logMessage(QString("Image loaded successfully from: %1").arg(filePath), MessageType::STATUS_MSG);
            } else {
                controller->logMessage(QString("Failed to load image from file: %1").arg(filePath), MessageType::ERROR_MSG);
            }
        }
    }
    event->acceptProposedAction();
}


void ImageView::displayImage(const QImage &image)
{
    if (scene) {
        controller->logMessage("Clearing previous scene items", MessageType::STATUS_MSG);
        scene->clear();

        if (originalImage.isNull()) {
            originalImage = image;
            controller->logMessage("Storing the original image", MessageType::STATUS_MSG);
        }

        QPixmap pixmap = QPixmap::fromImage(image);
        scene->addPixmap(pixmap);
        scene->setSceneRect(pixmap.rect());

        graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        controller->logMessage("Scene updated and fit to view", MessageType::STATUS_MSG);

        currentImage = image;

        if (controller && controller->isImpulseNoiseMode()) {
            clearOverlays();
            add_overlay_noise();
            controller->logMessage("Overlay noise re-applied", MessageType::STATUS_MSG);
        }
    }
}

void ImageView::clearImage()
{
    if (scene) {
        controller->logMessage("Clearing all items in the scene", MessageType::STATUS_MSG);
        scene->clear();
        scene->setSceneRect(0, 0, 0, 0);
        controller->logMessage("Scene rectangle reset", MessageType::STATUS_MSG);

        originalImage = QImage();
        currentImage = QImage();
        controller->logMessage("Original and current images cleared", MessageType::STATUS_MSG);
    }
}


void ImageView::wheelEvent(QWheelEvent *event)
{
    if (controller) {
        if (controller->isImpulseNoiseMode()) {
            handleImpulseNoiseMode(event);
        } else {
            handleZoom(event);
        }
    } else {
        event->ignore();
    }
}

void ImageView::handleImpulseNoiseMode(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        adjustNoiseDensity(event);
        emit impulseNoiseRequested();
        event->accept();
    } else {
        event->ignore();
    }
}

void ImageView::adjustNoiseDensity(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0; // Adjust sensitivity
    float stepSize = 0.01;

    float newNoiseDensity = controller->noise_density + delta * stepSize;
    newNoiseDensity = qRound(newNoiseDensity / stepSize) * stepSize;
    newNoiseDensity = qBound(0.0f, newNoiseDensity, 1.0f);
    controller->noise_density = newNoiseDensity;

    controller->logMessage(QString("Adjusted noise_density to: %1").arg(controller->noise_density), MessageType::STATUS_MSG);
}

void ImageView::handleZoom(QWheelEvent *event)
{
    qreal factor = 1.2;
    if (event->angleDelta().y() < 0) {
        factor = 1.0 / factor;
    }

    QTransform transform = graphicsView->transform();
    transform.scale(factor, factor);
    graphicsView->setTransform(transform);
    graphicsView->centerOn(graphicsView->mapToScene(graphicsView->viewport()->rect().center()));

    event->accept();
}
void ImageView::setController(ImagingInstrumentsController *controller)
{
    this->controller = controller;
    controller->logMessage("Controller set in ImageView", MessageType::STATUS_MSG);
}


void ImageView::saveImage()
{
    if (currentImage.isNull()) {
        QMessageBox::warning(this, tr("Save Image"), tr("No image to save."));
        controller->logMessage("No image to save", MessageType::ERROR_MSG);
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        "",
        tr("Images (*.png *.jpg *.bmp *.gif *.jpeg *.tiff)")
        );

    if (filePath.isEmpty()) {
        controller->logMessage("Save image operation cancelled", MessageType::STATUS_MSG);
        return;
    }

    if (!filePath.endsWith(".png") && !filePath.endsWith(".jpg") &&
        !filePath.endsWith(".bmp") && !filePath.endsWith(".gif") &&
        !filePath.endsWith(".jpeg") && !filePath.endsWith(".tiff")) {
        filePath.append(".png");
    }

    if (currentImage.save(filePath)) {
        QMessageBox::information(this, tr("Save Image"), tr("Image saved successfully."));
        controller->logMessage("Image saved successfully at: " + filePath, MessageType::STATUS_MSG);
    } else {
        QMessageBox::warning(this, tr("Save Image"), tr("Failed to save image."));
        controller->logMessage("Failed to save image at: " + filePath, MessageType::ERROR_MSG);
    }
}

void ImageView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu contextMenu(this);
    qDebug() << "Triggered Context Menu";

    // Setup actions based on mode
    if (controller) {
        if (controller->isImpulseNoiseMode()) {
            setupImpulseNoiseModeActions(&contextMenu);
        } else if (controller->isDrawingMode()) {
            setupDrawingModeActions(&contextMenu);
        } else {
            setupStandardActions(&contextMenu);
            setupCustomInstrumentsMenu(&contextMenu);

            // Add separator between Custom Instruments and Edit actions
            contextMenu.addSeparator();  // Add separator here

            setupEditActions(&contextMenu);
        }
    }

    // Estimate width based on the longest text in the menu
    int maxWidth = 0;
    foreach (QAction *action, contextMenu.actions()) {
        // Get the text of each action
        QString actionText = action->text();

        // Measure the width of the action's text using the font metrics
        QFontMetrics fontMetrics(action->font());
        int width = fontMetrics.horizontalAdvance(actionText);

        // Track the maximum width
        if (width > maxWidth) {
            maxWidth = width;
        }
    }

    // Add some padding to the calculated width
    maxWidth += 45; // Add some padding

    // Set the width of the context menu based on the longest text
    contextMenu.setFixedWidth(maxWidth);

    // Execute the context menu
    contextMenu.exec(event->globalPos());
}


void ImageView::setupCustomInstrumentsMenu(QMenu *contextMenu)
{
    QMenu *customInstrumentsMenu = new QMenu("More Instruments", this);
    controller->loadPlugins(customInstrumentsMenu);

    // Check for available custom instruments and add actions
    if (controller->hasCustomInstruments()) {
        for (const auto& plugin : controller->getCustomInstruments()) {
            QAction *pluginAction = new QAction(plugin->context_menu_str(), customInstrumentsMenu);
            connect(pluginAction, &QAction::triggered, this, [this, plugin]() {
                if (controller) {
                    controller->applyCustomInstrument(plugin);
                }
            });
            customInstrumentsMenu->addAction(pluginAction);
        }
    } else {
        customInstrumentsMenu->addAction("No Custom Instruments");
    }

    // Add the custom instruments menu to the context menu
    contextMenu->addMenu(customInstrumentsMenu);
}


void ImageView::onPluginTriggered(PluginInstrument *plugin)
{

}



void ImageView::setupImpulseNoiseModeActions(QMenu *menu)
{
    QAction *saveAction = createAction("Save", this, &ImageView::saveImage);
    menu->addAction(saveAction);

    QAction *exitImpulseNoiseAction = createAction("Exit Add Noise", this, &ImageView::exitImpulseNoiseMode);
    menu->addAction(exitImpulseNoiseAction);
}

void ImageView::setupDrawingModeActions(QMenu *menu)
{
    QAction *saveAction = createAction("Save", this, &ImageView::saveImage);
    menu->addAction(saveAction);

    QAction *exitDrawingAction = createAction("Exit Drawing", this, &ImageView::exitDrawingMode);
    menu->addAction(exitDrawingAction);
}

void ImageView::setupStandardActions(QMenu *menu)
{
    QAction *impulseNoiseAction = createAction("Add Noise", this, [this]() {
        if (controller) {
            controller->enterImpulseNoiseMode();
            qDebug() << "Entered Impulse Noise Mode";
            if (!currentImage.isNull()) {
                displayImage(currentImage);
            }
        }
    });
    menu->addAction(impulseNoiseAction);

    QAction *blurAction = createAction("Blur", this, [this]() {
        if (controller) {
            controller->applyBlur();
        }
    });
    menu->addAction(blurAction);

    QAction *deBlurAction = createAction("DeBlur", this, [this]() {
        if (controller) {
            controller->applyDeBlur();
        }
    });
    menu->addAction(deBlurAction);

    QAction *vectorFilterAction = createAction("Filter", this, [this]() {
        if (controller) {
            controller->applyVectorFilter();
        }
    });
    menu->addAction(vectorFilterAction);

    QAction *colorEnhancementAction = createAction("Color Enhancement", this, [this]() {
        if (controller) {
            controller->applyColorEnhancement();
        }
    });
    menu->addAction(colorEnhancementAction);

    QAction *edgeDetectionAction = createAction("Edge Detection", this, [this]() {
        if (controller) {
            controller->applySobelFilter();
        }
    });
    menu->addAction(edgeDetectionAction);

    QMenu *morphologyMenu = new QMenu("Morphology", this);
    setupMorphologyActions(morphologyMenu);
    menu->addMenu(morphologyMenu);


    // Call the new function for Save, Reset, and Return actions
    //setupEditActions(menu);
}

void ImageView::setupEditActions(QMenu *menu)
{
    menu->addAction(createAction("Save", this, &ImageView::saveImage));
    menu->addAction(createAction("Reset", this, [this]() {
        this->resetImage();
    }));

    menu->addAction(createAction("Return", this, [this]() {
        if (controller) {
            controller->switchToDragDropView();
        }
    }));
}

void ImageView::setupMorphologyActions(QMenu *menu)
{
    menu->addAction(createAction("Binary", this, [this]() {
        if (controller) {
            controller->applyBinarization();
        }
    }));

    menu->addAction(createAction("Erosion", this, [this]() {
        if (controller) {
            controller->applyErosion();
        }
    }));

    menu->addAction(createAction("Dilation", this, [this]() {
        if (controller) {
            controller->applyDilation();
        }
    }));

    menu->addAction(createAction("Opening", this, [this]() {
        if (controller) {
            controller->applyOpening();
        }
    }));

    menu->addAction(createAction("Closing", this, [this]() {
        if (controller) {
            controller->applyClosing();
        }
    }));
}

QAction* ImageView::createAction(const QString &text, QObject *receiver, std::function<void()> slot)
{
    QAction *action = new QAction(text, this);
    connect(action, &QAction::triggered, receiver, slot);
    return action;
}

QAction* ImageView::createAction(const QString &text, QObject *receiver, void (ImageView::*method)())
{
    QAction *action = new QAction(text, this);
    connect(action, &QAction::triggered, receiver, method);
    return action;
}
void ImageView::resetImage()
{
    const cv::Mat& originalImage = controller->getModel()->getOriginalInputImage();

    if (!originalImage.empty()) {
        controller->getModel()->inputImage = originalImage.clone();

        QImage resetImage(originalImage.data, originalImage.cols, originalImage.rows, originalImage.step, QImage::Format_RGB888);
        displayImage(resetImage);

        controller->logMessage("Image reset to original.", MessageType::STATUS_MSG);
    } else {
        QMessageBox::warning(this, tr("Reset Image"), tr("No original image available to reset."));
        controller->logMessage("No original image available to reset.", MessageType::ERROR_MSG);
    }
}

void ImageView::enterImpulseNoiseMode()
{
    if (controller) {
        controller->enterImpulseNoiseMode();
        controller->logMessage("Entered Impulse Noise Mode", MessageType::STATUS_MSG);

        if (!originalImage.isNull()) {
            displayImage(originalImage);
        }

        clearOverlays();
        add_overlay_noise();
    }
}

void ImageView::exitImpulseNoiseMode()
{
    if (controller) {
        controller->exitImpulseNoiseMode();
        controller->logMessage("Exited Impulse Noise Mode", MessageType::STATUS_MSG);
        clearOverlays();
    }
}


void ImageView::enterDrawingMode()
{
    if (controller) {
        controller->enterDrawingMode();
        qDebug() << "Entered Drawing Mode";

        if (!originalImage.isNull()) {
            displayImage(originalImage);
        }
    }
}

void ImageView::exitDrawingMode()
{
    qDebug() << "Exiting Drawing Mode";
    if (controller) {
        controller->exitDrawingMode();
        qDebug() << "Exited Drawing Mode";

    }
}
void ImageView::add_overlay_noise()
{
    clearOverlays();

    int overlayWidth = currentImage.width();
    int overlayHeight = currentImage.height();

    QImage overlayImage = setup_inlay_noise(overlayWidth, overlayHeight);
    draw_inlay_noise(overlayImage, overlayWidth, overlayHeight);

    QPixmap overlayPixmap = QPixmap::fromImage(overlayImage);
    QGraphicsPixmapItem *overlayItem = scene->addPixmap(overlayPixmap);
    overlayItem->setZValue(10);

    controller->logMessage("Overlay noise added", MessageType::STATUS_MSG);
}

QImage ImageView::setup_inlay_noise(int overlayWidth, int overlayHeight)
{
    QImage overlayImage(overlayWidth, overlayHeight, QImage::Format_ARGB32);
    overlayImage.fill(Qt::transparent);
    controller->logMessage("Setup inlay noise with dimensions: " + QString("%1 x %2").arg(overlayWidth).arg(overlayHeight), MessageType::STATUS_MSG);
    return overlayImage;
}

void ImageView::draw_inlay_noise(QImage &overlayImage, int overlayWidth, int overlayHeight)
{
    QPainter painter(&overlayImage);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    int overlayAreaHeight = overlayHeight * 0.25;
    int overlayY = overlayHeight - overlayAreaHeight;

    painter.setBrush(QColor(128, 128, 128, 100));
    painter.drawRect(0, overlayY, overlayWidth, overlayAreaHeight);

    int fontSize = overlayAreaHeight / 8;
    painter.setPen(Qt::white);
    painter.setFont(QFont("Verdana", fontSize));

    int verticalOffset = overlayAreaHeight / 2;

    QString controlText = "Noise Up/Down Ctrl + Wheel";
    QRect controlTextRect = painter.boundingRect(QRect(0, 0, overlayWidth, overlayAreaHeight), Qt::AlignLeft, controlText);
    painter.drawText(10, overlayY + verticalOffset - controlTextRect.height() / 2, controlText);

    QString noiseDensityText = QString("Noise Density: %1").arg(controller ? controller->noise_density : 0.0);
    QRect noiseDensityRect = painter.boundingRect(QRect(0, 0, overlayWidth, overlayAreaHeight), Qt::AlignLeft, noiseDensityText);
    painter.drawText(10, overlayY + verticalOffset + 50, noiseDensityText);

    controller->logMessage("Inlay noise drawn", MessageType::STATUS_MSG);
}

void ImageView::clearOverlays()
{
    controller->logMessage("Clearing all overlays", MessageType::STATUS_MSG);

    QList<QGraphicsItem*> itemsToRemove = scene->items();
    for (QGraphicsItem *item : itemsToRemove) {
        if (dynamic_cast<QGraphicsPixmapItem*>(item) && item->zValue() == 10) {
            scene->removeItem(item);
            delete item; // Clean up
        } else if (dynamic_cast<QGraphicsTextItem*>(item)) {
            scene->removeItem(item);
            delete item; // Clean up
        }
    }
    overlayTextItem = nullptr;

    controller->logMessage("All overlays cleared", MessageType::STATUS_MSG);
}


void ImageView::applyTheme(const QString &theme)
{
    if (theme == "Light") {
        setStyleSheet("background-color: white; color: black;");
    } else if (theme == "Dark") {
        setStyleSheet("background-color: black; color: white;");
    } else if (theme == "Cyberpunk") {
        setStyleSheet("background-color: #1E1E2F; color: #00FF7F;");  // Cyberpunk colors

        // Set the context menu's style for Cyberpunk
        QString contextMenuStyle = "QMenu { background-color: #1E1E2F; color: #00FF7F; }"
                                   "QMenu::item { padding: 5px; }"
                                   "QMenu::item:selected { background-color: #00FF7F; color: black; }"; // Selected item
        this->setStyleSheet(contextMenuStyle);
    }
    QString labelStyle = (theme == "Cyberpunk") ?
                             "QLabel { color: #00FF7F; font-size: 16px; }" :
                             "QLabel { font-size: 16px; }";

}

void ImageView::setDragging(bool enabled) {
    isDragging = enabled;
    if (isDragging) {
        graphicsView->setDragMode(QGraphicsView::ScrollHandDrag); // Enable dragging
    } else {
        graphicsView->setDragMode(QGraphicsView::NoDrag); // Disable dragging
    }
}
