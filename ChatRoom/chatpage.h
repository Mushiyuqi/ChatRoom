#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "userdata.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> m_user_info;

private slots:
    void on_send_btn_clicked();

signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);

};

#endif // CHATPAGE_H
