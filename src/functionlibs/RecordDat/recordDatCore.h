#pragma once
#include <QThread>
#include "BufferQueue.h"
#include <QMap>
#include <QMultiMap>
#include <QMutex>
#include <QStringList>
#include <QDebug>
#include <QTimer>
#include "recorddat_global.h"
#include "OperationDatabase.h"
#include <QFile>
#include <QByteArray>
#include <QIODevice>
#include "WriteToDisk.h"
#include <IDisksSetting.h>
#include <QEventLoop>
#include <QList>
#define OVERWRITE 0
#define  ADDWRITE 1

typedef int (__cdecl *recordDatCoreEventCb)(QString sEventName,QVariantMap tInfo,void *pUser);
typedef struct __tagRecordDatCoreProcInfo{
	recordDatCoreEventCb proc;
	void *pUser;
}tagRecordDatCoreProcInfo;
typedef struct __tagRecordDatCoreWndInfo{
	unsigned int uiHistoryRecordType;
	unsigned int uiCurrentRecordType;
	unsigned int uiPreFrame;
	unsigned int uiPreIFrame;
	unsigned int uiChannelInDatabaseId;
}tagRecordDatCoreWndInfo;
typedef struct __tagRecordDatCoreFileInfo{
	QString sFilePath;
	QMap<int ,tagRecordDatCoreWndInfo> tWndInfo;
	QMap<int ,int> tFristIFrameIndex;
	QMap<int ,int> tHistoryFrameIndex;
	QMap<int ,int >tHistoryIFrameIndex;
}tagRecordDatCoreFileInfo;
typedef struct __tagRecordDatDatabaseInfo{
	QList<int > tRemoveChannel;
	QMap<int ,int> tChannelInRecordDatabaseId;
	QMap<int ,int> tChannelInSearchDatabaseId;
}tagRecordDatDatabaseInfo;
typedef struct __tagRecordDatabaseMaxID{
	quint64 uiMaxRecordId;
	quint64 uiMaxSearchId;
}tagRecordDatabaseMaxID;
typedef enum __tagRecordDatStepCode{
	recordDat_init,//���������ʼ��
	recordDat_filePath,//����д�ļ���·��
	recordDat_initMemory,//��ʼ���ڴ��
	recordDat_writeMemory,//����֡д���ڴ������ݿ���Ŀ����
	recordDat_writeDisk,//�ڴ��д������
	recordDat_default,//����Ƿ���Ҫд�����̣�����Ƿ�������֡����
	recordDat_error,//����
	recordDat_reset,//���������
	recordDat_end//����
}tagRecordDatStepCode;
typedef enum __tagRecordDatWriteToBufferStepCode{
	WriteToBuffer_Init,
	WriteToBuffer_00,//historyType==currentType==0,���κβ���
	WriteToBuffer_01,//historyType==0,currentType!=0,��ʼ¼����Ҫ�ȴ�I֡
	WriteToBuffer_10,//historyType!=0,currentType==0,ֹͣ¼��
	WriteToBuffer_011,//historyType==currentType!=0,����û��ת�䣬����¼��
	WriteToBuffer_111,//historyType!=currentType!=0,����ת��������¼��
	WriteToBuffer_Write,//дһ֡����
	WriteToBuffer_end,
}tagRecordDatWriteToBufferStepCode;
typedef enum __tagRecordDatReset{
	recordDatReset_fileError,//�ļ�����
	recordDatReset_outOfDisk,//���̿ռ䲻��
	recordDatReset_searchDatabase,//�������ݿ����ʧ��
	recordDatReset_recordDatabase,//¼�����ݿ����ʧ��
	recordDatReset_writeToDisk,//д����ʧ��
}tagRecordDatReset;
typedef enum __tagRecordDatToDiskType{
	recordDatToDiskType_null,//������
	recordDatToDiskType_outOfTime,//��ʱдӲ��
	recordDatToDiskType_bufferFull,//������дӲ��
}tagRecordDatToDiskType;

