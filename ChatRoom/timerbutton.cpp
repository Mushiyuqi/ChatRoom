#include "timerbutton.h"
#include <QMouseEvent>

TimerButton::TimerButton(QWidget *parent):QPushButton(parent), m_counter(defult_time) {
    m_timer = new QTimer(this);
    // 连接信号与槽
    connect(m_timer, &QTimer::timeout, [this](){
        m_counter--;
        if(m_counter <= 0){
            // 刷新定时器
            UpDataTimer();
            return;
        }
        this->setText(QString::number(m_counter));
    });
}

TimerButton::~TimerButton()
{
    // 确保timer停止
    m_timer->stop();
}

void TimerButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton){
        // 处理鼠标左键释放事件
        this->setEnabled(false);
        this->setText(QString::number(m_counter));
        // 1秒更新一次
        m_timer->start(1000);
        emit clicked();
    }
    // 调用基类事件确保正确循环
    QPushButton::mouseReleaseEvent(e);
}

void TimerButton::UpDataTimer()
{
    // 关闭定时器
    m_timer->stop();
    // 重置时间
    m_counter = defult_time;
    // 重置按钮
    this->setText("获取验证码");
    this->setEnabled(true);
}
