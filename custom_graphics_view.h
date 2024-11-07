#ifndef CUSTOM_GRAPHICS_VIEW_H
#define CUSTOM_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDebug>
#include <QFileInfo>

// Forward declaration
class ImagingInstrumentsController;

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CustomGraphicsView(ImagingInstrumentsController *controller, QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    ImagingInstrumentsController *controller;  // Pointer to the controller
};

#endif // CUSTOM_GRAPHICS_VIEW_H
