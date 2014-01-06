#include "SetRecordTimeTest.h"
#include <guid.h>
#include <ISetRecordTime.h>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>


#define START_SETRECORDTIME_UNIT_TEST(ii) 	ISetRecordTime * ii = NULL; \
    pcomCreateInstance(CLSID_CommonLibPlugin, NULL, IID_ISetRecordTime,(void **)&ii); \
    QVERIFY2(NULL != ii,"Create SetRecordTime instance");

#define END_SETRECORDTIME_UNIT_TEST(ii) ii->Release(); 

SetRecordTimeTest::SetRecordTimeTest()
{

}
SetRecordTimeTest::~SetRecordTimeTest()
{

}


void SetRecordTimeTest::beforeSetRecordTimeTest()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("SetRecordTimeTest"))
    {
        db = QSqlDatabase::database("SetRecordTimeTest"); 
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE","SetRecordTimeTest");
    }
    QString sAppPath = QCoreApplication::applicationDirPath();
    QString sDatabasePath = sAppPath + "/system.db";
    db.setDatabaseName(sDatabasePath);
    db.open();

    QSqlQuery query1(db);
    query1.prepare("delete from recordtime");
    query1.exec();
   
    QSqlQuery query2(db);
    query2.prepare("update sqlite_sequence set seq=0 where name= 'recordtime'");
    query2.exec();

    QSqlQuery query3(db);
    query3.prepare("insert into recordtime(chl_id ,schedule_id ,weekday ,starttime ,endtime,enable  ) values(:chl_id,:schedule_id,:weekday,:starttime,:endtime,:enable)");
    query3.bindValue(":chl_id"     ,0);
    query3.bindValue(":schedule_id",0);
    query3.bindValue(":weekday"    ,0);
    query3.bindValue(":starttime"  ,"2013-12-31 00:00:00");
    query3.bindValue(":endtime"    ,"2013-12-31 23:59:59");
    query3.bindValue(":enable"     ,0);

    query3.exec();
    
    db.close();
}
//��������:
// ǰ�ã��������ݿ⣬���ݿ��д�����recordtime�����ݿ��н���һ����¼��0|0|0|��2013-12-31 00:00:00��|�� 2013-12-31 23:59:59�� | 0 
//      ������id��1��ʼ;
// 1. ���Ժ���ModifyRecordTime����������ͷ���ֵ: 
//      ��starttime��endtimeʹ�ø�ʽΪ���ַ��� | ����1
//      ��starttime��endtimeʹ�ø�ʽΪ��"yyyy-MM-dd hh:mm:ss"֮���������ʽ: ��yyyyMMdd hh:mm:ss��,��yyyy������2013/12/31 | ����1
//      ʹ�ú����ֵ: starttime ��Ϊ��2013-12-31 01:00:00�� endtime��Ϊ�� 2013-12-31 22:00:00��, enable ��Ϊ0��1�������  | ����0
//      ʹ����ȷ�ĸ�ʽ�������ֵ: starttime ��Ϊ��2012-12-31 01:00:00�� endtime��Ϊ�� 2013-12-31 22:00:00��, enable ��Ϊ0��1������� | ����1
void SetRecordTimeTest::SetRecordTimeTest1()
{
    beforeSetRecordTimeTest();
    START_SETRECORDTIME_UNIT_TEST(Itest);
    int nRet = -1;

    nRet = Itest->ModifyRecordTime(1, "", "", true);
    QVERIFY2(1 == nRet,"step 1:return");
    nRet = Itest->ModifyRecordTime(1, "20131230 00:00:00", "20131230 23:59:59", true);
    QVERIFY2(1 == nRet,"step 2:return");
    nRet = Itest->ModifyRecordTime(1, "2013", "2014", true);
    QVERIFY2(1 == nRet,"step 3:return");
    nRet = Itest->ModifyRecordTime(1, "2013/12/31 00:00:00", "2013/12/31 23:59:59", true);
    QVERIFY2(1 == nRet,"step 4:return");
    nRet = Itest->ModifyRecordTime(1, "2013-12-31 00:00:00", "2013/12/31 23:59:59", true);
    QVERIFY2(1 == nRet,"step 5:return");
    nRet = Itest->ModifyRecordTime(1, "2012-12-31 00:00:00", "2013-12-31 23:59:59", true);
    QVERIFY2(1 == nRet,"step 6:return");
    nRet = Itest->ModifyRecordTime(1, "2013-12-31 00:00:00", "2013-12-31 23:59:59", true);
    QVERIFY2(0 == nRet,"step 7:return");
    nRet = Itest->ModifyRecordTime(1, "2013-12-31 00:00:00", "2013-12-31 23:59:59", false);
    QVERIFY2(0 == nRet,"step 8:return");

    END_SETRECORDTIME_UNIT_TEST(Itest);
}

