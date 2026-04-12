# QModels

Tier-1 collection of reusable models, proxies, and helpers that sit on top of Qt's model/view stack. The goal is to expose strongly-typed C++ lists, trees, and dictionaries to QML with minimal boilerplate, plus a toolbox of proxy models for everyday UX patterns (checkable lists, selection syncing, CSV/JSON adapters, etc.).

---

## Installation

### CMake
```cmake
add_subdirectory(path/to/QModels)
target_link_libraries(MyApp PRIVATE QModels)
```

### qmake / Qt .pro
```qmake
include(/absolute/or/relative/path/to/QModels/QModels.pri)
```

### QML import
All models register under `Eco.Tier1.Models`. Import them once in QML:
```qml
import Eco.Tier1.Models 1.0
```

---

## Library map

| Area | Classes | What they solve |
| --- | --- | --- |
| **QObject-backed models** | `QObjectListModel`, `QObjectNestModel`, `QObjectTreeModel`, `QObjectListProperty` | Expose QObject collections (flat lists or trees) to QML with automatic role generation, iterators, ownership rules, and convenience callbacks. |
| **Variant-backed models** | `QVariantListModel`, `QVariantReaderModel`, `QJsonListModel`, `QCsvListModel`, `QNumberModel` | Quickly turn QVariant data (JSON arrays, CSV rows, integer ranges) into `QAbstractListModel` instances. |
| **Proxy & helper models** | `QCheckableProxyModel`, `QSelectionHelper`, `QModelHelper`, `QObjectListModelIterator`, `QModelMatcher` | Combine models, add selection/check states, or search/filter without reinventing plumbing. |
| **Standardised containers** | `QStandardObjectModel`, `QObjectNestModel`, `QVariantListModel` | Provide "batteries included" roles (`qtObject`, `modelData`, etc.) so every QML control works out of the box. |
| **Syncable extensions** | `syncable/` subfolder | Helpers to keep models in sync with backends or remote sources (see each class for details). |

The sections below highlight the most used pieces.

---

## 1. QObjectListModel - strongly typed QObject collections

