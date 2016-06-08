#pragma once

#include <QtWidgets>

#include "constraint.h"

class Widget : public QWidget {
    Q_OBJECT

    const static int circleRadius = 4;
    const static int normalLength = 12;
    const static Qt::GlobalColor normalColor = Qt::darkGray;

    QPolygonF polygon;
    QPointF *newPoint, *movedPoint;
    QColor color;

    bool antialiasing, normals;

    QPoint offset, lastPos;

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
    void drawPolygon(QPainter *p, QColor color);
    void drawArrow(const QPointF &o, const QPointF &v, QPainter *p);
    bool isPolygonClockwise();
    void check();
    QPointF reduceLP(QVector<Constraint> H);
    QPointF normal(const QPointF &v);
    QPointF normalized(const QPointF &v);
};
