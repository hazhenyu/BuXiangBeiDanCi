#pragma once

#include <QtWidgets/QWidget>
#include "dzy.h"
#include "database.h"
#include "ui_zhu.h"
#include <string>
#include <QString>
#include <QDebug>
#include <QtCore>
#include <QMouseEvent>//鼠标事件的包
#include <QGraphicsDropShadowEffect>
#include <QBitmap>
#include <QKeyEvent>
#include <QTime>
#include <QRegularExpressionValidator>// 正则表达式
#include <regex>// 正则表达式
#include <iostream>
#include <Mmsystem.h>// 这三个帮助播放MP3
#include <mciapi.h>
#include <stdio.h>
#include <process.h>// 线程
#include <mutex>
#include <QCoreApplication>
#include <ctime>
#pragma comment(lib, "Winmm.lib")

using namespace std;

class zhu : public QWidget
{
    Q_OBJECT

public:
    zhu(QWidget* parent = nullptr);
    ~zhu();
    void user_load();
    bool user_enroll();
    void screen_init();
    void user_datainit();
    void visitor_datainit();
    void main_screen();
    void help_English_standard(QString add);
    void learn_voca_screen();
    void storage_voca_screen();
    void inquire_voca_screen();
    void fetch_abcd();
    void help_branch(QString translation, int num);
    void prompt_text(QString str_1, QString str_2 = "", QString str_3 = "");
    void wrap_next();
    void show_circle();
    void remain_learn();
    void examine_English();
    void fetch_eline();
    void split_translation(string M[6], QString str);
    void manage_audio();
    void set_screen();
    void menu_screen();
    void book_screen();
    void awatar_screen();
    void return_push();
    void updata_set(userinfo& local_data_1);
    void showdzy(QString str);

public slots:
    //由于去掉显示界面的标题栏，无法拖动窗口，要重写关于窗口拖动的几个函数
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void current_Time();

private:
    Ui::zhuClass* ui;
    QString username;
    QString userpasswd;
    bool eye_wd[3];
    //窗口拖动参数
    QMouseEvent* e;
    //新的窗口拖动参数
    bool m_bDrag;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;

    // 判断当前处于哪一页面的参数// 0代表处于登陆界面
    int where_screen;// 1代表处于学习页面，2代表处于存修页面，3代表处于删差页面,4代表设置菜单相关界面
    int qian_screen;// 记录当前界面前一个界面是什么界面

    // 音标输入状态记录参数
    int Estandard_state;

    //点击按键A的参数
    bool k_A;
    bool k_E;
    bool k_V;
    bool k_I;
    bool k_Z;
    bool k_U;
    //固定窗口窗口按钮的参数
    bool emerge_parameter;
    //学习单词的状态参数
    int learn_state;
    //学习单词的序号参数
    int voca_num;
    // 记录数据库单词表的总数
    int voca_count;
    // 记录A、B、C、D、四个翻译
    QString A_translation;
    QString B_translation;
    QString C_translation;
    QString D_translation;
    // 记录正确的翻译
    QString T_translation;
    // 记录正确的单词
    QString T_voca;
    // 记录学习次数
    int learn_num;
    // 记录输出次数
    int output_num;
    // 记录错误输出的次数
    int error_num;
    // 记录错误输出的总次数
    int error_count;
    // 记录是否回答正确的参数
    bool is_right;
    // 记录该次学习是否结束的参数
    bool is_end;

    // 时间相关参数
    QDateTime start_time;
    QString start_date;
    QTime currenting_time;

    // 记录用户登陆的状态,1为游客登录；2为账号登陆。
    int user_load_state;
};