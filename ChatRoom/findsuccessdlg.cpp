#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"

FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDlg)
{
    ui->setupUi(this);
}

FindSuccessDlg::~FindSuccessDlg()
{
    delete ui;
}
