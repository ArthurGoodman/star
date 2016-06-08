#pragma once

#include <QtWidgets>

class Widget : public QWidget {
    Q_OBJECT

    const static int circleRadius = 4;
    const static int normalLength = 12;
    const static Qt::GlobalColor normalColor = Qt::darkGray;

    QPolygonF polygon;
    QPointF *newPoint, *movedPoint;
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
    double distance(const QPointF &a, const QPointF &b);
    void drawPolygon(const QPolygonF &poly, QPainter *p, QColor color);
    void drawArrow(const QPointF &o, const QPointF &v, QPainter *p);
    void closePolygon();
    bool isPolygonClockwise(const QPolygonF &polygon);
    void check();
    QPointF normal(const QPointF &v);
    QPointF normalized(const QPointF &v);
};
