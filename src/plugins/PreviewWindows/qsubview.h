#ifndef QSUBVIEW_H
#define QSUBVIEW_H

#include <QWidget>
#include <QString>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QVariantMap>
#include <IDeviceClient.h>
#include <IEventRegister.h>
#include <IVideoDecoder.h>
#include "PreviewWindowsGlobalSetting.h"
#include "QSubviewThread.h"
#include <IVideoRender.h>
#include <IRecorder.h>
#include <ISetRecordTime.h>
#include <QTime>
#include <QTimer>
#include <QMenu>
#include <ISwitchStream.h>
#include <IAudioPlayer.h>
#include "QSubViewObject.h"
#include <QLineEdit>
#include <QPixmap>
#include "ui_TitleView.h"
#include "ManageWidget.h"
int cbLiveStream(QString evName,QVariantMap evMap,void*pUser);
int cbForRecord(QString evName,QVariantMap evMap,void*pUser);
int cbDecodedFrame(QString evName,QVariantMap evMap,void*pUser);
int cbConnectError(QString evName,QVariantMap evMap,void*pUser);
int cbStateChange(QString evName,QVariantMap evMap,void*pUser);
int cbRecordState(QString evName,QVariantMap evMap,void*pUser);

class QSubView :public QWidget
{
	Q_OBJECT
public:
	QSubView(QWidget *parent = 0);
	~QSubView();

	virtual void paintEvent( QPaintEvent * );

	virtual void mouseDoubleClickEvent( QMouseEvent * );

	virtual void mousePressEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);

	int GetCurrentWnd();
	//int OpenCameraInWnd(const QString sAddress,unsigned int uiPort,const QString & sEseeId
	//	,unsigned int uiChannelId,unsigned int uiStreamId
	//	,const QString & sUsername,const QString & sPassword
	//	,const QString & sCameraname,const QString & sVendor);
	int OpenCameraInWnd(int chlId);
	int SetDevChannelInfo(int ChannelId);
	int CloseWndCamera();
	int GetWindowConnectionStatus();
	QVariantMap GetWindowInfo();
	int liveStreamRequire(int nChannel,int nStream,bool bOpen);
	//�ֶ�¼��
	int StartRecord();
	int StopRecord();
	int SetDevInfo(const QString&devname,int nChannelNum);

	//��Ƶ����
	int SetPlayWnd(int nWnd);
	int SetVolume(unsigned int uiPersent);
	QSubView* getCurWind();
	int AudioEnabled(bool bEnabled);
	void ScreenShot();
	int SwitchStream(int chlId);
	typedef enum __enQSubViewConnectStatus{
		STATUS_CONNECTED,
		STATUS_CONNECTING,
		STATUS_DISCONNECTED,
		STATUS_DISCONNECTING,
	}QSubViewConnectStatus;

public:
	//�ص�
	int PrevPlay(QVariantMap evMap);
	int ForRecord(QVariantMap evMap);
	int PrevRender(QVariantMap evMap);
	int CurrentStateChange(QVariantMap evMap);

	int SetDeviceByVendor(const QString & sVendor);
	void SetCurrentFocus(bool);
	void RecordState(QVariantMap evMap);
private:
	int cbInit();
	
	void In_CloseAutoConnect();
	void paintEventNoVideo( QPaintEvent * );
	void paintEventConnecting( QPaintEvent * );
	void paintEventCache(QPaintEvent *);
	void saveCacheImage();
	int GetDeviceInfo(int chlId);
public slots:
		virtual void timerEvent( QTimerEvent * );
		void OnRMousePressMenu();
		void OnCloseFromMouseEv();
		void OnSwitchStreamFromMouseEv();
		void OnConnectting();
		void OnDisConnecting();
		void OnConnected();
		void OnRenderHistoryPix();
		void OnCheckTime();
		void OnCreateAutoConnectTime();
		void In_OpenAutoConnect();
		
signals:
		void mouseDoubleClick(QWidget *,QMouseEvent *);
		void mousePressEvent(QWidget *,QMouseEvent *);
		void mouseLeftClick(QWidget *,QMouseEvent *);
		void SetCurrentWindSignl(QWidget *);
		void CurrentStateChangeSignl(QVariantMap evMap,QWidget *);
		void Connectting();
		void Connected();
		void DisConnecting();
		void RMousePressMenu();
		void RenderHistoryPix();
		void AutoConnectSignals();
		void CreateAutoConnectTimeSignals();
		void RecordStateSignals(bool );
private:
	DevCliSetInfo m_DevCliSetInfo;//�豸��Ϣ
	RecordDevInfo m_RecordDevInfo;
	RenderInfo m_HistoryRenderInfo;//��һ֡ͼ�����Ϣ
	IVideoRender *m_IVideoRender;
	IVideoDecoder *m_IVideoDecoder;
	IDeviceClient *m_IDeviceClientDecideByVendor;
	QSubViewObject m_QSubViewObject;
	IRecorder *m_pRecorder;
	ISetRecordTime *m_pRecordTime;
	static IAudioPlayer *m_pAudioPlayer;
	
	QSubViewConnectStatus m_CurrentState;//�豸��ǰ������״̬
	QSubViewConnectStatus m_HistoryState;//�豸��һ�ε�����״̬
	int iInitWidth;
	int iInitHeight;
	int m_nSampleRate;
	int m_nSampleWidth;
	//��־λ
	bool m_bRendering;
	bool m_bIsRecording;
	bool m_bIsRenderHistory;
	bool m_bIsAutoConnect;
	bool m_bStateAutoConnect;
	bool m_bIsAutoConnecting;
	bool m_bIsStartRecording;
	bool m_bIsAutoRecording;
	bool m_bIsFocus;
	bool m_bIsForbidConnect;
	static bool m_bIsAudioOpend;
	QTimer m_checkTime;

	Ui::titleview * ui;

	QMutex m_MutexdoubleClick;
	QMutex m_csRender;
	QMenu m_RMousePressMenu;

	QAction *m_QActionCloseView;
	QAction *m_QActionSwitchStream;
	//�������Ӻ����ڶϿ���ˢ��ͼƬ�ļ���
	int m_nCountDisConnecting;
	int m_CountConnecting;
	//ʱ��id
	int m_DisConnectingTimeId;
	int m_RenderTimeId;
	int m_AutoConnectTimeId;
	int m_ConnectingTimeId;
	int m_ForbidConnectTimeId;
	//�ƻ�¼��ˢ��ʱ��
	int m_RecordFlushTime;
	QList<RecordTimeInfo> m_lstReocrdTimeInfoList;

	static QSubView* m_pCurrView;
	//test
	ManageWidget *_manageWidget;
	QPixmap _cacheBackImage;
	QPixmap _ScreenShotImage;
};


#endif // QSUBVIEW_H
