#include "chatview.h"
#include <QScrollBar>
#include <QTimer>
#include <QEvent>
#include <QStyleOption>
#include <QPainter>

ChatView::ChatView(QWidget* parent): QWidget(parent), m_isAppended(false){
    QVBoxLayout* pMainlayout = new QVBoxLayout();
    this->setLayout(pMainlayout);
    pMainlayout->setContentsMargins(QMargins(0, 0, 0, 0));

    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    pMainlayout->addWidget(m_pScrollArea);

    QWidget* w = new QWidget(this);
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);

    QVBoxLayout* pHLayout_1 = new QVBoxLayout();
    pHLayout_1->addWidget(new QWidget(), 100000);
    w->setLayout(pHLayout_1);
    m_pScrollArea->setWidget(w);

    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar* pVscrollBar = m_pScrollArea->verticalScrollBar();
    connect(pVscrollBar, &QScrollBar::rangeChanged, this, &ChatView::on_vscrollbar_moved);

    //把垂直ScrollBar放到上边 而不是原来的并排
    QHBoxLayout *pHLayout_2 = new QHBoxLayout();
    pHLayout_2->addWidget(pVscrollBar, 0, Qt::AlignRight);
    pHLayout_2->setContentsMargins(QMargins(0, 0, 0, 0));
    m_pScrollArea->setLayout(pHLayout_2);
    pVscrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    InitStyleSheet();
}

void ChatView::AppendChatItem(QWidget *item)
{
    QVBoxLayout* vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    vl->insertWidget(vl->count()-1, item);
    m_isAppended = true;
}

void ChatView::PrependChatItem(QWidget *item)
{

}

void ChatView::InsertChatItem(QWidget *before, QWidget *item)
{

}

bool ChatView::eventFilter(QObject *watched, QEvent *e)
{
    if(e->type() == QEvent::Enter && watched == m_pScrollArea){
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }else if(e->type() == QEvent::Leave && watched == m_pScrollArea){
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(watched, e);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::on_vscrollbar_moved(int min, int max)
{
    if(m_isAppended){ // 添加item可能调用多次
        QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        // 500毫秒内可能调用多次
        QTimer::singleShot(500, [this]{
            m_isAppended = false;
        });
    }
}

void ChatView::InitStyleSheet()
{

}
