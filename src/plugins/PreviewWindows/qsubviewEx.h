#pragma once
#include <QWidget>
#include <QVariantMap>
#include "QSubviewRun.h"
#include <ManageWidget.h>
#include <QDebug>
#include <QEventLoop>
#include <QMouseEvent>
#include <QMenu>
#include <QTranslator>
#include <QPixmap>
#include <QPen>
#include <QPainter>
#include <QTimer>
#include "IUserManagerEx.h"

int cbStateChangeEx(QString evName,QVariantMap evMap,void*pUser);
int cbRecordStateEx(QString evName,QVariantMap evMap,void*pUser);
int cbConnectRefuseEx(QString evName,QVariantMap evMap,void*pUser);
int cbAuthorityEx(QString evName,QVariantMap evMap,void*pUser);
class qsubviewEx:public QWidget
{
	Q_OBJECT
public:
	qsubviewEx(QWidget *parent=0);
	~qsubviewEx();
	
public:
	virtual void paintEvent(QPaintEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void resizeEvent(QResizeEvent *);
	virtual void mousePressEvent(QMouseEvent *);
	virtual void changeEvent(QEvent *);

	int openPreview(int chlId);
	int closePreview();

	int switchStream();

	int getCurrentConnectStatus();
	int setDevChannelInfo(int chlId);//unnecessary
	QVariantMap getWindowInfo();

	// ��ʼ������,�����ڵ����������Ĳ������ýӿں����initAfterConstructor
	void setCurWindId(int nWindId);
	void initAfterConstructor();

	//�ֶ�¼��
	int startRecord();
	int stopRecord();
	int getRecordStatus();
	//��Ƶ ����
	int setPlayWnd(int nwnd);//unnecessary
	int setVolume(unsigned int uiPersent);
	int audioEnabled(bool bEnable);
	//����
	QVariantMap screenShot();
	//ȫ��
	int SetFullScreen(bool bFullScreen);
	//��̨����
	int openPTZ(int ncmd,int nspeed);
	int closePTZ(int ncmd);
	//��������
	void loadLanguage(QString tags);
	//���õ�ǰ���㴰��
	void setCurrentFocus(bool flags);
	void setDataBaseFlush();

	bool getDigtalViewIsClose();
	void deInitDigtalView();
	bool isSuitForDigitalZoom();
	void showDigitalView();
	void closeDigitalView();
public:
	//�ص�����
	int cbCStateChange(QVariantMap evMap);
	int cbCRecordState(QVariantMap evMap);
	int cbCConnectRefuse(QVariantMap evMap);
	int cbCAuthority(QVariantMap evMap);
public slots:
	void slbackToMainThread(QVariantMap evMap);
	void slmouseMenu();
	void slswitchStreamEx();
	void slclosePreview();
	void slMenRecorder();
	void slbackToManiWnd();
	// ���촰��
	void enableStretch(bool bEnable);
	void enableStretchEx(bool bEnable);
signals:
	void sgbackToMainThread(QVariantMap evMap);
	void sgmouseDoubleClick(QWidget *,QMouseEvent*);
	void sgmousePressEvent(QWidget*,QMouseEvent*);
	void sgmouseLeftClick(QWidget*,QMouseEvent *);
	void sgrecordState(bool);
	void sgmouseMenu();
	void sgconnectStatus(QVariantMap,QWidget *);
	void sgconnectRefuse(QVariantMap,QWidget *);
	void sgAuthority(QVariantMap,QWidget *);
	void sgbackToMainWnd();
	void sgVerify(QVariantMap vmap);
	void sgShutDownDigtalZoom();
private:
	void paintEventConnected(QPaintEvent *ev);
	void paintEventDisconnected(QPaintEvent *ev);
	void paintEventConnecting(QPaintEvent *ev);
	void paintEventDisconnecting(QPaintEvent *ev);
	tagDeviceInfo getDeviceInfo();
	QString getLanguageInfo(QString tags);
	void translateLanguage();
	int verify(qint64 mainCode, qint64 subCode);
private:
	typedef enum __tagConnectStatus{
		STATUS_CONNECTED,
		STATUS_CONNECTING,
		STATUS_DISCONNECTED,
		STATUS_DISCONNECTING,
	}tagConnectStatus;
	tagConnectStatus m_tCurConnectStatus;
	tagConnectStatus m_tHistoryConnectStatus;
	QSubviewRun m_sSubviewRun;
	ManageWidget *m_pManageWidget;
	bool m_bIsFocus;
	bool m_bIsRecording;
	QMenu m_mRightMenu;
	QAction *m_pClosePreviewAction;
	QAction *m_pSwitchStreamAciton;
	QAction *m_pRecorderAction;
	QAction *m_pBackMainViewAction;
	QAction * m_pStreachVideo;
	tagDeviceInfo m_tDeviceInfo;
	QTranslator *m_pTtanslator;
	int m_nConnectingCount;
	QTimer m_tConnectingTimer;
	static bool ms_bIsFullScreen;
	int m_nWindowIndex;
	bool m_bStretch; // �ڶ������ݴ�״̬���������е�״̬һ�£������ڵ��ò˵�������������ʱ����ҪƵ����ȡ����
	int m_chlId;
	
};

