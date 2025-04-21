#pragma once
#include <mysql.h>//引用数据库的包
#include <string>//string的包
#include <qstring.h>
#include <qdebug.h>
#include <iostream>
#include <stdio.h>
#include <QString>
#define MAX_LOGIN_TIMES 3
#pragma execution_character_set("utf-8")

using namespace std;

//定义一个结构体来管理账号参数
struct userinfo 
{
	int id;
	string username;
	string passwd;
	string remark;
	string nickname;
	string awatar_img;
	bool set_1;//在线状态
	bool set_2;//是否播放声音
	bool set_3;//程序结束是否显示数据
	bool set_4;//是否为新用户
	int set_5;//主题选择
};

//定义一个结构体来管理单词参数
struct vocabulary 
{
	string voca;
	string translation;
	string british_standard;
	int num;
};

//定义一个结构体来管理学习单词参数
struct learn_voca
{
	string voca;
	string translation;
	string british_standard;
	int num;
};

int safeAtoi(const char* str);

static bool connect_db_user(MYSQL& mysql);

static bool connect_db_voca(MYSQL& mysql);

bool fetch_user_info(userinfo& user);

bool enroll_user_account(userinfo& user);

bool fetch_all_userdata(userinfo& user);

bool storage_all_userdata(userinfo& user, int dd);

bool storage_vocabulary(vocabulary& voca);

learn_voca learn_vocabulary(int num);

learn_voca learn_vocabulary1(string v);

int record_vocanum();

QString fetch_translation(int num);

bool revise_voca(int num, QString str, vocabulary);
