#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class ChatView:public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent = nullptr);

    void AppendChatItem(QWidget* item);// 尾插
    void PrependChatItem(QWidget* item);// 头插
    void InsertChatItem(QWidget* before, QWidget* item);// 中间插入
    void RemoveAllItem();

protected:
    bool eventFilter(QObject* watched, QEvent* e) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void on_vscrollbar_moved(int min, int max);

private:
    void InitStyleSheet();

    QVBoxLayout* m_pVl;
    QScrollArea* m_pScrollArea;
    bool m_isAppended;
};
