#ifndef FILTERSQMLTYPES_H
#define FILTERSQMLTYPES_H

#include "filter.h"
#include "valuefilter.h"
#include "valuesfilter.h"
#include "modelfilter.h"
#include "indexfilter.h"
#include "regexpfilter.h"
#include "rangefilter.h"
#include "expressionfilter.h"
#include "anyoffilter.h"
#include "alloffilter.h"
#include <QQmlEngine>
#include <QCoreApplication>

struct QQsfpmAllOfFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::AllOfFilter)
    QML_NAMED_ELEMENT(AllOfFilter)
};

struct QQsfpmAnyOfFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::AnyOfFilter)
    QML_NAMED_ELEMENT(AnyOfFilter)
};

struct QQsfpmExpressionFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ExpressionFilter)
    QML_NAMED_ELEMENT(ExpressionFilter)
};

struct QQsfpmFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::Filter)
    QML_NAMED_ELEMENT(Filter)
    QML_UNCREATABLE("Abstract")
};

struct QQsfpmFilterContainerAttachedForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::FilterContainerAttached)
    QML_NAMED_ELEMENT(FilterContainer)
    QML_UNCREATABLE("Abstract")
    QML_ATTACHED(QQsfpm::FilterContainerAttached)
};

struct QQsfpmFilterContainerFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::FilterContainerFilter)
    QML_NAMED_ELEMENT(FilterContainerFilter)
    QML_UNCREATABLE("Abstract")
};

struct QQsfpmIndexFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::IndexFilter)
    QML_NAMED_ELEMENT(IndexFilter)
};

struct QQsfpmModelFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ModelFilter)
    QML_NAMED_ELEMENT(ModelFilter)
};

struct QQsfpmRangeFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::RangeFilter)
    QML_NAMED_ELEMENT(RangeFilter)
};

struct QQsfpmRegExpFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::RegExpFilter)
    QML_NAMED_ELEMENT(RegExpFilter)
};

struct QQsfpmRoleFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::RoleFilter)
    QML_NAMED_ELEMENT(RoleFilter)
    QML_UNCREATABLE("Abstract")
};

struct QQsfpmValueFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ValueFilter)
    QML_NAMED_ELEMENT(ValueFilter)
};

struct QQsfpmValuesFilterForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ValuesFilter)
    QML_NAMED_ELEMENT(ValuesFilter)
};

#endif // FILTERSQMLTYPES_H
