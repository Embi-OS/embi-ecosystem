#ifndef PROXYROLESQMLTYPES_H
#define PROXYROLESQMLTYPES_H

#include "proxyrole.h"
#include "aliasrole.h"
#include "methodrole.h"
#include "maprole.h"
#include "joinrole.h"
#include "switchrole.h"
#include "expressionrole.h"
#include "patternrole.h"
#include "regexprole.h"
#include "filterrole.h"
#include "relationrole.h"
#include <QQmlEngine>
#include <QCoreApplication>

struct QQsfpmAliasRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::AliasRole)
    QML_NAMED_ELEMENT(AliasRole)
};

struct QQsfpmExpressionRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ExpressionRole)
    QML_NAMED_ELEMENT(ExpressionRole)
};

struct QQsfpmFilterRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::FilterRole)
    QML_NAMED_ELEMENT(FilterRole)
};

struct QQsfpmJoinRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::JoinRole)
    QML_NAMED_ELEMENT(JoinRole)
};

struct QQsfpmMapRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::MapRole)
    QML_NAMED_ELEMENT(MapRole)
};

struct QQsfpmMethodRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::MethodRole)
    QML_NAMED_ELEMENT(MethodRole)
};

struct QQsfpmProxyRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::ProxyRole)
    QML_NAMED_ELEMENT(ProxyRole)
    QML_UNCREATABLE("Abstract")
};

struct QQsfpmPatternRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::PatternRole)
    QML_NAMED_ELEMENT(PatternRole)
};

struct QQsfpmRegExpRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::RegExpRole)
    QML_NAMED_ELEMENT(RegExpRole)
};

struct QQsfpmRelationRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::RelationRole)
    QML_NAMED_ELEMENT(RelationRole)
};

struct QQsfpmSingleRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::SingleRole)
    QML_NAMED_ELEMENT(SingleRole)
    QML_UNCREATABLE("Abstract")
};

struct QQsfpmSwitchRoleForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::SwitchRole)
    QML_NAMED_ELEMENT(SwitchRole)
};

struct QQsfpmSwitchRoleAttachedForeign {
    Q_GADGET
    QML_FOREIGN(QQsfpm::SwitchRole)
    QML_NAMED_ELEMENT(SwitchRoleAttached)
    QML_UNCREATABLE("Abstract")
    QML_ATTACHED(QQsfpm::SwitchRoleAttached)
};

#endif // PROXYROLESQMLTYPES_H
