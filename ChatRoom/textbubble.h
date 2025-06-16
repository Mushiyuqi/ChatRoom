#pragma once
#include <QTextEdit>
#include <QHBoxLayout>
#include "bubbleframe.h"

class TextBubble: BubbleFrame
{
public:
    TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject* watched, QEvent* e);

private:
    void AdjustTextHeight();
    void SetPlainText(const QString& text);
    void InitStyleSheet();

private:
    QTextEdit* m_pTextEdit;

};
