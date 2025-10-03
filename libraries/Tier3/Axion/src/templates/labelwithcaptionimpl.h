#ifndef LABELWITHCAPTIONIMPL_H
#define LABELWITHCAPTIONIMPL_H

#include <QQuickItem>

#include <QDefs>

class LabelWithCaptionImpl : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_PTR_PROPERTY(QQuickItem, labelItem, LabelItem, nullptr)
    Q_WRITABLE_PTR_PROPERTY(QQuickItem, captionItem, CaptionItem, nullptr)
    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)

    Q_WRITABLE_REF_PROPERTY(QString, text, Text, {})
    Q_WRITABLE_REF_PROPERTY(QFont, textFont, TextFont, {})
    Q_WRITABLE_REF_PROPERTY(QColor, textColor, TextColor, {})
    Q_WRITABLE_VAR_PROPERTY(bool, haveAText, HaveAText, true)
    Q_WRITABLE_REF_PROPERTY(QString, caption, Caption, {})
    Q_WRITABLE_REF_PROPERTY(QFont, captionFont, CaptionFont, {})
    Q_WRITABLE_REF_PROPERTY(QColor, captionColor, CaptionColor, {})
    Q_WRITABLE_VAR_PROPERTY(bool, haveACaption, HaveACaption, true)
    Q_WRITABLE_VAR_PROPERTY(Qt::Alignment, horizontalAlignment, HorizontalAlignment, Qt::AlignLeft)
    Q_WRITABLE_VAR_PROPERTY(Qt::Alignment, verticalAlignment, VerticalAlignment, Qt::AlignLeft)

    Q_WRITABLE_FUZ_PROPERTY(double, spacing, Spacing, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, topPadding, TopPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, leftPadding, LeftPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, rightPadding, RightPadding, 0)
    Q_WRITABLE_FUZ_PROPERTY(double, bottomPadding, BottomPadding, 0)

    Q_READONLY_FUZ_PROPERTY(double, implicitContentWidth, ImplicitContentWidth, 0)
    Q_READONLY_FUZ_PROPERTY(double, implicitContentHeight, ImplicitContentHeight, 0)

public:
    explicit LabelWithCaptionImpl(QQuickItem* parent = nullptr);

protected:
    void componentComplete() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    bool hasText() const;
    bool hasCaption() const;

    void updateImplicitContentSize();
    void layout();

    void computeTextOnlyRect(double availableWidth, double availableHeight);
    void computeCaptionOnlyRect(double availableWidth, double availableHeight);
    void computeTextCaptionRect(double availableWidth, double availableHeight);
    void computeEmpty(double availableWidth, double availableHeight);

private slots:
    void connectToLabelItem();
    void disconnectFromLabelItem();

    void connectToCaptionItem();
    void disconnectFromCaptionItem();

    void queueInvalidate();
    void invalidate();

private:
    bool m_complete = false;
    bool m_invalidateQueued = false;
};

#endif // LABELWITHCAPTIONIMPL_H
