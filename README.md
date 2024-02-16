# ChatApplication
高仿QQ
资源文件加QQ：758068382
一、ChatApplication项目环境搭建：
1、下载安装软件
安装Visual Studio 2017、Qt、MySQL、Navicat for MySQL
https://pan.baidu.com/s/1tZT6rhSVanO_lyP5Ama7fQ?pwd=0000 
2、安装MySQL及驱动
注：Qt的构建套件始终为Desktop Qt 5.14.2 MSVC2017 64bit
1.以管理员身份打开命令提示符CMD
2.cd D:\mysql_5.6.37\bin（如果cd后没反应，执行D:）
3.mysqld install
4.net start mysql
5.打开Navicat for MySQL，新建连接（无密码），双击数据库连接，连接成功后点击“用户”，点击root@localhost，输入新密码，点击保存
6.在Qt打开D:\Qt\Qt5.14.2\5.14.2\Src\qtbase\src\plugins\sqldrivers\mysql\mysql.pro
7.更改mysql.pro文件内容为以下内容并运行程序：
TARGET = qsqlmysql
HEADERS += $$PWD/qsql_mysql_p.h
SOURCES += $$PWD/qsql_mysql.cpp $$PWD/main.cpp
#QMAKE_USE += mysql
OTHER_FILES += mysql.json
INCLUDEPATH += D:/mysql_5.6.37/include
LIBS += D:/mysql_5.6.37/lib/libmysql.lib
DESTDIR = ../mysql/lib
PLUGIN_CLASS_NAME = QMYSQLDriverPlugin
include(../qsqldriverbase.pri)
8.拷贝D:\plugins\sqldrivers\qsqlmysql.dll和D:\plugins\sqldrivers\qsqlmysqld.dll到D:\Qt\Qt5.14.2\5.14.2\msvc2017_64\plugins\sqldrivers
9.拷贝D:\mysql_5.6.37\lib\libmysql.dll到D:\Qt\Qt5.14.2\5.14.2\msvc2017_64\bin
二、ChatApplication项目数据库配置：
1、创建帐户表
CREATE TABLE `tab_accounts` (
  `employeeID` int(11) NOT NULL COMMENT '员工工号',
  `account` char(20) NOT NULL COMMENT '账号',
  `code` char(20) NOT NULL COMMENT '密码',
  PRIMARY KEY (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
2、创建部门表
CREATE TABLE `tab_department` (
  `departmentID` int(11) NOT NULL AUTO_INCREMENT COMMENT '公司部门号（部门群号）',
  `department_name` char(20) NOT NULL COMMENT '部门名称',
  `picture` char(100) NOT NULL COMMENT '部门头像路径',
  `sign` char(50) NOT NULL COMMENT '部门口号',
  PRIMARY KEY (`departmentID`)
) ENGINE=InnoDB AUTO_INCREMENT=2004 DEFAULT CHARSET=utf8;
3、创建员工表
CREATE TABLE `tab_employees` (
  `departmentID` int(11) NOT NULL COMMENT '部门ID，群号',
  `employeeID` int(11) NOT NULL AUTO_INCREMENT COMMENT '工号，qq号',
  `employee_name` char(20) NOT NULL COMMENT '员工姓名',
  `employee_sign` char(200) DEFAULT NULL COMMENT '个性签名',
  `status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '1有效 0注销',
  `picture` char(100) NOT NULL COMMENT '员工头像路径',
  `online_status` tinyint(4) NOT NULL DEFAULT '1' COMMENT '在线状态 1离线 2在线',
  PRIMARY KEY (`employeeID`)
) ENGINE=InnoDB AUTO_INCREMENT=10012 DEFAULT CHARSET=utf8;
4、添加帐户表数据
INSERT INTO tab_accounts (employeeID, account, code) VALUES
(10000, 'tianyun', '142857'),
(10001, 'renyi', 'renyi'),
(10002, 'rener', 'rener'),
(10003, 'rensan', 'rensan'),
(10004, 'yanyi', 'yanyi'),
(10005, 'yaner', 'yaner'),
(10006, 'yansan', 'yansan'),
(10007, 'shiyi', 'shiyi'),
(10008, 'shier', 'shier'),
(10009, 'shisan', 'shisan'),
(10010, '小红', 'xiaohong'),
(10011, '小花', 'xiaohua'),
(10012, '小白', 'xiaobai');
5、添加部门表数据
INSERT INTO tab_department (departmentID, department_name, picture, sign) VALUES
(2000, '公司群', 'Resources\\qtqq_images\\gsq.png', '越分享，越拥有'),
(2001, '人事部', 'Resources\\qtqq_images\\rsg.png', '不好意思，你被炒了'),
(2002, '研发部', 'Resources\\qtqq_images\\yfb.png', '不会CV大法的程序猿不是一个合格的程序猿'),
(2003, '市场部', 'Resources\\qtqq_images\\scb.png', '走过路过，不要错过');
6、添加员工表数据
INSERT INTO tab_employees (departmentID, employeeID, employee_name, employee_sign, status, picture, online_status) VALUES
(2002, 10000, 'tianyun', '天才在此', 1, 'Resources\\MainWindow\\head_ps.jpg', 1),
(2001, 10001, 'renyi', '哈哈哈', 1, 'Resources\\qtqq_images\\r1.png', 1),
(2001, 10002, 'rener', '奋斗着', 1, 'Resources\\qtqq_images\\r2.png', 1),
(2001, 10003, 'rensan', '加油', 1, 'Resources\\qtqq_images\\r3.png', 1),
(2002, 10004, 'yanyi', '修bug中。。。', 1, 'Resources\\qtqq_images\\y1.png', 1),
(2002, 10005, 'yaner', '加班中', 1, 'Resources\\qtqq_images\\y2.png', 1),
(2002, 10006, 'yansan', '写bug中...', 1, 'Resources\\qtqq_images\\y3.png', 1),
(2003, 10007, 'shiyi', '梦游中', 1, 'Resources\\qtqq_images\\s1.png', 1),
(2003, 10008, 'shier', '前进', 1, 'Resources\\qtqq_images\\s2.png', 1),
(2003, 10009, 'shisan', '12345678..', 1, 'Resources\\qtqq_images\\s3.png', 1),
(2001, 10010, '小红', '红红火火', 1, 'Resources\\qtqq_images\\r1.png', 1),
(2002, 10011, '小花', '小花小草', 1, 'Resources\\qtqq_images\\y1.png', 1),
(2003, 10012, '小白', '蓝天白云', 1, 'Resources\\qtqq_images\\s1.png', 1);