Hard-forked from [QOlm](https://github.com/OlivierLDff/QOlm), `QObjectListModel` is **not** a template. Instead, you pass the `QMetaObject` of the type you want to host; the model then exposes every `Q_PROPERTY` of that meta-object as a role. Key features:

- Automatic role publishing driven by the provided `QMetaObject`; `qtObject` role always exposes the raw pointer.
- Qt ownership compliance: inserting a parentless object transfers ownership to the model; removing deletes it later.
- Rich API surface (`append/prepend/insert/remove/move`, batch operations, iterators, `indexOf`, `contains`, etc.).
- Observation hooks: override `onObjectAboutToBeInserted`, connect to `objectInserted(QObject*, index)`, or use lambda helpers (`onInserted`, `onRemoved`, `onMoved`).
- QQmlParserStatus integration to lazily complete models declared in QML.

### Quick start (C++)
```cpp
class Foo : public QObject {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    // ...
};

class FooList : public QObjectListModel {
    Q_OBJECT
public:
    FooList(QObject *parent = nullptr)
        : QObjectListModel(parent, &Foo::staticMetaObject) {}

    Q_INVOKABLE Foo *fooAt(int index) const {
        return qobject_cast<Foo*>(get(index));
    }
};

FooList list;
list.append(new Foo);          // ownership taken
list.insert(0, new Foo);
list.move(0, 1);
```

### Quick start (QML)
```qml
import Eco.Tier1.Models 1.0

ListView {
    model: FooList { 
        Foo {
            value: "hello"
        }
        Foo {
            value: "world"
        }
    }
    delegate: Text {
        property Foo foo: model.qtObject
        text: qsTr("%1 - %2").arg(index).arg(foo.value)
    }
}
```

Constructor arguments allow you to specify the meta-object and filter exported roles:
```cpp
QObjectListModel list(nullptr,
                      &Foo::staticMetaObject,
                      /* exposedRoles */ {"value"},
                      /* hiddenRoles */ {"internalRole"});
```

---

## 2. QObjectListProperty - expose child objects as QQmlListProperty

`QObjectListProperty<T>` wraps `QQmlListProperty` to manage children with the same API as `QObjectListModel` but aimed at QObject composition:
```cpp
class Foo : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<FooChild> children READ children CONSTANT)
public:
    QObjectListProperty<FooChild> m_children;
    QQmlListProperty<FooChild> children() { return m_children; }
};
```

You can still bind the property to QML repeaters, but now you get strong typing and helper methods on the C++ side.

---

## 3. Variant & data-driven models

- **`QVariantListModel`** - wrap any `QVariantList` and expose it to QML with full `QAbstractListModel` semantics.
- **`QVariantReaderModel`** - stream large JSON/CBOR arrays from a `QVariantReader` without materializing the entire dataset (see next section).
- **`QJsonListModel`** / **`QCsvListModel`** - load arrays from JSON or CSV files and surface them as roles (each column/field becomes a role).
- **`QNumberModel`** - emit an integer range without writing a custom model.

Use these when you don't want to promote your items to QObject but need a proper model instead of a raw `ListModel`.

---

## 4. Trees, nested objects, and standardised rows

- **`QObjectNestModel`** - flatten nested QObject hierarchies, giving each child its own row with parent/child relations.
- **`QObjectTreeModel`** - expose a QObject tree as `QAbstractItemModel` (suitable for `TreeView`). Roles mirror the QObject properties.
- **`QStandardObjectModel`** - convenience model that internally stores lightweight "standard items" with arbitrary roles; handy for quick prototypes.

---

## 5. Proxy models and helpers

| Class | Purpose |
| --- | --- |
| `QCheckableProxyModel` | Adds a `checked` role to any source model and keeps it in sync with `Qt::CheckStateRole`. |
| `QSelectionHelper` | Bridges `QItemSelectionModel` with QML (mirror current row(s), ask for selection changes, etc.). |
| `QModelHelper` | Attached object that gives any model live helpers: `count`, `isEmpty`, `map(row)`, `setProperty(row, role, value)`, `indexOf`, `contains`, `roleForName`, `roleName`. |
| `QModelMatcher` | Utility to search models or map QModelIndex roles programmatically. |
| `QObjectListModelIterator` | Range-based iterator for QObject lists (used internally; you can also leverage it in custom loops). |

Example: using `QModelHelper` in QML to access row data without a delegate:
```qml
ComboBox {
    model: vehiclesModel
    readonly property QtObject currentData: vehiclesModel.ModelHelper.map(currentIndex)
    contentItem: RowLayout {
        Image { source: currentData.imagePath }
        Label { text: currentData.name }
    }
}
```

---

## 6. QVariantReaderModel - streaming JSON/CBOR into QML

`QVariantReaderModel` bridges the [QJsonVariant](https://github.com/Romain-Donze/QJsonVariant) readers (`QJsonVariantReader`, `QCborVariantReader`) with the `QAbstractItemModel` API. Feed it raw JSON or CBOR via `setJson()` / `setCbor()` and it creates a **read-only, streaming model** where each row comes from the array being parsed.

### Key features

- Works with any `QVariantReader` implementation (JSON or CBOR).
- Optional **forward-only** mode (`forwardOnly: true`) holds only the current element in memory—ideal for huge arrays.
- Auto-generated roles: the first object's keys (or a placeholder) define roles; there's always a `qtVariant` role to access the raw QVariant.
- `estimateCount: true` extrapolates row counts when the reader cannot provide a length up front.
- Exposes parser status (`code`, `error`, `message`) and emits `error()` if the underlying reader fails.

### Example
```qml
import Eco.Tier1.Models 1.0

VariantReaderModel {
    id: streamModel
    forwardOnly: true
    placeholder: { "id": 0, "label": "" }

    Component.onCompleted: {
        streamModel.setJson(loadHugeJsonFromDisk());
    }
}

ListView {
    model: streamModel
    delegate: Text {
        text: `${model.id}: ${model.label}`
    }
}
```

When you call `setJson()` or `setCbor()`, the model enters the top-level array (reader must point to a list) and lazily reads elements as the view requests them. `fetchMore()` pulls the next item, and in forward-only mode the cache size stays at 1.

---

## 8. License

Same very-permissive "do-whatever-you-want" license as the rest of the Tier-1 libraries (close to WTFPL). See `LICENSE` for the friendly wording.
