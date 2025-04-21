# 英语单词学习助手（不想背单词）

一个基于Qt框架开发的英语单词学习桌面应用程序，帮助用户高效地学习、记忆和管理英语单词。

## 功能特点

### 用户系统

•    多用户注册/登录
•    游客模式快速使用
•    用户数据持久化存储
•    个性化设置（主题、声音等）

### 学习模式

•    中英对照选择题模式
•    英语单词拼写练习
•    实时语音朗读
•    智能错题记录
•    学习进度可视化

### 词库管理

•    单词的增删改查
•    英标输入辅助系统
•    词库实时统计

### 界面特点

- 无边框窗口设计
- 置顶显示功能
- 友好的用户提示界面
- 响应式交互设计

## 技术栈

- **前端框架**: Qt (C++)
- **数据库**: MySQL
- **音频处理**: Windows Media Control Interface (MCI)
- **多线程处理**: 使用C++标准库线程

## 系统要求

- Windows操作系统
- MySQL数据库服务
- 支持Qt的环境

## 安装说明

1. 克隆仓库到本地
   
   ```
   git clone https://github.com/yourusername/dzy.git
   ```

2. 配置MySQL数据库
   
   - 创建名为`user_database`和`vocabulary`的数据库
   - 在`database.cpp`文件中配置数据库连接信息

3. 使用Qt Creator或Visual Studio打开项目文件`dzy.sln`

4. 编译并运行项目

## 使用指南

1. 启动应用程序
2. 可选择注册新用户或使用游客模式
3. 使用主界面上的功能按钮进行单词学习和管理
4. 可通过设置按钮调整应用程序配置
5. Alt键可触发单词发音
6. Ctrl键显示输入提示

## 项目结构

vocabulary-learning-assistant/

├── src/

│   ├── main.cpp          # 程序入口

│   ├── zhu.cpp           # 主要功能实现

│   ├── database.cpp      # 数据库操作

│   └── dzy.cpp           # 辅助功能类

├── include/

│   ├── zhu.h             # 主类头文件

│   ├── database.h        # 数据库接口

│   └── dzy.h             # 辅助类定义

├── resources/

│   ├── audio/            # 音频资源

│   └── images/           # 图片资源

└── docs/                 # 文档

## 许可证

本项目采用 GNU General Public License v3.0 许可证 - 详情请参阅 [LICENSE.txt](LICENSE.txt) 文件

## 联系方式

如有任何问题或建议，请通过以下方式联系我们：

- 项目GitHub仓库: [https://github.com/[hazhenyu/BuXiangBeiDanCi](https://github.com/hazhenyu/BuXiangBeiDanCi)](https://github.com/yourusername/dzy)
- 电子邮件: 2499816052@qq.com
