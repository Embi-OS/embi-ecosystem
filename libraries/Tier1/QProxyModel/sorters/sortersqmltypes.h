#ifndef SOTERSQMLTYPES_H
#define SOTERSQMLTYPES_H

#include "sorter.h"
#include "rolesorter.h"
#include "stringsorter.h"
#include "filtersorter.h"
#include "expressionsorter.h"
#include "sortercontainer.h"
#include <QQmlEngine>
#include <QCoreApplication>

struct QQsfpmExpressionSorterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ExpressionSorter)
    QML_NAMED_ELEMENT(ExpressionSorter)
};

struct QQsfpmFilterSorterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::FilterSorter)
    QML_NAMED_ELEMENT(FilterSorter)
};

struct QQsfpmRoleSorterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::RoleSorter)
    QML_NAMED_ELEMENT(RoleSorter)
};

struct QQsfpmSorterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::Sorter)
    QML_NAMED_ELEMENT(Sorter)
    QML_UNCREATABLE("Abstract")
};

struct QQsfpmSorterContainerAttachedForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::SorterContainerAttached)
    QML_NAMED_ELEMENT(SorterContainer)
    QML_UNCREATABLE("Abstract")
    QML_ATTACHED(QQsfpm::SorterContainerAttached)
};

struct QQsfpmStringSorterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::StringSorter)
    QML_NAMED_ELEMENT(StringSorter)
};

#endif // SOTERSQMLTYPES_H
