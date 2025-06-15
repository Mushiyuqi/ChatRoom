#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

protected:
    virtual void paintEvent(QPaintEvent *event) override;


private:
    Ui::ChatPage *ui;
};

#endif // CHATPAGE_H
