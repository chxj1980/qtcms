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
#define OVERWRITE 0
#define  ADDWRITE 1
#define  BUFFERSIZE 120//��λ��M
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
}tagRecordDatCoreFileInfo;
typedef enum __tagRecordDatStepCode{
	recordDat_init,//���������ʼ��
	recordDat_filePath,//����д�ļ���·��
	recordDat_initMemory,//��ʼ���ڴ��
	recordDat_writeMemory,//����֡д���ڴ������ݿ���Ŀ����
	recordDat_writeDisk,//�ڴ��д������
	recordDat_default,//����Ƿ���Ҫд�����̣�����Ƿ�������֡����
	recordDat_error,//����
	recordDat_end//����
}tagRecordDatStepCode;
typedef enum __tagObtainFilePathStepCode{
	obtainFilePath_getDrive,//��ȡ��¼���̷�
	obtainFilePath_diskUsable,//��ʣ��ռ�Ŀ�¼����̷�
	obtainFilePath_diskFull,//ÿ���̷����Ѿ�¼��
	obtainFilePath_createFile,//����ļ������ڣ��򴴽��ļ�
	obtainFilePath_success,//��ȡ¼���ļ�·���ɹ�
	obtainFilePath_fail,//��ȡ¼���ļ�·��ʧ��
	obtainFilePath_end//����
}tagObtainFilePathStepCode;
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
protected:
	void run();
private slots:
	void slcheckBlock();
private:
	void eventCallBack(QString sEventName,QVariantMap tInfo);
	int obtainFilePath(QString &sWriteFilePath);//0:����д��1����д�ļ���2��û���ļ���д
	QString getUsableDisk(QString &sDiskLisk);
	QString getLatestItem(QString sDisk);
	bool checkFileIsFull(QString sFilePath);
	bool createNewFile(QString sFilePath);
private:
	QMap<int ,BufferQueue *> m_tBufferQueueMap;
	tagRecordDatCoreFileInfo m_tFileInfo;
	QMutex m_tBufferQueueMapLock;
	volatile bool m_bStop;
	QStringList m_tEventNameList;
	QMultiMap<QString ,tagRecordDatCoreProcInfo> m_tEventMap;
	int m_nPosition;
	QTimer m_tCheckIsBlockTimer;
	bool m_bIsBlock;
	OperationDatabase m_tOperationDatabase;
	QByteArray m_tFileHead;
	char *m_pDataBuffer1;
	char *m_pDataBuffer2;
	char *m_pDataBuffer;
};

