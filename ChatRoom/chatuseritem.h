#ifndef CHATUSERITEM_H
#define CHATUSERITEM_H

#include <QWidget>

namespace Ui {
class ChatUserItem;
}

class ChatUserItem : public QWidget
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();

private:
    Ui::ChatUserItem *ui;
};

#endif // CHATUSERITEM_H
