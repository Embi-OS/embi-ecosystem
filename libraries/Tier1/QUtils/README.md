# QUtils

QUtils is a collection of production-tested helpers that fill the gaps between Qt C++ and QML projects. It bundles reusable objects (file operations, signal throttling, JSON/variant mappers, event injectors, translators, watchdogs, etc.) so that applications share the same building blocks instead of rewriting plumbing in every product.

## Highlights
- **Ready-to-bind QML items** – most helpers expose `QML_NAMED_ELEMENT(...)`, meaning you can drop them directly in QML without wrappers.
- **Data mappers** – persist QVariant maps to QSettings, JSON, QObject properties, or `QAbstractItemModel` rows using identical APIs (`QVariantMapper`, `QJsonMapper`, `QObjectMapper`, `QModelMapper`, `QSettingsMapper`).
- **Runtime tooling** – utilities for copying/moving files asynchronously, controlling application fonts/cursors, capturing items, loading QML dynamically, measuring FPS, and watching UI responsiveness.
- **Input/event helpers** – key, mouse, and scan event notifiers expose high-level signals without manually wiring event filters.
- **Throttling/debouncing** – signal/object throttlers (based on KDAB’s implementations) are pre-registered for QML and available for template use in C++.

## Component overview

| Category | Components | Summary |
| --- | --- | --- |
| Application helpers | `QApplicationHelper`, `QTranslatorLoader`, `QWatchdog`, `QFrameTimer` | Control icons/cursors/fonts from QML, hot-reload translations, ensure UI heartbeat, and monitor FPS/jitter. |
| File & process tools | `QFileCopier`, `QLoopProcessor`, `QItemCapturer`, `QNetworkPing`, `QUtils::Filesystem`, `QUtils::Process` | Async copy/move/remove with progress, iterate expensive loops without freezing UI, capture `QQuickItem` snapshots, ping hosts, and wrap filesystem/process helpers. |
| Event notifiers | `QKeyEventNotifier`, `QMouseEventNotifier`, `QScanEventNotifier`, `QWatchdog` | Emit typed signals for input events or UI stalls without hand-written `eventFilter`s. |
| Mapping & persistence | `QVariantMapper` (base), `QObjectMapper`, `QModelMapper`, `QJsonMapper`, `QSettingsMapper`, `QVariantProperty`, `QVariantMapperPolicies` | Bidirectional sync between QVariant maps and objects/models/files with configurable select/submit policies, throttling, and dirty tracking. |
| JSON & QVariant utilities | `QJsonUtils`, `QJsonMapper`, `QVariantConnect`, `QJsonMapper::JsonFormat` | Convert QVariant↔JSON/CBOR, stream large payloads, and connect Qt signals carrying `QVariant` to strongly typed slots automatically. |
| QML loaders & singletons | `QQmlLoader`, `QObjectHelper`, `QSingleton`/`QQmlSingleton`, `QSettingsMapper` | Dynamically load QML components, manipulate QObject properties via attached helper, and instantiate thread-local or QML singletons. |
| Throttlers & timers | `QGenericSignalThrottler` (leading/trailing & debouncer variants), `QThrottler`, `QUtils::Throttle/Debounce`, `QSignalThrottler` | Debounce/throttle noisy signals or lambdas from C++ or QML with a consistent API. |

All headers/sources are located under `libraries/Tier1/QUtils/` and can be included à la carte.

## Installation

### CMake
```cmake
add_subdirectory(path/to/QUtils)
target_link_libraries(MyApp PRIVATE QUtils)
```
QUtils is mostly headers + QObject implementations. Linking the target ensures include paths and Qt resources are available. For header-only consumption:
```cmake
target_include_directories(MyApp PRIVATE path/to/QUtils)
```

### qmake / Qt .pro
```qmake
include(/absolute/or/relative/path/to/QUtils/QUtils.pri)
```

### Using as a source drop
Copy the `QUtils` folder into your repository and add it to your include path (or use `add_subdirectory`). The library depends only on Qt (Qt Quick modules for the QML helpers).

## Usage snippets

### 1. Persist view models with `QVariantMapper`
```cpp
#include <qsettingsmapper.h>

auto mapper = new QSettingsMapper("settings.json", "userProfile", parent);
mapper->setRoles({"name", "email", "language"});
mapper->select();                 // load from disk
mapper->setValue("language", "fr_CH");
if (mapper->isDirty()) {
    mapper->submit();             // writes only dirty keys back to QSettings
}
```
Switching to JSON, QObject, or `QAbstractItemModel` storage only requires swapping the mapper subclass – the API stays identical.

### 2. Drive long-running tasks without freezing the UI
```cpp
#include <qloopprocessor.h>

auto loop = new QLoopProcessor;
loop->onStart([] { qInfo() << "Preparing..."; return true; })
    ->onIterate([i = 0]() mutable {
        heavyWorkForRow(i++);
        return i < 1000;          // false stops the loop
    })
    ->onRelease([] { qApp->processEvents(); })
    ->onEnd([] { qInfo() << "All done"; });

loop->run();                      // runs asynchronously by default
```
`QFileCopier`, `QNetworkPing`, `QWatchdog`, and `QFrameTimer` follow the same event-driven design so they integrate with QML effortlessly.

### 3. Throttle bursty signals from QML
```qml
import QUtils 1.0

SignalTrailingThrottler {
    id: searchDelay
    timeout: 300
    onTriggered: searchModel.refresh()
}

TextField {
    onTextChanged: searchDelay.throttle()
}
```
Leading/trailing throttlers and debouncers are available. From C++ you can use `QUtils::Debounce([](auto v){ ... }, 250)` to wrap any callable.

### 4. Load QML dynamically with type safety
```cpp
#include <qqmlloader.h>

auto dialog = QQmlLoader::load("qrc:/ui/SettingsDialog.qml",
                               {{"initialTab", "network"}});
dialog->setParent(this);
```
Templated helpers (`QQmlLoader::load<MyType>(...)`) cast the result and warn through `QUTILSLOG_*` macros if types do not match.

## Requirements
- Qt 6.x for the provided QML registration macros (`QML_NAMED_ELEMENT`, `QML_SINGLETON`, etc.). Many components also work on Qt 5 if you adjust the registration macros manually.
- C++17 (matching the rest of the ecosystem).

## License

Like the rest of Tier1 libraries, QUtils uses an extremely permissive, “do-whatever-you-want” license (close to WTFPL). Embed it, modify it, redistribute it, or keep forks private as you see fit. See `LICENSE` for the exact wording.
