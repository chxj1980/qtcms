#pragma once
#include <QThread>
#include <IEventRegister.h>
#include <IRecorder.h>
#include <QMutex>
#include <QMultiMap>
#include <QDebug>
#include <QQueue>
#include <QEventLoop>
#include <QTimer>
#include <avilib.h>

#define AVENC_IDR		0x01
#define AVENC_PSLICE	0x02
#define AVENC_AUDIO		0x00
#define DATA_QUEUE_MAX_SIZE 120 //���洢5���ӵĻ��棬����������ͻᶪ��
typedef int (__cdecl *recorderExEventCb)(QString sEventName,QVariantMap tInfo,void *pUser);
typedef struct __tagRecorderExProcInfo{
	recorderExEventCb proc;
	void *pUser;
}tagRecorderExProcInfo;
typedef struct __tagRecorderExNode{
	int iChannel;
	unsigned int uiDataType;
	unsigned int uiBufferSize;
	unsigned int uiTicketCount;
	char *pBuffer;
	int iSampleRate;
	int iSampleWidth;
	char cEncode[8];
}tagRecorderExNode;
typedef struct __tagRecorderExInfo{
	QString sFilePath;
	QString sDeviceName;
	int iChannel;
}tagRecorderExInfo;
typedef enum __tagRecorderStepCode{
	REC_INIT,//���ļ��ĸ��������ʼ��
	REC_FRIST_I_FRAME,//�ȴ���һ��I֡
	REC_CREATE_PATH,//�����ļ�·��������¼�����ݱ�¼������������ռ䣬�����ļ���
	REC_OPEN_FILE,//���ļ�
	REC_SET_VIDEO_PARM,// �����ļ�����Ƶ���ĸ������
	REC_SET_AUDIO_PARM,//�����ļ�����Ƶ���ĸ������
	REC_WRITE_FRAME,//д�ļ�
	REC_CHECK_AND_UPDATE,//���Ӳ�̿ռ�,����ļ���С,�������ݿ�
	REC_WAIT_FOR_PACK,
	REC_PACK,//�ļ����
	REC_ERROR,
	REC_END,
}tagRecorderStepCode;
class RecorderEx:public QThread,
	public IRecorder,
	public IEventRegister
{
	Q_OBJECT
public:
	RecorderEx(void);
	~RecorderEx(void);
public:
	//IRecorder
	virtual int Start();
	virtual int Stop();
	virtual int InputFrame(QVariantMap& frameinfo);
	virtual int SetDevInfo(const QString& devname,int nChannelNum);

	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );
	virtual unsigned long __stdcall AddRef();
	virtual unsigned long __stdcall Release();

	virtual QStringList eventList();
	virtual int queryEvent(QString eventName,QStringList& eventParams);
	virtual int registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser);
protected:
	void run();
private slots:
	void slCheckBlock();
private:
	void clearData();
	void sleepEx(int iTime);
	void eventProcCall(QString sEvent,QVariantMap tInfo);
	bool createFilePath();
	int checkALL();//0:����¼��1��¼������2����������ֹͣ¼��
	int checkFileSize();//0:�ļ���С�㹻�ˣ����Դ����1����ʾ�ļ���С�����㣬����¼��2����ʾ�д���
	int checkDiskSize();//0:��ʾ���̿ռ��㹻������¼��1����ʾ���̿ռ䲻��
	bool upDateDataBase();//�������ݿ⣬0����ʾ���³ɹ���1����ʾ����ʧ��
	bool packFile(avi_t *pAviFile);
private:
	int m_nRef;
	QMutex m_csRef;
	QMutex m_csDataLock;
	QStringList m_tEventList;
	QMultiMap<QString,tagRecorderExProcInfo> m_tEventMap;
	bool m_bStop;
	QQueue<tagRecorderExNode> m_tDataQueue;
	int m_iSleepSwitch;
	QTimer m_tCheckBlockTimer;
	bool m_bBlock;
	int m_iPosition;
	int m_iVideoWidth;
	int m_iVideoHeight;
	int m_iFrameCount;
	int m_iLastTicket;
	unsigned int m_uiFrameCountArray[31];
	bool m_bFull;
	tagRecorderExInfo m_tRecorderInfo;
	quint64 m_iFristPts;
	quint64 m_iLastPts;
	int m_iCheckBlockCount;
	bool m_bChecKFileSize;
	bool m_bCheckDiskSize;
	bool m_bUpdateDatabase;
};

