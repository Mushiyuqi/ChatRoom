#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "global.h"

class BubbleFrame;

class ChatItemBase:public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget* parent = nullptr);
    void SetUserName(const QString& name);
    void SetUserIcon(const QPixmap& icon);
    void SetWidget(QWidget* bubble);

private:
    ChatRole m_role;
    QLabel* m_pNameLabel;
    QLabel* m_pIconLabel;
    QWidget* m_pBubble;

};

