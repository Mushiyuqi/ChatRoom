#include "tcpmanager.h"
#include <QAbstractSocket>
#include <QJsonDocument>
#include <qendian.h>
#include "usermanager.h"

TcpManager::~TcpManager()
{
    qDebug() << "TcpManager::~TcpManager destructed" ;
}

TcpManager &TcpManager::GetInstance()
{
    static TcpManager instance;
    return instance;
}

TcpManager::TcpManager():m_host(""), m_b_recv_pending(false), m_message_id(0), m_message_len(0) {
    // 连接成功
    QObject::connect(&m_socket, &QTcpSocket::connected, [&](){
        emit sig_con_success(true);
    });

    // 读取数据
    QObject::connect(&m_socket, &QTcpSocket::readyRead, [&](){
        // 当有数据可读时读取所有数据
        // 读取所有数据添加到缓冲区
        m_buffer.append(m_socket.readAll());
        QDataStream stream(&m_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_0);
        forever{
            // 先解析头部
            if(!m_b_recv_pending){
                // 检查缓冲区中的数据是否足够解析出一个消息头（消息ID + 消息长度）
                if(m_buffer.size() < static_cast<int>(sizeof(quint16) * 2)){
                    return; // 数据不够，等待更多数据
                }
                // 预读取消息ID和消息长度，但不从缓冲区中移除
                stream >> m_message_id >> m_message_len;

                //将buffer 中的前四个字节移除
                m_buffer = m_buffer.mid(sizeof(quint16) * 2);

                qDebug() << "TcpManager: QTcpSocket::readyRead"
                         << ", Message ID: " << m_message_id
                         << ", Length: " << m_message_len;
            }

            //buffer剩余长读是否满足消息体长度，不满足则退出继续等待接受
            if(m_buffer.size() < m_message_len){
                m_b_recv_pending = true;
                return;
            }

            // 读取消息体
            QByteArray messageBody = m_buffer.mid(0, m_message_len);
            m_buffer = m_buffer.mid(m_message_len);
            m_b_recv_pending = false;

            qDebug() << "TcpManager: receive body msg is " << messageBody ;
            HandleMsg(ReqId(m_message_id), m_message_len, messageBody);
        }
    });

    // 错误处理
    QObject::connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
    [&](QAbstractSocket::SocketError error){
        Q_UNUSED(error)
        qDebug() << "TcpManager Error:" << m_socket.errorString();
        switch (error) {
        case QTcpSocket::ConnectionRefusedError:
            qDebug() << "Connection Refused!";
            emit sig_con_success(false);
            break;
        case QTcpSocket::RemoteHostClosedError:
            qDebug() << "Remote Host Closed Connection!";
            break;
        case QTcpSocket::HostNotFoundError:
            qDebug() << "Host Not Found!";
            emit sig_con_success(false);
            break;
        case QTcpSocket::SocketTimeoutError:
            qDebug() << "Connection Timeout!";
            emit sig_con_success(false);
            break;
        case QTcpSocket::NetworkError:
            qDebug() << "Network Error!";
            break;
        default:
            qDebug() << "Other Error!";
            break;
        }
    });

    // 处理连接断开
    QObject::connect(&m_socket, &QTcpSocket::disconnected, [&](){
        qDebug() << "TcpManager Disconnected from server.";
    });

    //连接发送信号用来发送数据
    QObject::connect(this, &TcpManager::sig_send_data, this, &TcpManager::slot_send_data);

    // 注册消息
    InitHandlers();
}