// 2. ���Ժ���GetRecordTimeBydevId����������ͷ���ֵ: 
//      ʹ��ͨ����0��ѯ | ����Ĭ�ϵ��������ݵ�id��1
//      ʹ�ò����ڵ�ͨ���� 200��-200��ѯ | ����1�յ�QStringList
void SetRecordTimeTest::SetRecordTimeTest2()
{
    beforeSetRecordTimeTest();
    START_SETRECORDTIME_UNIT_TEST(Itest);
    QStringList strlist;
    //step1
    strlist = Itest->GetRecordTimeBydevId(0);
    QVERIFY2(strlist.size() == 1  ,"step 1:");
    QVERIFY2(strlist[0]     == "1","step 1:");
    //step2
    strlist.clear();
    strlist = Itest->GetRecordTimeBydevId(200);
    QVERIFY2(strlist.size() ==  0,"step 2:");
    strlist = Itest->GetRecordTimeBydevId(-200);
    QVERIFY2(strlist.size() ==  0,"step 2:");
    END_SETRECORDTIME_UNIT_TEST(Itest);
}
// 3.  ��ѯ���ݿ��¼�Ƿ�ɹ�
//     ���ݿ���Ԥ�õ�һ����¼0|0|0|��2013-12-31 00:00:00��|�� 2013-12-31 23:59:59�� | 0;
//        ������2����¼0|1|1|��2014-01-01 00:00:00��|�� 2014-01-01 23:59:59�� | 0;
//        ��1|0|1|��2014-01-01 00:00:00��|�� 2014-01-01 23:59:59�� | 0;
//     ʹ��0ͨ��Ϊ������GetRecordTimeBydevId��ѯͨ��0��id��¼ | ���ؽ����2��, �ֱ���"1""2"
//     ʹ��1ͨ��Ϊ������GetRecordTimeBydevId��ѯͨ��1��id��¼ | ���ؽ����1��,"3"
void SetRecordTimeTest::SetRecordTimeTest3()
{
    beforeSetRecordTimeTest();
    START_SETRECORDTIME_UNIT_TEST(Itest);
    QSqlDatabase db;
    if (QSqlDatabase::contains("SetRecordTimeTest"))
    {
        db = QSqlDatabase::database("SetRecordTimeTest"); 
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE","SetRecordTimeTest");
    }
    QString sAppPath = QCoreApplication::applicationDirPath();
    QString sDatabasePath = sAppPath + "/system.db";
    db.setDatabaseName(sDatabasePath);
    db.open();

    QSqlQuery query1(db);
    query1.prepare("insert into recordtime(chl_id ,schedule_id ,weekday ,starttime ,endtime,enable  ) values(:chl_id,:schedule_id,:weekday,:starttime,:endtime,:enable)");
    query1.bindValue(":chl_id"     ,0);
    query1.bindValue(":schedule_id" ,1);
    query1.bindValue(":weekday"    ,1);
    query1.bindValue(":starttime"  ,"2014-01-01 00:00:00");
    query1.bindValue(":endtime"    ,"2014-01-01 23:59:59");
    query1.bindValue(":enable"     ,0);

    query1.exec();

    QSqlQuery query2(db);
    query2.prepare("insert into recordtime(chl_id ,schedule_id ,weekday ,starttime ,endtime,enable  ) values(:chl_id,:schedule_id,:weekday,:starttime,:endtime,:enable)");
    query2.bindValue(":chl_id"     ,1);
    query2.bindValue(":schedule_id" ,0);
    query2.bindValue(":weekday"    ,1);
    query2.bindValue(":starttime"  ,"2014-01-01 00:00:00");
    query2.bindValue(":endtime"    ,"2014-01-01 23:59:59");
    query2.bindValue(":enable"     ,0);

    query2.exec();
    db.close();
    QStringList strlist;

    //step1
    strlist = Itest->GetRecordTimeBydevId(0);
    QVERIFY2(strlist.size() == 2,"step 1:");
    QVERIFY2(strlist[0]     == "1","step 1:");
    QVERIFY2(strlist[1]     == "2","step 1:");
    //step2
    strlist = Itest->GetRecordTimeBydevId(1);
    QVERIFY2(strlist.size() == 1,"step 2:");
    QVERIFY2(strlist[0]     == "3","step 2:");

    END_SETRECORDTIME_UNIT_TEST(Itest);
}
// 4. ʹ�÷Ƿ���������GetRecordTimeInfo ����;
//    ����recordtime_id ʹ��С��0����; | ����0����¼
//       recordtime_id ʹ�ó������ݿ�ʵ�����id����200  | ����0����¼
//       recordtime_id ʹ�ú������1  | ����1����¼, ���ء� chl_id��Ϊ0, ��schedule_id��Ϊ0, ��weekday������Ϊ0, ��starttime������Ϊ�� 2013-12-31 00:00:00��, endtime����Ϊ�� 2013-12-31 23:59:59��,enable����0
void SetRecordTimeTest::SetRecordTimeTest4()
{
    beforeSetRecordTimeTest();
    START_SETRECORDTIME_UNIT_TEST(Itest);
    QVariantMap vMap;
    //step1
    vMap = Itest->GetRecordTimeInfo(-200);
    QVERIFY2(vMap.size() == 0,"step 1:return");
    vMap.clear();
    //step2
    vMap = Itest->GetRecordTimeInfo(200);
    QVERIFY2(vMap.size() == 0,"step 2:return");
    vMap.clear();
    //step3
    vMap = Itest->GetRecordTimeInfo(1);
    QVERIFY2(0 == vMap["chl_id"].toInt()                ,"step 3:return");
    QVERIFY2(0 == vMap["schedule_id"].toInt()            ,"step 3:return");
    QVERIFY2(0 == vMap["weekday"].toInt()               ,"step 3:return");
    QVERIFY2(vMap["starttime"] == "2013-12-31 00:00:00" ,"step 3:return");
    QVERIFY2(vMap["endtime"]   == "2013-12-31 23:59:59" ,"step 3:return");
    QVERIFY2(0 ==  vMap["enable"]                       ,"step 3:return");

    END_SETRECORDTIME_UNIT_TEST(Itest);
}
// 5. �޸�ֵ�Ͳ�ѯֵ���һ��:
//       ʹ��GetRecordTimeInfo��ѯidΪ1��¼��ʱ���¼; | ���ء�chl_id��Ϊ0, ��schedule_id��Ϊ0, ��weekday������Ϊ0, ��starttime������Ϊ�� 2013-12-31 00:00:00��, endtime����Ϊ�� 2013-12-31 23:59:59��,enable����0
//       ʹ��ModifyRecordTime(ʹ����ȷ�Ĳ���)���޸�¼��ʱ��idΪ1�ļ�¼�Ŀ�ʼʱ��Ϊ�� 2014-01-01 00:00:00������ʱ��Ϊ�� 2014-01-01 23:59:59����¼��ʱ�� | ��������0
//       ��GetRecordTimeInfo��ѯidΪ1��¼��ʱ���¼; | ���ء�chl_id��Ϊ0, ��schedule_id��Ϊ0, ��weekday������Ϊ0, ��starttime������Ϊ�� 2014-01-01 00:00:00��, endtime����Ϊ�� 2014-01-01 23:59:59��,enable����0
void SetRecordTimeTest::SetRecordTimeTest5()
{
    beforeSetRecordTimeTest();
    START_SETRECORDTIME_UNIT_TEST(Itest);
    int nRet = -1;
    QVariantMap vMap;

    //step1
    vMap = Itest->GetRecordTimeInfo(1);
    QVERIFY2(0 == vMap["chl_id"].toInt()                ,"step 1:return");
    QVERIFY2(0 == vMap["schedule_id"].toInt()            ,"step 1:return");
    QVERIFY2(0 == vMap["weekday"].toInt()               ,"step 1:return");
    QVERIFY2(vMap["starttime"] == "2013-12-31 00:00:00" ,"step 1:return");
    QVERIFY2(vMap["endtime"]   == "2013-12-31 23:59:59" ,"step 1:return");
    QVERIFY2(0 ==  vMap["enable"]                       ,"step 1:return");
    //step2
    nRet = Itest->ModifyRecordTime(1, "2014-01-01 00:00:00", "2014-01-01 23:59:59", true);
    QVERIFY2(0 == nRet,"step 2:return");
    //step3
    vMap.clear();
    vMap = Itest->GetRecordTimeInfo(1);
    QVERIFY2(0 == vMap["chl_id"].toInt()                ,"step 3:return");
    QVERIFY2(0 == vMap["schedule_id"].toInt()            ,"step 3:return");
    QVERIFY2(0 == vMap["weekday"].toInt()               ,"step 3:return");
    QVERIFY2(vMap["starttime"] == "2014-01-01 00:00:00" ,"step 3:return");
    QVERIFY2(vMap["endtime"]   == "2014-01-01 23:59:59" ,"step 3:return");
    QVERIFY2(0 ==  vMap["enable"]                       ,"step 3:return");
    END_SETRECORDTIME_UNIT_TEST(Itest);
}

