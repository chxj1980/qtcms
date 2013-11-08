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
         printf("%22s\t%-10s\n", sKey.toLatin1().data(),sValue.toLatin1().data());
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

//1  ʹ���¼���SearchDeviceSuccessע��, ���������������.          |  Start()����0, Stop()����0, �ص������ܵõ���Ӧ���ݲ��ܴ�ӡ����
//    ʹ���¼���deviceSet ע�� , �����޴��¼�����ʱ���.   |  Start()����0, Stop()����0, ����û�ж�Ӧ��������ʹ�ûص����������ӡ�κ�����
void DvrSearchTest::DvrSearchCase1()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    //step1:
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    int nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    pRet->Release();
    QVERIFY2(-2 == nRet1,"No this Event");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 20*1000);

    dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");
    //step2:
    pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    nRet1 = pRet->registerEvent("deviceSet",DvrSearchTest::cbDeviceSetTest, NULL);
    pRet->Release();
    QVERIFY2(-2 == nRet1,"No this Event");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    tim.start();
    while(tim.elapsed()< 20*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");
    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//2  ʹ���¼���SearchDeviceSuccessע��,������Stop()ֹͣ,Ȼ��ˢ���ٵ���Start()����, �����ֹͣ
//       |    Start()��Stop()���η���0,  ���λص��������ܵõ���Ӧ���ݲ���ӡ����       
void DvrSearchTest::DvrSearchCase2()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    int nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"SearchDeviceSuccess Event is not exist!");
    pRet->Release();
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 20*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");


    dvrsearch->Flush();
    nRet1 =  dvrsearch->Start();
    QVERIFY2(-1 == nRet1,"Start() already called");
    tim.start();
    while(tim.elapsed()< 20*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//3  ʹ���¼���SearchDeviceSuccessע��, �����󲻵���Stop, �ٵ���Start����, Ȼ�����Stop. | ��һ��Start()����0, �ڶ��η���-1
//        Stop()����0
void DvrSearchTest::DvrSearchCase3()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    //step1
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    int nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"No this Event");
    pRet->Release();
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 25*1000);

    nRet1 = dvrsearch->Start();
    QVERIFY2(-1 == nRet1,"Start() didn't called");

    tim.start();
    while(tim.elapsed()< 25*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//4 ʹ���¼���SearchDeviceSuccessע��, ����,����setInterval()���ü��(�ϴ������),�۲��ӡ����ı仯 |  Start()��Stop()������0,setInterval()����0
void DvrSearchTest::DvrSearchCase4()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet, "No Event Register");
    int nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    pRet->Release();
    QVERIFY2(-2 == nRet1,"This Event exist");
    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 25*1000);

    nRet1 = dvrsearch->setInterval(8);
    QVERIFY2(0 == nRet1,"Interval is not set currect");
    nRet1 = dvrsearch->Start();
    QVERIFY2(-1 == nRet1,"Start() already called");
    tim.start();
    while(tim.elapsed()< 25*1000);

    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//5 ��ʹ���¼�deviceSetע��,����SearchDeviceSuccessע��,ʹ��eventList()�����¼��б� | registerEvent()������-2, Start()��Stop()������0,
void DvrSearchTest::DvrSearchCase5()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    pRet->Release();
    int nRet1 = pRet->registerEvent("deviceSet",DvrSearchTest::cbDeviceSetTest, NULL);
    QVERIFY2(-2 == nRet1,"Event Exists");
    nRet1 = pRet->registerEvent("SearchDeviceSuccess"  ,DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1,"Event Exists");

    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");
    QTime tim;
    tim.start();
    while(tim.elapsed()< 25*1000);

    QStringList strList =  pRet->eventList();
    QStringList::const_iterator it;
    for (it = strList.begin(); it != strList.end(); it ++)
    {
        QVERIFY2(*it == "deviceSet" || *it == "SearchDeviceSuccess"," EventList Wrong");
    }
    nRet1 = dvrsearch->Stop();
    QVERIFY2(0 == nRet1, "Stop() already called");
    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//6 ��ʹ���¼�deviceSetע��,����SearchDeviceSuccessע��,ʹ��queryEvent()���в�ѯ      |  registerEvent()������-2, Start()��Stop()������0,
//  "deviceSet"�¼���queryEvent()����-1, "SearchDeviceSuccess"�¼���queryEvent()����0
void DvrSearchTest::DvrSearchCase6()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    pRet->Release();
    int nRet1 = pRet->registerEvent("deviceSet",DvrSearchTest::cbDeviceSetTest, NULL);
    QVERIFY2(-2 == nRet1, "Event Exists");
    nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1, "Event Exists");

    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 25*1000);

    QStringList evParamList;
    nRet1 =  pRet->queryEvent("deviceSet", evParamList);
    QVERIFY2(-1 == nRet1,"Error");

    nRet1 =  pRet->queryEvent("SearchDeviceSuccess", evParamList);
    QVERIFY2(0 == nRet1,"Matched Error");

    dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}

//7 ��SearchDeviceSuccess����ע��2��, �۲�registerEvent()�ķ���ֵ, ����eventList()�г��¼��б�
// | registerEvent()��һ�η���-2,�ڶ��η���-1, eventList()���ص�listֻ��һ��"SearchDeviceSuccess"
void DvrSearchTest::DvrSearchCase7()
{
    START_DVRSEARCH_UNIT_TEST(dvrsearch);
    IEventRegister *pRet = dvrsearch->QueryEventRegister();
    QVERIFY2(NULL != pRet,"No Event Register");
    pRet->Release();
    int nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-2 == nRet1, "Event Exists");
    nRet1 = pRet->registerEvent("SearchDeviceSuccess",DvrSearchTest::cbDeviceFoundTest, NULL);
    QVERIFY2(-1 == nRet1, "Event Not Exists");

    nRet1 = dvrsearch->Start();
    QVERIFY2(0 == nRet1,"Start() already called");

    QTime tim;
    tim.start();
    while(tim.elapsed()< 25*1000);

    QStringList evList;
    evList =  pRet->eventList();
    QVERIFY2(1 == evList.count("SearchDeviceSuccess"),"Error");

    dvrsearch->Stop();
    QVERIFY2(0 == nRet1,"Stop() already called");

    END_DVRSEARCH_UNIT_TEST(dvrsearch);
}