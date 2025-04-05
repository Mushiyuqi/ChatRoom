#pragma once

#include <QWidget>
#include <functional>
#include <QStyle>
#include <QRegularExpression>

/**
 * @brief repolish 刷新qss
 */
extern std::function<void(QWidget*)> repolish;

