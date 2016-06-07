#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent) {
    resize(qApp->desktop()->size() / 1.3);
    move(qApp->desktop()->rect().center() - rect().center());
    setMinimumSize(qApp->desktop()->size() / 4);

    startTimer(16);

    setMouseTracking(true);

    newPoint = 0;
}

Widget::~Widget() {
    delete newPoint;
}

void Widget::timerEvent(QTimerEvent *) {
    update();
}

void Widget::mousePressEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        if (newPoint == 0)
            polygon.clear();

        if (distance(polygon.first(), e->pos()) <= 2 * circleRadius) {
            if (polygon.size() > 2)
                closePolygon();
        } else {
            polygon.append(e->pos());

            if (polygon.size() == 1)
                newPoint = new QPoint(e->pos());
        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *e) {
    if (newPoint != 0)
        *newPoint = e->pos();
}

void Widget::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_Escape:
        isFullScreen() ? showNormal() : qApp->quit();
        break;

    case Qt::Key_F11:
        isFullScreen() ? showNormal() : showFullScreen();
        break;

    case Qt::Key_Backspace:
        polygon.clear();
        break;
    }
}

void Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::lightGray);

    p.setRenderHint(QPainter::Antialiasing);

    drawPolygon(polygon, &p, Qt::red);
}

double Widget::distance(const QPoint &a, const QPoint &b) {
    return std::sqrt(std::pow(a.x() - b.x(), 2) + std::pow(a.y() - b.y(), 2));
}

void Widget::drawPolygon(const QPolygon &poly, QPainter *p, QColor color) {
    QPainterPath linePath, circlePath;
    circlePath.setFillRule(Qt::WindingFill);

    if (!poly.isEmpty()) {
        linePath.moveTo(poly.first());
        circlePath.addEllipse(poly.first(), circleRadius, circleRadius);
    }

    for (int i = 1; i < poly.size(); i++) {
        linePath.lineTo(poly.at(i));
        circlePath.addEllipse(poly.at(i), circleRadius, circleRadius);
    }

    if (!poly.isEmpty())
        linePath.lineTo(newPoint ? *newPoint : poly.first());

    if (newPoint != 0)
        circlePath.addEllipse(*newPoint, circleRadius, circleRadius);

    p->strokePath(linePath, QPen(color, 2));
    p->fillPath(circlePath, color.lighter());
    p->strokePath(circlePath, QPen(Qt::black, 1));
}

void Widget::closePolygon() {
    if (!isPolygonClockwise(polygon)) {
        QPolygon reversed;

        for (int i = polygon.size() - 1; i >= 0; i--)
            reversed.append(polygon[i]);

        polygon = reversed;
    }

    delete newPoint;
    newPoint = 0;
}

bool Widget::isPolygonClockwise(const QPolygon &polygon) {
    int sum = 0;

    for (int i = 0; i < polygon.size(); i++) {
        QPoint a = polygon[i];
        QPoint b = polygon[(i + 1) % polygon.size()];

        sum += (b.x() - a.x()) * (b.y() + a.y());
    }

    return sum < 0;
}
