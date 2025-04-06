#include "global.h"

std::function<void(QWidget*)> repolish = [](QWidget* w){
    // 去掉原来的样式
    w->style()->unpolish(w);
    // 更新样式
    w->style()->polish(w);
};
