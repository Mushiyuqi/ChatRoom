#pragma once
#include <QWidget>
#include "global.h"
#include <QLabel>

class StateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");

    ClickState GetCurState();
    void ClearState();

    void SetSelected(bool bselected);
    void AddRedPoint();
    void ShowRedPoint(bool show=true);

protected:
    void paintEvent(QPaintEvent* event) override;

    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

private:

    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;

    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;

    ClickState m_curstate;
    QLabel * m_red_point;

signals:
    void clicked(void);

signals:

public slots:
};


