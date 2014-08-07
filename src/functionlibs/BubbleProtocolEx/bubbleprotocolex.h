#ifndef BUBBLEPROTOCOLEX_H
#define BUBBLEPROTOCOLEX_H

#include "bubbleprotocolex_global.h"
#include <IEventRegister.h>
#include <QMutex>
#include <QStringList>
#include <QMultiMap>
#include <QDebug>
#include <QThread>
#include <IDeviceConnection.h>
#include <IRemotePreview.h>
#include <IRemotePlayback.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QHostAddress>
#include <QDateTime>
#include <QQueue>
#include <QEventLoop>
#include <QTimer>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>
#include <QList>
#include <QDomDocument>
#include "SearchRemoteFile.h"
typedef int (__cdecl *bubbleProtocolEventCb)(QString sEventName,QVariantMap tInfo,void *pUser);
typedef struct __tagBubbleProInfo{
	bubbleProtocolEventCb proc;
	void *pUser;
}tagBubbleProInfo;
typedef struct __tagBubbleDeviceInfo{
	QHostAddress tIpAddr;
	QVariantMap tPorts;
	QString sEseeId;
	QString sUserName;
	QString sPassword;
	int nPreChannel;
	int nPreStream;
	bool bPrePause;
	int nRemoteSearchChannel;
	int nRemoteSearchTypes;
	QDateTime tRemoteSearchStartTime;
	QDateTime tRemoteSearchEndTime;
	int nRemotePlayChannel;
	int nRemotePlayTypes;
	QDateTime tRemotePlayStartTime;
	QDateTime tRemotePlayEndTime;
	QString sRemotePlayFile;
	bool bRemotePlayPause;
	QString sRemotePlayFileName;
}tagBubbleDeviceInfo;
typedef enum __tagBubbleConnectStatusInfo{
	BUBBLE_DISCONNECTED,
	BUBBLE_CONNECTTING,
	BUBBLE_CONNECTED,
	BUBBLE_DISCONNECTING
}tagBubbleConnectStatusInfo;
typedef enum __tagBubbleReciveStep{
	BUBBLE_RECEIVE_HTTP,//���bubble�Ƿ���ͨ��http����
	BUBBLE_RECEIVE_FRAME,//��������֡
	BUBBLE_RECEIVE_WAITMOREBUFFER,//�ȴ����������
	BUBBLE_RECEIVE_END//���ս�������
}tagBubbleReciveStep;
typedef enum __tagBubbleControlStep{
	BUBBLE_AUTHORITY,//�û���֤
	BUBBLE_DISCONNECT,//�Ͽ�����
	BUBBLE_GETLIVESTREAM,//��ȡԤ������
	BUBBLE_STOPSTREAM,//ֹͣԤ��
	BUBBLE_PAUSESTREAM,//��ͣԤ��
	BUBBLE_HEARTBEAT,//����ָ��
	BUBBLE_GETPLAYBACKSTREAMBYTIME,//��ȡ�ط�����
	BUBBLE_GETPLAYBACKSTREAMBYFILENAME,//��ȡ�ط�����
	BUBBLE_PAUSEPLAYBACKSTREAM,//��ͣ�ط�����
	BUBBLE_STOPPLAYBACKSTREAM,//ֹͣ�ط�����
	BUBBLE_PTZ//��̨����
}tagBubbleControlStep;
typedef enum __tagBubbleRunStep{
	BUBBLE_RUN_CONNECT,//���ӵ��豸
	BUBBLE_RUN_RECEIVE,//���ܽ�������
	BUBBLE_RUN_CONTROL,//����ָ��
	BUBBLE_RUN_DISCONNECT,//�Ͽ�����
	BUBBLE_RUN_DEFAULT,//ȱʡ����
	BUBBLE_RUN_END//����
}tagBubbleRunStep;

