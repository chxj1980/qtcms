#pragma once
#include <QWidget>
#include "qwfw.h"
#include "PreviewWindowsGlobalSetting.h"
#include <IWindowDivMode.h>
#include <QtCore/QtCore>
#include <QtXml/QtXml>
#include <libpcom.h>
#include "IUserManager.h"
#include "IChannelManager.h"
#include <guid.h>
#include <qwfw_tools.h>
#include <qsubviewEx.h>
#include "ILocalSetting.h"
#include <QList>
#include <IDisplayWindowsManager.h>
#include <QVariantMap>
#include <QShowEvent>
#include <QHideEvent>
#include <QTimer>
class qpreviewwindowsex:public QWidget,
	public QWebPluginFWBase
{
	Q_OBJECT
public:
	qpreviewwindowsex(QWidget *parent = 0);
	virtual ~qpreviewwindowsex();

public:
	virtual void resizeEvent(QResizeEvent *);
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);
public slots:
	void AddEventProc( const QString sEvent,QString sProc ){m_mapEventProc.insertMulti(sEvent,sProc);};
	//������Ϣ
	virtual void nextPage();
	virtual void prePage();
	virtual int getCurrentPage();
	virtual int getPages();
	virtual int setDivMode( QString divModeName );
	virtual QString getCureentDivMode();
	virtual int GetCurrentWnd();
	int GetWindowConnectionStatus(unsigned int uiWndIndex);
	QVariantMap GetWindowInfo(unsigned int uiWndIndex);
	//����ر�Ԥ��
	int OpenCameraInWnd(unsigned int uiWndIndex
		,const QString sAddress,unsigned int uiPort,const QString & sEseeId
		,unsigned int uiChannelId,unsigned int uiStreamId
		,const QString & sUsername,const QString & sPassword
		,const QString & sCameraname
		,const QString & sVendor);
	
	int CloseWndCamera(unsigned int uiWndIndex);
	int CloseAll();
	//�л�����
	int SwithStream(unsigned int uiWndIndex,int chlId);
	//���ô��ڵ��豸��Ϣ 	
	int SetDevChannelInfo(unsigned int uiWndIndex,int ChannelId);//may be unnecessary
	//¼��
	int StartRecord(int nWndID);
	int StopRecord(int nWndID);
	int SetDevInfo(const QString&devname,int nChannelNum,int nWndID);//may be unnecessary
	//��Ƶ
	int SetVolume(unsigned int uiPersent);
	int AudioEnabled(bool bEnabled);
	//����
	void screenShot(QString sUser,int nType);
	//ȫ��
	void SetFullScreenFlag();
	void OnBackToMainWnd();
	//��̨
	int OpenPTZ(int nCmd, int nSpeed);
	int ClosePTZ(int nCmd);
	//�Ӵ����źź���
	void subWindowDblClick(QWidget*,QMouseEvent *);
	void subWindowMousePress(QWidget*,QMouseEvent *);
	void subWindowMouseRelease(QWidget*,QMouseEvent *);
	void subWindowConnectStatus(QVariantMap,QWidget *);
	void subWindowConnectRefuse(QVariantMap,QWidget *);
	void subWindowAuthority(QVariantMap,QWidget *);
	void subWindowScreenShot(QVariantMap,QWidget *);
	void subWindowVerify(QVariantMap vmap);
	// ͼ������
	void AllWindowStretch(bool bEnable);
	//�Զ���Ѳ
	void StartAutoPolling();
	void StopAutoPolling();
	void slPolling();
	//���ӷŴ�
	void shutDownDigtalZoom();
	void enableDigtalZoom();
	void ViewNewPosition(QRect tRect,int nWidth,int nHeight);
private:
	bool chlIsExist(int chlId);
	QString getLanguageLable();
	int OpenCameraInWnd(unsigned int uiWndIndex,int chlId);
	int getPollInterval();
	void hideDigitalView();
	void restoreDigitalView();
private:
	IWindowDivMode * m_divMode;
	qsubviewEx m_sPreviewWnd[MAX_WINDOWS_NUM];
	QList<QWidget*>m_pPreviewWndList;
	int m_nCurrentWnd;
	bool m_bAudioEnabled;
	QString m_sLastLanguageLabel;
	QTimer *m_pAutoPollingTimer;
	bool m_bIsEnableDigitalZoom;
	int m_nRestoreViewNum;
	bool m_bIsRestroreView;
};

