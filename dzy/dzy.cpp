#include "dzy.h"

dzy::dzy(QWidget *parent)
    : QWidget(parent),ui(new Ui::dzyClass)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//隐藏标题并显示在最上层
    this->resize(400, 300);//设置窗口中客户区的长宽
    this->move(675, 350);//设置窗口的位置
    connect(ui->pushButton_3, &QPushButton::clicked, [=]() {
        this->hide();
        });
}

void dzy::setText(QString str)
{
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    ui->textBrowser->setText(str);
}

dzy::~dzy()
{}
