#pragma once
#include <QLabel>
#include <QMouseEvent>

class ClickedOnceLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedOnceLabel(QWidget* parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent *ev) override;

signals:
    void clicked(QString);
};

