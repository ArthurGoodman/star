#include "constraint.h"

Constraint::Constraint()
    : a(QPointF()), b(0) {
}

Constraint::Constraint(const QPointF &a, const QPointF &b)
    : a(QPointF(a.y() - b.y(), b.x() - a.x())), b(a.x() * b.y() - b.x() * a.y()) {
}

bool Constraint::isViolatedBy(const QPointF &x) {
    return QPointF::dotProduct(a, x) + b < 0;
}

QPointF Constraint::intersect(const Constraint &other) {
    double a = -this->a.x() / this->a.y(), b = -other.a.x() / other.a.y(), c = -this->b / this->a.y(), d = -other.b / other.a.y();
    return QPointF((d - c) / (a - b), (a * d - b * c) / (a - b));
}
