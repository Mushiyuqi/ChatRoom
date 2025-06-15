#pragma once
#include <QLabel>

class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    enum ClickState{
        Normal = 0, // 闭眼
        Selected = 1, // 睁眼
    };
public:
    ClickedLabel(QWidget *parent=nullptr);
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");
    ClickState GetCurState();
private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;

    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;

    ClickState m_curstate;

signals:
    void clicked(void);
};
