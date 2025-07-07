#include "chatpage.h"
#include "ui_chatpage.h"
#include "chatitembase.h"
#include "textbubble.h"
#include "picturebubble.h"
#include <QStyleOption>
#include <QPainter>
#include "usermanager.h"
#include "tcpmanager.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);

    //设置按钮样式
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");

    //设置图标样式
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");

}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    m_user_info = user_info;
    // 设置ui界面
    ui->title_lb->setText(m_user_info->m_name);
    ui->chat_data_list->RemoveAllItem();
    for(auto& msg : user_info->m_chat_msgs){
        AppendChatMsg(msg);
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto selfInfo = UserManager::GetInstance().GetUserInfo();
    ChatRole role;

    if(msg->m_from_uid == selfInfo->m_uid){
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->SetUserName(selfInfo->m_name);
        pChatItem->SetUserIcon(QPixmap(selfInfo->m_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->m_msg_content);
        pChatItem->SetWidget(pBubble);
        ui->chat_data_list->AppendChatItem(pChatItem);
    }
    else{
        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        auto friend_info = UserManager::GetInstance().GetFriendById(msg->m_from_uid);
        if (friend_info == nullptr) {
            return;
        }
        pChatItem->SetUserName(friend_info->m_name);
        pChatItem->SetUserIcon(QPixmap(friend_info->m_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->m_msg_content);
        pChatItem->SetWidget(pBubble);
        ui->chat_data_list->AppendChatItem(pChatItem);
    }
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_clicked()
{
    if(m_user_info == nullptr)
        return;

    auto user_info = UserManager::GetInstance().GetUserInfo();
    auto pTextEdit = ui->chat_edit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->m_name;
    QString userIcon = user_info->m_icon;

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txtSize = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(userName);
        pChatItem->SetUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        if(type == "text")
        {
            //生成唯一id
            QUuid uuid = QUuid::createUuid();
            //转为字符串
            QString uuidString = uuid.toString();

            pBubble = new TextBubble(role, msgList[i].content);
            if(txtSize + msgList[i].content.length() > 1024){
                textObj["fromuid"] = user_info->m_uid;
                textObj["touid"] = m_user_info->m_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txtSize = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpManager::GetInstance().sig_send_data(ReqId::ID_TEXT_CHAT_MSG, jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            txtSize += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(),
                                                          user_info->m_uid, m_user_info->m_uid);
            emit sig_append_send_chat_msg(txt_msg);

            pBubble = new TextBubble(role, msgList[i].content);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(role, QPixmap(msgList[i].content));
        }
        else if(type == "file")
        {

        }
        if(pBubble != nullptr)
        {
            pChatItem->SetWidget(pBubble);
            ui->chat_data_list->AppendChatItem(pChatItem);
        }
    }

    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->m_uid;
    textObj["touid"] = m_user_info->m_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txtSize = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpManager::GetInstance().sig_send_data(ReqId::ID_TEXT_CHAT_MSG, jsonData);
}

