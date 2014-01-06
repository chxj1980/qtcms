#include "RecorderTest.h"
#include <guid.h>
#include <IDisksSetting.h>
#include <IRecorder.h>
#include <QtCore/QStringList>
#include <QtCore/QVariantMap>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include "netlib.h"
#pragma comment(lib,"netlib.lib")

#define START_RECORDER_UNIT_TEST(ii, iDiskSetting) 	IRecorder * ii = NULL; IDisksSetting *iDiskSetting = NULL;\
   pcomCreateInstance(CLSID_Recorder, NULL, IID_IRecorder,(void **)&ii);\
   pcomCreateInstance(CLSID_CommonLibPlugin, NULL, IID_IDiskSetting,(void **)&iDiskSetting);\
    QVERIFY2(NULL != ii,"Create Recorder instance");\
    QVERIFY2(NULL != iDiskSetting,"Create commonlib instance");\

#define END_RECORDER_UNIT_TEST(ii, iDiskSetting) ii->Release(); iDiskSetting->Release();\


RecorderTest::RecorderTest()
{

}
RecorderTest::~RecorderTest()
{

}
typedef struct test_nalu_header{
    unsigned long flag;
    unsigned long size;
    unsigned long isider;  //1 ��I֡, 0x02��P֡, 0x00����Ƶ֡
}NALU_HEADER_t;

void RecorderTest::beforeRecorderTest()
{
    QSqlDatabase db;
    if (QSqlDatabase::contains("RecorderTest"))
    {
        db = QSqlDatabase::database("RecorderTest"); 
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE","RecorderTest");
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
// ǰ�ã��������ݿ⣬���ݿ��д�����disks_setting�������ݿ��н���һ����¼��D:|true|128|1024|; ����DebugĿ¼��.h264�ļ�: CIF_12fps_128kbps.h264
// 1.����¼���ļ�д���С��·���Լ�������ռ��С: 
//      getEnableDisks��ȡ���÷���	����ֵΪIDisksSetting::OK | ���ֵΪ�� C:D:E:F:��
//      setUseDisks��������ʹ��E:F:�̴洢¼���ļ�	| ����ֵΪIDisksSetting::OK;
//      getUseDisks��֤ʹ�ô���ΪE:F: |	����ֵΪIDisksSetting::OK; ���ֵΪ��E:F:��
//      setFilePackageSize����¼���ļ�����СΪ200MB	| ����ֵΪIDisksSetting::OK;
//      getFilePackageSize��ȡ¼���ļ�����С������	| ����ֵΪIDisksSetting::OK;���ֵΪ��200��
//      setLoopRecording����Ϊѭ��¼��	            | ����ֵΪIDisksSetting::OK;
//      getLoopRecording��֤Ϊѭ��¼��	            | ����ֵΪtrue
//      setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB	 | ����ֵΪIDisksSetting::OK;
//      getDiskSpaceReservedSize��֤��һ������ֵ	| ����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//      ����SetDevInfo�����豸��Ϊ1000��ͨ����1     | ����ֵΪIRecorder:OK
//      ����Start()��ʼ¼��	                    | ����ֵΪIRecorder::OK
//      ����InputFrame ,���ò�����ȷ	            | ����ֵΪIRecorder:OK
//      �ڿ�ʼ��F:д����һ��������, ����SetDevInfo�����豸��Ϊ1001��ͨ����1  | ����ֵΪIRecorder:OK
//      ����Stop()ֹͣ     | ����ֵΪIRecorder::OK, ¼���ļ���С��Ϊ200MB�����Ҿ���EF����,E��ʣ�ౣ���ռ�5000MB
void RecorderTest::RecorderTest1()
{
    beforeRecorderTest();
    START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
    int nRet = -1;
    int nFileSize = 0;
    QString sDiskEnabled;
    int nDiskSpaceReservedSize = 0 ;

    nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
    QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
    nRet = pDiskSetting->setUseDisks("E:F:");
    QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
    sDiskEnabled.clear();
    nRet = pDiskSetting->getUseDisks(sDiskEnabled);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
    QVERIFY2(sDiskEnabled == "E:F:"     ,"getUseDisks :out");
    nRet = pDiskSetting->setFilePackageSize(200);
    QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
    nRet = pDiskSetting->getFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
    QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
    nRet = pDiskSetting->setLoopRecording(true);
    QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
    bool bIsLoopRecord = pDiskSetting->getLoopRecording();
    QVERIFY2(true == bIsLoopRecord      ,"getLoopRecording :return");
    nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
    QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
    nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
    QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");
    
    QString sDevName("1000");
    nRet = Itest->SetDevInfo(sDevName,  1);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
    nRet = Itest->Start();
    QVERIFY2(IRecorder::OK == nRet,"Start() :return");

    FILE *pFile = NULL;
    char  pFileName[] = "CIF_12fps_128kbps.h264";
    int   nlen = 0;
    char  data[1280*720];
    uint  nTimes = 0;
    uint  nTotalLoopTimes = 0;
    unsigned long long   i64FreeBytesAvailableOfE = 0;
    unsigned long long   i64TotalNumberOfBytesOfE = 0;
    unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;
    unsigned long long   i64FreeBytesAvailableOfF = 0;
    unsigned long long   i64TotalNumberOfBytesOfF = 0;
    unsigned long long   i64TotalNumberOfFreeBytesOfF = 0;
    GetDiskFreeSpaceExQ("E:\\"
        , &i64FreeBytesAvailableOfE
        , &i64TotalNumberOfBytesOfE
        , &i64TotalNumberOfFreeBytesOfE);
    GetDiskFreeSpaceExQ("F:\\"
        , &i64FreeBytesAvailableOfF
        , &i64TotalNumberOfBytesOfF
        , &i64TotalNumberOfFreeBytesOfF);
    nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024 + (float)i64FreeBytesAvailableOfF/1024/1024 - 10000) / 4.58);
    QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

    NALU_HEADER_t nhead;
    uint nTmp = (uint) (((float)i64FreeBytesAvailableOfE/1024/1024 - 5000) /4.58 );
    bool bIsDevInfoSetFlag = false;
    while (nTimes < nTotalLoopTimes)
    {
        if (feof(pFile))
        {
            rewind(pFile);
            ++nTimes;
        }
        if(nTimes > nTmp && !bIsDevInfoSetFlag)
        {
            bIsDevInfoSetFlag = true;
            sDevName.clear();
            sDevName = "1001";
            nRet = Itest->SetDevInfo(sDevName,  1);
            QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
        }
        memset(&nhead,0,sizeof(NALU_HEADER_t));
        memset(data,0,sizeof(data));
        QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
        QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
        nRet = Itest->InputFrame(nhead.isider, data,  nhead.size);
        QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
    }

    fclose(pFile);
    pFile=NULL;
    nRet = Itest->Stop();
    QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

    END_RECORDER_UNIT_TEST(Itest, pDiskSetting);
}

