#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ChatDialog),
    m_mode(ChatUIMode::ChatMode), m_state(ChatUIMode::ChatMode),
    m_b_loading(false)
{
    ui->setupUi(this);
    // 设置add按钮样式
    ui->add_btn->SetState("normal", "hover", "press");
    ui->add_btn->setProperty("state", "normal");

    // 设置search_edit
    ui->search_edit->SetMaxLength(15);

    // 默认情况下隐藏搜索列表
    ShowSearch(false);

    // 搜索栏外观行为设置
    QAction* searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/resource/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    QAction* clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/resource/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text){
        if(!text.isEmpty()){
            clearAction->setIcon(QIcon(":/resource/close_search.png"));
        }else{
            clearAction->setIcon(QIcon(":/resource/close_transparent.png"));
        }
    });
    connect(clearAction, &QAction::triggered, [this](){
        // 清除文本
        ui->search_edit->clear();
        // 清除焦点
        ui->search_edit->clearFocus();
        // 按下清除按钮则不显示搜索列表
        ShowSearch(false);
    });

}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch){
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        m_mode = ChatUIMode::SearchMode;
    }else if(m_state == ChatUIMode::ChatMode){
        ui->con_user_list->hide();
        ui->search_list->hide();
        ui->chat_user_list->show();
        m_mode = ChatUIMode::ChatMode;
    }else if(m_state == ChatUIMode::ContactMode){
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        m_mode = ChatUIMode::ContactMode;
    }
}
