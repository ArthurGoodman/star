#pragma once

#include <QtWidgets>

class Widget : public QWidget {
    Q_OBJECT

    const static int circleRadius = 4;

    QPolygon polygon;
    QPoint *newPoint, *movedPoint;
    QColor color;

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    void timerEvent(QTimerEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    double distance(const QPoint &a, const QPoint &b);
    void drawPolygon(const QPolygon &poly, QPainter *p, QColor color);
    void closePolygon();
    bool isPolygonClockwise(const QPolygon &polygon);
    void check();
};