void TcpManager::InitHandlers()
{
    // 处理用户登陆服务器回包
    m_handlers.insert(ReqId::ID_CHAT_LOGIN, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len)
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "TcpManager: Failed to create QJsonDocument.";
            int error = ErrorCodes::ERR_JSON;
            emit sig_login_failed(error);
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int error = ErrorCodes::ERR_JSON;
            qDebug() << "TcpManager:Login Failed, err is Json Parse Err" << error;
            emit sig_login_failed(error);
            return;
        }

        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            qDebug() << "TcpManager: Login Failed, err is " << error;
            emit sig_login_failed(error);
            return;
        }

        // 持续化数据
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();

        auto userInfo = std::make_shared<UserInfo>(uid, name, nick, icon, sex);
        UserManager::GetInstance().SetUserInfo(userInfo);
        UserManager::GetInstance().SetToken(jsonObj["token"].toString());

        // 获取好友申请
        if(jsonObj.contains("apply_list")){
            UserManager::GetInstance().AppendApplyList(jsonObj["apply_list"].toArray());
        }

        //添加好友列表
        if (jsonObj.contains("friend_list")) {
            UserManager::GetInstance().AppendFriendList(jsonObj["friend_list"].toArray());
        }

        // 切换页面
        qDebug() << "登陆成功";
        emit sig_switch_chatdlg();
    });
    // 处理搜索用户服务器回包
    m_handlers.insert(ReqId::ID_SEARCH_USER, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            emit sig_user_search(nullptr);
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            emit sig_user_search(nullptr);
            return;
        }

        if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS) {
            emit sig_user_search(nullptr);
            return;
        }

        auto searchInfo =  std::make_shared<SearchInfo>(
            jsonObj["uid"].toInt(), jsonObj["name"].toString(),
            jsonObj["nick"].toString(), jsonObj["desc"].toString(),
            jsonObj["sex"].toInt(), jsonObj["icon"].toString());

        emit sig_user_search(searchInfo);
    });
    // 处理添加好友服务器回包
    m_handlers.insert(ReqId::ID_ADD_FRIEND, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            return;
        }

        if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS) {
            return;
        }

        qDebug() << "Add Friend Require Success " ;
    });
    // 处理通知添加好友服务器回包
    m_handlers.insert(ReqId::ID_NOTIFY_ADD_FRIEND, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            return;
        }

        if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS) {
            return;
        }

        int fromUid = jsonObj["applyuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString desc = jsonObj["desc"].toString();
        QString icon = jsonObj["icon"].toString();
        QString nick = jsonObj["nick"].toString();
        int sex = jsonObj["sex"].toInt();

        auto applyInfo = std::make_shared<AddFriendApply>(
            fromUid, name, desc, icon, nick, sex);

        emit sig_friend_apply(applyInfo);
    });
    // 处理通知认证添加好友服务器回包
    m_handlers.insert(ReqId::ID_NOTIFY_AUTH_FRIEND, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            return;
        }

        if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS) {
            return;
        }

        int fromUid = jsonObj["fromuid"].toInt();
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();
        int sex = jsonObj["sex"].toInt();

        auto authInfo = std::make_shared<AuthInfo>(
            fromUid, name, nick, icon, sex);

        emit sig_add_auth_friend(authInfo);
    });
    // 处理认证添加好友服务器回包
    m_handlers.insert(ReqId::ID_AUTH_FRIEND, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            return;
        }

        if (jsonObj["error"].toInt() != ErrorCodes::SUCCESS) {
            return;
        }

        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit sig_auth_rsp(rsp);
    });
}

void TcpManager::HandleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter = m_handlers.find(id);
    if(find_iter == m_handlers.end()){
        qDebug() << "TcpManager::HandleMsg not found id ["<< id << "] to handle";
        return;
    }

    find_iter.value()(id, len, data);
}

void TcpManager::slot_tcp_connect(ServerInfo server)
{
    m_host = server.Host;
    m_port = static_cast<uint16_t>(server.Port.toUInt());
    m_socket.connectToHost(m_host, m_port);
}

void TcpManager::slot_tcp_disconnect(){
    m_socket.disconnectFromHost();
}

void TcpManager::slot_send_data(ReqId reqId, QByteArray dataBytes)
{
    uint16_t id = reqId;

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.size());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度
    out << id << len;

    // 添加字符串数据
    block.append(dataBytes);

    // 发送数据
    m_socket.write(block);
}
