#include "tcpmanager.h"

TcpManager &TcpManager::GetInstance()
{
    static TcpManager instance;
    return instance;
}

TcpManager::TcpManager() {}
