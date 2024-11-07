#include "paint_on_img.h"
#include <QMenu>
#include <QColorDialog>
#include <QFileDialog>
#include <QPainter>



PaintOnImg::PaintOnImg(QWidget *parent)
    : QWidget(parent), drawing(false), drawingColor(Qt::black) {
    image = QImage(size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent); // Fill with transparent color
    setAttribute(Qt::WA_OpaquePaintEvent, true); // Ensure proper drawing
    setMouseTracking(true); // Enable mouse tracking
}

PaintOnImg::~PaintOnImg() {}

void PaintOnImg::handleMousePressEvent(QMouseEvent *event) {
    qDebug() << "handleMousePressEvent triggered";
    mousePressEvent(event); // Call the protected method
}

void PaintOnImg::handleMouseMoveEvent(QMouseEvent *event) {
    qDebug() << "handleMouseMoveEvent triggered";
    mouseMoveEvent(event); // Call the protected method
}

void PaintOnImg::handleMouseReleaseEvent(QMouseEvent *event) {
    qDebug() << "handleMouseReleaseEvent triggered";
    mouseReleaseEvent(event); // Call the protected method
}

void PaintOnImg::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawImage(0, 0, image);
    qDebug() << "paintEvent triggered";
}

void PaintOnImg::mousePressEvent(QMouseEvent *event) {
    qDebug() << "PaintOnImg mousePressEvent triggered";
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Started drawing at:" << event->pos();
        saveStateToUndo(); // Save current state before drawing
        drawing = true;
        lastPoint = event->pos();
        event->accept(); // Accept the event to prevent further handling
    } else {
        qDebug() << "Mouse press with non-left button:" << event->button();
    }
}

void PaintOnImg::mouseMoveEvent(QMouseEvent *event) {
    qDebug() << "PaintOnImg mouseMoveEvent triggered";
    if (drawing) {
        qDebug() << "Drawing line from" << lastPoint << "to" << event->pos();
        QPainter painter(&image);
        painter.setPen(QPen(drawingColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(lastPoint, event->pos());
        lastPoint = event->pos();
        update(); // Refresh the widget
    } else {
        qDebug() << "Mouse move without drawing";
    }
}

void PaintOnImg::mouseReleaseEvent(QMouseEvent *event) {
    qDebug() << "PaintOnImg mouseReleaseEvent triggered";
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Stopped drawing";
        drawing = false;
    } else {
        qDebug() << "Mouse release with non-left button:" << event->button();
    }
}

void PaintOnImg::saveStateToUndo() {
    undoStack.push(QPixmap::fromImage(image));
    redoStack.clear();
}

void PaintOnImg::undo() {
    if (!undoStack.isEmpty()) {
        redoStack.push(QPixmap::fromImage(image));
        image = undoStack.pop().toImage();
        update();
    }
}

void PaintOnImg::redo() {
    if (!redoStack.isEmpty()) {
        undoStack.push(QPixmap::fromImage(image));
        image = redoStack.pop().toImage();
        update();
    }
}

void PaintOnImg::setDrawingColor(const QColor &color) {
    drawingColor = color;
    qDebug() << "Drawing color changed to:" << color;
}

void PaintOnImg::clearDrawing() {
    saveStateToUndo();
    image.fill(Qt::transparent); // Clear the drawing
    update();
    qDebug() << "Drawing cleared";
}