typedef enum __tagRecordDatTurnType{
	recordDatTurnType_noRecord,//historyType==currentType==0,���κβ���
	recordDatTurnType_beginRecord,//historyType==0,currentType!=0,��ʼ¼����Ҫ�ȴ�I֡
	recordDatTurnType_stopRecord,//historyType!=0,currentType==0,ֹͣ¼��
	recordDatTurnType_noTurn,//historyType==currentType!=0,����û��ת�䣬����¼��
	recordDatTurnType_turnType//historyType!=currentType!=0,����ת��������¼��
}tagRecordDatTurnType;
class recordDatCore:public QThread
{
	Q_OBJECT
public:
	recordDatCore(void);
	~recordDatCore(void);
public:
	bool startRecord();
	bool setBufferQueue(int nWnd,BufferQueue &tBufferQueue);
	bool removeBufferQueue(int nWnd);
	bool setRecordType(int nWnd,int nType,bool bFlags);
	void registerEvent(QString sEventName,int(__cdecl *proc)(QString,QVariantMap,void*),void *pUser);
	void reloadSystemDatabase();
protected:
	void run();
private slots:
	void slcheckBlock();
	void slsetWriteDiskFlag();
private:
	void eventCallBack(QString sEventName,QVariantMap tInfo);
	int obtainFilePath(QString &sWriteFilePath);//0:����д��1����д�ļ���2��û���ļ���д
	//QString getUsableDisk(QString &sDiskLisk);//����ֵ����ʣ��ռ���õ��̷�������������¼���̷��б�
	//QString getLatestItem(QString sDisk);//������ʽ��D��
	/*bool checkFileIsFull(QString sFilePath);*/
	/*bool createNewFile(QString sFilePath);*/
	bool getIsRecover();
	void sleepEx(quint64 uiTime);
	bool updateSearchDatabase();
	bool updateRecordDatabase(int nUpdateType);
	bool createSearchDatabaseItem(int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId);
	bool createRecordDatabaseItem(int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId);
	bool writeTodisk();
	void setChannelNumInFileHead();
	void setFileStartTime(quint64 &uiStartTime,quint64 &uiEndTime);
	int writeToBuffer(int nChannel,QString sFilePath);//����ֵ����λ����00��bufferδ��&&ûд��buffer��01��bufferδ��&&д��buffer��10��buffer����&&δд��buffer��11��buffer����&&д��buffer
private:
	QMap<int ,BufferQueue *> m_tBufferQueueMap;
	tagRecordDatCoreFileInfo m_tFileInfo;
	QMutex m_tBufferQueueMapLock;
	volatile bool m_bStop;
	QStringList m_tEventNameList;
	QMultiMap<QString ,tagRecordDatCoreProcInfo> m_tEventMap;
	int m_nPosition;
	QTimer m_tCheckIsBlockTimer;
	QTimer m_tWriteDiskTimer;
	bool m_bIsBlock;
	OperationDatabase m_tOperationDatabase;
	QByteArray m_tFileHead;
	char *m_pDataBuffer1;
	char *m_pDataBuffer2;
	char *m_pDataBuffer;
	tagRecordDatReset m_tResetType;
	tagRecordDatToDiskType m_tToDiskType;
	int m_nSleepSwitch;
	int m_nWriteMemoryChannel;
	bool m_bWriteDiskTimeFlags;
	int m_nWriteDiskTimeCount;
	tagRecordDatDatabaseInfo m_tDatabaseInfo;
	WriteToDisk m_tWriteToDisk;
	volatile bool m_bReloadSystemDatabase;
	QMultiMap<QString,tagRecordDatabaseMaxID> m_tRecordDatabaseMaxId;
	QList<quint64> m_tInsertSearchDatabaseTime;
	QMap<quint64,tagRecordItem> m_tRecordItem;
};

