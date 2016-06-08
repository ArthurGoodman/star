#pragma once

#include <QtWidgets>

class Constraint {
    QPointF a;
    double b;

public:
    Constraint();
    Constraint(const QPointF &a, const QPointF &b);

    bool isViolatedBy(const QPointF &x);

    QPointF intersect(const Constraint &other);
};