// 2. ����¼���ļ�д���С��·���Լ�������ռ��С
//getEnableDisks��ȡ���÷���	         |  ����ֵΪIDisksSetting::OK; ���ֵΪ�� C:D:E:F:��
//setUseDisks��������ʹ��E:F:�̴洢¼���ļ�	 |  ����ֵΪIDisksSetting::OK;
//getUseDisks��֤ʹ�ô���ΪE:F:	         |  ����ֵΪIDisksSetting::OK; ���ֵΪ��E:F:��
//setFilePackageSize����¼���ļ�����СΪ200MB|	����ֵΪIDisksSetting::OK;
//getFilePackageSize��ȡ¼���ļ�����С������	  |����ֵΪIDisksSetting::OK;���ֵΪ��200��
//setLoopRecording����Ϊѭ��¼��	         | ����ֵΪIDisksSetting::OK;
//getLoopRecording��֤Ϊѭ��¼��	         | ����ֵΪtrue
//setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB	| ����ֵΪIDisksSetting::OK;
//getDiskSpaceReservedSize��֤��һ������ֵ  |	����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//����SetDevInfo�����豸��Ϊ2000��ͨ����4     | ����ֵΪIRecorder:OK
//����Start()��ʼ¼��	                     |  ����ֵΪIRecorder::OK

//��CIF_12fps_128kbps.h264�ļ�,��ȡ����֡����ͨ��InputFrameд�� ,�ظ�n��, (n������ߴ��̿��ÿռ��С�й�)|	����ֵΪIRecorder:OK
//����Stop()ֹͣ,                         |  ����ֵΪIRecorder::OK, ¼���ļ���С��Ϊ200MB����,��EF����¼��,E�̱����ռ�Ϊ5000MB����

