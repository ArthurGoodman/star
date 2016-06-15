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
    static const double epsilon = 1e-6;

    double p = -a.x() / a.y(), q = -other.a.x() / other.a.y(), r = -b / a.y(), s = -other.b / other.a.y();

    if (fabs(a.y()) < epsilon)
        return QPointF(-b / a.x(), -other.a.x() / other.a.y() * (-b / a.x()) - other.b / other.a.y());

    if (fabs(other.a.y()) < epsilon)
        return QPointF(-other.b / other.a.x(), -a.x() / a.y() * (-other.b / other.a.x()) - b / a.y());

    if (fabs(p - q) < epsilon)
        return QPointF(-b / a.x(), 0);

    return QPointF((s - r) / (p - q), (p * s - q * r) / (p - q));
}
