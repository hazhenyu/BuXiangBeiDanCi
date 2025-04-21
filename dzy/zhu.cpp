#include "zhu.h"
#include "dzy.h"
#include <qtimer.h>
#include <QTime>
#include <QStackedWidget>
#include <mysql.h>
using namespace std;

volatile bool g_bEndMusicThread;//防止被编译器优化（修改）
QString gT_voca;
vocabulary voca;
userinfo user;//这个是数据库的接收的数据
bool ret = false;
int times = 0;//记录登录次数
userinfo local_data;//记录的本地数据

zhu::zhu(QWidget* parent)
    : QWidget(parent),
    ui(new Ui::zhuClass)
{
    ui->setupUi(this);
    // 显示界面初始化
    screen_init();

    // 登录函数与注册函数 
    user_load();
    user_enroll();

    // 账号使用者的数据初始化
    main_screen();

    //显示数据库连接状态
    MYSQL mysql;
    if (connect_db_user(mysql) && connect_db_voca(mysql))
    {
        cout << "数据库连接成功" << endl;
    }
}

zhu::~zhu()
{
    delete ui;
}

//窗口数据初始化，显示界面相关的一些东西
void zhu::screen_init()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::NoDropShadowWindowHint);//窗口置顶，设置为无边框,禁用窗口投影
    setFixedSize(this->width(), this->height());//禁止拖动窗口大小
    //记录程序启动时间
    ui->textBrowser_2->setText("");
    start_time = QDateTime::currentDateTime();
    start_date = start_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
    qDebug() << start_date;
    prompt_text("程序启动时间：", start_date, "\n");
    //取得初始单词数
    voca_count = record_vocanum();
    ui->label_11->setText(QString::number(voca_count));
    where_screen = 0;
    //初始化参数
    is_right = false;
    is_end = false;
    output_num = 0;
    error_num = 0;
    learn_num = 0;
    voca_num = 1;
    learn_state = 2;
    eye_wd[0] = false;
    eye_wd[1] = false;
    eye_wd[2] = false;
    resize(820, 820);
    ui->label_2->hide();
    ui->label_3->hide();
    ui->passwd_lineEdit->setEchoMode(QLineEdit::Password);  //隐藏密码
    ui->passwd_lineEdit_1->setEchoMode(QLineEdit::Password);  //隐藏密码
    ui->passwd_lineEdit_2->setEchoMode(QLineEdit::Password);  //隐藏密码
    Estandard_state = 0;
    k_A = false;
    k_V = false;
    k_E = false;
    k_I = false;
    k_Z = false;
    k_U = false;
    ui->textBrowser->hide();
    ui->standard_lineEdit->setReadOnly(true);
    ui->pushButton_5->setIcon(QIcon(":/res/image/quit_icon.png"));
    ui->pushButton_14->setIcon(QIcon(":/res/image/min_icon.png"));
    ui->pushButton_15->setIcon(QIcon(":/res/image/emerge_icon_1.png"));
    emerge_parameter = false;

    ui->set_pushButton->setIcon(QIcon(":/res/image/set_icon.png"));
    ui->menu_pushButton->setIcon(QIcon(":/res/image/menu_icon.png"));
    ui->book_pushButton->setIcon(QIcon(":/res/image/book_icon.png"));

    ui->lcdNumber->hide();
    current_Time();
    ui->pushButton_6->setStyleSheet("QPushButton{background-color:rgba(255,178,0,100%);\
                                            color: white;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
    ui->pushButton_11->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                            color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
    ui->pushButton_12->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                            color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");

    ui->pushButton_21->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(0,250,250,100%);}");
    ui->pushButton_22->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(0,250,250,100%);}");
    ui->pushButton_23->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(0,250,250,100%);}");

    ui->audio_pushButton->setIcon(QIcon(":/res/image/audio_0.png"));
    ui->audio_pushButton->setStyleSheet("QPushButton{border-radius:20px;background-color:rgba(250,250,250,100%);}");
    ui->audio_pushButton->setGeometry(400, 115, 40, 40);
    ui->translation_label->hide();
    ui->pushButton_20->setIcon(QIcon(":/res/image/return_icon.png"));
    ui->frame_5->hide();
    SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置窗口的显示层级和初始位置
    ui->name_lineEdit->setPlaceholderText("请输入账号");
    ui->passwd_lineEdit->setPlaceholderText("请输入密码");
    ui->standard_lineEdit->setPlaceholderText("按ctrl键显示提示");
    ui->standard_lineEdit_2->setPlaceholderText("按ctrl键显示提示");
    ui->standard_lineEdit_5->setPlaceholderText("按ctrl键显示提示");
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->stackedWidget_5->setCurrentIndex(0);
}
//登录界面
void zhu::user_load()
{
    // 点击眼睛图标，显示密码
    connect(ui->pushButton_3, &QPushButton::clicked, [=]() {
        if (eye_wd[0] == false) 
        {
            ui->passwd_lineEdit->setEchoMode(QLineEdit::Normal);  //不隐藏密码
            eye_wd[0] = true;
            ui->pushButton_3->setStyleSheet(tr("border-image:url(:/res/image/mm_eye_open.png);"));
        }
        else if (eye_wd[0] == true) 
        {
            ui->passwd_lineEdit->setEchoMode(QLineEdit::Password);  //隐藏密码
            eye_wd[0] = false;
            ui->pushButton_3->setStyleSheet(tr("border-image:url(:/res/image/mm_eye_close.png);"));
        }
        });
    //点击登录按钮
    connect(ui->loadButton, &QPushButton::clicked, [=]() {
        user.username = ui->name_lineEdit->text().toStdString();
        user.passwd = ui->passwd_lineEdit->text().toStdString();
        ret = fetch_user_info(user);
        qDebug() << "ret:" << ret;
        if (ret == false) 
        {
            ui->label_2->show();
            times++;
            if (times >= 3) 
            {
                cout << "三次输入机会已用完，强制退出程序" << endl;
                QCoreApplication::quit();
            }
            cout << "第" << times << "次输入错误，剩余" << 3 - times << "次输入机会" << endl;;
        }
        else
        {
            times++;
            cout << "第" << times << "次输入成功，程序启动" << endl;
            where_screen = 1;
            user_load_state = 2;
            user_datainit();
            ui->page_3->show();
            ui->page_3->setGeometry(0, 0, 800, 800);
            ui->page->hide();
        }
        });

    //点击游客登录按钮
    connect(ui->pushButton, &QPushButton::clicked, [=]() {
        cout << "游客登录" << endl;
        where_screen = 1;
        user_load_state = 1;
        visitor_datainit();
        showdzy("当前为游客登录");
        ui->page_3->show();
        ui->page_3->setGeometry(0, 0, 800, 800);
        ui->page->hide();
        });
}
//弹出子窗口
void zhu::showdzy(QString str)
{
    dzy* d = new dzy;
    d->show();
    d->setText(str);
}
//注册界面
bool zhu::user_enroll()
{
    //注册界面的隐藏密码
    connect(ui->pushButton_2, &QPushButton::clicked, [=]() {
        if (eye_wd[1] == false) 
        {
            ui->passwd_lineEdit_1->setEchoMode(QLineEdit::Normal);  //不隐藏密码
            eye_wd[1] = true;
            ui->pushButton_2->setStyleSheet(tr("border-image:url(:/res/image/mm_eye_open.png);"));
        }
        else if (eye_wd[1] == true) 
        {
            ui->passwd_lineEdit_1->setEchoMode(QLineEdit::Password);  //隐藏密码
            eye_wd[1] = false;
            ui->pushButton_2->setStyleSheet(tr("border-image:url(:/res/image/mm_eye_close.png);"));
        }
        });
    connect(ui->pushButton_4, &QPushButton::clicked, [=]() {
        if (eye_wd[2] == false) 
        {
            ui->passwd_lineEdit_2->setEchoMode(QLineEdit::Normal);  //不隐藏密码
            eye_wd[2] = true;
            ui->pushButton_4->setStyleSheet(tr("border-image:url(:/res/image/mm_eye_open.png);"));
        }
        else if (eye_wd[2] == true)
        {
            ui->passwd_lineEdit_2->setEchoMode(QLineEdit::Password);  //隐藏密码
            eye_wd[2] = false;
            ui->pushButton_4->setStyleSheet(tr("border-image:url(:/res/image/mm_eye_close.png);"));
        }
        });

    //在登录界面点击注册按钮
    connect(ui->enrollButton, &QPushButton::clicked, [=]() {
        ui->page_2->show();
        ui->page_2->setGeometry(0, 0, 800, 800);
        ui->name_lineEdit->clear();
        ui->passwd_lineEdit->clear();
        ui->label_2->hide();
        ui->page->hide();
        });
    //在注册界面点击返回按钮
    connect(ui->loadButton_2, &QPushButton::clicked, [=]() {
        ui->page->show();
        ui->page->setGeometry(0, 0, 800, 800);
        ui->name_lineEdit_2->clear();
        ui->passwd_lineEdit_1->clear();
        ui->passwd_lineEdit_2->clear();
        ui->label_3->hide();
        ui->page_2->hide();
        });
    //点击注册按钮
    connect(ui->enrollButton_2, &QPushButton::clicked, [=]() {
        user.username = ui->name_lineEdit_2->text().toStdString();
        user.passwd = ui->passwd_lineEdit_1->text().toStdString();
        string repeatwd = ui->passwd_lineEdit_2->text().toStdString();
        if (user.username == "") 
        {
            cout << "账号输入为空" << endl;
            return false;
        }
        if (user.passwd != repeatwd) 
        {
            ui->label_3->show();
            cout << "注册密码与重复密码不一致，请检查后重新输入" << endl;
            return false;
        }
        bool ret = false;
        showdzy("注册成功，请登录");
        ret = enroll_user_account(user);
        if (ret)
        {
            ui->page->show();           
            ui->page->setGeometry(0, 0, 800, 800);
            ui->page_2->hide();
            user.set_4 = 1;
        }
        });
    return false;
}
//用户登录数据初始化
void zhu::user_datainit()
{
    // 对相关数据进行默认初始化
    ui->avatar_pushButton->setIcon(QIcon(":/res/image/avatar_bk_1.jpg"));
    ui->avatar_pushButton->setIconSize(QSize(90, 90));

    local_data.username = user.username;
    local_data.nickname = user.nickname;
    local_data.passwd = user.passwd;
    local_data.remark = user.remark;
    // 记录是否在线
    local_data.set_1 = user.set_1;
    // 记录是否播放声音
    local_data.set_2 = user.set_2;
    // 记录程序结束时是否显示相关数据
    local_data.set_3 = user.set_3;
    // 主题选择
    local_data.set_5 = user.set_5;

    // 连接数据库，获取用户数据，根据用户数据进行初始化
    fetch_all_userdata(user);
    qDebug() << "all fetch:";
    cout << user.id << user.username << user.passwd << user.remark << user.nickname << user.awatar_img << endl;
    qDebug() << user.set_1 << user.set_2 << user.set_3 << user.set_4 << user.set_5;
    user.set_1 = 1;// 1表示处于登录状态，此时该账号不能被重复登录；
    storage_all_userdata(user, 0);
    cout << "___________以下是更新函数_______________" << endl;
    updata_set(user);
}
//游客登陆数据初始化
void zhu::visitor_datainit()
{
    // 设置头像图片
    ui->avatar_pushButton->setIcon(QIcon(":/res/image/avatar_bk_2.png"));
    ui->avatar_pushButton->setIconSize(QSize(90, 90));

    local_data.username = "游客用户";
    local_data.nickname = "游客用户";
    local_data.passwd = "暂无";
    local_data.remark = "暂无";
    // 记录是否在线
    local_data.set_1 = true;
    // 记录是否播放声音
    local_data.set_2 = true;
    // 记录程序结束时是否显示相关数据
    local_data.set_3 = true;

    local_data.set_5 = 1;// 主题选择

    // 以下是更新函数
    cout << "___________以下是更新函数_______________" << endl;
    updata_set(local_data);
}
//获取相关设置并进行更新
void zhu::updata_set(userinfo& local_data_1)
{
    // 设置昵称
    ui->label_55->setText(local_data_1.nickname.c_str());
    // 设置账号
    ui->label_4->setText(local_data_1.username.c_str());
    ui->label_47->setText(local_data_1.username.c_str());
    // 设置id
    ui->label_54->setText(QString::number(local_data_1.id));
    // 设置个性签名
    ui->label_60->setText(local_data_1.remark.c_str());
    // 设置
    if (local_data_1.set_2) {
        ui->checkBox_4->setChecked(true);
    }
    if (local_data_1.set_3) {
        ui->checkBox_5->setChecked(true);
    }
    switch (local_data_1.set_5) {
    case 0:
        ui->pushButton_8->setChecked(true);
        break;
    case 1:
        ui->radioButton_9->setChecked(true);
        break;
    }
}
//主界面的函数
void zhu::main_screen()
{
    //将程序按钮设在最上层
    ui->pushButton_5->setWindowFlag(Qt::WindowStaysOnTopHint);
    ui->pushButton_14->setWindowFlag(Qt::WindowStaysOnTopHint);
    ui->pushButton_15->setWindowFlag(Qt::WindowStaysOnTopHint);
    //退出程序按钮
    connect(ui->pushButton_5, &QPushButton::clicked, [=]() {
        // 退出前对数据进行保存
        if (user_load_state == 2)
        {
            user.set_1 = false;
            user.set_4 = 0;
            storage_all_userdata(user, 0);
        }
        cout << "退出程序" << endl;
        QCoreApplication::quit();
        });
    //最小化程序按钮
    connect(ui->pushButton_14, &QPushButton::clicked, [=]() {
        cout << "最小化程序" << endl;
        this->showMinimized();
        });
    //固定显示于最顶层
    connect(ui->pushButton_15, &QPushButton::clicked, [=]() {
        bool status = 0;
        if (!emerge_parameter) {
            //窗口置顶
            SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            //设置窗口焦点
            SwitchToThisWindow(HWND(this->winId()), true);
            status = 1;
            emerge_parameter = true;
            ui->pushButton_15->setIcon(QIcon(":/res/image/emerge_icon_2.png"));
        }
        if (emerge_parameter && status == 0) 
        {
            //如果不加下面这行,会一直置顶
            SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            emerge_parameter = false;
            ui->pushButton_15->setIcon(QIcon(":/res/image/emerge_icon_1.png"));
        }
        });
    //设置界面
    set_screen();
    //头像界面
    awatar_screen();
    //菜单界面
    menu_screen();
    //书按钮
    book_screen();
    //学习单词的界面
    learn_voca_screen();
    //存储单词界面
    storage_voca_screen();
    //查找单词界面
    inquire_voca_screen();

}
//用来管理设置界面的函数
void zhu::set_screen()
{   // 点击设置按钮转移到设置界面
    connect(ui->set_pushButton, &QPushButton::clicked, [=]() {
        if (where_screen != 4)
        {
            qian_screen = where_screen;
        }
        where_screen = 4;
        cout << "set pushButton" << endl;
        ui->frame_5->show();
        ui->frame_5->raise();
        ui->pushButton_6->hide();
        ui->pushButton_11->hide();
        ui->pushButton_12->hide();
        ui->awatar_page->hide();
        ui->menu_page->hide();
        ui->book_page->hide();
        ui->set_page->show();
        ui->set_page->setGeometry(0, 0, 800, 596);
        ui->label_40->setText("设置");
        });
        return_push();
} 
//用来管理菜单界面的函数
void zhu::menu_screen()
{
    connect(ui->menu_pushButton, &QPushButton::clicked, [=]() {
        if (where_screen != 4) {
            qian_screen = where_screen;
        }
        where_screen = 4;
        cout << "menu pushButton" << endl;
        ui->frame_5->show();
        ui->frame_5->raise();
        ui->pushButton_6->hide();
        ui->pushButton_11->hide();
        ui->pushButton_12->hide();
        ui->set_page->hide();
        ui->awatar_page->hide();
        ui->book_page->hide();
        ui->menu_page->show();
        ui->menu_page->setGeometry(0, 0, 800, 596);
        ui->label_40->setText("菜单");
        });
    return_push();
    ui->pushButton_24->setStyleSheet("QPushButton{background-color:transparent;;\
                        color:rgb(255,255,190);}\
                        QPushButton:hover{ color:rgb(255,255,220); }");
    //外观与主题按钮
    connect(ui->pushButton_24, &QPushButton::clicked, [=]() {
        cout << "外观与主题" << endl;
        ui->pushButton_24->setStyleSheet("QPushButton{background-color:transparent;;\
                            color:rgb(255,255,190);}\
                            QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_25->setStyleSheet("QPushButton{background-color:transparent;;\
                            color:rgb(0,0,0);}\
                            QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_26->setStyleSheet("QPushButton{background-color:transparent;;\
                            color:rgb(0,0,0);}\
                            QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_27->setStyleSheet("QPushButton{background-color:transparent;;\
                            color:rgb(0,0,0);}\
                            QPushButton:hover{ color:rgb(255,255,255); }");
        ui->page_7->setGeometry(0, 0, 638, 596);
        ui->page_8->hide();
        ui->page_9->hide();
        ui->page_10->hide();
        ui->page_7->show();
        });
    //单词库按钮
    connect(ui->pushButton_26, &QPushButton::clicked, [=]() {
        cout << "单词库" << endl;
        ui->pushButton_26->setStyleSheet("QPushButton{background-color:transparent;;\
                                color:rgb(255,255,190);}\
                                QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_25->setStyleSheet("QPushButton{background-color:transparent;;\
                                color:rgb(0,0,0);}\
                                QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_24->setStyleSheet("QPushButton{background-color:transparent;;\
                                color:rgb(0,0,0);}\
                                QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_27->setStyleSheet("QPushButton{background-color:transparent;;\
                                color:rgb(0,0,0);}\
                                QPushButton:hover{ color:rgb(255,255,255); }");
        ui->page_8->setGeometry(0, 0, 638, 596);
        ui->page_7->hide();
        ui->page_9->hide();
        ui->page_10->hide();
        ui->page_8->show();
        });
    //帮助按钮
    connect(ui->pushButton_25, &QPushButton::clicked, [=]() {
        cout << "帮助" << endl;
        ui->pushButton_25->setStyleSheet("QPushButton{background-color:transparent;;\
                                    color:rgb(255,255,190);}\
                                    QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_26->setStyleSheet("QPushButton{background-color:transparent;;\
                                    color:rgb(0,0,0);}\
                                    QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_24->setStyleSheet("QPushButton{background-color:transparent;;\
                                    color:rgb(0,0,0);}\
                                    QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_27->setStyleSheet("QPushButton{background-color:transparent;;\
                                    color:rgb(0,0,0);}\
                                    QPushButton:hover{ color:rgb(255,255,255); }");
        ui->page_9->setGeometry(0, 0, 638, 596);
        ui->page_7->hide();
        ui->page_8->hide();
        ui->page_10->hide();
        ui->page_9->show();
        });
    // 关于开发者按钮
    connect(ui->pushButton_27, &QPushButton::clicked, [=]() {
        cout << "关于开发者" << endl;
        ui->pushButton_27->setStyleSheet("QPushButton{background-color:transparent;;\
                                        color:rgb(255,255,190);}\
                                        QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_26->setStyleSheet("QPushButton{background-color:transparent;;\
                                        color:rgb(0,0,0);}\
                                        QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_24->setStyleSheet("QPushButton{background-color:transparent;;\
                                        color:rgb(0,0,0);}\
                                        QPushButton:hover{ color:rgb(255,255,255); }");
        ui->pushButton_25->setStyleSheet("QPushButton{background-color:transparent;;\
                                        color:rgb(0,0,0);}\
                                        QPushButton:hover{ color:rgb(255,255,255); }");
        ui->page_10->setGeometry(0, 0, 638, 596);
        ui->page_7->hide();
        ui->page_8->hide();
        ui->page_9->hide();
        ui->page_10->show();
        });
}
//用来管理book界面的函数
void zhu::book_screen()
{
    connect(ui->book_pushButton, &QPushButton::clicked, [=]() {
        if (where_screen != 4) {
            qian_screen = where_screen;
        }
        where_screen = 4;
        cout << "book pushButton" << endl;
        ui->frame_5->show();
        ui->frame_5->raise();
        ui->pushButton_6->hide();
        ui->pushButton_11->hide();
        ui->pushButton_12->hide();
        ui->set_page->hide();
        ui->awatar_page->hide();
        ui->menu_page->hide();
        ui->book_page->show();
        ui->book_page->setGeometry(0, 0, 800, 596);
        ui->label_40->setText("记录");
        });
}
//用来管理头像界面的函数
void zhu::awatar_screen()
{
    connect(ui->avatar_pushButton, &QPushButton::clicked, [=]() {
        if (user_load_state == 1) {
            showdzy("很抱歉，由于游客用户的数据无法持久性的保存于数据库中，重新载入程序时，相关参数将会被重置为系统默认值，因此不支持空间功能，请见谅。");
        }
        if (where_screen != 4) {
            qian_screen = where_screen;
        }
        where_screen = 4;
        cout << "avatar pushButton" << endl;
        ui->frame_5->show();
        ui->frame_5->raise();
        ui->pushButton_6->hide();
        ui->pushButton_11->hide();
        ui->pushButton_12->hide();
        ui->set_page->hide();
        ui->menu_page->hide();
        ui->book_page->hide();
        ui->awatar_page->show();
        ui->awatar_page->setGeometry(0, 0, 800, 596);
        QString str1;
        QString str2;
        if (user_load_state == 1) 
        {
            str1 = local_data.nickname.c_str();
        }
        if (user_load_state == 2) {
            str1 = user.nickname.c_str();
        }
        str2 = str1 + "的空间";
        ui->label_40->setText(str2);
        });

}
//返回主界面
void zhu::return_push()
{
    // 点击返回按钮
    connect(ui->pushButton_20, &QPushButton::clicked, [=]() {
        where_screen = qian_screen;
        qian_screen = 4; 
        ui->frame_5->lower();
        ui->frame_5->hide();
        ui->pushButton_6->show();
        ui->pushButton_11->show();
        ui->pushButton_12->show();
        });
}
//主界面左侧显示提示
void zhu::prompt_text(QString str_1, QString str_2, QString str_3)
{
    QString a = ui->textBrowser_2->toPlainText();
    QString b = a + str_1 + str_2 + str_3;
    ui->textBrowser_2->setText(b);
}
//学习单词的界面
void zhu::learn_voca_screen()
{
    //切换到学习单词界面的按钮点击
    connect(ui->pushButton_6, &QPushButton::clicked, [=]() {
        where_screen = 1;
        ui->learn_voca_page->show();
        ui->learn_voca_page->setGeometry(0, 0, 600, 600);
        ui->storage_voca_page->hide();
        ui->inquire_voca_page->hide();
        ui->pushButton_6->setStyleSheet("QPushButton{background-color:rgba(255,178,0,100%);\
                                                color: white;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        ui->pushButton_11->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                                color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        ui->pushButton_12->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                                color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        });

    // 点击开始按钮，向数据库索要数据
    connect(ui->pushButton_7, &QPushButton::clicked, [=]() {
        if (learn_state == 1) 
        {
            wrap_next();
            if (learn_state == 1) 
            {
                ui->pushButton_7->hide();
            }
        }
        if (learn_state == 2) 
        {
            if (is_end) 
            {
                wrap_next();
            }
            examine_English();
            if (learn_state == 1) 
            {
                ui->pushButton_7->hide();
            }
        }
        });

    // 点击翻译选项卡
    connect(ui->A_pushButton, &QPushButton::clicked, [=]() {
        if (A_translation == T_translation && A_translation != "") {
            if (!is_end) 
            {
                prompt_text("A_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (A_translation != T_translation && A_translation != "") 
        {
            if (!is_end) 
            {
                prompt_text("A_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        });
    connect(ui->B_pushButton, &QPushButton::clicked, [=]() {
        if (B_translation == T_translation && B_translation != "") {
            if (!is_end) {
                prompt_text("B_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (B_translation != T_translation && B_translation != "") {
            if (!is_end) {
                prompt_text("B_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        });
    connect(ui->C_pushButton, &QPushButton::clicked, [=]() {
        if (C_translation == T_translation && C_translation != "") {
            if (!is_end) {
                prompt_text("C_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (C_translation != T_translation && C_translation != "") {
            if (!is_end) {
                prompt_text("C_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        });
    connect(ui->D_pushButton, &QPushButton::clicked, [=]() {
        if (D_translation == T_translation && D_translation != "") {
            if (!is_end) {
                prompt_text("D_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (D_translation != T_translation && D_translation != "") {
            if (!is_end) {
                prompt_text("D_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        });

    // 点击播放单词读音的按钮
    connect(ui->audio_pushButton, &QPushButton::clicked, [=]() {
        qDebug() << "audio_pushButton";
        manage_audio();
        });


    connect(ui->standard_lineEdit_4, &QLineEdit::textChanged, [=]() {
        if (learn_state == 2) {
            QString str= ui->standard_lineEdit_4->text();
            ui->voca_label_2->setText(str);
            ui->voca_label_2->setStyleSheet("font: 22pt '宋体'; color:rgb(95, 15, 225);");; 
            ui->voca_label_2->show();
        }
        });
}
//获取四个选项的翻译
void zhu::fetch_abcd()
{
    //保证以下五个不能够重复
    int a = -1;
    int b = -1;
    int c = -1;
    int d = -1;

    //初始化种子为随机值
    srand((unsigned int)time(NULL));
    int i = 0;
    while (i <= 4) {
        //产生一个1-voca_count之间的数
        int j = rand() % voca_count + 1;
        if (j != voca_num && j != a && j != b && j != c && j != d) {
            i++;
            switch (i) {
            case 1:
                a = j;
                break;
            case 2:
                b = j;
                break;
            case 3:
                c = j;
                break;
            case 4:
                d = j;
                break;
            }
        }
    }
    //分别把序号传进去，获取相应的单词翻译充当选项
    A_translation = fetch_translation(a);
    B_translation = fetch_translation(b);
    C_translation = fetch_translation(c);
    D_translation = fetch_translation(d);
    // 记录正确的翻译
    T_translation = fetch_translation(voca_num);
    // 产生一个1-4之间的数
    int answer = rand() % 4 + 1;
    switch (answer) {
    case 1:
        A_translation = fetch_translation(voca_num);
        break;
    case 2:
        B_translation = fetch_translation(voca_num);
        break;
    case 3:
        C_translation = fetch_translation(voca_num);
        break;
    case 4:
        D_translation = fetch_translation(voca_num);
        break;
    }
    help_branch(A_translation, 1);
    help_branch(B_translation, 2);
    help_branch(C_translation, 3);
    help_branch(D_translation, 4);
}
//协助分割的函数
void Stringsplit(const string& str, const string& split, vector<string>& res)
{
    regex reg(split);//寻找split
    sregex_token_iterator pos(str.begin(), str.end(), reg, -1);//遍历str并分割，-1为返回剩余部分
    decltype(pos) end;//自动推导类型 
    for (; pos != end; ++pos)//string字符以\0结尾
    {
        res.push_back(pos->str());
    }
}
//对翻译进行切割，以便显示
void zhu::split_translation(string M[6], QString str)
{
    string str1 = str.toStdString(); 
    //v.让；允许；由；准许； n.出租；（发球时的）擦网球；租借； 网络释义： 假设；容许；小；
    // 使用字符串分词
    vector<string> strList1;// 定义一个字符串容器
    Stringsplit(str1, "网络释义", strList1);
    string str2 = strList1[0];
    //v.让；允许；由；准许； n.出租；（发球时的）擦网球；租借；
    vector<string> strList2;
    Stringsplit(str2, "[.]", strList2);
    int i = strList2.size() - 1;
    M[0] = to_string(i);
    //出租；（发球时的）擦网球；租借
    M[1] = strList2[0] + ".";
    //v.
    vector<string> strT1;
    vector<string> strT2;
    vector<string> strT3;
    vector<string> strT4;
    if (i == 1) //第一个词性
    {
        M[1] += strList2[1];
    }
    if (i >= 2) 
    {
        Stringsplit(strList2[1], "；", strT1);
        for (int i = 0; i < strT1.size() - 1; i++) 
        {
            M[1] += (strT1[i] + "；");
        }
        M[2] = strT1[strT1.size() - 1] + ".";
    }
    if (i == 2) //第二个词性
    {
        M[2] += strList2[2];
    }
    if (i >= 3) 
    { 
        Stringsplit(strList2[2], "；", strT2);
        for (int i = 0; i < strT2.size() - 1; i++) 
        {
            M[2] += (strT2[i] + "；");
        }
        M[3] = strT2[strT2.size() - 1] + ".";
    }
    if (i == 3) //第三个词性
    {
        M[3] += strList2[3];
    }
    if (i >= 4) 
    {
        Stringsplit(strList2[3], "；", strT3);
        for (int i = 0; i < strT3.size() - 1; i++) 
        {
            M[3] += (strT3[i] + "；");
        }
        M[4] = strT3[strT3.size() - 1] + ".";
    }
    if (i == 4) //第四个词性
    {
        M[4] += strList2[4];
    }
    if (i >= 5) 
    {
        Stringsplit(strList2[4], "；", strT4);
        for (int i = 0; i < strT4.size() - 1; i++) 
        {
            M[4] += (strT4[i] + "；");
        }
        M[5] = strT3[strT4.size() - 1] + ".";
    }
    if (i == 5) //第五个词性
    {
        M[5] += strList2[5];
    }
}
//当翻译有多重语义时或翻译太长无法一行显示时，帮助进行分行显示
void zhu::help_branch(QString translation, int num)
{
    // 创建一个数组，使用后销毁
    string* M = new string[6];
    split_translation(M, translation);
    int i = stoi(M[0]);
    QString res = "";
    QString a = M[1].c_str();
    QString b = M[2].c_str();
    QString c = M[3].c_str();
    QString d = M[4].c_str();
    QString e = M[5].c_str();
    delete[] M;
    M = NULL;
    // 在其本身的词意较简的前提下
    // 如果某一字符过长，将其继续分行
    int max_length = 50;
    if (num <= 4 && i <= 2)//选项
    {
        max_length = 11;
    }
    else if (num > 4 && i <= 3)//标签
    {
        max_length = 17;
    }
    int A = a.length();
    if (A > max_length) 
    {
        a = a.mid(0, max_length) + "\n" + a.mid(max_length, -1);//-1代表字符串尾
    }
    int B = b.length();
    if (B > max_length) 
    {
        b = b.mid(0, max_length) + "\n" + b.mid(max_length, -1);
    }
    int C = c.length();
    if (C > max_length) 
    {
        c = c.mid(0, max_length) + "\n" + c.mid(max_length, -1);
    }
    int D = d.length();
    if (D > max_length) 
    {
        d = d.mid(0, max_length) + "\n" + d.mid(max_length, -1);
    }
    int E = e.length();
    if (E > max_length) 
    {
        e = e.mid(0, max_length) + "\n" + e.mid(max_length, -1);
    }

    bool end = false;//防止重复执行
    if (b == "") 
    {
        res = a;
        end = true;
    }
    if (c == "" && end == false)
    {//两行
        res = a + "\n" + b;
        end = true;
    }
    if (d == "" && end == false)
    {//三行
        res = a + "\n" + b + "\n" + c;
        end = true;
    }
    if (e == "" && end == false)
    {//四行
        res = a + "\n" + b + "\n" + c + "\n" + d;
        end = true;
    }
    if (e != "" && end == false)
    {//五行
        res = a + "\n" + b + "\n" + c + "\n" + d + "\n" + e;
        end = true;
    }
    switch (num) 
    {
    case 1:
        ui->A_pushButton->setText(res);
        break;
    case 2:
        ui->B_pushButton->setText(res);
        break;
    case 3:
        ui->C_pushButton->setText(res);
        break;
    case 4:
        ui->D_pushButton->setText(res);
        break;
    case 5:
        ui->translation_label->setText(res);
        break;
    }
}
//将string转换为LPCWSTR
LPCWSTR StringToWString(string& str)
{
    wchar_t* wstr = new wchar_t[str.length() + 1];//+1是为了给\0留出空间
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wstr, str.length() + 1);//将多字节字符转换为宽字符
    return wstr;
}
//将wstring转换为string
string WStringToString(const wstring& wstr)
{
    int nChars = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);//计算转换后的字符串所需的字符数
    string str(nChars,'\0');//创建string对象
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], nChars, nullptr, nullptr);//将宽字符转换为多字节字符
    return str;
}
//发送MCI命令
bool SendToMCI(string command)
{
    LPCWSTR result = StringToWString(command);
    if (!mciSendString(result, NULL, 0, 0))//发送MCI给多媒体设备，成功则返回0
    {
        delete[] result;
        return true;
    }
    delete[] result;
    return false;
}
//从MCI获取音乐状态
string GetFromMCI(string command)
{
    wchar_t res[20];
    LPCWSTR result = StringToWString(command);
    mciSendString(result, res, 20, 0);//发送MCI并将结果储存到res中
    delete[] result;
    return WStringToString(res);
}
//音乐线程的方法
unsigned __stdcall ThreadPlayMusic(LPVOID lpParameter)
{
    QString open = "OPEN " + gT_voca + " ALIAS MUSIC";
    string Open = open.toStdString();//构造MCI命令，用于打开音乐文件
    string Close = "CLOSE MUSIC";//用于关闭音乐文件
    string Status = "status MUSIC mode";//用于获取状态
    string Play = "PLAY MUSIC FROM 0";//从音乐文件的开始位置播放
    mutex gThreadMutex;//定义互斥锁，保护不被其他线程中断

    while (true)
    {
        Sleep(1);//使线程休眠一毫秒
        {
            lock_guard<mutex> lock(gThreadMutex);//进行加锁
            if (g_bEndMusicThread == true) 
            {//接收主线程发送的退出信号
                SendToMCI(Open);
                SendToMCI(Play);
                string res1 = GetFromMCI(Status);//获取音乐状态
                if (res1 == "stopped")
                    SendToMCI(Close);//如果音乐停止，关闭并返回
                break;
            }
        }
        string res2 = GetFromMCI(Status);//再次确认
        if (res2 == "stopped")
            break;
    }
    return 0;
}
//该函数用来管理音频
void zhu::manage_audio()
{
    //点击播放单词读音的按钮
    gT_voca = "res/audio/voca_pronunciation/" + T_voca + ".mp3";
    g_bEndMusicThread = false;//初始化子线程标志
    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadPlayMusic, NULL, 0, NULL);//创建线程
    g_bEndMusicThread = true;//通知子线程播放读音
    WaitForSingleObject(hThread, INFINITE);//线程结束之后再释放资源
}
//将学习单词的下一个封装起来
void zhu::wrap_next()
{
    QRegularExpression regExp("^[A-Za-z]+$");//创建只能包含字母的正则表达式
    QValidator* validator = new QRegularExpressionValidator(regExp, this);//验证用户输入是否符合规则
    ui->standard_lineEdit_4->setValidator(validator);//应用验证器
    ui->standard_lineEdit_4->setAttribute(Qt::WA_InputMethodEnabled, false);//禁止使用输入法
    // 初始化随机数种子
    srand((unsigned int)time(NULL));
    // 产生一个1到voca_count的数
    voca_num = rand() % voca_count + 1;
    ui->standard_lineEdit_4->setFocus();
    ui->standard_lineEdit_4->setText("");
    ui->pushButton_21->hide();
    ui->pushButton_22->hide();
    ui->pushButton_23->hide();
    error_num = 0;
    output_num = 0;
    is_right = false;
    is_end = false;
    learn_voca v = learn_vocabulary(voca_num);
    T_voca = v.voca.c_str();
    //产生一个1或2的数
    learn_state = rand() % 2 + 1;
    ui->voca_label->setText(v.voca.c_str());
    if (learn_state == 1) 
    {
        fetch_abcd();
        ui->A_pushButton->show();
        ui->B_pushButton->show();
        ui->C_pushButton->show();
        ui->D_pushButton->show();
        ui->translation_label->hide();
        ui->voca_label->setGeometry(100, 50, 400, 50);
        ui->standard_label->setGeometry(175, 100, 250, 50);
        ui->voca_label->show();
    }
    if (learn_state == 2) 
    {
        ui->A_pushButton->hide();
        ui->B_pushButton->hide();
        ui->C_pushButton->hide();
        ui->D_pushButton->hide();
        ui->voca_label->setText("");
        ui->voca_label->setGeometry(100, 50, 400, 50);
        ui->standard_label->setGeometry(175, 100, 250, 50);
        ui->translation_label->setGeometry(58, 230, 484, 233);
        ui->translation_label->show();
    }
    ui->standard_label->setText(v.british_standard.c_str());
    help_branch(v.translation.c_str(), 5);
    if (learn_num == 0) 
    {
        ui->audio_pushButton->setGeometry(400, 105, 40, 40);
        prompt_text("Go! start learning vocabulary;\n");
        if (learn_state == 1)
        {
            ui->pushButton_7->hide();
        }
        ui->pushButton_7->setText("Next");
    }
    learn_num++;
}
//提示答题次数
void zhu::show_circle()
{
    if (output_num == 1 && is_right) {
        ui->pushButton_21->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(0,250,250,100%);}");
        ui->pushButton_21->show();
    }
    if (output_num == 1 && !is_right) {
        ui->pushButton_21->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(250,0,250,100%);}");
        ui->pushButton_21->show();
    }
    if (output_num == 2 && is_right) {
        ui->pushButton_22->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(0,250,250,100%);}");
        ui->pushButton_22->show();
    }
    if (output_num == 2 && !is_right) {
        ui->pushButton_22->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(250,0,250,100%);}");
        ui->pushButton_22->show();
    }
    if (output_num == 3 && is_right) {
        ui->pushButton_23->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(0,250,250,100%);}");
        ui->pushButton_23->show();
    }
    if (output_num == 3 && !is_right) {
        ui->pushButton_23->setStyleSheet("QPushButton{border-radius:25px;background-color:rgba(250,0,250,100%);}");
        ui->pushButton_23->show();
    }
}
//显示答案
void zhu::remain_learn()
{
    if (is_right && output_num <= 3) 
    {
        ui->voca_label->setText(T_voca);
        ui->A_pushButton->hide();
        ui->B_pushButton->hide();
        ui->C_pushButton->hide();
        ui->D_pushButton->hide();
        ui->pushButton_7->show();
        ui->voca_label->show();
        ui->standard_label->show();
        ui->translation_label->show();
        manage_audio();
    }
    if (!is_right && output_num == 3) 
    {
        ui->voca_label->setText(T_voca);
        ui->A_pushButton->hide();
        ui->B_pushButton->hide();
        ui->C_pushButton->hide();
        ui->D_pushButton->hide();
        ui->pushButton_7->show();
        ui->voca_label->show();
        ui->standard_label->show();
        ui->translation_label->show();
        manage_audio();
    }
    if (output_num >= 3) 
    {
        is_end = true;
    }
}
//检查输入的英语单词是否正确
void zhu::examine_English()
{
    if (learn_num == 0) 
    {
        wrap_next();
        return;
    }
    QString answer = ui->standard_lineEdit_4->text();
    if (answer == T_voca && answer != "") 
    {
        if (!is_end) 
        {
            prompt_text(answer, " is right;\n");
            is_right = true;
            is_end = true;
            output_num++;
            show_circle();
            remain_learn();
        }
    }
    if (answer != T_voca && answer != "")
    {
        if (!is_end) 
        {
            prompt_text(answer, " is error;\n");
            output_num++;
            error_num++;
            show_circle();
            remain_learn();
        }
    }
}
// 按下Enter后获取lineedit的字符串并判断
void zhu::fetch_eline()
{
    QString answer = ui->standard_lineEdit_4->text();
    if (answer == "a" || answer == "A") {
        if (A_translation == T_translation && A_translation != "") {
            if (!is_end) {
                prompt_text("A_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (A_translation != T_translation && A_translation != "") {
            if (!is_end) {
                prompt_text("A_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        return;
    }
    if (answer == "b" || answer == "B") {
        if (B_translation == T_translation && B_translation != "") {
            if (!is_end) {
                prompt_text("B_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (B_translation != T_translation && B_translation != "") {
            if (!is_end) {
                prompt_text("B_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        return;
    }
    if (answer == "c" || answer == "C") {
        if (C_translation == T_translation && C_translation != "") {
            if (!is_end) {
                prompt_text("C_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (C_translation != T_translation && C_translation != "") {
            if (!is_end) {
                prompt_text("C_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        return;
    }
    if (answer == "d" || answer == "D") {
        if (D_translation == T_translation && D_translation != "") {
            if (!is_end) {
                prompt_text("D_pushButton is true;\n");
                is_right = true;
                is_end = true;
                output_num++;
                show_circle();
                remain_learn();
            }
        }
        if (D_translation != T_translation && D_translation != "") {
            if (!is_end) {
                prompt_text("D_pushButton is false;\n");
                output_num++;
                error_num++;
                show_circle();
                remain_learn();
            }
        }
        return;
    }
}
//储存和修改单词界面
void zhu::storage_voca_screen()
{
    //转换到储存单词界面的按钮
    connect(ui->pushButton_11, &QPushButton::clicked, [=]() {
        where_screen = 2;
        qDebug() << "storage vocabulary";
        ui->learn_voca_page->hide();
        ui->storage_voca_page->show();
        ui->storage_voca_page->setGeometry(0, 0, 600, 600);
        ui->inquire_voca_page->hide();
        ui->pushButton_11->setStyleSheet("QPushButton{background-color:rgba(255,178,0,100%);\
                                                color: white;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        ui->pushButton_6->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                                color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        ui->pushButton_12->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                                color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        });

    //插入单词按钮
    connect(ui->pushButton_9, &QPushButton::clicked, [=]() {
        voca.voca = (ui->voca_lineEdit_2->text().toUtf8().data() != NULL) ? ui->voca_lineEdit_2->text().toStdString() : "";
        voca.british_standard = (ui->standard_lineEdit_2->text().toUtf8().data() != NULL) ? ui->standard_lineEdit_2->text().toStdString() : "";
        voca.translation = (ui->translation_lineEdit_2->text().toUtf8().data() != NULL) ? ui->translation_lineEdit_2->text().toStdString() : "";
        voca.num = (ui->voca_num_lineEdit_2->text().toUtf8().data() != NULL) ? ui->voca_num_lineEdit_2->text().toInt() : 0;
        storage_vocabulary(voca);
        voca_count = record_vocanum();
        ui->label_11->setText(QString::number(voca_count));
        });
    //一键清除按钮
    connect(ui->all_clear_pushButton, &QPushButton::clicked, [=]() {
        ui->voca_lineEdit->setText("");
        ui->standard_lineEdit->setText("");
        ui->translation_lineEdit->setText("");
        ui->voca_num_lineEdit->setText("");
        ui->voca_lineEdit_2->setText("");
        ui->standard_lineEdit_2->setText("");
        ui->translation_lineEdit_2->setText("");
        ui->voca_num_lineEdit_2->setText("");
        });

    //修改单词按钮
    connect(ui->pushButton_8, &QPushButton::clicked, [=]() {
        QString v = ui->voca_lineEdit->text();
        int n = ui->voca_num_lineEdit->text().toInt();
        voca.voca = (ui->voca_lineEdit_2->text().toUtf8().data() != NULL) ? ui->voca_lineEdit_2->text().toStdString() : "";
        voca.british_standard = (ui->standard_lineEdit_2->text().toUtf8().data() != NULL) ? ui->standard_lineEdit_2->text().toStdString() : "";
        voca.translation = (ui->translation_lineEdit_2->text().toUtf8().data() != NULL) ? ui->translation_lineEdit_2->text().toStdString() : "";
        voca.num = (ui->voca_num_lineEdit_2->text().toUtf8().data() != NULL) ? ui->voca_num_lineEdit_2->text().toInt() : 0;
        revise_voca(n, v, voca);
        });

    //输入转换按钮
    connect(ui->pushButton_10, &QPushButton::clicked, [=]() {
        switch (Estandard_state)
        {
        case 0:
            ui->pushButton_10->setText("左侧转换为英标输入");
            Estandard_state = 1;
            break;
        case 1:
            ui->pushButton_10->setText("右侧转换为英标输入");
            Estandard_state = 2;
            break;
        case 2:
            ui->pushButton_10->setText("查询转换为英标输入");
            Estandard_state = 3;
            break;
        case 3:
            ui->pushButton_10->setText("未转换为英标输入");
            ui->textBrowser->hide();
            Estandard_state = 0;
            break;
        }
        });

}
//帮助显示英语英标
void zhu::help_English_standard(QString add)
{
    QString str1;
    QString str2;
    switch (Estandard_state) {
    case 0:
        break;
    case 1:
        str1 = ui->standard_lineEdit->text();
        str2 = str1 + add;
        ui->standard_lineEdit->setText(str2);
        break;
    case 2:
        str1 = ui->standard_lineEdit_2->text();
        str2 = str1 + add;
        ui->standard_lineEdit_2->setText(str2);
        break;
    case 3:
        str1 = ui->standard_lineEdit_5->text();
        str2 = str1 + add;
        ui->standard_lineEdit_5->setText(str2);
        break;
    }
}
//查询和删除单词界面
void zhu::inquire_voca_screen()
{
    connect(ui->pushButton_12, &QPushButton::clicked, [=]() {
        where_screen = 3;
        qDebug() << "inquire vocabulary";
        ui->learn_voca_page->hide();
        ui->storage_voca_page->hide();
        ui->inquire_voca_page->show();
        ui->inquire_voca_page->setGeometry(0, 0, 600, 600);
        showdzy("仅支持根据单词查找");
        ui->pushButton_12->setStyleSheet("QPushButton{background-color:rgba(255,178,0,100%);\
                                                color: white;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        ui->pushButton_6->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                                color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        ui->pushButton_11->setStyleSheet("QPushButton{background-color:rgba(128,128,128,100%);\
                                                color: black;   border-radius: 10px;  border: 2px groove gray; border-style: outset;}");
        });
    connect(ui->pushButton_18, &QPushButton::clicked, [=]() {
        string v = (ui->voca_lineEdit_3->text().toUtf8().data() != NULL) ? ui->voca_lineEdit_3->text().toStdString() : "";
        learn_voca temp = learn_vocabulary1(v);
        ui->translation_lineEdit_3->setText((temp.translation).c_str());
        ui->standard_lineEdit_5->setText((temp.british_standard).c_str());
        ui->voca_num_lineEdit_3->setText(QString::number(temp.num));
        });
}
//获取当前时间并在屏幕上显示出来
void zhu::current_Time()
{
    currenting_time = QTime::currentTime();
    QString str = currenting_time.toString("hh:mm");
    ui->lcdNumber->display(str);
    ui->lcdNumber->show();
}
//鼠标按下事件
void zhu::mousePressEvent(QMouseEvent* e)//鼠标点击
{

    if (e->button() == Qt::LeftButton) 
    {

        m_bDrag = true;
        //获得鼠标的初始位置
        mouseStartPoint = e->globalPos();
        //获得窗口的初始位置
        windowTopLeftPoint = this->frameGeometry().topLeft();
    }
}
void zhu::mouseMoveEvent(QMouseEvent* e)//鼠标移动
{
    if (m_bDrag)
    {
        //获得鼠标移动的距离
        QPoint distance = e->globalPos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
}
void zhu::mouseReleaseEvent(QMouseEvent* e)//鼠标释放
{
    if (e->button() == Qt::LeftButton)	
    {
        m_bDrag = false;
    }
}

//键盘按键事件
void zhu::keyPressEvent(QKeyEvent* e)
{

    if (where_screen == 1) 
    {
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) 
        {   //Enter
            prompt_text("Enter;\n");
            if (learn_state == 2) 
            {
                if (is_end) {
                    wrap_next();
                }
                examine_English();
                if (learn_state == 1) 
                {
                    ui->pushButton_7->hide();
                }
            }
            if (learn_state == 1) 
            {
                if (is_end) {
                    wrap_next();
                }
                fetch_eline();
            }
        }
        if (e->key() == Qt::Key_Alt) 
        {   //Alt
            manage_audio();
            prompt_text("Alt;\n");
        }
    }

    if (where_screen == 2 || where_screen == 3) 
    {
        // 如果按下了CTRL键
        if (e->modifiers() == Qt::ControlModifier && Estandard_state != 0)
        {
            ui->textBrowser->show();
        }
        if (e->key() == Qt::Key_A) 
        {
            k_A = true;
        }
        if (e->key() == Qt::Key_E) 
        {
            k_E = true;
        }
        if (e->key() == Qt::Key_V) 
        {
            k_V = true;
        }
        if (e->key() == Qt::Key_I) 
        {
            k_I = true;
        }
        if (e->key() == Qt::Key_Z) 
        {
            k_Z = true;
        }
        if (e->key() == Qt::Key_U) 
        {
            k_U = true;
        }
        if (e->key() == Qt::Key_Apostrophe) 
        {
            help_English_standard("ˈ");
        }
        if (e->key() == Qt::Key_Slash) 
        {
            help_English_standard("ˌ");
        }
        if (k_A) {
            switch (e->key())
            {
                // A+O键
            case Qt::Key_O:
                help_English_standard("ɑː");
                break;
            case Qt::Key_R:
                help_English_standard("æ");
                break;
            case Qt::Key_I:
                help_English_standard("aɪ");
                break;
            case Qt::Key_E:
                help_English_standard("eɪ");
                break;
            case Qt::Key_W:
                help_English_standard("aʊ");
                break;
            case Qt::Key_G:
                help_English_standard("ɡ");
                break;
            }
        }
        if (k_V) {
            switch (e->key())
            {
            case Qt::Key_O:
                help_English_standard("ɔː");
                break;
            case Qt::Key_A:
                help_English_standard("ʌ");
                break;
            case Qt::Key_U:
                help_English_standard("ʊ");
                break;
            case Qt::Key_H:
                help_English_standard("θ");
                break;
            case Qt::Key_E:
                help_English_standard("ɜː");
                break;
            case Qt::Key_W:
                help_English_standard("ɒ");
                break;
            case Qt::Key_S:
                help_English_standard("ʃ");
                break;
            case Qt::Key_T:
                help_English_standard("ð");
                break;
            case Qt::Key_Z:
                help_English_standard("dʒ");
                break;
            case Qt::Key_G:
                help_English_standard("ŋ");
                break;
            }
        }
        if (k_E) {
            switch (e->key())
            {
            case Qt::Key_O:
                help_English_standard("ʊə");
                break;
            case Qt::Key_I:
                help_English_standard("ɪə");
                break;
            case Qt::Key_W:
                help_English_standard("əʊ");
                break;
            case Qt::Key_D:
                help_English_standard("ə");
                break;
            }
        }
        if (k_I) {
            switch (e->key())
            {
            case Qt::Key_O:
                help_English_standard("ɔɪ");
                break;
            case Qt::Key_P:
                help_English_standard("ɪ");
                break;
            case Qt::Key_Semicolon:
                help_English_standard("iː");
                break;
            }
        }
        if (k_Z) {
            switch (e->key())
            {
            case Qt::Key_D:
                help_English_standard("=ʒ");
                break;
            }
        }
        if (k_U) {
            switch (e->key())
            {
            case Qt::Key_Semicolon:
                help_English_standard("uː");
                break;
            }
        }
    }
}

void zhu::keyReleaseEvent(QKeyEvent* e)
{
    if (where_screen == 2 || where_screen == 3) {
        switch (e->key())
        {
        case Qt::Key_A:
            k_A = false;
            break;
        case Qt::Key_E:
            k_E = false;
            break;
        case Qt::Key_V:
            k_V = false;
            break;
        case Qt::Key_I:
            k_I = false;
            break;
        case Qt::Key_Z:
            k_Z = false;
            break;
        case Qt::Key_U:
            k_U = false;
            break;
        }
    }
}