void RecorderTest::RecorderTest2()
{
    beforeRecorderTest();
    START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
    int nRet = -1;
    int nFileSize = 0;
    QString sDiskEnabled;
    int nDiskSpaceReservedSize = 0 ;

    nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
    QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
    nRet = pDiskSetting->setUseDisks("E:F:");
    QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
    sDiskEnabled.clear();
    nRet = pDiskSetting->getUseDisks(sDiskEnabled);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
    QVERIFY2(sDiskEnabled == "E:F:"     ,"getUseDisks :out");
    nRet = pDiskSetting->setFilePackageSize(200);
    QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
    nRet = pDiskSetting->getFilePackageSize(nFileSize);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
    QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
    nRet = pDiskSetting->setLoopRecording(true);
    QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
    bool bIsLoopRecord = pDiskSetting->getLoopRecording();
    QVERIFY2(true == bIsLoopRecord      ,"getLoopRecording :return");
    nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
    QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
    nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
    QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
    QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");

    QString sDevName("2000");
    nRet = Itest->SetDevInfo(sDevName,  4);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
    nRet = Itest->Start();
    QVERIFY2(IRecorder::OK == nRet,"Start() :return");


    FILE *pFile = NULL;
    char  pFileName[] = "CIF_12fps_128kbps.h264";
    int   nlen = 0;
    char  data[1280*720];
    uint  nTimes = 0;
    uint  nTotalLoopTimes = 0;
    unsigned long long   i64FreeBytesAvailableOfE = 0;
    unsigned long long   i64TotalNumberOfBytesOfE = 0;
    unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;
    unsigned long long   i64FreeBytesAvailableOfF = 0;
    unsigned long long   i64TotalNumberOfBytesOfF = 0;
    unsigned long long   i64TotalNumberOfFreeBytesOfF = 0;
    GetDiskFreeSpaceExQ("E:\\"
        , &i64FreeBytesAvailableOfE
        , &i64TotalNumberOfBytesOfE
        , &i64TotalNumberOfFreeBytesOfE);
    GetDiskFreeSpaceExQ("F:\\"
        , &i64FreeBytesAvailableOfF
        , &i64TotalNumberOfBytesOfF
        , &i64TotalNumberOfFreeBytesOfF);
    nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024 + (float)i64FreeBytesAvailableOfF/1024/1024 - 10000) / 4.58);
    QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

    NALU_HEADER_t nhead;
    while (nTimes < nTotalLoopTimes )
    {
        if (feof(pFile))
        {
            rewind(pFile);
            ++nTimes;
        }
        memset(&nhead,0,sizeof(NALU_HEADER_t));
        memset(data,0,sizeof(data));
        QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
        QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
        nRet = Itest->InputFrame(nhead.isider, data,  nhead.size);
        QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
    }

    fclose(pFile);
    pFile=NULL;

    nRet = Itest->Stop();
    QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

    END_RECORDER_UNIT_TEST(Itest, pDiskSetting);
}
// 3.  ���� ��ѭ��¼���Ƿ�����ֹͣ�������ǺͲ��Է��������ռ��С�Լ�·���Ƿ���ȷ
//getEnableDisks��ȡ���÷���	| ����ֵΪIDisksSetting::OK; ���ֵΪ�� C:D:E:F:��
//setUseDisks��������ʹ��F:�̴洢¼���ļ� | ����ֵΪIDisksSetting::OK;
//getUseDisks��֤ʹ�ô���ΪF:   |	����ֵΪIDisksSetting::OK; ���ֵΪ��F: ��
//setFilePackageSize����¼���ļ�����СΪ200MB	| ����ֵΪIDisksSetting::OK;
//getFilePackageSize��ȡ¼���ļ�����С������	| ����ֵΪIDisksSetting::OK;���ֵΪ��200��
//setLoopRecording����Ϊ��ѭ��¼��	 | ����ֵΪIDisksSetting::OK;
//getLoopRecording��֤Ϊѭ��¼��	| ����ֵΪfalse
//setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB	| ����ֵΪIDisksSetting::OK;
//getDiskSpaceReservedSize��֤��һ������ֵ  |	����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//����SetDevInfo�����豸��Ϊ3000��ͨ����1     | ����ֵΪIRecorder:OK
//����Start()��ʼ¼��	 | ����ֵΪIRecorder::OK
//��CIF_12fps_128kbps.h264�ļ�,��ȡ����֡����ͨ��InputFrameд�� ,�ظ�n��,, ������Stop () | 	����ֵΪIRecorder:OK
//���F:����¼���ļ���С, ��ʣ��ռ��С, �Ƿ���¼���ļ�������
void RecorderTest::RecorderTest3()
{
	beforeRecorderTest();
	START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
	int nRet = -1;
	int nFileSize = 0;
	QString sDiskEnabled;
	int nDiskSpaceReservedSize = 0 ;

	nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
	QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
	nRet = pDiskSetting->setUseDisks("F:");
	QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
	sDiskEnabled.clear();
	nRet = pDiskSetting->getUseDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
	QVERIFY2(sDiskEnabled == "F:"     ,"getUseDisks :out");
	nRet = pDiskSetting->setFilePackageSize(200);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
	nRet = pDiskSetting->getFilePackageSize(nFileSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
	QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
	nRet = pDiskSetting->setLoopRecording(false);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
	bool bIsLoopRecord = pDiskSetting->getLoopRecording();
	QVERIFY2(false == bIsLoopRecord      ,"getLoopRecording :return");
	nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
	nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
	QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");

    QString sDevName("3000");
    nRet = Itest->SetDevInfo(sDevName,  1);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	nRet = Itest->Start();
	QVERIFY2(IRecorder::OK == nRet,"Start() :return");


	FILE *pFile = NULL;
	char  pFileName[] = "CIF_12fps_128kbps.h264";
	int   nlen = 0;
	char  data[1280*720];
	uint  nTimes = 0;
	uint  nTotalLoopTimes = 0;
	unsigned long long   i64FreeBytesAvailableOfE = 0;
	unsigned long long   i64TotalNumberOfBytesOfE = 0;
	unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;
	
	GetDiskFreeSpaceExQ("E:\\"
		, &i64FreeBytesAvailableOfE
		, &i64TotalNumberOfBytesOfE
		, &i64TotalNumberOfFreeBytesOfE);
	nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024 - 5000) / 4.58);
	QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

	NALU_HEADER_t nhead;
	while (nTimes < nTotalLoopTimes )
	{
		if (feof(pFile))
		{
			rewind(pFile);
			++nTimes;
		}
		memset(&nhead,0,sizeof(NALU_HEADER_t));
		memset(data,0,sizeof(data));
		QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
		QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
		nRet = Itest->InputFrame(nhead.isider, data,  nhead.size);
		QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	}

	fclose(pFile);
	pFile=NULL;

	nRet = Itest->Stop();
	QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

	END_RECORDER_UNIT_TEST(Itest, pDiskSetting);
}
// 4. ����InputFrame����cbuf�ͷ���ֵ
//getEnableDisks��ȡ���÷���	 | ����ֵΪIDisksSetting::OK; ���ֵΪ�� C:D:E:F:��
//setUseDisks��������ʹ��F:�̴洢¼���ļ�	| ����ֵΪIDisksSetting::OK;
//getUseDisks��֤ʹ�ô���ΪF:	            | ����ֵΪIDisksSetting::OK; ���ֵΪ�� F:��
//setFilePackageSize����¼���ļ�����СΪ200MB |	����ֵΪIDisksSetting::OK;
//getFilePackageSize��ȡ¼���ļ�����С������	 | ����ֵΪIDisksSetting::OK;���ֵΪ��200��
//setLoopRecording����Ϊѭ��¼�� |	����ֵΪIDisksSetting::OK;
//getLoopRecording��֤Ϊѭ��¼�� |	����ֵΪtrue
//setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB | 	����ֵΪIDisksSetting::OK;
//getDiskSpaceReservedSize��֤��һ������ֵ  |	����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//����SetDevInfo�����豸��Ϊ4000��ͨ����1     | ����ֵΪIRecorder:OK
//����Start()��ʼ¼��	 | ����ֵΪIRecorder::OK
//��CIF_12fps_128kbps.h264�ļ�,��ȡ����֡����ͨ��InputFrameд��,����InputFrameʱ���ò���cbufΪNULL, ����������ȷ | 	����ֵΪIRecorder: E_PARAMETER_ERROR
//����Stop()ֹͣ	 | ����ֵΪIRecorder::OK,  F��¼���ļ���
void RecorderTest::RecorderTest4()
{
	beforeRecorderTest();
	START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
	int nRet = -1;
	int nFileSize = 0;
	QString sDiskEnabled;
	int nDiskSpaceReservedSize = 0 ;

    nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
	QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
	nRet = pDiskSetting->setUseDisks("F:");
	QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
	sDiskEnabled.clear();
	nRet = pDiskSetting->getUseDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
	QVERIFY2(sDiskEnabled == "F:"       ,"getUseDisks :out");
	nRet = pDiskSetting->setFilePackageSize(200);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
	nRet = pDiskSetting->getFilePackageSize(nFileSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
	QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
	nRet = pDiskSetting->setLoopRecording(true);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
	bool bIsLoopRecord = pDiskSetting->getLoopRecording();
	QVERIFY2(true == bIsLoopRecord      ,"getLoopRecording :return");
	nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
	nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
	QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");

    QString sDevName("4000");
    nRet = Itest->SetDevInfo(sDevName,  1);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	nRet = Itest->Start();
	QVERIFY2(IRecorder::OK == nRet,"Start() :return");


	FILE *pFile = NULL;
	char  pFileName[] = "CIF_12fps_128kbps.h264";
	int   nlen = 0;
	char  data[1280*720];
	uint  nTimes = 0;
	uint  nTotalLoopTimes = 0;
	unsigned long long   i64FreeBytesAvailableOfE = 0;
	unsigned long long   i64TotalNumberOfBytesOfE = 0;
	unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;

	GetDiskFreeSpaceExQ("E:\\"
		, &i64FreeBytesAvailableOfE
		, &i64TotalNumberOfBytesOfE
		, &i64TotalNumberOfFreeBytesOfE);
	nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024  - 5000) / 4.58);
	QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

	NALU_HEADER_t nhead;
	while (nTimes < nTotalLoopTimes)
	{
		if (feof(pFile))
		{
			rewind(pFile);
			++nTimes;
		}
		memset(&nhead,0,sizeof(NALU_HEADER_t));
		memset(data,0,sizeof(data));
		QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
		QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
		nRet = Itest->InputFrame(nhead.isider, NULL,  nhead.size);
		QVERIFY2(IRecorder::E_PARAMETER_ERROR == nRet  ,"InputFrame :return");
	}

	fclose(pFile);
	pFile=NULL;
	nRet = Itest->Stop();
	QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

	END_RECORDER_UNIT_TEST(Itest, pDiskSetting);
}
// 5. ����InputFrame����type�ͷ���ֵ
//getEnableDisks��ȡ���÷���	| ����ֵΪIDisksSetting::OK; ���ֵΪ�� C:D:E:F:��
//setUseDisks��������ʹ��F:�̴洢¼���ļ� |	����ֵΪIDisksSetting::OK;
//getUseDisks��֤ʹ�ô���ΪF: | 	����ֵΪIDisksSetting::OK; ���ֵΪ�� F:��
//setFilePackageSize����¼���ļ�����СΪ200MB|	����ֵΪIDisksSetting::OK;
//getFilePackageSize��ȡ¼���ļ�����С������	 | ����ֵΪIDisksSetting::OK;���ֵΪ��200��
//setLoopRecording����Ϊѭ��¼��	| ����ֵΪIDisksSetting::OK;
//getLoopRecording��֤Ϊѭ��¼��	| ����ֵΪtrue
//setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB	| ����ֵΪIDisksSetting::OK;
//getDiskSpaceReservedSize��֤��һ������ֵ	| ����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//����SetDevInfo�����豸��Ϊ5000��ͨ����1     | ����ֵΪIRecorder:OK
//����Start()��ʼ¼��	| ����ֵΪIRecorder::OK
//��CIF_12fps_128kbps.h264�ļ�,��ȡ����֡����ͨ��InputFrameд��,����InputFrame ʱ���ò���typeΪ0x3, ����������ȷ	| ����ֵΪIRecorder: E_PARAMETER_ERROR
//����Stop()ֹͣ	| ����ֵΪIRecorder::OK,  F��¼���ļ���
void RecorderTest::RecorderTest5()
{
	beforeRecorderTest();
	START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
	int nRet = -1;
	int nFileSize = 0;
	QString sDiskEnabled;
	int nDiskSpaceReservedSize = 0 ;

	nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
	QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
	nRet = pDiskSetting->setUseDisks("F:");
	QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
	sDiskEnabled.clear();
	nRet = pDiskSetting->getUseDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
	QVERIFY2(sDiskEnabled == "F:"       ,"getUseDisks :out");
	nRet = pDiskSetting->setFilePackageSize(200);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
	nRet = pDiskSetting->getFilePackageSize(nFileSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
	QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
	nRet = pDiskSetting->setLoopRecording(true);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
	bool bIsLoopRecord = pDiskSetting->getLoopRecording();
	QVERIFY2(true == bIsLoopRecord      ,"getLoopRecording :return");
	nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
	nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
	QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");

    QString sDevName("5000");
    nRet = Itest->SetDevInfo(sDevName,  1);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	nRet = Itest->Start();
	QVERIFY2(IRecorder::OK == nRet,"Start() :return");

	FILE *pFile = NULL;
	char  pFileName[] = "CIF_12fps_128kbps.h264";
	int   nlen = 0;
	char  data[1280*720];
	uint  nTimes = 0;
	uint  nTotalLoopTimes = 0;
	unsigned long long   i64FreeBytesAvailableOfE = 0;
	unsigned long long   i64TotalNumberOfBytesOfE = 0;
	unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;

	GetDiskFreeSpaceExQ("E:\\"
		, &i64FreeBytesAvailableOfE
		, &i64TotalNumberOfBytesOfE
		, &i64TotalNumberOfFreeBytesOfE);
	nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024  - 5000) / 4.58);
	QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

	NALU_HEADER_t nhead;
	while (nTimes < nTotalLoopTimes)
	{
		if (feof(pFile))
		{
			rewind(pFile);
			++nTimes;
		}
		memset(&nhead,0,sizeof(NALU_HEADER_t));
		memset(data,0,sizeof(data));
		QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
		QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
		nRet = Itest->InputFrame(0x3, data,  nhead.size);
		QVERIFY2(IRecorder::E_PARAMETER_ERROR == nRet  ,"InputFrame :return");
	}

	fclose(pFile);
	pFile=NULL;
	nRet = Itest->Stop();
	QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

	END_RECORDER_UNIT_TEST(Itest, pDiskSetting);
}

