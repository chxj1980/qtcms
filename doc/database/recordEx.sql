create table search_record
(
id integer primary key autoincrement,
nWndId integer,
nRecordType integer,
nStartTime integer,
nEndTime integer
);
create table record(
id integer primary key autoincrement,
nWndId integer,
nRecordType integer,
nStartTime integer,
nEndTime integer,
sFilePath char(64)
);
create table RecordFileStatus(
id integer primary key autoincrement,
sFilePath char(64),
nLock integer, //0 :������1������
nDamage integer, //0:���ã�1���ļ���
nInUse integer, //0��û��ʹ�ã�1:�Ѿ�ʹ��
nFileNum integer//·��ֵ��0000/0000/0000/0001.dat ==1; 
);