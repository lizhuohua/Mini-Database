--create语句
create table school(name char);
create table student(
	id char primary key,
	name char not null,
	age integer,
	school char foreign key references school(name)
);

--insert语句
insert into school values('USTC');
insert into school values('PKU');
insert into school values('THU');
insert into student values('PB13011071','lzh',21,'USTC');
insert into student values('PB13011001','abc',20,'USTC');

--select语句
select * from student;
select id,name from student where age=20;

--update语句
update student set name='lizhuohua' where id='PB13011071';

--delete语句
delete from student where name='lizhuohua';

--Primary Key
insert into student values('PB13011001','a',22,'PKU');

--Not Null
insert into student values('PB13011071',null,21,'USTC');

--类型检查
insert into student values(20,'lzh',20,'USTC');

--Foreign Key
insert into student values('PB13011071','lzh',21,'SJTU');