// 6. ����InputFrame����buffersize�ͷ���ֵ
//getEnableDisks��ȡ���÷���	| ����ֵΪIDisksSetting::OK; ���ֵΪ�� C:D:E:F:��
//setUseDisks��������ʹ��F:�̴洢¼���ļ� | ����ֵΪIDisksSetting::OK;
//getUseDisks��֤ʹ�ô���ΪF:  | 	����ֵΪIDisksSetting::OK; ���ֵΪ�� F:��
//setFilePackageSize����¼���ļ�����СΪ200MB	| ����ֵΪIDisksSetting::OK;
//getFilePackageSize��ȡ¼���ļ�����С������	 | ����ֵΪIDisksSetting::OK;���ֵΪ��200��
//setLoopRecording����Ϊѭ��¼�� |	����ֵΪIDisksSetting::OK;
//getLoopRecording��֤Ϊѭ��¼�� |	����ֵΪtrue
//setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB	| ����ֵΪIDisksSetting::OK;
//getDiskSpaceReservedSize��֤��һ������ֵ	| ����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//����SetDevInfo�����豸��Ϊ6000��ͨ����1     | ����ֵΪIRecorder:OK
//����Start()��ʼ¼��	| ����ֵΪIRecorder::OK
//��CIF_12fps_128kbps.h264�ļ�,��ȡ����֡����ͨ��InputFrameд�� ,�ظ�n��,����InputFrameʱ���ò���buffersizeΪ-300, ����������ȷ	| ����ֵΪIRecorder: E_PARAMETER_ERROR
//����Stop()ֹͣ	| ����ֵΪIRecorder::OK,  F��¼���ļ���
void RecorderTest::RecorderTest6()
{
	beforeRecorderTest();
	START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
	int nRet = -1;
	int nFileSize = 0;
	QString sDiskEnabled;
	int nDiskSpaceReservedSize = 0 ;

	nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
	QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
	nRet = pDiskSetting->setUseDisks("F:");
	QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
	sDiskEnabled.clear();
	nRet = pDiskSetting->getUseDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
	QVERIFY2(sDiskEnabled == "F:"       ,"getUseDisks :out");
	nRet = pDiskSetting->setFilePackageSize(200);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
	nRet = pDiskSetting->getFilePackageSize(nFileSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
	QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
	nRet = pDiskSetting->setLoopRecording(true);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
	bool bIsLoopRecord = pDiskSetting->getLoopRecording();
	QVERIFY2(true == bIsLoopRecord      ,"getLoopRecording :return");
	nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
	nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
	QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");

    QString sDevName("6000");
    nRet = Itest->SetDevInfo(sDevName,  1);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	nRet = Itest->Start();
	QVERIFY2(IRecorder::OK == nRet,"Start() :return");

	FILE *pFile = NULL;
	char  pFileName[] = "CIF_12fps_128kbps.h264";
	int   nlen = 0;
	char  data[1280*720];
	uint  nTimes = 0;
	uint  nTotalLoopTimes = 0;
	unsigned long long   i64FreeBytesAvailableOfE = 0;
	unsigned long long   i64TotalNumberOfBytesOfE = 0;
	unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;

	GetDiskFreeSpaceExQ("E:\\"
		, &i64FreeBytesAvailableOfE
		, &i64TotalNumberOfBytesOfE
		, &i64TotalNumberOfFreeBytesOfE);
	nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024  - 5000) / 4.58);
	QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

	NALU_HEADER_t nhead;
	while (nTimes < nTotalLoopTimes)
	{
		if (feof(pFile))
		{
			rewind(pFile);
			++nTimes;
		}
		memset(&nhead,0,sizeof(NALU_HEADER_t));
		memset(data,0,sizeof(data));
		QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
		QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
		nRet = Itest->InputFrame(nhead.isider, data,  -300);
		QVERIFY2(IRecorder::E_PARAMETER_ERROR == nRet  ,"InputFrame :return");
	}

	fclose(pFile);
	pFile=NULL;
	nRet = Itest->Stop();
	QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

	END_RECORDER_UNIT_TEST(Itest, pDiskSetting);
}

