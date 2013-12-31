#include "TestCommonLibSet.h"
#include <guid.h>
#include <ILocalSetting.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlQuery>

#define  START_AREA_UNIT_TEST(ii) ILocalSetting *ii=NULL;\
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_ILocalSetting,(void**)&ii);\
	QVERIFY2(NULL!=ii,"Create ILocalSetting instance");
#define END_AREA_UNIT_TEST(ii) ii->Release();

TestCommonLibSet::TestCommonLibSet(void)
{
}


TestCommonLibSet::~TestCommonLibSet(void)
{
}
//����������

void TestCommonLibSet::beforeAreaTest()
{
	QSqlDatabase db;
	if (QSqlDatabase::contains("LocalSettingTest"))
	{
		db=QSqlDatabase::database("LocalSettingTest");
	}
	else{
		db=QSqlDatabase::addDatabase("QSQLITE","DisksSettingTest");
	}
	QString sAppPath=QCoreApplication::applicationDirPath();
	QString sDatabasePath=sAppPath+"/system.db";
	db.setDatabaseName(sDatabasePath);
	db.open();

	QSqlQuery query1(db);
	query1.prepare("delete from general_setting");
	query1.exec();
	db.close();
}
//1.���� int setLaunguage(const QString & sLanguage)���������
//ǰ��������������ݿ�
//���裺
//step1������Ƿ������ַ����硰agagl���������ַ���μ� ����1��Ԥ�ڣ�����E_PARAMETER_ERROR 
//step2:����QString getLaunguage();Ԥ�ڣ����ء�en_GB����Ĭ��ֵ)
//step3:  ����������������ȷ����������1���硰zh_CN�� ;Ԥ�ڣ� ����S_OK  
//step4:����QString getLaunguage();Ԥ�ڣ�����zh_CN
//step5:����Ƿ������ַ����硰agagl���������ַ���μ� ����1 ;Ԥ�ڣ�����E_PARAMETER_ERROR
//step6:����QString getLaunguage() ;Ԥ�ڣ�����zh_CN
void TestCommonLibSet::test1()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	QString m_sLanguage="agagl";
	int nRet=-1;
	nRet=ILocalSet->setLanguage(m_sLanguage);
	QVERIFY2(ILocalSetting::E_PARAMETER_ERROR==nRet,"step 1:return");

	m_sLanguage.clear();
	m_sLanguage=ILocalSet->getLanguage();
	QVERIFY2("en_GB"==m_sLanguage,"step 2:return");

	m_sLanguage.clear();
	nRet=-1;
	m_sLanguage.append("zh_CN");
	nRet=ILocalSet->setLanguage(m_sLanguage);
	QVERIFY2(ILocalSetting::OK==nRet,"step 3:return");

	m_sLanguage.clear();
	m_sLanguage=ILocalSet->getLanguage();
	QVERIFY2("zh_CN"==m_sLanguage,"step 4:return");

	m_sLanguage.clear();
	nRet=-1;
	nRet=ILocalSet->setLanguage("agagl");
	QVERIFY2(ILocalSetting::E_PARAMETER_ERROR==nRet,"step 5:return");

	m_sLanguage=ILocalSet->getLanguage();
	QVERIFY2("zh_CN"==m_sLanguage,"step 6:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//2.���� QString getLaunguage()���������
//ǰ��������������ݿ�
//���裺
//step1�����ú�����Ԥ�ڣ����ء�en_GB����Ĭ��ֵ��
//step2:����setLaunguage����zh_CN����;Ԥ�ڣ�����S_OK
//step3: ���ú��� ;Ԥ�ڣ�����zh_CN  
void TestCommonLibSet::test2()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	QString m_sLanguage;
	m_sLanguage=ILocalSet->getLanguage();
	QVERIFY2("en_GB"==m_sLanguage,"step 1:return");
	m_sLanguage.clear();
	int nRet=-1;
	nRet=ILocalSet->setLanguage("zh_CN");
	QVERIFY2(ILocalSetting::OK==nRet,"step 2:return");
	m_sLanguage=ILocalSet->getLanguage();
	QVERIFY2("zh_CN"==m_sLanguage,"step 3:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//3.���� int setAutoPollingTime(int aptime)���������
//ǰ��������������ݿ�,�����������뷶ΧΪ��30-86400��
//���裺
//step1������������ ��0����Ԥ�ڣ�����E_PARAMETER_ERROR
//step2:����int getAutoPollingTime();Ԥ�ڣ�����30��Ĭ��ֵ��
//step3:������������ ��60��;Ԥ�ڣ� ����S_OK  
//step4:����int getAutoPollingTime();Ԥ�ڣ����ء�60��
//step5:���������� ��0��;Ԥ�ڣ�����E_PARAMETER_ERROR
//step6:����int getAutoPollingTime();Ԥ�ڣ� ���ء�60�� 
void TestCommonLibSet::test3()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	nRet=ILocalSet->setAutoPollingTime(0);
	QVERIFY2(ILocalSetting::E_PARAMETER_ERROR==nRet,"step 1:return");
	nRet=-1;
	nRet=ILocalSet->getAutoPollingTime();
	QVERIFY2(30==nRet,"step 2:return");
	nRet=-1;
	nRet=ILocalSet->setAutoPollingTime(60);
	QVERIFY2(ILocalSetting::OK==nRet,"step 3:return");
	nRet=-1;
	nRet=ILocalSet->getAutoPollingTime();
	QVERIFY2(60==nRet,"step 4:return");
	nRet=-1;
	nRet=ILocalSet->setAutoPollingTime(0);
	QVERIFY2(ILocalSetting::E_PARAMETER_ERROR==nRet,"step 5:return");
	nRet=-1;
	nRet=ILocalSet->getAutoPollingTime();
	QVERIFY2(60==nRet,"step 6:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//4.���� int getAutoPollingTime()���������
//ǰ��������������ݿ�
//���裺
//step1�����ú�����Ԥ�ڣ�����30
//step2: ���ú���setAutoPollingTime��60�� ;Ԥ�ڣ�����S_OK
//step3: ���ú��� ;Ԥ�ڣ����� 60 
void TestCommonLibSet::test4()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	nRet=ILocalSet->getAutoPollingTime();
	QVERIFY2(30==nRet,"step 1:return");
	nRet=-1;
	nRet=ILocalSet->setAutoPollingTime(60);
	QVERIFY2(nRet==60,"step 2:return");
	nRet=-1;
	nRet=ILocalSet->getAutoPollingTime();
	QVERIFY2(60==nRet,"step 3:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//5.����int setSplitScreenMode(const QString &smode)���������
//ǰ��������������ݿ�
//���裺
//step1:������� Ϊ�գ�Ԥ�ڣ�����E_PARAMETER_ERROR
//step2: ����QString getSplitScreenMode() ;Ԥ�ڣ����� ��div4_4��(Ĭ��ֵ)
//step3: ������� Ϊ��div2_2�� ;Ԥ�ڣ� ����S_OK  
//step4:����QString getSplitScreenMode();Ԥ�ڣ����� div2_2 
//step5:������� Ϊ��;Ԥ�ڣ�����E_PARAMETER_ERROR
//step6:����QString getSplitScreenMode();Ԥ�ڣ� ���� div2_2 
void TestCommonLibSet::test5()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	QString m_smode;
	nRet=ILocalSet->setSplitScreenMode(m_smode);
	QVERIFY2(ILocalSetting::E_PARAMETER_ERROR==nRet,"step 1:return");
	m_smode=ILocalSet->getSplitScreenMode();
	QVERIFY2("div4_4"==m_smode,"step 2:return");
	nRet=-1;
	m_smode.clear();
	nRet=ILocalSet->setSplitScreenMode("div2_2");
	QVERIFY2(ILocalSetting::OK==nRet,"step 3:return");
	m_smode=ILocalSet->getSplitScreenMode();
	QVERIFY2("div2_2"==m_smode,"step 4:return");
	nRet=-1;
	m_smode.clear();
	nRet=ILocalSet->setSplitScreenMode(m_smode);
	QVERIFY2(ILocalSetting::E_PARAMETER_ERROR==nRet,"step 5:return");
	nRet=-1;
	m_smode.clear();
	nRet=ILocalSet->setSplitScreenMode("div2_2");
	QVERIFY2("div2_2"==m_smode,"step 6:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//6.���� int getSplitScreenMode()���������
//ǰ��������������ݿ�
//���裺
//step1�����ú�����Ԥ�ڣ����� ��div4_4����Ĭ�ϣ�
//step2: ���ú���setSplitScreenMode����div1_1����;Ԥ�ڣ�����S_OK
//step3: ���ú��� ;Ԥ�ڣ�����div1_1
void TestCommonLibSet::test6()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	QString m_smode;
	m_smode=ILocalSet->getSplitScreenMode();
	QVERIFY2("div4_4"==m_smode,"step 1:return");
	m_smode.clear();
	m_smode.append("div1_1");
	nRet=ILocalSet->setSplitScreenMode(m_smode);
	QVERIFY2(ILocalSetting::OK==nRet,"step 2:return");
	m_smode.clear();
	m_smode=ILocalSet->getSplitScreenMode();
	QVERIFY2("div1_1"==m_smode,"step 1:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//7.����int setAutoLogin(bool alogin)���������
//ǰ��������������ݿ�
//���裺
//step1:���� ��ȷ���� ��false����Ԥ�ڣ����� S_OK
//step2: ����bool getAutoLogin();Ԥ�ڣ����� ��false��
//step3: ���� ��ȷ���� ��true�� ;Ԥ�ڣ� ����S_OK  
//step4:����bool getAutoLogin();Ԥ�ڣ� ���� ��true�� 
void TestCommonLibSet::test7()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	nRet=ILocalSet->setAutoLogin(false);
	QVERIFY2(ILocalSetting::OK==nRet,"step 1:return");
	bool bRet=true;
	bRet=ILocalSet->getAutoLogin();
	QVERIFY2(false==bRet,"step 2:return");
	nRet=-1;
	nRet=ILocalSet->setAutoLogin(true);
	QVERIFY2(ILocalSetting::OK,"step 3:return");
	bRet=false;
	bRet=ILocalSet->getAutoLogin();
	QVERIFY2(true==bRet,"step 4:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//8.���� bool getAutoLogin()���������
//ǰ��������������ݿ�
//���裺
//step1�����ú�����Ԥ�ڣ����� false ��Ĭ��ֵ��
//step2:  ���ú���int setAutoLogin(true) ;Ԥ�ڣ�����S_OK
//step3: ���ú��� ;Ԥ�ڣ����� true
void TestCommonLibSet::test8()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	bool bRet=true;
	bRet=ILocalSet->getAutoLogin();
	QVERIFY2(false==bRet,"step 1:return");
	int nRet=-1;
	nRet=ILocalSet->setAutoLogin(true);
	QVERIFY2(ILocalSetting::OK==nRet,"step 2:return");
	bRet=false;
	bRet=ILocalSet->getAutoLogin();
	QVERIFY2(true==bRet,"step 3:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//9.����int setAutoSyncTime(bool synctime)���������
//ǰ��������������ݿ�
//���裺
//step1:���� ��ȷ���� ��false����Ԥ�ڣ����� S_OK
//step2: ����bool getAutoSyncTime();Ԥ�ڣ����� ��false��
//step3: ���� ��ȷ���� ��true�� ;Ԥ�ڣ� ����S_OK  
//step4:����bool getAutoSyncTime();Ԥ�ڣ� ���� ��true�� 
void TestCommonLibSet::test9()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	nRet=ILocalSet->setAutoSyncTime(false);
	QVERIFY2(ILocalSetting::OK==nRet,"step 1:return");
	bool bRet=true;
	bRet=ILocalSet->getAutoSyncTime();
	QVERIFY2(false==bRet,"step 2:return");
	nRet=-1;
	nRet=ILocalSet->setAutoSyncTime(true);
	QVERIFY2(ILocalSetting::OK==nRet,"step 3:return");
	bRet=false;
	bRet=ILocalSet->getAutoSyncTime();
	QVERIFY2(true==bRet,"step 4:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//10.���� bool getAutoSyncTime()���������
//ǰ��������������ݿ�
//���裺
//step1�����ú�����Ԥ�ڣ����� false ��Ĭ��ֵ��
//step2:  ���ú���int setAutoSyncTime(true);Ԥ�ڣ�����S_OK
//step3: ���ú��� ;Ԥ�ڣ����� true
void TestCommonLibSet::test10()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	bool bRet=true;
	bRet=ILocalSet->getAutoSyncTime();
	QVERIFY2(false==bRet,"step 1:return");
	int nRet=-1;
	nRet=ILocalSet->setAutoSyncTime(true);
	QVERIFY2(ILocalSetting::OK==nRet,"step 2:return");
	bRet=false;
	bRet=ILocalSet->getAutoSyncTime();
	QVERIFY2(true==bRet,"step 3:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//11.����int setAutoConnent(bool aconnent)���������
//ǰ��������������ݿ�
//���裺
//step1:���� ��ȷ���� ��false����Ԥ�ڣ����� S_OK
//step2: ����bool getAutoConnent();Ԥ�ڣ����� ��false��
//step3: ���� ��ȷ���� ��true�� ;Ԥ�ڣ� ����S_OK  
//step4:����bool getAutoConnent();Ԥ�ڣ� ���� ��true�� 
void TestCommonLibSet::test11()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	int nRet=-1;
	nRet=ILocalSet->setAutoConnect(false);
	QVERIFY2(ILocalSetting::OK==nRet,"step 1:return");
	bool bRet=true;
	bRet=ILocalSet->getAutoConnect();
	QVERIFY2(false==bRet,"step 2:return");
	nRet=-1;
	nRet=ILocalSet->setAutoConnect(true);
	QVERIFY2(ILocalSetting::OK==nRet,"step 3:return");
	bRet=false;
	bRet=ILocalSet->getAutoConnect();
	QVERIFY2(true==bRet,"step 4:return");
	END_AREA_UNIT_TEST(ILocalSet);
}
//12.���� bool getAutoConnent()���������
//ǰ��������������ݿ�
//���裺
//step1�����ú�����Ԥ�ڣ����� false ��Ĭ��ֵ��
//step2:  ���ú���int setAutoConnent(true);Ԥ�ڣ�����S_OK
//step3: ���ú��� ;Ԥ�ڣ����� true
void TestCommonLibSet::test12()
{
	beforeAreaTest();
	START_AREA_UNIT_TEST(ILocalSet);
	bool bRet=true;
	bRet=ILocalSet->getAutoConnect();
	QVERIFY2(false==bRet,"step 1:return");
	int nRet=-1;
	nRet=ILocalSet->setAutoConnect(true);
	QVERIFY2(ILocalSetting::OK==nRet,"step 2:return");
	bRet=false;
	bRet=ILocalSet->getAutoConnect();
	QVERIFY2(true==bRet,"step 3:return");
	END_AREA_UNIT_TEST(ILocalSet);
}

