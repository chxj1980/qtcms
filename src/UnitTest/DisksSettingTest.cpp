#include "DisksSettingTest.h"
#include <guid.h>
#include <IDisksSetting.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>


#define START_DISKSETTING_UNIT_TEST(ii) 	IDisksSetting * ii = NULL; \
    pcomCreateInstance(CLSID_CommonLibPlugin, NULL, IID_IDiskSetting,(void **)&ii); \
    QVERIFY2(NULL != ii,"Create DisksSetting instance");

#define END_DISKSETTING_UNIT_TEST(ii) ii->Release(); 

DisksSettingTest::DisksSettingTest()
{

}
DisksSettingTest::~DisksSettingTest()
{

}


void DisksSettingTest::beforeDisksSettingTest()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("DisksSettingTest"))
    {
        db = QSqlDatabase::database("DisksSettingTest"); 
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE","DisksSettingTest");
    }
    QString sAppPath = QCoreApplication::applicationDirPath();
    QString sDatabasePath = sAppPath + "/system.db";
    db.setDatabaseName(sDatabasePath);
    db.open();

    QSqlQuery query1(db);
    query1.prepare("delete from general_setting");
    query1.exec();

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
//��������:
// ǰ�ã��������ݿ⣬���ݿ��д�����disks_setting�����ݿ��н���һ����¼��D:|true|128|1024|
// ���������Ŀ��ͬ��Ӳ��A,B;A��ΪC,D,E,F��;B��ΪC,D,E,F,G��, ���ݿ�Ĭ�϶�ֻʹ��D�̷���¼��;¼���ļ�Ĭ�����512MB

