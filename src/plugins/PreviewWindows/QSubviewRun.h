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
#include <IRecordDat.h>
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
#include <QMutex>
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
	INITRECORD,//��ʼ��¼��
	UPDATEDATABASE,//�������ݿ�
	DEINITRECORD,//�ر�¼��
	SETMANUALRECORD,//�����˹�¼��
	SETMOTIONRECORD,//�ƶ�¼��
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
	QString m_sConnectMethod;
	QWidget *m_pWnd;
	QWidget *m_pMainWnd;
}tagDeviceInfo;
typedef struct _tagRenderInfo{
	char *pData;
	char *pYdata;
	char *pUdata;
	char *pVdata;
	int nWidth;
	int nHeight;
	int nYStride;
	int nUVStride;
	int nLineStride;
	QString sPixeFormat;
	int nFlags;
}tagRenderInfo;
class QSubviewRun:public QThread
{
	Q_OBJECT
public:
	QSubviewRun(void);
	~QSubviewRun(void);
public:
	//Ԥ����Ƶ
	void openPreview(int chlId,QWidget *pWnd,QWidget *pMainWnd);
	void stopPreview();
	void enableStretch(bool bStretch);
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
	int startManualRecord();
	int stopManualRecord();
	int getRecordStatus();
	
	//�������ݿ�
	void setDatabaseFlush(bool flag);

	void setFoucs(bool bEnable);
	void setWindId(int nWindId);
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
	int cbCDecodeFrameEx(QString evName,QVariantMap evMap,void*pUser);
	int cbCRecordState(QString evName,QVariantMap evMap,void*pUser);
	int cbCConnectRefuse(QString evName,QVariantMap evMap,void*pUser);
	int cbCAuthority(QString evName,QVariantMap evMap,void*pUser);
	int cbCMotionDetection(QString evName,QVariantMap evMap,void*pUser);
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
	void sleepEx(int time);
	void renderSaveFrame();
	
public slots:
	void slstopPreviewrun();
private slots:
	void slbackToMainThread(QVariantMap evMap);
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
	IRecordDat *m_pRecordDat;
	IAudioPlayer *m_pAudioPlay;
	QMultiMap<QString ,tagProcInfo> m_eventMap;
	QStringList m_eventNameList;
	volatile bool m_stop;
	tagDeviceInfo m_tDeviceInfo;
	int m_ptzCmd;
	int m_ptzSpeed;
	int m_ptzCmdEx;
	bool m_bIsPtzAutoOpen;
	bool m_bIsManualRecord;
	bool m_bIsPreDecode;
	bool m_bIsPreRender;
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
	int m_nSecondPosition;
	Qt::HANDLE m_hMainThread;
	int m_nSleepSwitch;
	int m_nWindId;
	int m_nRecordType;
	tagRenderInfo m_tRenderInfo;
	volatile bool m_bIsSaveRenderFrame;
	int m_nHisWeekDay;
	int m_nCheckPreCount;
	int m_nMotionRecordTime;
	QMutex m_tStepCodeLock;
};
  
