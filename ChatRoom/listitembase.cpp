#include "listitembase.h"

ListItemBase::ListItemBase(QWidget *parent): QWidget(parent)
{

}

void ListItemBase::SetItemType(ListItemType itemType)
{
    m_itemType = itemType;
}

ListItemType ListItemBase::GetItemType()
{
    return m_itemType;
}


