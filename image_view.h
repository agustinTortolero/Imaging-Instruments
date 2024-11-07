#ifndef IMAGE_VIEW_H
#define IMAGE_VIEW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>
#include <QGraphicsTextItem>
#include <QWheelEvent>
#include <QScreen> //qt6
#include <QVBoxLayout>
#include <QScreen>
#include <QScrollBar>

#include "plugin_interface.h"
#include "custom_graphics_view.h"

class ImagingInstrumentsController; // Forward declaration

class ImageView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageView(QWidget *parent = nullptr);
    ~ImageView();

    ImageView* imageView;
    void displayImage(const QImage &image);
    void setController(ImagingInstrumentsController *controller); // Add this method
    void resetImage();
    void clearImage();

    void setDragging(bool enabled);
    QGraphicsScene *scene;
    QGraphicsView *graphicsView;

    void onPluginTriggered(PluginInstrument *plugin);
private:
    bool paint_mode;

    QImage currentImage;
    ImagingInstrumentsController *controller;

    bool isDragging;
    QPoint lastMousePos;

    QImage originalImage;
    QGraphicsTextItem *overlayTextItem;

    void setupImpulseNoiseModeActions(QMenu *menu);
    void setupDrawingModeActions(QMenu *menu);

    void setupStandardActions(QMenu *menu);
    void setupMorphologyActions(QMenu *menu);

    QAction* createAction(const QString &text, QObject *receiver, std::function<void()> slot);
    QAction* createAction(const QString &text, QObject *receiver, void (ImageView::*method)());

    bool drawMode;
    QPainterPath painterPath;

    void togglePaintMode();
    void updateScene();
    void finalizeDrawing();

    QPainterPath drawPath;
    QPixmap drawingPixmap;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void setupCustomInstrumentsMenu(QMenu *contextMenu);
    void setupEditActions(QMenu *menu);

public slots:
    void applyTheme(const QString &theme);

private slots:
    void saveImage();
    void enterImpulseNoiseMode();
    void exitImpulseNoiseMode();

    void enterDrawingMode();
    void exitDrawingMode();

    void add_overlay_noise();
    QImage setup_inlay_noise(int overlayWidth, int overlayHeight);
    void draw_inlay_noise(QImage &overlayImage, int overlayWidth, int overlayHeight);
    void clearOverlays();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void handleImpulseNoiseMode(QWheelEvent *event);
    void adjustNoiseDensity(QWheelEvent *event);
    void handleZoom(QWheelEvent *event);

    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void impulseNoiseRequested();
    void imageDropped(const QImage &image); // Signal for image drop
};

#endif
