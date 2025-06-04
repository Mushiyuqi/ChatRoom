#include "MsgNode.h"

RecvNode::RecvNode(const short msg_len, const short msg_id): MsgNode(msg_len), m_msg_id(msg_id) {
}

SendNode::SendNode(const char* msg, const short msg_len,
                   const short msg_id): MsgNode(HEAD_TOTAL_LEN + msg_len), m_msg_id(msg_id) {
    // 先发送id, 转为网络字节序
    short msg_id_net = boost::asio::detail::socket_ops::host_to_network_short(msg_id);
    memcpy(m_data, &msg_id_net, HEAD_ID_LEN);
    // 再发送数据, 转为网络字节序
    short msg_len_net = boost::asio::detail::socket_ops::host_to_network_short(msg_len);
    memcpy(m_data + HEAD_ID_LEN, &msg_len_net, HEAD_DATA_LEN);
    memcpy(m_data + HEAD_TOTAL_LEN, msg, msg_len);
}

