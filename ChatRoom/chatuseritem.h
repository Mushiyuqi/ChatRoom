#ifndef CHATUSERITEM_H
#define CHATUSERITEM_H

#include <QWidget>
#include "listitembase.h"

namespace Ui {
class ChatUserItem;
}

class ChatUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();

    QSize sizeHint() const override;

    void SetInfo(QString name, QString head, QString msg);

private:
    Ui::ChatUserItem *ui;

    QString m_name;
    QString m_head;
    QString m_msg;
};

#endif // CHATUSERITEM_H
