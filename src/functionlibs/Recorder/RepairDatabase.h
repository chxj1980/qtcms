#pragma once
class RepairDatabase
{
public:
	RepairDatabase(void);
	~RepairDatabase(void);
public:
	int fixExceptionalData();//0:�ɹ���1��û�н����޸���Ϊ��2�������޸�ʧ�ܵ���Ŀ
private:
	int repairRecordDatabase();
	int repairSearchDatabase(); 
};

