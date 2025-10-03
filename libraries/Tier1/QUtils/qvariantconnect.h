#ifndef QVARIANTCONNECT_H
#define QVARIANTCONNECT_H

#include <QObject>
#include <QDate>
#include <QVariant>

namespace QUtils {

template <typename L, int N> struct List_Select { typedef typename List_Select<typename L::Cdr, N - 1>::Value Value; };
template <typename L> struct List_Select<L,0> { typedef typename L::Car Value; };

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 1 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 1, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                     const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                     Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func2> SlotType;

    auto execution =
            [=,
            receiver = const_cast<typename SlotType::Object *>(receiver)]
                (const QVariant& value)
    {
        using arguments = typename SlotType::Arguments;
        // using raw_type = typename QtPrivate::List_Select<arguments, 0>::Value;
        using raw_type = typename List_Select<arguments, 0>::Value;
        using T = typename std::remove_cv<typename std::remove_reference<raw_type>::type>::type;
        (receiver->*slot)(value.value<T>());
    };

    return QObject::connect(sender, signal, receiver, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 2 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 2, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                     const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                     Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func2> SlotType;

    auto execution =
            [=,
            receiver = const_cast<typename SlotType::Object *>(receiver)]
                (const QVariant& value, auto&&... args)
    {
        using arguments = typename SlotType::Arguments;
        // using raw_type = typename QtPrivate::List_Select<arguments, 0>::Value;
        using raw_type = typename List_Select<arguments, 0>::Value;
        using T = typename std::remove_cv<typename std::remove_reference<raw_type>::type>::type;
        (receiver->*slot)(value.value<T>(), std::forward<decltype(args)>(args)...);
    };

    return QObject::connect(sender, signal, receiver, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 1 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 0, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                     const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                     Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func2> SlotType;

    auto execution =
            [=,
            receiver = const_cast<typename SlotType::Object *>(receiver)]
                (const QVariant&)
    {
        (receiver->*slot)();
    };

    return QObject::connect(sender, signal, receiver, std::move(execution), type);
}

template <typename T, typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 1 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == -1 &&
                        !QtPrivate::FunctionPointer<Func2>::IsPointerToMemberFunction, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                     const QObject *context, Func2 slot,
                     Qt::ConnectionType type = Qt::AutoConnection)
{
    auto execution =
            [=,
            slot = std::move(slot)]
                (const QVariant& value) mutable
    {
        slot(value.value<T>());
    };

    return QObject::connect(sender, signal, context, std::move(execution), type);
}

template <typename T, typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 1 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == -1, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
{
    return connectFromQtVariant<T>(sender, signal, sender, std::move(slot), Qt::DirectConnection);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 0 &&
                        !QtPrivate::FunctionPointer<Func2>::IsPointerToMemberFunction, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                     const QObject *context, Func2 slot,
                     Qt::ConnectionType type = Qt::AutoConnection)
{
    auto execution =
            [=,
            slot = std::move(slot)]
                (const QVariant&) mutable
    {
        slot();
    };

    return QObject::connect(sender, signal, context, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 0, QMetaObject::Connection>::type
connectFromQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
{
    return connectFromQtVariant(sender, signal, sender, std::move(slot), Qt::DirectConnection);
}


//──────────────────────────────────────────────────────────────────────


template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 1 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 1, QMetaObject::Connection>::type
connectToQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                   const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                   Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func1> SignalType;
    typedef QtPrivate::FunctionPointer<Func2> SlotType;

    using arguments = typename SignalType::Arguments;
    // using raw_type = typename QtPrivate::List_Select<arguments, 0>::Value;
    using raw_type = typename List_Select<arguments, 0>::Value;
    using T = typename std::remove_cv<typename std::remove_reference<raw_type>::type>::type;
    auto execution =
            [=,
            receiver = const_cast<typename SlotType::Object *>(receiver)]
                (const T& value)
    {
        (receiver->*slot)(QVariant::fromValue(value));
    };

    return QObject::connect(sender, signal, receiver, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 2 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 2, QMetaObject::Connection>::type
connectToQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                   const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                   Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func1> SignalType;
    typedef QtPrivate::FunctionPointer<Func2> SlotType;

    using arguments = typename SignalType::Arguments;
    // using raw_type = typename QtPrivate::List_Select<arguments, 0>::Value;
    using raw_type = typename List_Select<arguments, 0>::Value;
    using T = typename std::remove_cv<typename std::remove_reference<raw_type>::type>::type;
    auto execution =
            [=,
            receiver = const_cast<typename SlotType::Object *>(receiver)]
                (const T& value, auto&&... args)
    {
        (receiver->*slot)(QVariant::fromValue(value), std::forward<decltype(args)>(args)...);
    };

    return QObject::connect(sender, signal, receiver, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func1>::ArgumentCount) >= 1 && int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 0, QMetaObject::Connection>::type
connectToQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                   const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                   Qt::ConnectionType type = Qt::AutoConnection)
{
    typedef QtPrivate::FunctionPointer<Func2> SlotType;

    auto execution =
            [=,
            receiver = const_cast<typename SlotType::Object *>(receiver)]
                (const QVariant&)
    {
        (receiver->*slot)();
    };

    return QObject::connect(sender, signal, receiver, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 0 &&
                        !QtPrivate::FunctionPointer<Func2>::IsPointerToMemberFunction, QMetaObject::Connection>::type
connectToQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                   const QObject *context, Func2 slot,
                   Qt::ConnectionType type = Qt::AutoConnection)
{
    auto execution =
            [=,
            slot = std::move(slot)]
                (const QVariant&) mutable
    {
        slot();
    };

    return QObject::connect(sender, signal, context, std::move(execution), type);
}

template <typename Func1, typename Func2>
typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) == 0, QMetaObject::Connection>::type
connectToQtVariant(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
{
    return connectToQtVariant(sender, signal, sender, std::move(slot), Qt::DirectConnection);
}

};


#endif // QVARIANTCONNECT_H
