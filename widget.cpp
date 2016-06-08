#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent) {
    resize(qApp->desktop()->size() / 1.3);
    move(qApp->desktop()->rect().center() - rect().center());
    setMinimumSize(qApp->desktop()->size() / 4);

    startTimer(16);

    setMouseTracking(true);

    qsrand(QTime::currentTime().msec());

    newPoint = 0;
    movedPoint = 0;

    antialiasing = true;
    normals = true;
}

Widget::~Widget() {
    delete newPoint;
}

void Widget::timerEvent(QTimerEvent *) {
    update();
}

void Widget::mousePressEvent(QMouseEvent *e) {
    if (e->buttons() & Qt::LeftButton) {
        if (newPoint == 0) {
            for (int i = 0; i < polygon.size(); i++)
                if (distance(polygon[i], e->pos()) <= 2 * circleRadius) {
                    movedPoint = &polygon[i];
                    return;
                }

            polygon.clear();
            color = Qt::blue;
        }

        if (distance(polygon.first(), e->pos()) <= 2 * circleRadius) {
            if (polygon.size() > 2) {
                delete newPoint;
                newPoint = 0;

                check();
            }
        } else {
            polygon << e->pos();

            if (polygon.size() == 1)
                newPoint = new QPointF(e->pos());
        }
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *) {
    movedPoint = 0;
}

void Widget::mouseMoveEvent(QMouseEvent *e) {
    if (movedPoint != 0) {
        *movedPoint = e->pos();
        check();
    } else if (newPoint != 0)
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

    case Qt::Key_A:
        antialiasing = !antialiasing;
        break;

    case Qt::Key_N:
        normals = !normals;
        break;
    }
}

void Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::lightGray);

    if (antialiasing)
        p.setRenderHint(QPainter::Antialiasing);

    drawPolygon(&p, color);
}

double Widget::distance(const QPointF &a, const QPointF &b) {
    return std::sqrt(std::pow(a.x() - b.x(), 2) + std::pow(a.y() - b.y(), 2));
}

void Widget::drawPolygon(QPainter *p, QColor color) {
    QPainterPath linePath, circlePath;
    circlePath.setFillRule(Qt::WindingFill);

    if (!polygon.isEmpty()) {
        linePath.moveTo(polygon.first());
        int d = newPoint == 0 && distance(mapFromGlobal(QCursor::pos()), polygon.first()) <= 2 * circleRadius ? 2 : 0;
        circlePath.addEllipse(polygon.first(), circleRadius + d, circleRadius + d);
    }

    for (int i = 1; i < polygon.size(); i++) {
        linePath.lineTo(polygon[i]);
        int d = newPoint == 0 && distance(mapFromGlobal(QCursor::pos()), polygon[i]) <= 2 * circleRadius ? 2 : 0;
        circlePath.addEllipse(polygon[i], circleRadius + d, circleRadius + d);
    }

    if (!polygon.isEmpty())
        linePath.lineTo(newPoint ? *newPoint : polygon.first());

    if (newPoint != 0)
        circlePath.addEllipse(*newPoint, circleRadius, circleRadius);

    p->strokePath(linePath, QPen(color, 2));
    p->fillPath(circlePath, color.lighter());
    p->strokePath(circlePath, QPen(Qt::black, 1));

    if (newPoint == 0 && normals)
        for (int i = 0; i < polygon.size(); i++) {
            QPointF a = polygon[i], b = polygon[(i + 1) % polygon.size()];
            QPointF o = (a + b) / 2;
            drawArrow(o, normal(b - a), p);
        }
}

void Widget::drawArrow(const QPointF &o, const QPointF &v, QPainter *p) {
    QPointF end = o + v * normalLength;
    QPointF norm = normal(v);

    p->setPen(normalColor);
    p->drawLine(o, end);

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);

    path.moveTo(end);
    path.lineTo(end - v * normalLength / 3 + norm * normalLength / 4);
    path.lineTo(end - v * normalLength / 3 - norm * normalLength / 4);

    p->fillPath(path, normalColor);
}

bool Widget::isPolygonClockwise() {
    int sum = 0;

    for (int i = 0; i < polygon.size(); i++) {
        QPointF a = polygon[i], b = polygon[(i + 1) % polygon.size()];
        sum += (b.x() - a.x()) * (b.y() + a.y());
    }

    return sum < 0;
}

void Widget::check() {
    if (!isPolygonClockwise()) {
        int movedIndex = -1;

        if (movedPoint != 0)
            movedIndex = polygon.indexOf(*movedPoint);

        QPolygonF reversed;

        for (int i = polygon.size() - 1; i >= 0; i--)
            reversed << polygon[i];

        polygon = reversed;

        if (movedIndex != -1)
            movedPoint = &polygon[polygon.size() - 1 - movedIndex];
    }

    color = Qt::darkGreen;

    QVector<Constraint> H;

    for (int i = 0; i < polygon.size(); i++)
        H << Constraint(polygon[i], polygon[(i + 1) % polygon.size()]);

    try {
        reduceLP(H);
    } catch (...) {
        color = Qt::red;
    }
}

QPointF Widget::reduceLP(QVector<Constraint> H) {
    if (H.size() == 2) {
        return H[0].intersect(H[1]);
    }

    int i = qrand() % H.size();
    Constraint h = H[i];

    QVector<Constraint> G(H);
    G.remove(i);

    QPointF x = reduceLP(G);

    if (h.isViolatedBy(x)) {
        QPointF a = h.intersect(G[0]), b = QPointF();
        int ai = 0;

        for (int i = 1; i < G.size(); i++) {
            x = h.intersect(G[i]);

            if (b.isNull()) {
                if (G[i].isViolatedBy(a)) {
                    if (G[ai].isViolatedBy(x))
                        throw 0;
                    else {
                        a = x;
                        ai = i;
                    }
                } else if (!G[ai].isViolatedBy(x))
                    b = x;
            } else {
                if (G[i].isViolatedBy(a)) {
                    if (G[i].isViolatedBy(b))
                        throw 0;
                    else
                        a = x;
                } else if (G[i].isViolatedBy(b))
                    b = x;
            }
        }

        x = a;
    }

    return x;
}

QPointF Widget::normal(const QPointF &v) {
    return normalized(QPointF(-v.y(), v.x()));
}

QPointF Widget::normalized(const QPointF &v) {
    return v / distance(QPointF(), v);
}