class  BubbleProtocolEx:public QThread,
	public IEventRegister,
	public IRemotePreview,
	public IDeviceConnection,
	public IRemotePlayback
{
	Q_OBJECT
public:
	BubbleProtocolEx();
	~BubbleProtocolEx();

public:
	virtual long __stdcall QueryInterface( const IID & iid,void **ppv );

	virtual unsigned long __stdcall AddRef();
	virtual unsigned long __stdcall Release();

	virtual QStringList eventList();
	virtual int queryEvent(QString eventName,QStringList& eventParams);
	virtual int registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser);
	
	//interface for device connection
	virtual int setDeviceHost(const QString &sIpAddr);
	virtual int setDevicePorts(const QVariantMap &tPorts);
	virtual int setDeviceId(const QString &sEseeId);
	virtual int setDeviceAuthorityInfomation(QString sUserName,QString sPassword);
	virtual int connectToDevice();
	virtual int authority();
	virtual int disconnect();
	virtual int getCurrentStatus();
	virtual QString getDeviceHost();
	virtual QString getDeviceid();
	virtual QVariantMap getDevicePorts();

	//interface for remote preview
	virtual int getLiveStream(int nChannel,int nStream);
	virtual int stopStream();
	virtual int pauseStream(bool bPause);
	virtual int getStreamCount();
	virtual int getStreamInfo(int nStreamId,QVariantMap &tStreamInfo);

	 //interface for remote record
	virtual int startSearchRecFile(int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime);
	virtual int getPlaybackStreamByTime(int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime);
	virtual int getPlaybackStreamByFileName(int nChannel,const QString &sFileName);
	virtual int pausePlaybackStream(bool bPause);
	virtual int stopPlaybackStream();
	//callback
	int cbFoundFile(QVariantMap &evmap);
	int cbRecFileSearchFinished(QVariantMap &evmap);
	int cbRecFileSearchFail(QVariantMap &evmap);
protected:
	void run();
private:
	void eventProcCall(QString sEvent,QVariantMap tInfo);
	void sleepEx(int nTime);
	bool analyzeBubbleInfo();
	bool analyzePreviewInfo();
	bool analyzeRemoteInfo();
	bool cmdAuthority();
	bool cmdDisConnect();
	bool cmdGetLiveStream();
	bool cmdGetLiveStreamEx();
	bool cmdStopStream();
	bool cmdPauseStream();
	bool cmdHeartBeat();
	bool cmdGetPlayBackStreamByTime();
	bool cmdGetPlayBackStreamByFileName();
	bool cmdPausePlayBackStream();
	bool cmdStopPlayBackStream();
	bool cmdPtz();
	bool sendLiveStreamCmdEx(bool flags);
	bool sendLiveStreamCmd(bool flags);
	bool checkRemoteFileIsExist();
	QString checkXML(QString sSource);
private slots:
	void slCheckoutBlock();
	void slBackToMainThread(QVariantMap evMap);
signals:
	void sgBackToMainThread(QVariantMap evMap);
private:
	int m_nRef;
	QMutex m_csRef;
	QMutex m_csStepCode;
	QStringList m_sEventList;
	QMultiMap<QString,tagBubbleProInfo> m_tEventMap;
	tagBubbleConnectStatusInfo m_tCurrentConnectStatus;
	tagBubbleConnectStatusInfo m_tHistoryConnectStatus;
	tagBubbleDeviceInfo m_tDeviceInfo;
	QQueue<int> m_tStepCode;
	bool m_bStop;
	int m_nSleepSwitch;
	int m_nPosition;
	int m_nSecondPosition;
	bool m_bBlock;
	QTimer m_tCheckoutBlockTimer;
	QTcpSocket *m_pTcpSocket;
	QByteArray m_tBuffer;
	QList<char> m_tPreviewCode;
	QList<char> m_tRemoteCode;
	bool m_bIsSupportHttp;
	QList<QList<tagBubbleHttpStreamInfo>>m_tHttpStreamList;
	volatile bool m_bWaitForConnect;
	SearchRemoteFile m_tSearchRemoteFile;
	int m_nBuiltTreadId;
};

#endif // BUBBLEPROTOCOLEX_H