// 7. ����SetDevInfo����nChannelNum�ͷ���ֵ
//getEnableDisks��ȡ���÷��� |	����ֵΪIDisksSetting::OK; ���ֵΪ�� C:D:E:F:��
//setUseDisks��������ʹ��F:�̴洢¼���ļ� |����ֵΪIDisksSetting::OK;
//getUseDisks��֤ʹ�ô���ΪF:|	����ֵΪIDisksSetting::OK; ���ֵΪ�� F:��
//setFilePackageSize����¼���ļ�����СΪ200MB|	����ֵΪIDisksSetting::OK;
//getFilePackageSize��ȡ¼���ļ�����С������|	����ֵΪIDisksSetting::OK;���ֵΪ��200��
//setLoopRecording����Ϊѭ��¼��	|����ֵΪIDisksSetting::OK;
//getLoopRecording��֤Ϊѭ��¼��	|����ֵΪtrue
//setDiskSpaceReservedSize���ô��̱����ռ��СΪ5000MB |	����ֵΪIDisksSetting::OK;
//getDiskSpaceReservedSize��֤��һ������ֵ | ����ֵΪIDisksSetting::OK;���ֵΪ��5000��
//����SetDevInfo�����豸��Ϊ7000��ͨ����1     | ����ֵΪIRecorder:OK
//����Start()��ʼ¼�� | ����ֵΪIRecorder::OK
//��CIF_12fps_128kbps.h264�ļ�,��ȡ����֡����ͨ��InputFrameд�� ,�ظ�n��,������ȷ�Ĳ���	| ����ֵΪIRecorder: :OK
//�ﵽn�κ�, ����SetDevInfo����nChannelNumΪ17��-200 devname��Ϊ"7001"| ����ֵΪIRecorder: : E_PARAMETER_ERROR
//����Stop()ֹͣ | ����ֵΪIRecorder::OK,  F����¼���ļ�������Ŀ¼Ϊ17��-200

