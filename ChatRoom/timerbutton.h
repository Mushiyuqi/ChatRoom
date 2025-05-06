#pragma once
#include <QPushButton>
#include <QTimer>

class TimerButton:public QPushButton
{
public:
    explicit TimerButton(QWidget *parent = nullptr);
    ~TimerButton();
    void mouseReleaseEvent(QMouseEvent *e) override;
    void UpDataTimer();
private:
    QTimer* m_timer;
    int m_counter;
    // 默认时长
    static constexpr int defult_time = 60;
};

