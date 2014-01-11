#include "LocalPlayerSynPlaybackTest.h"
#include <guid.h>
#include "ILocalRecordSearch.h"
#include "ILocalPlayer.h"
#include <IEventRegister.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QStringList>

#define START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(ii) 	ILocalPlayer * ii = NULL; \
    pcomCreateInstance(CLSID_LocalPlayer,NULL,IID_ILocalPlayer,(void **)&ii); \
    QVERIFY2(NULL != ii,"Create Local Player Syn_Playback instance");

#define END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(ii) ii->Release(); 

void LocalPlayerSynPlaybackTest::beforeTest()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("LocalPlayerSynPlaybackTest"))
    {
        db = QSqlDatabase::database("LocalPlayerSynPlaybackTest"); 
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE","LocalPlayerSynPlaybackTest");
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

int LocalPlayerSynPlaybackTest::cbProc(QString evName, QVariantMap evMap, void *pUser)
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

LocalPlayerSynPlaybackTest::LocalPlayerSynPlaybackTest()
{
    for (int i = 0; i < sizeof(m_PlaybackWnd) / sizeof(m_PlaybackWnd[0]); ++i)
    {
        m_PlaybackWnd[i].setParent(this);
    }
}

LocalPlayerSynPlaybackTest::~LocalPlayerSynPlaybackTest()
{
}

//��������
//ǰ������:  һ��Ӳ��,��ΪC,D,E,F��, ���ݿ�general_setting����Ĭ��ʹ��D�̷���¼��
//       D��������JAREC/2014-01-11/1000/CHL01/094717.avi��JAREC/2014-01-11/1000/CHL02/094717.avi�Լ�
//       JAREC/2014-01-11/1001/CHL01/114717..avi��JAREC/2014-01-11/1000/CHL01/143429.avi
//       JAREC/2014-01-11/1000/CHL02/143429.avi
//       F��������JAREC/2014-01-11/1000/CHL01/124717.avi��JAREC/2014-01-11/1000/CHL02/124717.avi

//1. AddFileIntoPlayGroup�����Լ�����ֵ����
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//    ��������:startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 07:00:00��, ������������ȷֵ | ����3:ʧ��
//    ��������:startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ���Ŵ���ָ���NULL  | ����3:ʧ��
//    ��������:startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ������������ȷֵ  | ����0, ��ӳɹ�
//    ��������:startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ����������4����ȫһ�� | ����2, ���ʧ�ܣ������Ѿ���ռ��
void LocalPlayerSynPlaybackTest::TestCase1()
{
    beforeTest();
    START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
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
    //steps
    QStringList sFileList;
    sFileList<<"D:/JAREC/2014-01-11/1000/CHL01/094717.avi"
             <<"D:/JAREC/2014-01-11/1001/CHL01/114717.avi"
             <<"D:/JAREC/2014-01-11/1000/CHL01/143429.avi";
    QDateTime sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    QDateTime sEnd = QDateTime::fromString("2014-01-11 07:00:00", "yyyy-MM-dd hh:mm:ss");
    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[0].ui->widget_display , sStart, sEnd);
    QVERIFY2(3 == nRet, "AddFileIntoPlayGroup return Error! : step1");

    sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss"); 
    nRet = Itest->AddFileIntoPlayGroup(sFileList, NULL, sStart, sEnd);
    QVERIFY2(3 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss"); 
    nRet = Itest->AddFileIntoPlayGroup(sFileList, m_PlaybackWnd[2].ui->widget_display, sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step3");

    sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss"); 
    nRet = Itest->AddFileIntoPlayGroup(sFileList,  m_PlaybackWnd[2].ui->widget_display , sStart, sEnd);
    QVERIFY2(2 == nRet, "AddFileIntoPlayGroup return Error! : step4");
    END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
}


//2. SetSynGroupNum���Բ���ֵ�ͷ���ֵ
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//    ������-3  |  ����1����ʧ��
//    ������0   |  ����1����ʧ��
//    ������4   |  ����0���óɹ�
//    ������100 |  ����1����ʧ��
void LocalPlayerSynPlaybackTest::TestCase2()
{
    beforeTest();
    START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
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
    //steps:
    nRet = Itest->SetSynGroupNum(-3);
    QVERIFY2(1 == nRet, "SetSynGroupNum return Error! :step1");
    nRet = Itest->SetSynGroupNum(0);
    QVERIFY2(1 == nRet, "SetSynGroupNum return Error! :step2");
    nRet = Itest->SetSynGroupNum(4);
    QVERIFY2(0  == nRet, "SetSynGroupNum return Error! :step3");
    nRet = Itest->SetSynGroupNum(100);
    QVERIFY2(1  == nRet, "SetSynGroupNum return Error! :step4");
    END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
}

//3. ����ͬ��������,����û�ж���ӵ����������
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//    SetSynGroupNum��������Ϊ2;  |  ����0:�ɹ�
//    ����2�ε��ú���AddFileIntoPlayGroup ������startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ������������ȷֵ | ����0:��ӳɹ�
//    �ٵ��ú���AddFileIntoPlayGroup ������startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ������������ȷֵ | ����1:ͨ��������
void LocalPlayerSynPlaybackTest::TestCase3()
{
    beforeTest();
    START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
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
    //steps:
    QStringList sFileList;
    sFileList<<"D:/JAREC/2014-01-11/1000/CHL01/094717.avi"
        <<"D:/JAREC/2014-01-11/1001/CHL01/114717.avi"
        <<"D:/JAREC/2014-01-11/1000/CHL01/143429.avi";

    nRet = Itest->SetSynGroupNum(2);
    QVERIFY2(0 == nRet, "SetSynGroupNum return Error! : step1");

    QDateTime sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    QDateTime sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss");
    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[0].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[1].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[2].ui->widget_display , sStart, sEnd);
    QVERIFY2(1 == nRet, "AddFileIntoPlayGroup return Error! : step3");

    END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
}

//4. ���Բ���, ��ͣ, ��ͣ�����, ֹͣ
//    ע���¼�:"GetRecordDate","GetRecordFile"��"SearchStop"
//    SetSynGroupNum��������Ϊ2;  |  ����0:�ɹ�
//    ����3�ε��ú���AddFileIntoPlayGroup ������startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ������������ȷֵ | ǰ2�η���0:��ӳɹ�
//���һ�η���1: ͨ��������
//    ����GroupPlay  | ����0
//    10���Ӻ����GroupPause  | ����0
//    10���Ӻ����GroupContinue  | ����0
//    10���Ӻ����GroupStop  | ����0
void LocalPlayerSynPlaybackTest::TestCase4()
{
    beforeTest();
    START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
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
    //steps:
    QStringList sFileList;
    sFileList<<"D:/JAREC/2014-01-11/1000/CHL01/094717.avi"
        <<"D:/JAREC/2014-01-11/1001/CHL01/114717.avi"
        <<"D:/JAREC/2014-01-11/1000/CHL01/143429.avi";

    nRet = Itest->SetSynGroupNum(2);
    QVERIFY2(0 == nRet, "SetSynGroupNum return Error! : step1");

    QDateTime sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    QDateTime sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss");
    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[0].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[1].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[2].ui->widget_display , sStart, sEnd);
    QVERIFY2(1 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->GroupPlay();
    QVERIFY2(0 == nRet, "GroupPlay return Error! : step3");

    QTest::qWait(10000*60);

    nRet = Itest->GroupPause();
    QVERIFY2(0 == nRet, "GroupPause return Error! : step4");

    QTest::qWait(10000*60);

    nRet = Itest->GroupContinue();
    QVERIFY2(0 == nRet, "GroupContinue return Error! : step5");

    QTest::qWait(10000*60);

    nRet = Itest->GroupStop();
    QVERIFY2(0 == nRet, "GroupStop return Error! : step6");
    END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
}

//5. ���Բ���ֹͣ�������������Ƿ�ɹ�
//   SetSynGroupNum ����ͬ��������Ϊ3  |  ����0:�ɹ�
//   3�ε���AddFileIntoPlayGroup ������startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ������������ȷֵ | 3�η���0:��ӳɹ�
//   ����GroupPlay	 | ����0
//   10���Ӻ����GroupStop	| ����0
//   5���Ӻ����GroupPlay	 | ����0
//   10���Ӻ����GroupStop	| ����0
void LocalPlayerSynPlaybackTest::TestCase5()
{
    beforeTest();
    START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
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
    //steps:
    QStringList sFileList;
    sFileList<<"D:/JAREC/2014-01-11/1000/CHL01/094717.avi"
        <<"D:/JAREC/2014-01-11/1001/CHL01/114717.avi"
        <<"D:/JAREC/2014-01-11/1000/CHL01/143429.avi";

    nRet = Itest->SetSynGroupNum(3);
    QVERIFY2(0 == nRet, "SetSynGroupNum return Error! : step1");

    QDateTime sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    QDateTime sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss");
    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[0].ui->widget_display  , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[1].ui->widget_display  , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[2].ui->widget_display  , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->GroupPlay();
    QVERIFY2(0 == nRet, "GroupPlay return Error! : step3");

    QTest::qWait(10000*60);

    nRet = Itest->GroupStop();
    QVERIFY2(0 == nRet, "GroupStop return Error! : step4");

    QTest::qWait(10000*60);

    nRet = Itest->GroupPlay();
    QVERIFY2(0 == nRet, "GroupPlay return Error! : step5");

    QTest::qWait(10000*60);

    nRet = Itest->GroupStop();
    QVERIFY2(0 == nRet, "GroupStop return Error! : step6");
    END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
}

//6  ���Կ���/���ٲ����Ƿ�ɹ�
//   SetSynGroupNum ����ͬ��������Ϊ3	|  ����0:�ɹ�
//   3�ε���AddFileIntoPlayGroup ������startΪ�� 2014-01-11 08:00:00��,endΪ�� 2014-01-11 22:00:00��, ������������ȷֵ	3��
//   	|  ����0:��ӳɹ�
//   ����GroupPlay  |  ����0
//   5���Ӻ����GroupSpeedFast2����	|  ����0
//   5���Ӻ����GroupSpeedFast8����	|  ����0
//   5���Ӻ����GroupSpeedSlow����1/2����	|  ����0
//   5���Ӻ����GroupSpeedNormal������������	|  	����0
//   5���Ӻ�GroupStopֹͣ����	|  ����0
void LocalPlayerSynPlaybackTest::TestCase6()
{
    beforeTest();
    START_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
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
    //steps:
    QStringList sFileList;
    sFileList<<"D:/JAREC/2014-01-11/1000/CHL01/094717.avi"
        <<"D:/JAREC/2014-01-11/1001/CHL01/114717.avi"
        <<"D:/JAREC/2014-01-11/1000/CHL01/143429.avi";

    nRet = Itest->SetSynGroupNum(3);
    QVERIFY2(0 == nRet, "SetSynGroupNum return Error! : step1");

    QDateTime sStart = QDateTime::fromString("2014-01-11 08:00:00", "yyyy-MM-dd hh:mm:ss");
    QDateTime sEnd   = QDateTime::fromString("2014-01-11 22:00:00", "yyyy-MM-dd hh:mm:ss");
    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[0].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[1].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->AddFileIntoPlayGroup(sFileList,m_PlaybackWnd[2].ui->widget_display , sStart, sEnd);
    QVERIFY2(0 == nRet, "AddFileIntoPlayGroup return Error! : step2");

    nRet = Itest->GroupPlay();
    QVERIFY2(0 == nRet, "GroupPlay return Error! : step3");

    QTest::qWait(1000*60);

    nRet = Itest->GroupSpeedFast(2);
    QVERIFY2(0 == nRet, "GroupSpeedFast return Error! : step4");
    QTest::qWait(5000*60);
    nRet = Itest->GroupSpeedFast(8);
    QVERIFY2(0 == nRet, "GroupSpeedFast return Error! : step5");

    QTest::qWait(5000*60);
    nRet = Itest->GroupSpeedSlow(2);
    QVERIFY2(0 == nRet, "GroupSpeedSlow return Error! : step6");

    QTest::qWait(5000*60);
    nRet = Itest->GroupSpeedNormal();
    QVERIFY2(0 == nRet, "GroupSpeedNormal return Error! : step7");

    QTest::qWait(5000*60);
    nRet = Itest->GroupStop();
    QVERIFY2(0 == nRet, "GroupStop return Error! : step8");

    END_LOCALPLAYERSYNPLAYBACK_UNIT_TEST(Itest);
}







