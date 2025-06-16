#pragma once
#include <QWidget>
#include "global.h"

class ListItemBase: public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget* parent = nullptr);

    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    ListItemType m_itemType;

};

