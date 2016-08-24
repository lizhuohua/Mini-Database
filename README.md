# Mini-Database
## 介绍
这是一个简单的SQL解释器。

是USTC岳丽华老师的数据库附加实验，完成实验期末直接加5分，所以不想写也得写。

* create，insert，select，update，delete等SQL语句的简单实现
* select 支持where，但只支持一个，select不支持嵌套
* 支持整数和字符串两种数据类型
* 简单实现了primary key，not null，foreign key，类型检查
* 能拒绝不满足上述完整性要求的语句

## 使用方法
Linux:
确保已经安装了flex和bison
```bash
git clone https://github.com/lizhuohua/Mini-Database.git
cd Mini-Database
make
cd bin
./sql
```
在test目录下有一个实例文件，演示了所实现的功能。