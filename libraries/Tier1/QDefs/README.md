# QDefs

QDefs is a header-only grab bag of C++ macros and helpers that remove the repetitive parts of writing Qt / Qt Quick code. It focuses on making QML-friendly APIs as easy to declare in C++ as they are in QML by generating properties, enums, flags, singletons, logging helpers, and utility functions in one line.

## Highlights
- **Zero-boilerplate QML properties** – declare readable, writable, callable, constant, abstract, or fuzzy-compared properties with a single macro call.
- **Enum, flag, and singleton helpers** – expose `Q_ENUM`, `Q_FLAG`, attached properties, and QML singletons without hand-writing glue code.
- **Instrumentation macros** – quickly wrap a function call to log its execution time or print formatted log separators.
- **Utility toolbox** – QVariant navigation, math helpers, hashing shortcuts, filesystem helpers, UUID/QString utilities, etc.

## Header overview

| Header | Purpose |
| --- | --- |
| `qpropertydefs.h` | All `Q_*_PROPERTY` macros (writable/readonly/constant/abstract/callable/bindable + `VAR`, `PTR`, `REF`, `ENU`, `FUZ` specialisations). |
| `qenumdefs.h` / `qflagdefs.h` | `Q_ENUM_CLASS` and `Q_FLAG_CLASS` helpers exposing enums/flags to both C++ and QML (with runtime conversion helpers). |
| `qsingletondefs.h` | `Q_OBJECT_SINGLETON` and `Q_OBJECT_QML_SINGLETON` macros to materialise per-process or QML singletons. |
| `qattacheddefs.h` | Helpers to implement attached properties (`Q_OBJECT_ATTACHED`, `Q_OBJECT_CHILD_ATTACHED`). |
| `qoperatordefs.h` | Generate comparison operators once you wrote `operator==`/`operator<`. |
| `qtimedefs.h` / `qlogdefs.h` | Execution-time wrappers (`Q_MEASURE_TIME`, `Q_DEBUG_TIME`, `Q_TRACE_TIME`) and logging helpers (`qTrace`, `qNotice`, `qLogLine`). |
| `qvariantdefs.h` | QVariant conversion helpers (`qVariantFromJSVariant`, nested map navigation/set/remove/take helpers). |
| `qutildefs.h` | Math helpers (`qFuzzyModulo`, regression, sigma/median), QString/file/path helpers, event-loop utilities, etc. |
| `qprojectdefs.h` | Defaulted project metadata macros (`PROJECT_VERSION`, `PROJECT_NAME`, …). |

Everything ships as headers, so you can cherry-pick only the files you need.

## Installation

### As a Git submodule / source drop
1. Add the folder `libraries/Tier1/QDefs/` to your project (copy or git submodule).
2. Add the folder to your include paths and `#include` the headers you need.

### CMake
```cmake
add_subdirectory(path/to/QDefs)
target_link_libraries(MyApp PRIVATE QDefs)
```
The library only publishes headers, so linking simply makes the include path available. If you prefer not to add the target, use:
```cmake
target_include_directories(MyApp PRIVATE path/to/QDefs)
```

### qmake / Qt .pro
```qmake
include(/absolute/or/relative/path/to/QDefs/QDefs.pri)
```
This adds the headers to your include path and lets you `#include <qpropertydefs.h>` (or any other header) directly.

## Usage samples

### Declare QML-ready properties without boilerplate
```cpp
#include <qpropertydefs.h>

class Settings : public QObject
{
    Q_OBJECT
    Q_WRITABLE_VAR_PROPERTY(QString, title, Title, "Untitled");
    Q_READONLY_VAR_PROPERTY(bool, busy, Busy, false);
    Q_WRITABLE_FUZ_PROPERTY(float, value, Value, 1.6);
};
```
Macros are available for:

- Access level: `Q_WRITABLE_*`, `Q_READONLY_*`, `Q_CONSTANT_*`, `Q_ABSTRACT_*`, `Q_CALLABLE_*`, and `Q_REQUIRED_*`.
- Type helpers: `VAR` (value), `PTR`, `REF`, `FUZ` (floating point setter using `qFuzzyCompare`).
- Extras: automatic notifier signals, `reset` methods, `AboutToChange` hooks for writable variants.

Use `Q_BINDABLE_*` variants when the property must support Qt 6 bindings from C++:
```cpp
class BindableSettings : public QObject
{
    Q_OBJECT
    Q_BINDABLE_REF_PROPERTY(BindableSettings, QString, title, Title, "Untitled");
};
```
Bindable macros require the owning class as first argument and currently cover `VAR`, `REF`, and `FUZ` properties.

### Expose enums and flags to QML in one line
```cpp
#include <qenumdefs.h>
#include <qflagdefs.h>

Q_ENUM_CLASS(ColorModeNS, ColorMode,
    Normal = 0,
    Inverted,
    Night
)

Q_FLAG_CLASS(UserRoleNS, UserRoles, UserRole,
    Reader = 0x01,
    Editor = 0x02,
    Admin  = 0x04
)
```
Both macros declare the enum/flag, register it with Qt meta-object, and create a QML singleton wrapper so you can write `ColorMode.Normal` or call helper methods (`ColorMode.asString(value)`).

### Singletons and attached properties
```cpp
#include <qsingletondefs.h>

class ThemeManager final : public QObject
{
    Q_OBJECT
    Q_OBJECT_QML_SINGLETON(ThemeManager)
private:
    explicit ThemeManager(QObject *parent = nullptr) : QObject(parent) {}
};
```
Use `Q_OBJECT_ATTACHED(MyAttached, QObject)` to expose a helper object as QML attached properties. `Q_OBJECT_CHILD_ATTACHED` reuses an already-created child instance instead of allocating one per attachment.

### Timing, logging and utilities
- Wrap any call with `Q_MEASURE_TIME(blockOrFunctionCall);` to print its duration in milliseconds (`Q_DEBUG_TIME`/`Q_TRACE_TIME` use different log levels).
- Use `qLogLine("Title")` or `qLogLineMessage("Title", '=')` to print formatted separators aligned to 100 characters.
- `qVariantGetNestedValue`, `qVariantSetNestedValue`, `qVariantInsertNestedValue`, etc. walk through nested QVariant maps using `"foo.bar.baz"` keys.
- `qutildefs.h` hosts reusable math helpers (fuzzy compare/modulo, regression, sigma, median), string/path helpers, UUID formatting, event-loop waiting helpers, etc.

## Requirements
- Qt 6+ for the `QML_NAMED_ELEMENT`/`QML_SINGLETON` helpers (the macros are still usable on Qt 5 if you avoid the Qt 6–only pieces).
- C++17 (matching Qt’s default minimum in this repository).

## License

QDefs follows a very permissive "do-whatever-you-want" license (similar to WTFPL). You are free to use, modify, redistribute, embed, or close-source it without attribution. See `LICENSE` for the exact wording.

## Contributing

Issues and pull requests that add new macro families, documentation, or examples are welcome. If you have an edge case that could be generalised, open an issue describing the code you wish you did not have to write manually.
