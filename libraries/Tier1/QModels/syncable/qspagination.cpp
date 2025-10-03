#include "qspagination.h"

QSPagination::QSPagination(QObject *parent):
    QObject(parent)
{
    connect(this, &QSPagination::enabledChanged, this, &QSPagination::invalidated);

    connect(this, &QSPagination::perPageChanged, this, &QSPagination::invalidate);
    connect(this, &QSPagination::pageChanged, this, &QSPagination::invalidate);
}

void QSPagination::invalidate()
{
    if (m_enabled)
        emit this->invalidated();
}
