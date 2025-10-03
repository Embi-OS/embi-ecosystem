#ifndef AXION_HELPERTYPES_H
#define AXION_HELPERTYPES_H

#include <QDefs>
#include <QUtils>
#include <QModels>
#include <QQuickWindow>

class TextTheme : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, primaryFont, PrimaryFont, "Roboto")

    Q_PROPERTY(QFont headline1 MEMBER headline1 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline2 MEMBER headline2 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline3 MEMBER headline3 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline4 MEMBER headline4 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline5 MEMBER headline5 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline6 MEMBER headline6 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline7 MEMBER headline7 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont headline8 MEMBER headline8 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont title1 MEMBER title1 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont title2 MEMBER title2 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont subtitle1 MEMBER subtitle1 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont subtitle2 MEMBER subtitle2 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont body1 MEMBER body1 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont body2 MEMBER body2 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont button MEMBER button NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont capital MEMBER capital NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont caption1 MEMBER caption1 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont caption2 MEMBER caption2 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont overline MEMBER overline NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont hint1 MEMBER hint1 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont hint2 MEMBER hint2 NOTIFY familyChanged FINAL)
    Q_PROPERTY(QFont code MEMBER code CONSTANT FINAL)

public:
    explicit TextTheme(QObject* parent = nullptr);

    QFont headline1;
    QFont headline2;
    QFont headline3;
    QFont headline4;
    QFont headline5;
    QFont headline6;
    QFont headline7;
    QFont headline8;
    QFont title1;
    QFont title2;
    QFont subtitle1;
    QFont subtitle2;
    QFont body1;
    QFont body2;
    QFont button;
    QFont capital;
    QFont caption1;
    QFont caption2;
    QFont overline;
    QFont hint1;
    QFont hint2;
    QFont code;

signals:
    void familyChanged();
};

Q_ENUM_CLASS(ItemPositions, ItemPosition,
             None,
             TopLeft,
             TopStart,
             Top,
             TopEnd,
             TopRight,
             RightStart,
             Right,
             RightEnd,
             BottomRight,
             BottomEnd,
             Bottom,
             BottomStart,
             BottomLeft,
             LeftEnd,
             Left,
             LeftStart,
             Center)

Q_ENUM_CLASS(ImageFillModes, ImageFillMode,
             Stretch,
             PreserveAspectFit,
             PreserveAspectCrop,
             Tile,
             TileVertically,
             TileHorizontally,
             Pad,)

class Theme : public QObject,
              public QQmlSingleton<Theme>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_WRITABLE_VAR_PROPERTY(bool, flat, Flat, false)
    Q_WRITABLE_VAR_PROPERTY(bool, fine, Fine, false)
    Q_WRITABLE_VAR_PROPERTY(bool, sharp, Sharp, false)
    Q_WRITABLE_VAR_PROPERTY(bool, dense, Dense, false)

    Q_WRITABLE_REF_PROPERTY(QString, label, Label, "")
    Q_WRITABLE_REF_PROPERTY(QString, icon, Icon, "qrc:/images/logo.svg")
    Q_WRITABLE_REF_PROPERTY(QString, logo, Logo, "qrc:/images/logo.svg")
    Q_WRITABLE_REF_PROPERTY(QString, splash, Splash, "qrc:/images/logo.svg")
    Q_WRITABLE_REF_PROPERTY(QString, backgroundImage, BackgroundImage, "")
    Q_WRITABLE_VAR_PROPERTY(int, backgroundAspect, BackgroundAspect, ImageFillModes::PreserveAspectCrop)

    Q_WRITABLE_PTR_PROPERTY(TextTheme, textTheme, TextTheme, nullptr)

    Q_WRITABLE_REF_PROPERTY(QColor, colorAccent,            ColorAccent,            0xBC1142)
    Q_WRITABLE_REF_PROPERTY(QColor, colorVariant,           ColorVariant,           0x00B1FF)
    Q_WRITABLE_REF_PROPERTY(QColor, colorOption,            ColorOption,            0xFF55FF)

    Q_WRITABLE_REF_PROPERTY(QColor, colorBlack,             ColorBlack,             Qt::black)
    Q_WRITABLE_REF_PROPERTY(QColor, colorWhite,             ColorWhite,             Qt::white)
    Q_WRITABLE_REF_PROPERTY(QColor, colorBackground,        ColorBackground,        0x282B2C)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimaryDarkest,    colorPrimaryDarkest,    0x181B1C)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimaryDarker,     ColorPrimaryDarker,     0x181B1C)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimaryDark,       ColorPrimaryDark,       0x2E3132)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimary,           ColorPrimary,           0x36393B)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimaryLight,      ColorPrimaryLight,      0x44474A)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimaryLighter,    ColorPrimaryLighter,    0x44474A)
    Q_WRITABLE_REF_PROPERTY(QColor, colorPrimaryLightest,   ColorPrimaryLightest,   0x44474A)

    Q_WRITABLE_REF_PROPERTY(QColor, colorTrace,             ColorTrace,             0x424242)
    Q_WRITABLE_REF_PROPERTY(QColor, colorDebug,             ColorDebug,             0x90A4AE)
    Q_WRITABLE_REF_PROPERTY(QColor, colorInfo,              ColorInfo,              0x2196F3)
    Q_WRITABLE_REF_PROPERTY(QColor, colorWarning,           ColorWarning,           0xFFEB3B)
    Q_WRITABLE_REF_PROPERTY(QColor, colorError,             ColorError,             0xFF9800)
    Q_WRITABLE_REF_PROPERTY(QColor, colorFatal,             ColorFatal,             0xB71C1C)
    Q_WRITABLE_REF_PROPERTY(QColor, colorSuccess,           ColorSuccess,           0x4CAF50)

    Q_WRITABLE_REF_PROPERTY(QColor, black,                  Black,                  Qt::black)
    Q_WRITABLE_REF_PROPERTY(QColor, white,                  White,                  Qt::white)
    Q_WRITABLE_REF_PROPERTY(QColor, blueGrey,               BlueGrey,               0x607D8B)
    Q_WRITABLE_REF_PROPERTY(QColor, grey,                   Grey,                   0xAAAAAA)
    Q_WRITABLE_REF_PROPERTY(QColor, brown,                  Brown,                  0x795548)
    Q_WRITABLE_REF_PROPERTY(QColor, deepOrange,             DeepOrange,             0xFF5722)
    Q_WRITABLE_REF_PROPERTY(QColor, orange,                 Orange,                 0xFF8F00)
    Q_WRITABLE_REF_PROPERTY(QColor, amber,                  Amber,                  0xFFC107)
    Q_WRITABLE_REF_PROPERTY(QColor, yellow,                 Yellow,                 0xF3FF00)
    Q_WRITABLE_REF_PROPERTY(QColor, lime,                   Lime,                   0x8BC34A)
    Q_WRITABLE_REF_PROPERTY(QColor, green,                  Green,                  0x77FF00)
    Q_WRITABLE_REF_PROPERTY(QColor, teal,                   Teal,                   0x009688)
    Q_WRITABLE_REF_PROPERTY(QColor, cyan,                   Cyan,                   0x00BCD4)
    Q_WRITABLE_REF_PROPERTY(QColor, lightBlue,              LightBlue,              0x03A9F4)
    Q_WRITABLE_REF_PROPERTY(QColor, blue,                   Blue,                   0x00DDFF)
    Q_WRITABLE_REF_PROPERTY(QColor, indigo,                 Indigo,                 0x3F51B5)
    Q_WRITABLE_REF_PROPERTY(QColor, deepPurple,             DeepPurple,             0x673AB7)
    Q_WRITABLE_REF_PROPERTY(QColor, purple,                 Purple,                 0x9C27B0)
    Q_WRITABLE_REF_PROPERTY(QColor, pink,                   Pink,                   0xE91E63)
    Q_WRITABLE_REF_PROPERTY(QColor, red,                    Red,                    0xFF0020)
    Q_WRITABLE_REF_PROPERTY(QColor, oxfordBlue,             OxfordBlue,             0x09102B)

