#include "dvrsearchtest.h"
#include <guid.h>
#include <IEventRegister.h>
#include <IDeviceSearch.h>


#define START_DVRSEARCH_UNIT_TEST(ii) 	IDeviceSearch * ii = NULL; \
	pcomCreateInstance(CLSID_DvrSearch, NULL, IID_IDeviceSearch,(void **)&ii); \
	QVERIFY2(NULL != ii,"Create dvr search instance");

#define END_DVRSEARCH_UNIT_TEST(ii) ii->Release(); 

DvrSearchTest::DvrSearchTest()
{

}

DvrSearchTest::~DvrSearchTest()
{

}

//��������: 
//ǰ��: ��2̨DVR, ���ò���.      �ٶ��������ص����� , ��ÿ�εõ������ݽ��д�ӡ���.
int DvrSearchTest::cbDeviceFoundTest(QString evName,QVariantMap evMap,void *pUser)
{
     QVariantMap::const_iterator it;
     for (it= evMap.begin();it != evMap.end(); ++it )
     {
         QString sKey = it.key();
         QString sValue = it.value().toString();
         printf("%s\t%s\n", sKey.toLatin1().data(),sValue.toLatin1().data());
     }
     
    return 0;
}
int DvrSearchTest::cbDeviceSetTest(QString evName,QVariantMap evMap,void *pUser)
{
    QVariantMap::const_iterator it;
    for (it= evMap.begin();it != evMap.end(); ++it )
    {
        QString sKey = it.key();
        QString sValue = it.value().toString();
        printf("%s\t%s\n", sKey.toLatin1().data(),sValue.toLatin1().data());
    }
    return 0;
}

//1  ʹ���¼���deviceFoundע��, ���������������.          |  Start()����0, Stop()����0, �ص������ܵõ���Ӧ���ݲ��ܴ�ӡ����
//    ʹ���¼���deviceSet ע�� , �����޴��¼�����ʱ���.   |  Start()����0, Stop()����0, ����û�ж�Ӧ��������ʹ�ûص����������ӡ�κ�����
void DvrSearchTest::DvrSearchCase1()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    //step1:
    int nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceFound",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"No this Event");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 15*1000);

    dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");
    //step2:
    nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceSet",DvrSearchTest::cbDeviceSetTest, NULL);
    QVERIFY2(-2 == nRet1,"No this Event");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");
    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//2  ʹ���¼���deviceFoundע��,������Stop()ֹͣ,Ȼ��ˢ���ٵ���Start()����, �����ֹͣ
//       |    Start()��Stop()���η���0,  ���λص��������ܵõ���Ӧ���ݲ���ӡ����       
void DvrSearchTest::DvrSearchCase2()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);

    int nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceFound",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"deviceFound Event is not exist!");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");


    dvrsearch->Flush();
    nRet1 =  dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");
    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//3  ʹ���¼���deviceFoundע��, �����󲻵���Stop, �ٵ���Start����, Ȼ�����Stop. | ��һ��Start()����0, �ڶ��η���-1
//        Stop()����0
void DvrSearchTest::DvrSearchCase3()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    //step1
    int nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceFound",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"No this Event");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->Start();
    QVERIFY2(-1 == nRet1,"Start() didn't called");

    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//4 ʹ���¼���deviceFoundע��, ����,����setInterval()���ü��(�ϴ������),�۲��ӡ����ı仯 |  Start()��Stop()������0,setInterval()����0
void DvrSearchTest::DvrSearchCase4()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    int nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceFound",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"This Event exist");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->setInterval(5);
    QVERIFY2(0 == nRet1,"Interval is not set currect");
    nRet1 = dvrsearch->Start();
    QVERIFY2(-1 == nRet1,"Start() already called");
    tim.start();
    while(tim.elapsed()< 15*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//5 ��ʹ���¼�deviceSetע��,����deviceFoundע��,ʹ��eventList()��ӡ�¼��б� | registerEvent()������-2, Start()��Stop()������0,
void DvrSearchTest::DvrSearchCase5()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    int nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceSet",DvrSearchTest::cbDeviceSetTest, NULL);
    QVERIFY2(-2 == nRet1,"Event Exists");
    nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceFound",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"Event Exists");

    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");
    QTime tim;
    tim.start();
    while(tim.elapsed()< 15*1000);

    QStringList strList =  (dvrsearch->QueryEventRegister())->eventList();
    QStringList::const_iterator it;
    for (it = strList.begin(); it != strList.end(); it ++)
    {
        QVERIFY2(*it == "deviceSet" || *it == "deviceFound"," EventList Wrong");
    }
    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1, "Stop() already called");
    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//6 ��ʹ���¼�deviceSetע��,,����deviceFoundע��,,ʹ��queryEvent()���в�ѯ      |  registerEvent()������-2, Start()��Stop()������0,
//  "deviceSet"�¼���queryEvent()����-1, "deviceFound"�¼���queryEvent()����0
void DvrSearchTest::DvrSearchCase6()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    int nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceSet",DvrSearchTest::cbDeviceSetTest, NULL);
    QVERIFY2(-2 == nRet1, "Event Exists");
    nRet1 =(dvrsearch->QueryEventRegister())->registerEvent("deviceFound",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1, "Event Exists");

    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 15*1000);

    QStringList evParamList;
    nRet1 = (dvrsearch->QueryEventRegister())->queryEvent("deviceSet", evParamList);
    QVERIFY2(-1 == nRet1,"Error");

    nRet1 = (dvrsearch->QueryEventRegister())->queryEvent("deviceFound", evParamList);
    QVERIFY2(0 == nRet1,"Matched Error");

    dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

