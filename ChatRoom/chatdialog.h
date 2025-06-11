#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"

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

private:
    void ShowSearch(bool bsearch = false);

    Ui::ChatDialog *ui;

    ChatUIMode m_mode;
    ChatUIMode m_state;
    bool m_b_loading;
};

#endif // CHATDIALOG_H
