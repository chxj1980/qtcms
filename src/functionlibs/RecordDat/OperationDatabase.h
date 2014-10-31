#pragma once
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <IDisksSetting.h>
#include <QDebug>
#include <QtSql>
#include <QMap>
#include "recorddat_global.h"
#include "freeDisk.h"
#include <QThread>
#include <QQueue>
#include <QMutex>
#include "sqlite3.h"
//ʹ�ð�ȫ����
//1.���̴�С���ܳ�����256*256*256*128M
//2.
#define MAXFILENUM 256
#define FILLNEWFILESIZE 1048576
typedef struct __tagSystemDatabaseInfo{
	QString sAllRecordDisk;//���е�¼���̷�
	volatile bool bIsRecover;//�Ƿ񸲸�¼��
	volatile quint64 uiDiskReservedSize;//���̱����ռ�
}tagSystemDatabaseInfo;
typedef enum __tagOperationDatabaseCode{
	OperationDatabase_init,
	OperationDatabase_obtainFilePath,//��ȡ¼���ļ�·��
	OperationDatabase_updateRecordDatabase,//����¼�����ݿ�
	OperationDatabase_updateSearchDatabase,//�����������ݿ�
	OperationDatabase_createSearchDatabaseItem,//�����������ݿ�item
	OperationDatabase_createRecordDatabaseItem,//����¼�����ݿ�item
	OperationDatabase_setRecordFileStatus,//�����ļ����ݿ�״̬
	OperationDatabase_reloadSystemDatabase,//����ϵͳ���ݿ�
	OperationDatabase_clearInfoInDatabase,//������ݿ�����
	OperationDatabase_isRecordDataExistItem,
	OperationDatabase_getMaxDatabaseId,
	OperationDatabase_default
}tagOperationDatabaseCode;
typedef struct __tagCodeWithParm{
	tagOperationDatabaseCode tCode;
	QVariantMap tInfo;
}tagCodeWithParm;
class OperationDatabase:public QThread
{
	Q_OBJECT
public:
	OperationDatabase(void);
	~OperationDatabase(void);
public:
	void reloadSystemDatabase();
	bool updateRecordDatabase(QList<int> tIdList,QVariantMap tInfo,QString sFilePath);//uiEndTime
	bool updateSearchDatabase(QList<int> tIdList,QVariantMap tInfo,QString sFilePath);//uiEndTime
	bool createSearchDatabaseItem(int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId);
	bool createRecordDatabaseItem(int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId);
	void setRecordFileStatus(QString sFilePath,QVariantMap tInfo);
	bool getIsRecover();
	tagSystemDatabaseInfo getSystemDatabaseInfo();
	bool isDiskSpaceOverReservedSize();
	bool isRecordDataExistItem();
	//
	int obtainFilePath(QString &sWriteFilePath);//0:����д��1����д�ļ���2��û���ļ���д
	void clearInfoInDatabaseEx(QString sFilePath);//unuse
	bool startOperationDatabase();
	bool stopOperationDatabase();
	bool getMaxDatabaseId(quint64 &uiMaxRecordId,quint64 &uiMaxSearchId,QString sFilePath);
protected:
	void run();
private:
	bool createRecordDatabase(QString sDatabasePath);
	void priSetRecordFileStatus(QString sFilePath,QVariantMap tInfo);
	quint64 countFileNum(QString sFilePath);
	bool execCommand(QSqlQuery & tQuery,QString sCommand);
	SQLITE_API int sqlite3_exec_reTry(
		sqlite3*,                                  /* An open database */
		const char *sql,                           /* SQL to be evaluated */
		int (*callback)(void*,int,char**,char**),  /* Callback function */
		void *,                                    /* 1st argument to callback */
		char **errmsg                              /* Error msg written here */
		);
private:
	int priObtainFilePath(QString &sWriteFilePath);//0:����д��1����д�ļ���2��û���ļ���д
	bool priUpdateRecordDatabase(QList<int> tIdList,QVariantMap tInfo,QString sFilePath);//uiEndTime
	bool priUpdateSearchDatabase(QList<int> tIdList,QVariantMap tInfo,QString sFilePath);//uiEndTime
	bool priCreateSearchDatabaseItem(int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId);
	bool priCreateRecordDatabaseItem(int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId);
	void priSetRecordFileStatusEx(QString sFilePath,QVariantMap tInfo);
	void priReloadSystemDatabase();
	void priClearInfoInDatabase(QString sFilePath);
	bool PriIsRecordDataExistItem();
	bool PriGetMaxDatabaseId(quint64 &uiMaxRecordId,quint64 &uiMaxSearchId,QString sFilePath);
	void clearInfoIndatabaseWithNativeAPIs(QString sFilePath);
	//
	bool createNewFile(QString sFilePath);
	QString createLatestItemEx(QString sDisk);//���ڴ��̻��пռ䣬����ģʽ
	bool checkFileIsFull(QString sFilePath);
	QString getLatestItemEx(QString sDisk);//d:
	QString getLatestItemExx(QString sDisk);//d:
	QString getOldestItemEx(QString sDisk,quint64 &uiStartTime);//d:
	//QString getOldestItemExx(QString sDisk);//d:
	QString getUsableDiskEx(QString &sDiskLisk);//����ֵ����ʣ��ռ���õ��̷�������������¼���̷��б�
private:
	IDisksSetting *m_pDisksSetting;
	tagSystemDatabaseInfo m_tSystemDatabaseInfo;
	QMap<QString,QMap<int ,QString>> m_tDeleteFileList;
	freeDisk m_tFreeDisk;
	QQueue<tagCodeWithParm> m_tStepCode;
	bool m_bStop;
	QByteArray m_tFileHead;
	char *m_pNewFile;
	QMutex m_tStepCodeLock;
	QMutex m_tObtainFilePathLock;
	QQueue<QVariantMap> m_tObtainFilePathResult;//sWriteFilePath nReturn
	QQueue<bool> m_tIsRecordDataExistItemResult;//nReturn
	QQueue<QVariantMap> m_tGetMaxDatabaseIdResult;//uiMaxRecordId uiMaxSearchId
	QMap<QString,sqlite3 * >m_tNativeApiList;
};

