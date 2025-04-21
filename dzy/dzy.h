#pragma once

#include <QtWidgets/QWidget>
#include "ui_dzy.h"
#include "zhu.h"
#include <qdebug.h>
#include <QPushButton>
#include <string>
#include <QString>


class dzy : public QWidget
{
    Q_OBJECT

public:
    dzy(QWidget *parent = nullptr);
    void setText(QString str);
    ~dzy();

private:
    Ui::dzyClass* ui;
};
