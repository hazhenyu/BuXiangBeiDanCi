#include "database.h"
#include <mysql.h>
#define DB_UUUU "user_database" 
#define DB_VOCA "vocabulary"
#define DB_HOST "Ip地址"
#define DB_PORT 3306
#define DB_USER "root"
#define DB_USER_PASSWD "数据库密码"
using namespace std;

learn_voca L_voca;

//重写atoi,防止程序异常
int safeAtoi(const char* c) 
{
	if (c == nullptr || *c == '\0') 
	{
		// 处理空指针或空字符串的情况
		cerr << "无效的输入" << std::endl;
		return 0; 
	}

	return atoi(c);
}
//连接用户数据库
bool connect_db_user(MYSQL& mysql)
{
	//设置句柄
	mysql_init(&mysql);
	//调整为相应的字符集
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8mb4");
	//进行连接
	if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWD, DB_UUUU, DB_PORT, NULL, 0) == NULL) 
	{
		cout << "用户数据库连接失败：" << mysql_error(&mysql) << endl;
		return false;
	}
	return true;
}
//连接单词数据库
bool connect_db_voca(MYSQL& mysql)
{
	//设置句柄
	mysql_init(&mysql);
	//调整为相应的字符集
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8mb4");
	//进行连接
	if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWD, DB_VOCA, DB_PORT, NULL, 0) == NULL) 
	{
		cout << "单词数据库连接失败：" << mysql_error(&mysql) << endl;
		return false;
	}
	return true;
}
//登录函数
bool fetch_user_info(userinfo& user)
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_user(mysql) == false) 
	{
		return false;
	}
	//查找数据库是否有对应的密码和账号
	snprintf(sql, 256, "select id from vocabulary_user where username = '%s' and password = md5('%s');", user.username.c_str(), user.passwd.c_str());	
	ret = mysql_query(&mysql, sql);
	if (ret) 
	{
		cerr << "登录失败：" << sql << ", 错误原因为： " << mysql_errno(&mysql) << endl;
		mysql_close(&mysql);
		return false;
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	if (row == NULL) 
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return false;
	}
	user.id = atoi(row[0]);
	cout << "userId：" << user.id << endl;
	mysql_free_result(res);
	mysql_close(&mysql);
	return true;
}
//注册函数
bool enroll_user_account(userinfo& user)
{
	MYSQL mysql;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_user(mysql) == false) 
	{
		return false;
	}
	//注册新的账号密码
	snprintf(sql, 256, "insert into vocabulary_user (username,password) values('%s',md5('%s'));", user.username.c_str(), user.passwd.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret != 0) 
	{
		cerr << "注册失败：" << sql << ", 错误原因为： " << mysql_errno(&mysql) << endl;
		mysql_close(&mysql);
		return false;
	}	
	cout << "注册成功" << endl;
	mysql_close(&mysql);
	return true;
}
//账号登陆进入后，获取所有存储的信息，用于初始化
bool fetch_all_userdata(userinfo& user)
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	int ret = 0;
	//连接数据库
	if (connect_db_user(mysql) == false) 
	{
		return false;
	}
	// 根据id进行获取
	snprintf(sql, 256, "select * from vocabulary_user  where id = %d;", user.id);
	ret = mysql_query(&mysql, sql);
	if (ret) 
	{
		cerr << "登录连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
		mysql_close(&mysql);
		return false;
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	if (row == NULL) 
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return false;
	}
	user.id = safeAtoi(row[0]);
	user.nickname = (row[4] != NULL) ? row[4] : "";
	user.awatar_img = (row[5] != NULL) ? row[5] : "";
	user.set_1 = safeAtoi(row[6]);
	user.set_2 = safeAtoi(row[7]);
	user.set_3 = safeAtoi(row[8]);
	user.set_4 = safeAtoi(row[9]);
	user.set_5 = safeAtoi(row[10]);
	mysql_free_result(res);
	mysql_close(&mysql);
	return true;
}
//将所有更改的用户信息存储与数据库中
bool storage_all_userdata(userinfo& user, int select)
{
	MYSQL mysql;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_user(mysql) == false) 
	{
		return false;
	}
	// 0表示存储所有信息，其余如下
	if (select == 1 || select == 0)//账号名称
	{   
		cout << "存储账号名称：" << user.username << endl;
		snprintf(sql, 256, "update vocabulary_user set username = '%s' where id = % d; ", user.username.c_str(), user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 2 || select == 0)//密码
	{	
		if (select != 0)
		{
			cout << "存储密码：" << user.passwd << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set password = md5('%s') where id = % d; ", user.passwd.c_str(), user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 3 || select == 0)//备注
	{	
		if (select != 0)
		{
			cout << "存储备注：" << user.remark << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set remark = '%s' where id = % d; ", user.remark.c_str(), user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 4 || select == 0)//昵称
	{	
		if (select != 0)
		{
			cout << "存储昵称：" << user.nickname << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set nickname = '%s' where id = % d; ", user.nickname.c_str(), user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 5 || select == 0)//头像
	{
		if (select != 0)
		{
			cout << "存储头像：" << user.awatar_img << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set awatar_img = '%s' where id = % d; ", user.awatar_img.c_str(), user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 6 || select == 0)//在线状态
	{
		if (select != 0)
		{
			cout << "在线状态改为：" << user.set_1 << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set set_1 = %d where id= %d; ", user.set_1, user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;			
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 7 || select == 0)//是否播放声音
	{
		if (select != 0)
		{
			cout << "是否播放声音：" << user.set_2 << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set set_2 = %d where id= %d; ", user.set_2, user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;			
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 8 || select == 0)//程序结束时是否显示数据
	{
		if (select != 0)
		{
			cout << "是否显示数据：" << user.set_3 << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set set_3 = %d where id= %d; ", user.set_3, user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;			
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 9 || select == 0)//是否为新用户
	{
		if (select != 0)
		{
			cout << "是否为新用户：" << user.set_4 << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set set_4 = %d where id= %d; ", user.set_4, user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;			
			mysql_close(&mysql);
			return false;
		}
	}
	if (select == 10 || select == 0)//主题选择
	{
		if (select != 0)
		{
			cout << "主题选择：" << user.set_5 << endl;
		}
		snprintf(sql, 256, "update vocabulary_user set set_5 = %d where id= %d; ", user.set_5, user.id);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "存储连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
			mysql_close(&mysql);
			return false;
		}
	}
	mysql_close(&mysql);
	return true;
}
//存储单词函数
bool storage_vocabulary(vocabulary& voca)
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_voca(mysql) == false) 
	{
		return false;
	}
	//储存新的单词数据
	if (voca.num == 0)
	{
		snprintf(sql, 256, "insert into vocabulary_table (vocabulary,british_standard,chinese_translation) values('%s','%s','%s');",
		    voca.voca.c_str(), voca.british_standard.c_str(), voca.translation.c_str());
	}
	if (voca.num > 0)
	{
		snprintf(sql, 256, "insert into vocabulary_table (vocabulary,british_standard,chinese_translation,num) values('%s','%s','%s','%d');",
			voca.voca.c_str(), voca.british_standard.c_str(), voca.translation.c_str(), voca.num);
	}
	ret = mysql_query(&mysql, sql);
	if (ret != 0) 
	{
		cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql) << endl;
		mysql_close(&mysql);
		return false;
	}
	res = mysql_store_result(&mysql);
	mysql_free_result(res);
	mysql_close(&mysql);
	return true;
}
//访问单词数据函数(根据序号)
learn_voca learn_vocabulary(int num)
{
	L_voca.voca = "";
	L_voca.british_standard = "";
	L_voca.translation = "";
	L_voca.num = 0;

	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_voca(mysql) == false)
	{
		return L_voca;
	}
	snprintf(sql, 256, "select vocabulary,british_standard,chinese_translation from vocabulary_table where num =%d;", num);
	ret = mysql_query(&mysql, sql);
	if (ret != 0)
	{
		cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
		mysql_close(&mysql);
		return L_voca;
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	if (row == NULL)
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return L_voca;
	}
	L_voca.voca = (row[0] != NULL) ? row[0] : "";
	L_voca.british_standard = (row[1] != NULL) ? row[1] : "";
	L_voca.translation = (row[2] != NULL) ? row[2] : "";
	L_voca.num = num;
	mysql_free_result(res);
	mysql_close(&mysql);
	return L_voca;
}
//访问单词数据函数(根据单词)
learn_voca learn_vocabulary1(string v)
{
	L_voca.voca = "";
	L_voca.british_standard = "";
	L_voca.translation = "";
	L_voca.num = 0;

	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_voca(mysql) == false)
	{
		return L_voca;
	}
	snprintf(sql, 256, "select british_standard,chinese_translation,num from vocabulary_table where vocabulary ='%s';", v.c_str());
	ret = mysql_query(&mysql, sql);
	if (ret != 0)
	{
		cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
		mysql_close(&mysql);
		return L_voca;
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	if (row == NULL)
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return L_voca;
	}
	L_voca.voca = v;
	L_voca.british_standard = (row[0] != NULL) ? row[0] : "";
	L_voca.translation = (row[1] != NULL) ? row[1] : "";
	L_voca.num = (row[2] != NULL) ? stoi(row[2]) : 0;
	mysql_free_result(res);
	mysql_close(&mysql);
	return L_voca;
}
//返回单词总数量
int record_vocanum()
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_voca(mysql) == false) 
	{
		return -1;
	}
	snprintf(sql, 256, "select count(*) from vocabulary_table;");
	ret = mysql_query(&mysql, sql);
	if (ret != 0) 
	{
		cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
		mysql_close(&mysql);
		return -1;
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	int num = atoi(row[0]);
	mysql_free_result(res);
	mysql_close(&mysql);
	return num;
}
//传入单词序号获取单词翻译
QString fetch_translation(int num)
{
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[256];
	bool ret = false;
	//连接数据库
	if (connect_db_voca(mysql) == false) 
	{
		return "null";
	}
	snprintf(sql, 256, "select chinese_translation from vocabulary_table where num =%d;",num);
	ret = mysql_query(&mysql, sql);
	if (ret != 0) 
	{
		cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
		mysql_close(&mysql);
		return "null";
	}
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);
	if (row == NULL) 
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return "null";
	}
	QString translation = row[0];
	mysql_free_result(res);
	mysql_close(&mysql);
	return translation;
}
//传入单词序号或单词，修改单词属性
bool revise_voca(int num, QString str, vocabulary voca)
{
	cout << "A:" << voca.voca << "   B:" << voca.british_standard << "   C:" << voca.translation << "     D:" << voca.num << endl;
	if (num == 0 && str == "") 
	{
		return false;
	}
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[512];
	bool ret = false;
	if (connect_db_voca(mysql) == false) 
	{
		return false;
	}
	if (voca.voca != "") 
	{
		snprintf(sql, 256, "update vocabulary_table set vocabulary = '%s' where num = % d; ", voca.voca.c_str(), num);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
			mysql_close(&mysql);
			return false;
		}
		res = mysql_store_result(&mysql);
	}
	if (voca.british_standard != "") 
	{
		snprintf(sql, 256, "update vocabulary_table set british_standard = '%s' where num = % d; ", voca.british_standard.c_str(), num);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
			mysql_close(&mysql);
			return false;
		}
		res = mysql_store_result(&mysql);
	}
	if (voca.translation != "") 
	{
		snprintf(sql, 512, "update vocabulary_table set chinese_translation = '%s' where num = % d; ", voca.translation.c_str(), num);
		ret = mysql_query(&mysql, sql);
		if (ret != 0) 
		{
			cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
			mysql_close(&mysql);
			return false;
		}
		res = mysql_store_result(&mysql);
	}
	if (voca.num != 0) 
	{
		snprintf(sql, 256, "update vocabulary_table set num = %d where num = % d; ", voca.num, num);
		ret = mysql_query(&mysql, sql);
		if (ret != 0)
		{
			cerr << "连接失败：" << sql << "错误原因为：" << mysql_errno(&mysql);
			mysql_close(&mysql);
			return false;
		}
		res = mysql_store_result(&mysql);
	}
	mysql_free_result(res);
	mysql_close(&mysql);
	return true;
}