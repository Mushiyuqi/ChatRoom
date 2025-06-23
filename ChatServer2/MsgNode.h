#pragma once
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "const.h"

using boost::asio::ip::tcp;

class MsgNode {
    friend class CSession;

public:
    explicit MsgNode(const short max_len): m_total_len(max_len), m_cur_len(0) {
        m_data = new char[m_total_len + 1];
        m_data[m_total_len] = '\0';
    }

    ~MsgNode() {
        delete[] m_data;
    }

    void Clear() {
        ::memset(m_data, 0, m_total_len);
        m_cur_len = 0;
    }

    short m_total_len;
    short m_cur_len;
    char* m_data;
};

class RecvNode : public MsgNode {
public:
    explicit RecvNode(const short msg_len, const short msg_id);
    short m_msg_id;
};

class SendNode : public MsgNode {
public:
    explicit SendNode(const char* msg, const short msg_len, const short msg_id);
    short m_msg_id;
};
