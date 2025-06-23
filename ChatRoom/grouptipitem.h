#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H

#include <QWidget>

namespace Ui {
class GroupTipItem;
}

class GroupTipItem : public QWidget
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();

private:
    Ui::GroupTipItem *ui;
};

#endif // GROUPTIPITEM_H
