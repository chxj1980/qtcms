#include "RemoteBackupTest.h"
#include <guid.h>
#include "IEventRegister.h"
#include "IRemoteBackup.h"

#define START_REMOTEBACKUP_UNIT_TEST(ii) 	IRemoteBackup * ii = NULL; \
    pcomCreateInstance(CLSID_DeviceClient, NULL, IID_IRemoteBackup,(void **)&ii); \
    QVERIFY2(NULL != ii,"Create remotebackup instance");

#define END_REMOTEBACKUP_UNIT_TEST(ii) ii->Release(); 

bool stopbackup ;
RemoteBackupTest::RemoteBackupTest()
{
}

RemoteBackupTest::~RemoteBackupTest()
{
}

//���ݽ����¼��ص�
int cbStopBackup(QString evName, QVariantMap evMap, void *pUser)
{ 
    if("backupEvent" == evName)
    {
		
		if ("stopBackup" == evMap["types"].toString())
		{
			stopbackup = true;
		}
		else if ("startBackup" == evMap["types"].toString())
		{
		}
		else if ("diskfull" == evMap["types"].toString())
		{
			stopbackup = true;
		}
		else if ("backupFinished" == evMap["types"].toString())
		{
			stopbackup = true;
		}
		else if ("noStream" == evMap["types"].toString())
		{
			stopbackup = true;
		}
		qDebug()<<evMap["types"].toString();
		
    }

    return 0;
}
//ǰ������ ���ڷ���D  ��D�´���Ŀ¼RemoteBackup
//���ڿ������� ip"192.168.2.132" �˿�8785
//׼���Ϸ�ͨ����0~31  �Ϸ�ʱ�� ��ʼʱ��(ϵͳ��ǰʱ��-1Сʱ)<����ʱ�䣨ϵͳ��ǰʱ�䣩 �Ϸ�·��F:/RemoteBackup

//1.  ����������� 
void RemoteBackupTest::RemoteBackupCase1()
{
    START_REMOTEBACKUP_UNIT_TEST(Itest);
	
	int nRet;
	int channel = 1;
	int types = 15;
	QString sAddr = "192.168.2.132";
	unsigned int uiport = 8785;
	QString eseeid = "12345678";
	QDateTime etime =  QDateTime::currentDateTime();
	QDateTime stime = QDateTime(etime.date(),QTime(etime.time().hour()-1,etime.time().minute()));
	QString spath = "F:/RemoteBackup";
	/*QString sAddr = "192.168.2.113";
	unsigned int uiport = 80*/;
	/*QDateTime stime = QDateTime::fromString("2013-3-23 12:00:00","yyyy-MM-dd hh:mm:ss");
	QDateTime etime =  QDateTime::fromString("2013-3-23 12:30:00","yyyy-MM-dd hh:mm:ss");*/


	//����������Ϣ����(����������"129.168.2.210")
	nRet = Itest->startBackup("129.168.2.210",uiport,eseeid,channel,types,etime,stime,spath);
	QVERIFY2(IRemoteBackup::E_PARAMETER_ERROR == nRet  ,"Illegal host");

	//����ͨ���Ų���(-1)
	nRet = Itest->startBackup(sAddr,uiport,eseeid,-1,types,stime,etime,spath);
	QVERIFY2(IRemoteBackup::E_PARAMETER_ERROR == nRet  ,"Illegal channel");

	//����ʱ�����(��ֹʱ��Ի�)
	nRet = Itest->startBackup(sAddr,uiport,eseeid,channel,types,etime,stime,spath);
	QVERIFY2(IRemoteBackup::E_PARAMETER_ERROR == nRet  ,"Illegal times");

	//���󱸷�·������("ADBC:/DDDD")
	nRet = Itest->startBackup(sAddr,uiport,eseeid,channel,types,stime,etime,"ADBC:/DDDD");
	QVERIFY2(IRemoteBackup::E_PARAMETER_ERROR == nRet  ,"Illegal path");
 
	//�Ϸ���������
	nRet = Itest->startBackup(sAddr,uiport,eseeid,channel,types,stime,etime,spath);
	QVERIFY2(IRemoteBackup::OK == nRet  ,"start backup! legal param");
	QTest::qSleep(20000);
	Itest->stopBackup();

    END_REMOTEBACKUP_UNIT_TEST(Itest);
}
 //2.  �ֶ�����/�رձ��� ���¼��ص�
void RemoteBackupTest::RemoteBackupCase2()
{
	START_REMOTEBACKUP_UNIT_TEST(Itest);

	int nRet;
	int channel = 1;
	int types = 15;
	QString sAddr = "192.168.2.132";
	unsigned int uiport = 8785;
	QString eseeid = "12345678";
	QDateTime etime =  QDateTime::currentDateTime();
	QDateTime stime = QDateTime(etime.date(),QTime(etime.time().hour()-1,etime.time().minute()));
	QString spath = "F:/RemoteBackup";

	IEventRegister *pRegist = NULL;
	nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
	QVERIFY2(S_OK == nRet, "QueryInterface Error!");
	nRet = pRegist->registerEvent("backupEvent", cbStopBackup, NULL);
	QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");

	nRet = Itest->startBackup(sAddr,uiport,eseeid,channel,types,stime,etime,spath);
	QVERIFY2(IRemoteBackup::OK == nRet  ,"start backup! legal param");

	QTest::qSleep(20000);//20��

	nRet = nRet = Itest->stopBackup();
	QVERIFY2(IRemoteBackup::OK == nRet  ,"stop backup! ");

	END_REMOTEBACKUP_UNIT_TEST(Itest);
}
// 3. ��ȡ���ݽ��� �� ���ݽ����¼��ص�����
void RemoteBackupTest::RemoteBackupCase3()
{
	START_REMOTEBACKUP_UNIT_TEST(Itest);

	int nRet;
	int channel = 1;
	int types = 15;
	QString sAddr = "192.168.2.132";
	unsigned int uiport = 8785;
	QString eseeid = "12345678";
	QDateTime etime =  QDateTime::currentDateTime();
	QDateTime stime = QDateTime(etime.date(),QTime(etime.time().hour()-1,etime.time().minute()));
	QString spath = "F:/RemoteBackup";

	IEventRegister *pRegist = NULL;
	nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
	QVERIFY2(S_OK == nRet, "QueryInterface Error!");
	nRet = pRegist->registerEvent("backupEvent", cbStopBackup, NULL);
	QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
	pRegist->Release();

	stopbackup = false;
	nRet = Itest->startBackup(sAddr,uiport,eseeid,channel,types,stime,etime,spath);
	QVERIFY2(IRemoteBackup::OK == nRet  ,"start backup! legal param");

	while(!stopbackup)
	{
		qDebug("get the progress is : %.2f%",Itest->getProgress()*100);
		QTest::qSleep(1000);
	}

	END_REMOTEBACKUP_UNIT_TEST(Itest);
}
