#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "chatuseritem.h"
#include "loadingdlg.h"
#include <QAction>
#include <QRandomGenerator>

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

    // 测试用例
    AddChatUserList();
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user,
            this, &ChatDialog::slot_loading_chat_user);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

std::vector<QString>  strs ={"hello world !",
                             "nice to meet u",
                             "New year，new life",
                             "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you"};

std::vector<QString> heads = {
    ":/resource/head_1.jpg",
    ":/resource/head_2.jpg",
    ":/resource/head_3.jpg",
    ":/resource/head_4.jpg",
    ":/resource/head_5.jpg"
};

std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};


void ChatDialog::AddChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();

        auto*chat_user_item = new ChatUserItem();
        chat_user_item->SetInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_item->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_item);
    }
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

void ChatDialog::slot_loading_chat_user()
{
    // 判断当前是否是正在加载数据
    if(m_b_loading){
        return;
    }

    m_b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    AddChatUserList();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    m_b_loading = false;

}
