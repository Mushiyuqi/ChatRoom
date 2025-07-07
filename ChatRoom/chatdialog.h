#pragma once
#include <QDialog>
#include <QList>
#include "global.h"
#include "userdata.h"

class QListWidgetItem;
class AddFriendApply;

class StateWidget;
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

    void AddChatUserList();
    void AddContactUserList();
    void ClearLabelState(StateWidget *lb);

protected:
    bool eventFilter(QObject *, QEvent *) override;

private:
    void AddLBGroup(StateWidget* lb);
    void ShowSearch(bool bsearch = false);
    void HandleGlobalMousePress(QMouseEvent* event);
    void SetSelectChatItem(int uid);
    void SetSelectChatPage(int uid);

    Ui::ChatDialog *ui;

    ChatUIMode m_mode;
    ChatUIMode m_state;
    bool m_b_loading;
    QList<StateWidget*> m_lb_list;
    QMap<int, QListWidgetItem*> m_chat_items_added;
    int m_cur_chat_uid;

private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString& str);
    void slot_apply_friend(std::shared_ptr<AddFriendApply>);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> ui);
    void slot_loading_contact_user();

    void slot_item_clicked(QListWidgetItem *item);
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);
};