// 6. �޸�ֵ�Ͳ�ѯֵ���һ��:
//       ʹ��GetRecordTimeInfo��ѯidΪ1��¼��ʱ���¼; | ���ء�chl_id��Ϊ0, ��schedule_id��Ϊ0, ��weekday������Ϊ0, ��starttime������Ϊ�� 2013-12-31 00:00:00��, endtime����Ϊ�� 2013-12-31 23:59:59��,enable����0
//       ʹ��ModifyRecordTime(ʹ�ô���Ĳ���)���޸�¼��ʱ��idΪ1�ļ�¼�Ŀ�ʼʱ��Ϊ�� 2012-01-01 00:00:00������ʱ��Ϊ�� 2014-01-01 23:59:59����¼��ʱ�� | ��������0
//       ��GetRecordTimeInfo��ѯidΪ1��¼��ʱ���¼; | ���ء�chl_id��Ϊ0, ��schedule_id��Ϊ0, ��weekday������Ϊ0, ��starttime������Ϊ�� 2014-01-01 00:00:00��, endtime����Ϊ�� 2014-01-01 23:59:59��,enable����0
void SetRecordTimeTest::SetRecordTimeTest6()
{
    beforeSetRecordTimeTest();
    START_SETRECORDTIME_UNIT_TEST(Itest);
    int nRet = -1;
    QVariantMap vMap;

    //step1
    vMap = Itest->GetRecordTimeInfo(1);
    QVERIFY2(0 == vMap["chl_id"].toInt()                ,"step 1:return");
    QVERIFY2(0 == vMap["schedule_id"].toInt()            ,"step 1:return");
    QVERIFY2(0 == vMap["weekday"].toInt()               ,"step 1:return");
    QVERIFY2(vMap["starttime"] == "2013-12-31 00:00:00" ,"step 1:return");
    QVERIFY2(vMap["endtime"]   == "2013-12-31 23:59:59" ,"step 1:return");
    QVERIFY2(0 ==  vMap["enable"]                       ,"step 1:return");
    //step2
    nRet = Itest->ModifyRecordTime(1, "2012-01-01 00:00:00", "2014-01-01 23:59:59", true);
    QVERIFY2(1 == nRet,"step 2:return");
    //step3
    vMap.clear();
    vMap = Itest->GetRecordTimeInfo(1);
    QVERIFY2(0 == vMap["chl_id"].toInt()                ,"step 3:return");
    QVERIFY2(0 == vMap["schedule_id"].toInt()            ,"step 3:return");
    QVERIFY2(0 == vMap["weekday"].toInt()               ,"step 3:return");
    QVERIFY2(vMap["starttime"] == "2013-12-31 00:00:00" ,"step 3:return");
    QVERIFY2(vMap["endtime"]   == "2013-12-31 23:59:59" ,"step 3:return");
    QVERIFY2(0 ==  vMap["enable"]                       ,"step 3:return");
    END_SETRECORDTIME_UNIT_TEST(Itest);
}
