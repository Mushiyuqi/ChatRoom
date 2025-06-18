#pragma once
#include <QTextEdit>
#include <QHBoxLayout>
#include "bubbleframe.h"

class TextBubble:public BubbleFrame
{
    Q_OBJECT
    // 增大最大宽度，防止出现单一字符在第二排输出
    const int width_buffer = 10;
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
