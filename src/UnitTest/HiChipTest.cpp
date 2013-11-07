#include "HiChipTest.h"
#include <QDebug>
#include <guid.h>
#include <IEventRegister.h>
#include <IDeviceSearch.h>
#include <QDateTime>
#include <QWaitCondition>
#include <QMutex>

#define START_HICHIPSEARCH_UNIT_TEST(ii) 	IDeviceSearch * ii = NULL; \
	pcomCreateInstance(CLSID_HiChipSearch,NULL,IID_IDeviceSearch,(void **)&ii); \
	QVERIFY2(NULL != ii,"Create device manager instance");\
	QMutex Sig;\
	Sig.lock();

#define END_HICHIPSEARCH_UNIT_TEST(ii)		Sig.unlock();\
	ii->Release(); 

//����������
//ǰ�ã�,���������������һ̨IPC����¼Ĭ�ϲ�����ȷ�����Թ��������ݲ����޸ġ�
//1,��ע���¼�SearchDeviceSuccess������start������ʼ������ͨ���ص�����ȡ��							��start����0��ȡ�������ݣ�stop����0,����ȡ������
//��ѯ��Ϣ�����úõ����ݶԱȣ��鿴�Ƿ�һ�¡�����stop������  
//2,ע���¼�SearchDeviceSuccess������start������ʼ������ͨ���ص�����ȡ��							��start����0���ص�ȡ�õ����������õ�����һ�£�stop����0,����ȡ������
//��ѯ��Ϣ�����úõ����ݶԱȣ��鿴�Ƿ�һ�¡� ����stop������ 
//3,ע���¼�SearchDeviceSuccess������start������ʼ�������ٴε���start����ͨ							��start��һ�η���0��ȡ�õĲ�ѯ��Ϣ�����õ�һ�£��ڶ��η���-1��stop����0,����ȡ������
//���ص�����ȡ�ò�ѯ��Ϣ�����úõ����ݶԱȣ��鿴�Ƿ�һ�¡�����stop������
//4,ע���¼�SearchDeviceSuccess������start������ʼ����������start��������							��start����0��ȡ�õĲ�ѯ��Ϣ�����õ�һ�£�stop��һ�η���0���ڶ��η���-1,����ȡ������
//�ε���stop����ͨ���ص�����ȡ�ò�ѯ��Ϣ�����úõ����ݶԱȣ��鿴�Ƿ�һ�¡�
//5,ע���¼�SearchDeviceSuccess������start������ʼ����������flush����ˢ�£�							��start����0��flush����0���ص�ȡ�õ����������õ�����һ�£�stop����0,����ȡ������
//ͨ���ص�����ȡ�ò�ѯ��Ϣ�����úõ����ݶԱȣ��鿴�Ƿ�һ�¡�����stop������ 
//6,ע���¼�SearchDeviceSuccess������start������ʼ��������¼�յ����ݵ�ʱ��T1��						��start����OK��setInterval����OK��T2>T1��T2=30��
//����setInterval��������ʱ����Ϊ30S����¼�յ����ݵ�ʱ��T2���Ƚ�T1,T2��


int __cdecl OUTPUT(QString evname,QVariantMap info,void *puser)
{
	if ( "SearchDeviceSuccess" == evname )
	{
		QVariantMap::iterator it;
		for (it = info.begin(); it != info.end(); it++)
		{
			qDebug()<<it.key()<<":"<<it.value().toString();
		}
	}
	return 0;
}


HiChipUnitTest::HiChipUnitTest()
{

}

HiChipUnitTest::~HiChipUnitTest()
{

}


void WaitSeconds(int second)
{
	QDateTime start = QDateTime::currentDateTime();
	while(1)
	{
		QDateTime end = QDateTime::currentDateTime();
		if (end.toTime_t() - start.toTime_t() > second)
		{
			break;
		}		
	}
}

