#include "statewidget.h"
#include <QStyleOption>
#include <QVBoxLayout>
#include <QMouseEvent>

StateWidget::StateWidget(QWidget *parent) : QWidget(parent),m_curstate(ClickState::Normal)
{
    setCursor(Qt::PointingHandCursor);
    //添加红点
    AddRedPoint();
}

void StateWidget::AddRedPoint()
{
    //添加红点示意图
    m_red_point = new QLabel();
    m_red_point->setObjectName("red_point");
    m_red_point->setPixmap(QPixmap(":/resource/red_point.png"));
    QVBoxLayout* layout2 = new QVBoxLayout;
    m_red_point->setAlignment(Qt::AlignCenter);
    layout2->addWidget(m_red_point);
    layout2->setContentsMargins(QMargins(0, 0, 0, 0));
    this->setLayout(layout2);
    m_red_point->setVisible(false);
}

void StateWidget::SetState(QString normal, QString hover, QString press,
                           QString select, QString select_hover, QString select_press)
{
    m_normal = normal;
    m_normal_hover = hover;
    m_normal_press = press;

    m_selected = select;
    m_selected_hover = select_hover;
    m_selected_press = select_press;

    setProperty("state",normal);
    repolish(this);
}

ClickState StateWidget::GetCurState(){
    return m_curstate;
}

void StateWidget::ClearState()
{
    m_curstate = ClickState::Normal;
    setProperty("state", m_normal);
    repolish(this);
    update();
}

void StateWidget::SetSelected(bool bselected)
{
    if(bselected){
        m_curstate = ClickState::Selected;
        setProperty("state", m_selected);
        repolish(this);
        update();
        return;
    }

    m_curstate = ClickState::Normal;
    setProperty("state", m_normal);
    repolish(this);
    update();
    return;
}

void StateWidget::ShowRedPoint(bool show)
{
    m_red_point->setVisible(true);
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return;
}

// 处理鼠标点击事件
void StateWidget::mousePressEvent(QMouseEvent* event)  {
    if (event->button() == Qt::LeftButton) {
        if(m_curstate == ClickState::Selected){
            qDebug()<<"PressEvent , already to selected press: "<< m_selected_press;
            //emit clicked();
            // 调用基类的mousePressEvent以保证正常的事件处理
            QWidget::mousePressEvent(event);
            return;
        }

        if(m_curstate == ClickState::Normal){
            qDebug()<<"PressEvent , change to selected press: "<< m_selected_press;
            m_curstate = ClickState::Selected;
            setProperty("state",m_selected_press);
            repolish(this);
            update();
        }

        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if(m_curstate == ClickState::Normal){
            //qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",m_normal_hover);
            repolish(this);
            update();

        }else{
            //qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",m_selected_hover);
            repolish(this);
            update();
        }
        emit clicked();
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QWidget::mousePressEvent(event);
}

void StateWidget::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(m_curstate == ClickState::Normal){
        //qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",m_normal_hover);
        repolish(this);
        update();

    }else{
        //qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",m_selected_hover);
        repolish(this);
        update();
    }

    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(m_curstate == ClickState::Normal){
        // qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",m_normal);
        repolish(this);
        update();

    }else{
        // qDebug()<<"leave , change to select normal : "<< _selected;
        setProperty("state",m_selected);
        repolish(this);
        update();
    }
    QWidget::leaveEvent(event);
}

