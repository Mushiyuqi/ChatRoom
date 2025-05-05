#include "global.h"

QString gate_url_prefix = "";

std::function<void(QWidget*)> repolish = [](QWidget* w){
    // 去掉原来的样式
    w->style()->unpolish(w);
    // 更新样式
    w->style()->polish(w);
};

// SHA-256加密
std::function<QString(QString)> Sha256Hash = [](const QString input){
    CryptoPP::SHA256 hash;
    std::string result;
    CryptoPP::StringSource ss(input.toStdString(), true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(result)
                )
            )
        );
    return QString::fromStdString(result);
};
