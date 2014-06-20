#pragma once
#include <QThread>
#include <QDebug>
#include <QQueue>
#include <IDeviceClient.h>
#include <guid.h>
#include <QMultiMap>
#include <QStringList>
#include <IChannelManager.h>
#include <IDeviceManager.h>
#include <QFile>
#include <QtXml/QtXml>
#include <QWidget>
#include <IVideoDecoder.h>
#include <IVideoRender.h>
#include <IEventRegister.h>
#include <IRecorder.h>
#include <ISwitchStream.h>
#include <IPTZControl.h>
#include <QTimer>
#include <ISetRecordTime.h>
typedef int (__cdecl *previewRunEventCb)(QString eventName,QVariantMap info,void *pUser);
typedef struct _tagProcInfo{
	previewRunEventCb proc;
	void *puser;
}tagProcInfo;
typedef enum __tagStepCode{
	OPENPREVIEW,//��ʼԤ��
	SWITCHSTREAM,//ui�л�����
	SWITCHSTREAMEX,//���ڲ˵��л�����
	OPENPTZ,//��̨����
	CLOSEPTZ,//�ر���̨����
	AUTORECONNECT,//�Զ�����
	IPCAUTOSWITCHSTREAM,//ipc �Զ��л�����
	STARTRECORD,//����¼��
	STOPRECORD,//�ر�¼��
	AUTOSYNTIME,//�Զ�ͬ��ʱ��
	DEFAULT,//ȱʡ �޶���
	END,//����
}tagStepCode;
typedef enum __tagStepRegistCode{
	DEVICECLIENT,//ע���豸�ص�����
	DECODE,//ע�����ص�����
	RECORD,//ע��¼��ص�����
}tagStepRegistCode;
typedef struct __tagRecorderTimeInfo{
	int nEnable;
	int nWeekDay;
	QTime startTime;
	QTime endTime;
}tagRecorderTimeInfo;
typedef struct _tagDeviceInfo{
	QString m_sAddress;
	unsigned int m_uiPort;
	QString m_sEseeId;
	unsigned int m_uiChannelId;
	int m_uiChannelIdInDataBase;
	unsigned int m_uiStreamId;
	QString m_sUsername;
	QString m_sPassword;
	QString m_sCameraname;
	QString m_sVendor;
	QString m_sDeviceName;
	QWidget *m_pWnd;
}tagDeviceInfo;

class QSubviewRun:public QThread
{
	Q_OBJECT
public:
	QSubviewRun(void);
	~QSubviewRun(void);
public:
	void openPreview(int chlId,QWidget *pWnd);
	void stopPreview();
	void switchStream(int chlId);
	void switchStreamEx();
	void openPTZ(int nCmd,int nSpeed);
	void closePTZ(int nCmd);
	void registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void*),void *pUser);
	int startRecord();
	int stopRecord();
	void setDatabaseFlush(bool flag);
public:
	//call back
	int cbCConnectState(QString evName,QVariantMap evMap,void *pUser);
	int cbCPreviewData( QString evName,QVariantMap evMap,void *pUuer );
	int cbCDecodeData( QString evName,QVariantMap evMap,void *pUser );
	int cbCRecorderData( QString evName,QVariantMap evMap,void*pUser );
	int cbCConnectError(QString evName,QVariantMap evMap,void*pUser );
	int cbCDecodeFrame(QString evName,QVariantMap evMap,void*pUser);
	int cbCRecordState(QString evName,QVariantMap evMap,void*pUser);
private:
	bool liveSteamRequire();
	void eventCallBack(QString eventName,QVariantMap evMap);
	bool createDevice();
	bool registerCallback(int registcode);
	bool connectToDevice();
	void ipcAutoSwitchStream();
	void saveToDataBase();
	bool openPTZ();
	bool closePTZ();
private slots:
	void slstopPreview();
	void slbackToMainThread(QVariantMap evMap);
	void slplanRecord();
signals:
	void sgstopPreview();
	void sgbackToMainThread(QVariantMap evMap);
protected:
	void run();
private:
	typedef enum __enQSubviewRunConnectStatus{
		STATUS_CONNECTED,
		STATUS_CONNECTING,
		STATUS_DISCONNECTED,
		STATUS_DISCONNECTING,
	}QSubviewRunConnectStatus;

	QQueue<int> m_stepCode;
	QSubviewRunConnectStatus m_currentStatus;
	QSubviewRunConnectStatus m_historyStatus;
	IDeviceClient *m_pdeviceClient;
	IVideoRender *m_pIVideoRender;
	IVideoDecoder *m_pIVideoDecoder;
	IRecorder *m_pRecorder;
	QMultiMap<QString ,tagProcInfo> m_eventMap;
	QStringList m_eventNameList;
	bool m_stop;
	tagDeviceInfo m_deviceInfo;
	int m_newchlid;
	int m_ptzCmd;
	int m_ptzSpeed;
	int m_ptzCmdEx;
	bool m_bIsPtzAutoOpen;
	bool m_bIsAutoRecording;
	bool m_bIsRecord;
	QTimer m_planRecordTimer;
	bool m_bIsdataBaseFlush;
};

