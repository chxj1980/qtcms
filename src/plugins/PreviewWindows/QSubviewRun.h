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
#include <QList>
#include <QTime>
#include <ILocalSetting.h>
#include <IAutoSycTime.h>
#include <IAudioPlayer.h>
#include <qtconcurrentrun.h>
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
	AUDIOENABLE,//��������
	SETVOLUME,//��������
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
	//Ԥ����Ƶ
	void openPreview(int chlId,QWidget *pWnd);
	void stopPreview();
	//�л�����
	void switchStream();
	void switchStreamEx();
	void ipcSwitchStream();
	//��̨����
	void openPTZ(int nCmd,int nSpeed);
	void closePTZ(int nCmd);
	//ע��ص�����
	void registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void*),void *pUser);
	//¼��
	int startRecord();
	int stopRecord();

	void setDatabaseFlush(bool flag);
	void setFoucs(bool bEnable);
	//��Ƶ
	void setVolume(unsigned int uiPersent);
	void audioEnabled(bool bEnable);
	
	QVariantMap screenShot();
	tagDeviceInfo deviceInfo();
public:
	//call back
	int cbCConnectState(QString evName,QVariantMap evMap,void *pUser);
	int cbCPreviewData( QString evName,QVariantMap evMap,void *pUuer );
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
	void backToMainThread(QVariantMap evMap);
	
public slots:
	void slstopPreviewrun();
private slots:
	void slbackToMainThread(QVariantMap evMap);
	void slplanRecord();
	void slsetRenderWnd();
	void slcheckoutBlock();
	void slstopPreview();
signals:
	void sgbackToMainThread(QVariantMap evMap);
	void sgsetRenderWnd();
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
	volatile QSubviewRunConnectStatus m_currentStatus;
	volatile QSubviewRunConnectStatus m_historyStatus;
	IDeviceClient *m_pdeviceClient;
	IVideoRender *m_pIVideoRender;
	IVideoDecoder *m_pIVideoDecoder;
	IRecorder *m_pRecorder;
	IAudioPlayer *m_pAudioPlay;
	QMultiMap<QString ,tagProcInfo> m_eventMap;
	QStringList m_eventNameList;
	bool m_stop;
	tagDeviceInfo m_deviceInfo;
	int m_ptzCmd;
	int m_ptzSpeed;
	int m_ptzCmdEx;
	bool m_bIsPtzAutoOpen;
	bool m_bIsAutoRecording;
	bool m_bIsRecord;
	QTimer m_planRecordTimer;
	bool m_bIsdataBaseFlush;
	QList<tagRecorderTimeInfo> m_lstReocrdTimeInfoList;
	bool m_bIsSysTime;
	static unsigned int m_volumePersent;
	static bool m_bIsAudioOpen;
	bool m_bIsFocus;
	int m_sampleRate;
	int m_sampleWidth;
	int m_nInitWidth;
	int m_nInitHeight;
	bool m_bScreenShot;
	QString m_sScreenShotPath;
	volatile bool m_bClosePreview;
	QTimer m_checkIsBlockTimer;
	volatile bool m_bIsBlock;
	int m_nPosition;
	Qt::HANDLE m_hMainThread;
};
  
