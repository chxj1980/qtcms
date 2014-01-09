#include "LocalRecordSearchTest.h"
#include <guid.h>
#include "ILocalRecordSearch.h"
#include <IEventRegister.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#define START_LOCALRECORDSEARCH_UNIT_TEST(ii) 	ILocalRecordSearch * ii = NULL; \
    pcomCreateInstance(CLSID_LocalPlayer,NULL,IID_ILocalRecordSearch,(void **)&ii); \
    QVERIFY2(NULL != ii,"Create Local Player Record_Search instance");

#define END_LOCALRECORDSEARCH_UNIT_TEST(ii) ii->Release(); 

void LocalRecordSearchTest::beforeTest()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("LocalRecordSearchTest"))
    {
        db = QSqlDatabase::database("LocalRecordSearchTest"); 
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE","LocalRecordSearchTest");
    }
    QString sAppPath = QCoreApplication::applicationDirPath();
    QString sDatabasePath = sAppPath + "/system.db";
    db.setDatabaseName(sDatabasePath);
    db.open();

    QSqlQuery query1(db);
    query1.prepare("delete from general_setting");
    query1.exec();
    QSqlQuery query1_1(db);
    query1_1.prepare("update sqlite_sequence set seq=0 where name= 'general_setting'");
    query1_1.exec();

    QSqlQuery query2(db);
    query2.prepare("insert into general_setting(name,value) values(:name,:value)");
    query2.bindValue(":name","storage_usedisks");
    query2.bindValue(":value","D:");
    query2.exec();
    QSqlQuery query3(db);
    query3.prepare("insert into general_setting(name,value) values(:name,:value)");
    query3.bindValue(":name","storage_cover");
    query3.bindValue(":value","true");
    query3.exec();
    QSqlQuery query4(db);
    query4.prepare("insert into general_setting(name,value) values(:name,:value)");
    query4.bindValue(":name","storage_filesize");
    query4.bindValue(":value","128");
    query4.exec();
    QSqlQuery query5(db);
    query5.prepare("insert into general_setting(name,value) values(:name,:value)");
    query5.bindValue(":name","storage_reservedsize");
    query5.bindValue(":value","1024");
    query5.exec();
    db.close();
}

int LocalRecordSearchTest::cbProc(QString evName, QVariantMap evMap, void *pUser)
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

LocalRecordSearchTest::LocalRecordSearchTest()
{

}

LocalRecordSearchTest::~LocalRecordSearchTest()
{

}
//��������
//ǰ������:  һ��Ӳ��,��ΪC,D,E,F��, ���ݿ�general_setting����Ĭ��ʹ��D�̷���¼��
//       D��������JAREC/2014-01-08/1000/CHL01/094717.avi��JAREC/2014-01-08/1000/CHL02/094717.avi�Լ�
//       JAREC/2014-01-08/1001/CHL01/094717.avi��JAREC/2014-01-08/1000/CHL02/124717.avi
//       F��������JAREC/2014-01-08/1000/CHL01/124717.avi��JAREC/2014-01-08/1000/CHL02/124717.avi

//1. �Ϸ��������Է���ֵ���׳����¼���ֵ
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//    ʹ�ò���Ϊ��1000��(ΪQString����) | ����OK, ���¼�GetRecordDate�׳�,devnameΪ��1000��, dateΪ�� 2014-01-08��
//�׳�SearchStop�¼�: "stopevent"��Ӧ��GetRecordDate��
//    ʹ�ò���Ϊ��1002��(ΪQString����) | ����OK, ֻ��SearchStop�¼��׳�: "stopevent"��Ӧ��GetRecordDate��
void LocalRecordSearchTest::TestCase1()
{
    beforeTest();
    START_LOCALRECORDSEARCH_UNIT_TEST(Itest);
    int nRet = 0;
    QString evName("GetRecordDate");
    IEventRegister *pRegist = NULL;
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();

    evName = "GetRecordFile";
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();
    evName = "SearchStop";
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();

    nRet = Itest->searchDateByDeviceName("1000");
    QVERIFY2(ILocalRecordSearch::OK == nRet, "searchDateByDeviceName return Error!");

    nRet = Itest->searchDateByDeviceName("1002");
    QVERIFY2(ILocalRecordSearch::OK == nRet, "searchDateByDeviceName return Error!");
    END_LOCALRECORDSEARCH_UNIT_TEST(Itest);
}

//2. searchVideoFile�����Լ��׳����¼���ֵ
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//    sdevnameʹ�ò���Ϊ��1000��(ΪQString����), sdate�� 2014-01-08��, sbegintimeΪ��13:50:00��, sendtimeΪ��16:00:00��;
//schannellistΪ��1;2;3;��  | ����OK, �׳��¼�GetRecordFile��SearchStop�¼�, �¼�GetRecordFile��
//filename:�� 143429.avi����Ӧ filepath:�� D: JAREC/2014-01-08/1000/CHL02/ 143429.avi��, 
//filesize: 0.8��0.2, channelnum:�� 2��, 
//startTime:�� 14:34:29����Ӧ stopTime:��14:37:53��
//�׳���SearchStop�¼���: "stopevent"��Ӧ�� GetRecordFile��

//    sdevnameʹ�ò���Ϊ��1000��(ΪQString����), sdate�� 2014-01-08��, sbegintimeΪ��13:50:00��, sendtimeΪ��16:00:00��;
//schannellistΪ��3;4;��   |  ����OK, ֻ��SearchStop�¼��׳�: "stopevent"��Ӧ�� GetRecordFile��

//    sdevnameʹ�ò���Ϊ��1000��(ΪQString����), sdate�� 2014-01-08��, sbegintimeΪ��13:50:00��, sendtimeΪ��16:00:00��;
//schannellistΪ��1234��    |  ����E_PARAMETER_ERROR,���¼��׳�

//    sdevnameʹ�ò���Ϊ��1000��(ΪQString����), sdate�� 2014-01-08��, sbegintimeΪ��33:50:00��, sendtimeΪ��16:66:00��;
//schannellistΪ��1;2;3;��  |  ����E_PARAMETER_ERROR,���¼��׳�

void LocalRecordSearchTest::TestCase2()
{
    beforeTest();
    START_LOCALRECORDSEARCH_UNIT_TEST(Itest);
    int nRet = 0;
    QString evName("GetRecordDate");
    IEventRegister *pRegist = NULL;
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();

    evName = "GetRecordFile";
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();
    evName = "SearchStop";
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();
    QString sDevName("1000"),sDate("2014-01-08"),sBeginTime("13:50:00"),sEndTime("16:00:00"),sChannelList("1;2;3;");
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::OK == nRet, "searchVideoFile return Error!");
    
    sDevName = "1000";
    sDate    = "2014-01-08";
    sBeginTime = "13:50:00";
    sEndTime   = "16:00:00";
    sChannelList = "3;4;";
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::OK == nRet, "searchVideoFile return Error!");

    sDevName = "1000";
    sDate    = "2014-01-08";
    sBeginTime = "13:50:00";
    sEndTime   = "16:00:00";
    sChannelList = "1234";
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::E_PARAMETER_ERROR == nRet, "searchVideoFile return Error!");

    sDevName   = "1000";
    sDate      = "2014-01-08";
    sBeginTime = "33:50:00";
    sEndTime   = "16:66:00";
    sChannelList = "1;2;3;";
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::E_PARAMETER_ERROR == nRet, "searchVideoFile return Error!");
    END_LOCALRECORDSEARCH_UNIT_TEST(Itest);
}


//3. searchVideoFileʹ���ʵ�����������������׳���ͬ���¼�
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//   sdevnameʹ�ò���Ϊ��1000��, sdate�� 2014-01-09��, sbegintimeΪ��13:50:00��, sendtimeΪ��16:00:00��;
//schannellistΪ��1;2;3;��  |  ����OK, �׳�SearchStop�¼�: "stopevent"��Ӧ�� GetRecordFile��
//   sdevnameʹ�ò���Ϊ��1000��, sdate�� 2014-01-08��, sbegintimeΪ��13:50:00��, sendtimeΪ��16:00:00��;
//schannellistΪ��1;��      |  ����OK,�׳�SearchStop�¼�: "stopevent"��Ӧ�� GetRecordFile��
//   sdevnameʹ�ò���Ϊ��1000��(ΪQString����), sdate�� 2014-01-08��, sbegintimeΪ��14:35:00��, sendtimeΪ��16:00:00��;
//schannellistΪ��2;��  |  ����OK, ���¼�GetRecordFile�׳�, filename:�� 143429.avi����Ӧ filepath:�� D: JAREC/2014-01-08/1000/CHL02/ 143429.avi��
//,filesize: 0.8��0.2, channelnum:�� 2��, startTime:�� 14:34:29����Ӧ stopTime:��14:37:53��;
//��SearchStop�¼��׳�: "stopevent"��Ӧ�� GetRecordFile��
void LocalRecordSearchTest::TestCase3()
{
    beforeTest();
    START_LOCALRECORDSEARCH_UNIT_TEST(Itest);
    int nRet = 0;
    QString evName("GetRecordDate");
    IEventRegister *pRegist = NULL;
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();

    evName = "GetRecordFile";
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();
    evName = "SearchStop";
    nRet = Itest->QueryInterface(IID_IEventRegister, (void**)&pRegist);
    QVERIFY2(S_OK == nRet, "QueryInterface Error!");
    nRet = pRegist->registerEvent(evName, cbProc, NULL);
    QVERIFY2(IEventRegister::OK == nRet, "Event Regist Error!");
    pRegist->Release();

    QString sDevName("1000");
    QString sDate("2014-01-09");
    QString sBeginTime("13:50:00");
    QString sEndTime("16:00:00");
    QString sChannelList("1;2;3;");
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::OK == nRet, "searchVideoFile return Error!");

    sDevName = "1000";
    sDate    = "2014-01-08";
    sBeginTime = "13:50:00";
    sEndTime   = "16:00:00";
    sChannelList = "1;";
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::OK  == nRet, "searchVideoFile return Error!");

    sDevName   = "1000";
    sDate      = "2014-01-08";
    sBeginTime = "14:50:00";
    sEndTime   = "16:00:00";
    sChannelList = "2;";
    nRet = Itest->searchVideoFile(sDevName, sDate, sBeginTime, sEndTime,sChannelList);
    QVERIFY2(ILocalRecordSearch::OK  == nRet, "searchVideoFile return Error!");

    END_LOCALRECORDSEARCH_UNIT_TEST(Itest);
}

