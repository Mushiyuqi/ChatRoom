#include "logindialog.h"
#include "ui_logindialog.h"
#include "clickedlabel.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    // 初始化头像
    InitHead();
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    // 设置忘记密码按钮的状态
    ui->forget_label->SetState("normal", "hover", "", "selected", "selected_hover", "");
    connect(ui->forget_label, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::InitHead()
{
    // 加载图片
    QPixmap originalPixmap(":/resource/head_0.png");
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建一个和原始图片相同大小的QPixmap，用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent); // 用透明色填充

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);// 设置抗锯齿，使圆角更加平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0,0,originalPixmap.width(), originalPixmap.height(), 15, 15);
    painter.setClipPath(path);

    // 将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0, 0, originalPixmap);

    // 设置绘制好的圆角图片到QLabel上
    ui->head_label->setPixmap(roundedPixmap);
}

void LoginDialog::slot_forget_pwd()
{
    emit switchReset();
}

void LoginDialog::on_login_btn_clicked()
{

}

