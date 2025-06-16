#pragma once
#include <QHBoxLayout>
#include <QPixmap>
#include "bubbleframe.h"

#define PIC_MAX_WIDTH 100
#define PIC_MAX_HEIGHT 90

class PictureBubble: BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(ChatRole role, const QPixmap& picture, QWidget *parent = nullptr);
};
