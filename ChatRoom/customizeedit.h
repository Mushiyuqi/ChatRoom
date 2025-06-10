#pragma once
#include <QLineEdit>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent = nullptr);
    void SetMaxLength(int maxLen);

protected:
    void focusOutEvent(QFocusEvent *) override;

private:
    void LimitTextLength(QString text);


    int m_max_len;
signals:
    void sig_focus_out();
};