void HiChipUnitTest::DeviceTestCase1()
{
	START_HICHIPSEARCH_UNIT_TEST(ITest);

	int Ret = ITest->Start();
	QVERIFY2(0 == Ret, "start thread");

	WaitSeconds(15);
	Ret = ITest->Stop();
	QVERIFY2(0 == Ret, "stop thread");

	END_HICHIPSEARCH_UNIT_TEST(ITest);
}

void HiChipUnitTest::DeviceTestCase2()
{
	START_HICHIPSEARCH_UNIT_TEST(ITest);

	QString evName("SearchDeviceSuccess");
	int Ret = ITest->QueryEventRegister()->registerEvent(evName, OUTPUT,NULL);
	QVERIFY2(IEventRegister::OK == Ret, "Regist event success");

	Ret = ITest->Start();
	QVERIFY2(0 == Ret, "start thread");
	WaitSeconds(15);
	Ret = ITest->Stop();
	QVERIFY2(0 == Ret, "stop thread");

	END_HICHIPSEARCH_UNIT_TEST(ITest);
}


void HiChipUnitTest::DeviceTestCase3()
{
	START_HICHIPSEARCH_UNIT_TEST(ITest);

	QString evName("SearchDeviceSuccess");
	int Ret = ITest->QueryEventRegister()->registerEvent(evName, OUTPUT,NULL);
	QVERIFY2(IEventRegister::OK == Ret, "Regist event success");
	Ret = ITest->Start();
	QVERIFY2(0 == Ret, "start thread");

	WaitSeconds(10);
	Ret = ITest->Start();
	QVERIFY2(-1 == Ret, "start thread again");

	Ret = ITest->Stop();
	QVERIFY2(0 == Ret, "stop thread");

	END_HICHIPSEARCH_UNIT_TEST(ITest);
}

void HiChipUnitTest::DeviceTestCase4()
{
	START_HICHIPSEARCH_UNIT_TEST(ITest);

	QString evName("SearchDeviceSuccess");
	int Ret = ITest->QueryEventRegister()->registerEvent(evName, OUTPUT,NULL);
	QVERIFY2(IEventRegister::OK == Ret, "Regist event success");
	Ret = ITest->Start();
	QVERIFY2(0 == Ret, "start thread");

	WaitSeconds(10);
	Ret = ITest->Stop();
	QVERIFY2(0 == Ret, "stop thread");

	Ret = ITest->Stop();
	QVERIFY2(-1 == Ret, "stop thread again");

	END_HICHIPSEARCH_UNIT_TEST(ITest);
}

void HiChipUnitTest::DeviceTestCase5()
{
	START_HICHIPSEARCH_UNIT_TEST(ITest);

	QString evName("SearchDeviceSuccess");
	int Ret = ITest->QueryEventRegister()->registerEvent(evName, OUTPUT,NULL);
	QVERIFY2(IEventRegister::OK == Ret, "Regist event success");
	Ret = ITest->Start();
	QVERIFY2(0 == Ret, "start thread");

	WaitSeconds(10);
	Ret = ITest->Flush();
	QVERIFY2(0 == Ret, "flush");

	Ret = ITest->Stop();
	QVERIFY2(0 == Ret, "stop thread");

	END_HICHIPSEARCH_UNIT_TEST(ITest);
}
//�������޷���¼ʱ����
void HiChipUnitTest::DeviceTestCase6()
{
	START_HICHIPSEARCH_UNIT_TEST(ITest);

	QString evName("SearchDeviceSuccess");
	int Ret = ITest->QueryEventRegister()->registerEvent(evName, OUTPUT,NULL);
	QVERIFY2(IEventRegister::OK == Ret, "Regist event success");
	Ret = ITest->Start();
	QVERIFY2(0 == Ret, "start thread");

	WaitSeconds(10);
	Ret = ITest->setInterval(10);
	QVERIFY2(0 == Ret, "change time interval");

	Ret = ITest->Stop();
	QVERIFY2(0 == Ret, "stop thread");

	END_HICHIPSEARCH_UNIT_TEST(ITest);
}