protected:
    friend QQmlSingleton<Theme>;
    explicit Theme(QObject* parent = nullptr) :
        QObject(parent),
        m_textTheme(new TextTheme(this))
    {}
};

class ThemeAttached : public QObject
{
    Q_OBJECT
    QML_ATTACHED(Theme)
    QML_UNCREATABLE("Attached")
    QML_ELEMENT
public:
    static Theme* qmlAttachedProperties(QObject* object)
    {
        Q_UNUSED(object)
        return Theme::Get();
    }
};

class IconObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_WRITABLE_REF_PROPERTY(QString, name, Name, {})
    Q_WRITABLE_REF_PROPERTY(QString, source, Source, {})
    Q_WRITABLE_VAR_PROPERTY(double, width, Width, 24)
    Q_WRITABLE_VAR_PROPERTY(double, height, Height, 24)
    Q_WRITABLE_VAR_PROPERTY(double, rotation, Rotation, 0)
    Q_WRITABLE_VAR_PROPERTY(Qt::AspectRatioMode, aspectRatio, AspectRatio, Qt::IgnoreAspectRatio)
    Q_WRITABLE_REF_PROPERTY(QColor, color, Color, {})
    Q_WRITABLE_VAR_PROPERTY(bool, cache, Cache, true)

public:
    explicit IconObject(QObject* parent = nullptr) : QObject(parent) {};

    bool isEmpty() const{
        return m_name.isEmpty() && m_source.isEmpty();
    }
};

Q_ENUM_CLASS(InfoObjectTypes, InfoObjectType,
             Normal,
             Warning,
             Failure,
             Error,
             Success,)

Q_ENUM_CLASS(StepObjectStates, StepObjectState,
             NotDone,
             Current,
             Done,
             Failed,
             Skipped)

class AxionHelper : public QObject,
                    public QQmlSingleton<AxionHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_REF_PROPERTY(QVariantMap, arguments, Arguments, {})

    Q_READONLY_VAR_PROPERTY(bool, pendingRestart, PendingRestart, false)
    Q_READONLY_VAR_PROPERTY(bool, pendingReboot, PendingReboot, false)

protected:
    friend QQmlSingleton<AxionHelper>;
    explicit AxionHelper(QObject *parent = nullptr);

public:
    static void parseArguments();

    static void registerComponents(QQmlEngine& engine);
    static void registerImageProvider(QQmlEngine& engine);
    static void registerFontsPath(const QString& path=":/fonts");

    static QMetaObject::Connection onRestartAccepted(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);
    static QMetaObject::Connection onRebootAccepted(std::function<void()> callback, const Qt::ConnectionType &connection = Qt::AutoConnection);

public slots:
    static void warningWrongPage(const QString& page, int index=0);
    static void showAbout();
    static void showAboutQt();

    static void warningRestart(const QString& title="", const QString& details="");
    static void warningReboot(const QString& title="", const QString& details="");

    static void criticalRestart(const QString& message="", const QString& traces="");
    static void criticalReboot(const QString& message="", const QString& traces="");

    static bool markForRestart();
    static bool markForReboot();

signals:
    void restartAccepted();
    void rebootAccepted();
};

#endif // AXION_HELPERTYPES_H
