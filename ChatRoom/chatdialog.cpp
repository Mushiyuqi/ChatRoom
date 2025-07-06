#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "chatuseritem.h"
#include "loadingdlg.h"
#include <QAction>
#include <QRandomGenerator>
#include <QMouseEvent>
#include "tcpmanager.h"
#include "usermanager.h"

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
    connect(ui->search_edit, &QLineEdit::textChanged, [this, clearAction](const QString &text){
        if(!text.isEmpty()){
            clearAction->setIcon(QIcon(":/resource/close_search.png"));
            // 按下清除按钮则显示搜索列表
            ShowSearch(true);
        }else{
            clearAction->setIcon(QIcon(":/resource/close_transparent.png"));
            // 按下清除按钮则不显示搜索列表
            ShowSearch(false);
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

    // 侧边栏添加头像
    QPixmap pixmap(":/resource/head_0.png"); // 加载图片
    ui->side_head_lb->setPixmap(pixmap); // 将图片设置到QLabel上
    QPixmap scaledPixmap = pixmap.scaled( ui->side_head_lb->size(), Qt::KeepAspectRatio); // 将图片缩放到label的大小
    ui->side_head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->side_head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小

    // 设置侧边栏行为
    ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
    ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");

    // 将侧边栏按钮添加到按钮组中
    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);

    // 注册侧边栏事件处理
    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    //链接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    //检测鼠标点击位置判断是否要清空搜索框
    this->installEventFilter(this); // 安装事件过滤器

    // 设置聊天按钮为选中状态
    ui->side_chat_lb->SetSelected(true);

    //为searchlist 设置search edit
    ui->search_list->SetSearchEdit(ui->search_edit);

    // 连接加载联系人的信号和槽函数
    connect(&TcpManager::GetInstance(), &TcpManager::sig_friend_apply, this, &ChatDialog::slot_apply_friend);

    //连接认证添加好友信号
    connect(&TcpManager::GetInstance(), &TcpManager::sig_add_auth_friend, this, &ChatDialog::slot_add_auth_friend);

    //链接自己认证回复信号
    connect(&TcpManager::GetInstance(), &TcpManager::sig_auth_rsp, this, &ChatDialog::slot_auth_rsp);

}

void ChatDialog::AddLBGroup(StateWidget* lb)
{
    m_lb_list.push_back(lb);
}



ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::AddChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();

        auto*chat_user_item = new ChatUserItem();
        auto user_info = std::make_shared<UserInfo>(0, names[name_i], names[name_i], heads[head_i], 0, strs[str_i]);
        chat_user_item->SetInfo(user_info);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_item->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_item);
    }
}

void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for(auto & ele: m_lb_list){
        if(ele == lb){
            continue;
        }

        ele->ClearState();
    }
}

bool ChatDialog::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        HandleGlobalMousePress(mouseEvent);
    }

    return QDialog::eventFilter(watched, event);
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

void ChatDialog::HandleGlobalMousePress(QMouseEvent *event)
{
    // 实现点击位置的判断和处理逻辑
    // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
    if( m_mode != ChatUIMode::SearchMode){
        return;
    }

    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPos());
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->search_list->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_edit->clear();
        // search_edit会监听text的变化，从而不显示搜索列表
        // ShowSearch(false);
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

void ChatDialog::slot_side_chat()
{
    qDebug()<< "receive side chat clicked";
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    m_state = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact(){
    qDebug()<< "receive side contact clicked";
    ClearLabelState(ui->side_contact_lb);
    //设置
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    m_state = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    if (!str.isEmpty()) {
        ShowSearch(true);
    }
}

void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply)
{
    qDebug() << "ChatDialog::slot_apply_friend, from_uid is " << apply->m_from_uid;

    bool already = UserManager::GetInstance().AlreadyApply(apply->m_from_uid);
    if(already){
        // 已经有申请了退出
        return;
    }

    // 添加好友申请到列表与用户管理
    UserManager::GetInstance().AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_user_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info)
{
    auto isFriend = UserManager::GetInstance().CheckFriendById(auth_info->m_uid);
    if(isFriend) return;

    UserManager::GetInstance().AddFriend(auth_info);

    int randomValue = QRandomGenerator::global()->bounded(100);
    int str_i = randomValue%strs.size();
    int head_i = randomValue%heads.size();

    auto* chat_user_wid = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    m_chat_items_added.insert(auth_info->m_uid, item);
}

void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto isFriend = UserManager::GetInstance().CheckFriendById(auth_rsp->m_uid);
    if(isFriend) return;

    UserManager::GetInstance().AddFriend(auth_rsp);

    int randomValue = QRandomGenerator::global()->bounded(100);
    int str_i = randomValue%strs.size();
    int head_i = randomValue%heads.size();

    auto* chat_user_wid = new ChatUserItem();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    m_chat_items_added.insert(auth_rsp->m_uid, item);
}

