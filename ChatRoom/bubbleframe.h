#pragma once
#include <QFrame>
#include <QHBoxLayout>
#include "global.h"

class BubbleFrame:public QFrame
{
    Q_OBJECT
    const int WIDTH_SANJIAO = 8; // 聊天气泡三角形的宽度
public:
    BubbleFrame(ChatRole role, QWidget* parent = nullptr);
    void SetMargin(int margin);

    void SetWidget(QWidget* w);

protected:
    void paintEvent(QPaintEvent* e);

private:
    QHBoxLayout* m_pHLayout;
    ChatRole m_role;
    int m_margin;
};

