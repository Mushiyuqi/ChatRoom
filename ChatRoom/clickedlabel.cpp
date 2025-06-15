#include "clickedlabel.h"
#include "global.h"
#include <QMouseEvent>

ClickedLabel::ClickedLabel(QWidget *parent):QLabel(parent), m_curstate(ClickState::Normal)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickedLabel::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(m_curstate == ClickState::Normal){
        setProperty("state",m_normal_hover);
        repolish(this);
        update();

    }else{
        setProperty("state",m_selected_hover);
        repolish(this);
        update();
    }

    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(m_curstate == ClickState::Normal){
        setProperty("state",m_normal);
        repolish(this);
        update();

    }else{
        setProperty("state",m_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton){
        if(m_curstate == ClickState::Normal){
            m_curstate = ClickState::Selected;
            setProperty("state", m_selected_press);
            repolish(this);
            update();
        }else{
            m_curstate = ClickState::Normal;
            setProperty("state", m_normal_press);
            repolish(this);
            update();
        }
    }
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton){
        if(m_curstate == ClickState::Normal){
            setProperty("state", m_normal_hover);
            repolish(this);
            update();
        }else{
            setProperty("state", m_selected_hover);
            repolish(this);
            update();
        }
        emit clicked();
    }
    QLabel::mouseReleaseEvent(ev);
}

void ClickedLabel::SetState(QString normal, QString hover, QString press,
                            QString select, QString select_hover, QString select_press)
{
    m_normal = normal;
    m_normal_hover = hover;
    m_normal_press = press;

    m_selected = select;
    m_selected_hover = select_hover;
    m_selected_press = select_press;

    setProperty("state", normal);
    repolish(this);
}

ClickedLabel::ClickState ClickedLabel::GetCurState()
{
    return m_curstate;
}
