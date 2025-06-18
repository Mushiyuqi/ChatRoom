#pragma once
#include <QFrame>
#include <QHBoxLayout>
#include "global.h"

const int WIDTH_SANJIAO = 8; // 聊天气泡三角形的宽度

class BubbleFrame:public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget* parent = nullptr);
    void SetMargin(int margin);

    void SetWidget(QWidget* w);

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    QHBoxLayout* m_pHLayout;
    ChatRole m_role;
    int m_margin;
};

