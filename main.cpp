#include <QApplication>

#include "widget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    qDebug() << Constraint(QPointF(2, 2), QPointF(5, 5)).intersect(Constraint(QPointF(1, 4), QPointF(1, 1)));

    Widget w;
    w.show();

    return a.exec();
}
