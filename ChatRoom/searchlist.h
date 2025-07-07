#pragma once
#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <QDialog>
#include <memory>
#include "userdata.h"
#include "loadingdlg.h"


class SearchList: public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget *parent = nullptr);
    void CloseFindDlg();
    void SetSearchEdit(QWidget* edit);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void WaitPending(bool pending = true);
    void AddTipItem();

    bool m_send_pending;
    std::shared_ptr<QDialog> m_find_dlg;
    QWidget* m_search_edit;
    LoadingDlg * m_loadingDialog;

private slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_user_search(std::shared_ptr<SearchInfo> si);

signals:
    void sig_jump_chat_item(std::shared_ptr<SearchInfo> si);
};

