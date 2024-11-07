#ifndef PAINT_ON_IMG_H
#define PAINT_ON_IMG_H

#include <QWidget>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#include <QStack>
#include <QPixmap>
#include <QColor>
#include <QContextMenuEvent>

class PaintOnImg : public QWidget {
    Q_OBJECT

public:
    PaintOnImg(QWidget *parent = nullptr);
    ~PaintOnImg();

    void undo();
    void redo();
    void setDrawingColor(const QColor &color);
    void clearDrawing(); // New method for clearing the drawing

    void handleMousePressEvent(QMouseEvent *event);

    void handleMouseMoveEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);


protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    //void contextMenuEvent(QContextMenuEvent *event) override; // New


private:
    QImage image;
    bool drawing;
    QPoint lastPoint;

    QStack<QPixmap> undoStack;
    QStack<QPixmap> redoStack;

    QColor drawingColor;

    void saveStateToUndo();
};

#endif // PAINT_ON_IMG_H