// 1. ʹ��Ӳ��A,setUseDisks����¼��ʹ�õĴ��̷���, ʹ�ø�ʽΪ: "DD:",��D;��,��G:��|����E_PARAMETER_ERROR ;
//           ʹ����ȷ��ʽ��"E:", ����S_OK 
void DisksSettingTest::DisksSettingTest1()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    // step1
    nRet = Itest->setUseDisks("DD:");
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 1:return");
    nRet = Itest->setUseDisks("D;");
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 1:return");
    nRet = Itest->setUseDisks("G:");
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 1:return");
    // step2
    nRet = Itest->setUseDisks("E:");
    QVERIFY2(IDisksSetting::OK == nRet,"step 2:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}

// 2.  ʹ��Ӳ��A,setUseDisks����¼��ʹ�õĴ���ΪE:F:, getUseDisks��ѯ���ز��ȶ�����ֵ�봫��ֵ�Ƿ�һ��
//    | ���÷���S_OK, ��ѯ����E:F:
void DisksSettingTest::DisksSettingTest2()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    //step1
    nRet = Itest->setUseDisks("E:F:");
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    //step2
    QString sRet;
    nRet = Itest->getUseDisks(sRet);
    QVERIFY2(sRet == "E:F:" ,"step 2:return");
    QVERIFY2(nRet == IDisksSetting::OK ,"step 2:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}
// 3.  ʹ��Ӳ��A, getEnableDisks��ȡϵͳ���õĴ��̷��� | ���ؽ��Ϊ"D:E:F:"
//     ʹ��Ӳ��B, getEnableDisks��ȡϵͳ���õĴ��̷��� | ���ؽ��Ϊ"D:E:F:G:" 
void DisksSettingTest::DisksSettingTest3()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    QString sRet;
    //step1
    nRet = Itest->getEnableDisks(sRet);
    QVERIFY2(sRet == "C:D:E:F:","step 1:return"); //Ӳ��A
    //QVERIFY2(sRet == "D:E:F:G:","step 1:return"); //Ӳ��B
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    
    END_DISKSETTING_UNIT_TEST(Itest);
}
// 4. ʹ��Ӳ��A,setFilePackageSize����¼���ļ�����СΪС��0����-200 | ����E_PARAMETER_ERROR
//    ʹ��Ӳ��A,setFilePackageSize����¼���ļ�����СΪ����Ĭ�����ֵ��700MBʱ | ����E_PARAMETER_ERROR
//    ʹ��Ӳ��A,setFilePackageSize����¼���ļ�����СΪ500MB  | ����S_OK 
void DisksSettingTest::DisksSettingTest4()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    int nFileSize = -200;
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 1:return");
    nFileSize = 0;
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 2:return");
    nFileSize = 700;
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 3:return");
    nFileSize = 500;
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 4:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}
// 5. ʹ��Ӳ��A,setFilePackageSize����¼���ļ�����СΪ500MB, ��getFilePackageSize������������ֵ�Ƚ� | setFilePackageSize����S_OK , ����500
//    ʹ��Ӳ��A,setFilePackageSize����¼���ļ�����СΪ0MB, ��getFilePackageSize������������ֵ�Ƚ� | setFilePackageSize����E_PARAMETER_ERROR , ����֮ǰ��ֵ500
//    ʹ��Ӳ��A,setFilePackageSize����¼���ļ�����СΪ-200MB, ��getFilePackageSize������������ֵ�Ƚ� | setFilePackageSize����E_PARAMETER_ERROR , ����֮ǰ��ֵ500
void DisksSettingTest::DisksSettingTest5()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    int nFileSize = 500;
    //step1
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    nFileSize = 0;
    nRet = Itest->getFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    QVERIFY2(nFileSize == 500,"step 1:return");
    //step2
    nFileSize = 0;
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 2:return");
    nFileSize = 0;
    nRet = Itest->getFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 2:return");
    QVERIFY2(nFileSize == 500,"step 2:return");

    //step3
    nFileSize = -200;
    nRet = Itest->setFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 2:return");
    nFileSize = 0;
    nRet = Itest->getFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 2:return");
    QVERIFY2(nFileSize == 500,"step 2:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}

// 6.  ʹ��Ӳ��A,setLoopRecording����ѭ��¼��bcoverΪfalse, ����getLoopRecording ��ȡ��ǰ״̬,Ȼ��
//     ����setLoopRecording����ѭ��¼��, ����getLoopRecording ��ȡ��ǰ״̬ | �ڶ��ε���setLoopRecording���õ�true��
//     getLoopRecording ����ֵһ��
void DisksSettingTest::DisksSettingTest6()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    bool bRet = false;
    nRet = Itest->setLoopRecording(false);
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    bRet = Itest->getLoopRecording();
    QVERIFY2(false == bRet,"step 2:return");

    nRet = Itest->setLoopRecording(true);
    QVERIFY2(IDisksSetting::OK == nRet,"step 3:return");
    bRet = Itest->getLoopRecording();
    QVERIFY2(true == bRet,"step 4:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}
// 7.  ʹ��Ӳ��A��D����, D�̴�С194560MB, ���ÿռ�178176MB;
//     setDiskSpaceReservedSize���ô��̱����ռ��С, ������Ϊ-2000��0  | ����E_PARAMETER_ERROR
//     ������Ϊ���ڴ��̷�����С����: 394560MB  | ����E_PARAMETER_ERROR
//     ��������Ϊ�������550MB  | ����S_OK
//     ������Ϊ���ڴ��̷������ô�С����: 394560MB  | ����E_PARAMETER_ERROR
void DisksSettingTest::DisksSettingTest7()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    int spacereservedsize = -2000;
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 1:return");

    spacereservedsize =  0;
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 2:return");

    spacereservedsize =  394560;
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 3:return");

    spacereservedsize =  550;
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 4:return");

    spacereservedsize =  394560;
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 5:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}
//8. ʹ��Ӳ��A��D����, D�̴�С194560MB, ���ÿռ�178176MB;
//   setDiskSpaceReservedSize���ú������2048MB, ��getDiskSpaceReservedSize����ֵ�Ƚ� | ǰ�߷���S_OK, ���ߴ���ֵΪ2048
//   setDiskSpaceReservedSize���ò��������-2000MB, ��getDiskSpaceReservedSize����ֵ�Ƚ� | ǰ�߷���E_PARAMETER_ERROR, ���ߴ���ֵΪ֮ǰ���õ�2048
void DisksSettingTest::DisksSettingTest8()
{
    beforeDisksSettingTest();
    START_DISKSETTING_UNIT_TEST(Itest);
    int nRet = -1;
    int spacereservedsize = 2048;
    //step1
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    spacereservedsize = 0;
    nRet = Itest->getDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 1:return");
    QVERIFY2(spacereservedsize == 2048,"step 1:return");
    //step2
    spacereservedsize = -2000;
    nRet = Itest->setDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::E_PARAMETER_ERROR == nRet,"step 2:return");
    spacereservedsize = 0;
    nRet = Itest->getDiskSpaceReservedSize(spacereservedsize);
    QVERIFY2(IDisksSetting::OK == nRet,"step 2:return");
    QVERIFY2(spacereservedsize == 2048,"step 2:return");
    END_DISKSETTING_UNIT_TEST(Itest);
}
