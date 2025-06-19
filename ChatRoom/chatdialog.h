#pragma once
#include <QDialog>
#include <QList>
#include "global.h"

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
    void ClearLabelState(StateWidget *lb);

protected:
    bool eventFilter(QObject *, QEvent *) override;

private:
    void AddLBGroup(StateWidget* lb);
    void ShowSearch(bool bsearch = false);
    void HandleGlobalMousePress(QMouseEvent* event);

    Ui::ChatDialog *ui;

    ChatUIMode m_mode;
    ChatUIMode m_state;
    bool m_b_loading;
    QList<StateWidget*> m_lb_list;

private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString& str);
};