void RecorderTest::RecorderTest7()
{
	beforeRecorderTest();
	START_RECORDER_UNIT_TEST(Itest, pDiskSetting);
	int nRet = -1;
	int nFileSize = 0;
	QString sDiskEnabled;
	int nDiskSpaceReservedSize = 0 ;

	nRet = pDiskSetting->getEnableDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getEnableDisks :return");
	QVERIFY2(sDiskEnabled == "C:D:E:F:" ,"getEnableDisks :out");
	nRet = pDiskSetting->setUseDisks("F:");
	QVERIFY2(IDisksSetting::OK == nRet  ,"setUseDisks :return");
	sDiskEnabled.clear();
	nRet = pDiskSetting->getUseDisks(sDiskEnabled);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getUseDisks :return");
	QVERIFY2(sDiskEnabled == "F:"     ,"getUseDisks :out");
	nRet = pDiskSetting->setFilePackageSize(200);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setFilePackageSize :return");
	nRet = pDiskSetting->getFilePackageSize(nFileSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getFilePackageSize :return");
	QVERIFY2(200 == nFileSize           ,"getFilePackageSize :out");
	nRet = pDiskSetting->setLoopRecording(false);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setLoopRecording :return");
	bool bIsLoopRecord = pDiskSetting->getLoopRecording();
	QVERIFY2(false == bIsLoopRecord      ,"getLoopRecording :return");
	nRet = pDiskSetting->setDiskSpaceReservedSize(5000);
	QVERIFY2(IDisksSetting::OK == nRet  ,"setDiskSpaceReservedSize :return");
	nRet = pDiskSetting->getDiskSpaceReservedSize(nDiskSpaceReservedSize);
	QVERIFY2(IDisksSetting::OK == nRet  ,"getDiskSpaceReservedSize :return");
	QVERIFY2(5000 == nDiskSpaceReservedSize,"getDiskSpaceReservedSize :out");

    QString sDevName("7000");
    nRet = Itest->SetDevInfo(sDevName,  1);
    QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	nRet = Itest->Start();
	QVERIFY2(IRecorder::OK == nRet,"Start() :return");


	FILE *pFile = NULL;
	char  pFileName[] = "CIF_12fps_128kbps.h264";
	int   nlen = 0;
	char  data[1280*720];
	uint  nTimes = 0;
	uint  nTotalLoopTimes = 0;
	unsigned long long   i64FreeBytesAvailableOfE = 0;
	unsigned long long   i64TotalNumberOfBytesOfE = 0;
	unsigned long long   i64TotalNumberOfFreeBytesOfE = 0;

	GetDiskFreeSpaceExQ("E:\\"
		, &i64FreeBytesAvailableOfE
		, &i64TotalNumberOfBytesOfE
		, &i64TotalNumberOfFreeBytesOfE);
	nTotalLoopTimes = (uint)( ((float)i64FreeBytesAvailableOfE/1024/1024 - 5000) / 4.58);
	QVERIFY2 (NULL != (pFile = fopen(pFileName,"rb")), "File Open Error");

	NALU_HEADER_t nhead;
    bool bIsDevInfoSetFlag = false;
	while (nTimes < nTotalLoopTimes - 900)
	{
		if (feof(pFile))
		{
			rewind(pFile);
			++nTimes;
		}
		if(nTimes > nTotalLoopTimes - 1000 && !bIsDevInfoSetFlag)
		{
			bIsDevInfoSetFlag = true;
            sDevName.clear();
			sDevName="7001";
			nRet = Itest->SetDevInfo(sDevName,  17);
			QVERIFY2(IRecorder::E_PARAMETER_ERROR== nRet  ,"SetDevInfo :return");
			nRet = Itest->SetDevInfo(sDevName,  -200);
			QVERIFY2(IRecorder::E_PARAMETER_ERROR== nRet  ,"SetDevInfo :return");
		}
		memset(&nhead,0,sizeof(NALU_HEADER_t));
		memset(data,0,sizeof(data));
		QVERIFY2((nlen = fread(&nhead,sizeof(NALU_HEADER_t),1,pFile)) > 0, "fread Error");
		QVERIFY2((nlen = fread(data,1,nhead.size,pFile)) > 0, "fread Error");
		nRet = Itest->InputFrame(nhead.isider, data,  nhead.size);
		QVERIFY2(IRecorder::OK == nRet  ,"InputFrame :return");
	}
	fclose(pFile);
	pFile=NULL;

	nRet = Itest->Stop();
	QVERIFY2(IRecorder::OK == nRet,"Stop() :return");

	END_RECORDER_UNIT_TEST(Itest, pDiskSetting);

}