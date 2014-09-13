#include "deviceclient.h"
#include <guid.h>

DeviceClient::DeviceClient():
    m_nRef(0),
	m_DeviceConnecton(NULL),
    m_DeviceConnectonBubble(NULL),
    m_DeviceConnectonHole(NULL),
    m_DeviceConnectonTurn(NULL),
    m_bIsInitFlags(false),
    m_bCloseingFlags(false),
    m_CurStatus(IDeviceClient::STATUS_DISCONNECTED),
    m_pRemotePlayback(NULL),
	m_nChannels(0),
	m_nSpeedRate(0),
    m_nStartTimeSeconds(0),
    m_bGroupStop(false),
	m_channelWithAudio(-1),
    m_pProtocolPTZ(NULL)
{
	pcomCreateInstance(CLSID_Bubble,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnectonBubble);
	/*pcomCreateInstance(CLSID_BubbleProtocol,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnectonBubble);*/
	pcomCreateInstance(CLSID_Hole,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnectonHole);
	pcomCreateInstance(CLSID_Turn,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnectonTurn);
	m_EventList<<"LiveStream"<<"SocketError"<<"StateChangeed"<<"CurrentStatus"<<"foundFile"<<"recFileSearchFinished"<<"ForRecord"<<"bufferStatus"<<"recFileSearchFail"<<"ConnectRefuse";

	DeviceClientInfoItem devcliInfo;

	devcliInfo.proc=cbXStateChange;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("StateChangeed",devcliInfo);


	devcliInfo.proc=cbXFoundFile;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("foundFile",devcliInfo);

	devcliInfo.proc=cbXRecFileSearchFinished;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("recFileSearchFinished",devcliInfo);

	devcliInfo.proc=cbXLiveStream;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("LiveStream",devcliInfo);

	devcliInfo.proc=cbXSocketError;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("SocketError",devcliInfo);

	devcliInfo.proc=cbXRecFileSearchFail;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("recFileSearchFail",devcliInfo);

	devcliInfo.proc=cbXConnectRefuse;
	devcliInfo.puser=this;
	m_EventMapToPro.insert("ConnectRefuse",devcliInfo);
	if (NULL != m_DeviceConnectonBubble)
	{
		m_DeviceConnectonBubble->QueryInterface(IID_IRemotePlayback, (void**)&m_pRemotePlayback);
	}
	m_groupMap.clear();
	//
	m_remotePlayback.registerEvent("foundFile",cbXFoundFile,this);
	m_remotePlayback.registerEvent("StateChangeed",cbXStateChange,this);
	m_remotePlayback.registerEvent("recFileSearchFinished",cbXRecFileSearchFinished,this);
	m_remotePlayback.registerEvent("SocketError",cbXSocketError,this);
	m_remotePlayback.registerEvent("recFileSearchFail",cbXRecFileSearchFail,this);
}

DeviceClient::~DeviceClient()
{
	closeAll();
	if (NULL!=m_DeviceConnectonBubble)
	{
		m_DeviceConnectonBubble->Release();
		m_DeviceConnectonBubble=NULL;
	}
	if (NULL!=m_DeviceConnectonHole)
	{
		m_DeviceConnectonHole->Release();
		m_DeviceConnectonHole=NULL;
	}
	if (NULL!=m_DeviceConnectonTurn)
	{
		m_DeviceConnectonTurn->Release();
		m_DeviceConnectonTurn=NULL;
	}
	if (NULL != m_pRemotePlayback)
	{
		m_pRemotePlayback->Release();
	}

	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL != it->playManager)
		{
			delete it->playManager;
			it->playManager = NULL;
		}
		msleep(10);
		if (NULL != it->bufferManager)
		{
			delete it->bufferManager;
			it->bufferManager = NULL;
		}
	}
	m_groupMap.clear();
}

long __stdcall DeviceClient::QueryInterface(const IID & iid,void **ppv)
{
	if (IID_IDeviceClient==iid)
	{
		*ppv=static_cast<IDeviceClient*>(this);
	}
	else if (IID_IPcomBase==iid)
	{
		*ppv=static_cast<IPcomBase *>(this);
	}
	else if (IID_IEventRegister==iid)
	{
		*ppv=static_cast<IEventRegister*>(this);
	}
	else if (IID_IDeviceSearchRecord==iid)
	{
		*ppv=static_cast<IDeviceSearchRecord*>(this);
	}
	else if (IID_IDeviceGroupRemotePlayback==iid)
	{
		*ppv=static_cast<IDeviceGroupRemotePlayback*>(this);
	}
	else if (IID_IRemoteBackup==iid)
	{
		*ppv=static_cast<IRemoteBackup*>(this);
	}
	else if (IID_IPTZControl == iid)
	{
		*ppv=static_cast<IPTZControl*>(this);
	}
	else if (IID_IDeviceAuth == iid)
	{
		*ppv=static_cast<IDeviceAuth *>(this);
	}
	else 
	{
		*ppv=NULL;
		return E_NOINTERFACE;
	}
	static_cast <IPcomBase*>(this)->AddRef();
	return S_OK;
}

unsigned long __stdcall DeviceClient::AddRef()
{
	m_csRef.lock();
	m_nRef++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long __stdcall DeviceClient::Release()
{
	int nRet=0;
	m_csRef.lock();
	m_nRef--;
	nRet=m_nRef;
	m_csRef.unlock();
	if (0==nRet)
	{
		delete this;
	}
	return nRet;
}

QStringList DeviceClient::eventList()
{
	return m_EventList;
}

int DeviceClient::queryEvent(QString eventName,QStringList& eventParams)
{
	if (!m_EventList.contains(eventName))
	{
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}
	if ("LiveStream"==eventName)
	{
		eventParams<<"channel"<<"pts"<<"length"<<"data"<<"frametype"<<"width"<<"height"<<"vcodec"<<"samplerate"<<"samplewidth"<<"audiochannel"<<"acodec";
	}
 	if ("foundFile" == eventName)
 	{
 		eventParams<<"channel"<<"types"<<"start"<<"end"<<"filename";
 	}
	if ("recFileSearchFinished" == eventName)
	{
		eventParams<<"total";
	}
	if ("bufferStatus" == eventName)
	{
		eventParams<<"wind"<<"Persent";
	}
	return IEventRegister::OK;
}

int DeviceClient::registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser)
{
	if ("BackupStatusChange" == eventName)
	{
		m_RemoteBackup.SetBackupEvent(eventName,proc,pUser);
		return IEventRegister::OK;
	}
	if ("progress"==eventName)
	{
		m_RemoteBackup.SetBackupEvent(eventName,proc,pUser);
		return IEventRegister::OK;
	}
	if (!m_EventList.contains(eventName))
	{
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}
	//����ԣ��Ƿ��ظ�ע��
	if (m_EventMap.contains(eventName))
	{
		DeviceClientInfoItem devcliInfo;
		devcliInfo.proc=proc;
		devcliInfo.puser=pUser;
		m_EventMap.replace(eventName,devcliInfo);
		return IEventRegister::OK;
	}
	DeviceClientInfoItem devcliInfo;
	devcliInfo.proc=proc;
	devcliInfo.puser=pUser;

	m_EventMap.insert(eventName,devcliInfo);

	return IEventRegister::OK;
}

int DeviceClient::connectToDevice(const QString &sAddr,unsigned int uiPort,const QString &sEseeId)
{
	// ���״̬
	// �������򷵻ش���
    m_bCloseingFlags=false;
	m_ports.insert("media",uiPort);
	//ע��ص�����
    if (false==m_bIsInitFlags)
	{
		cbInit();
	}
	//�����һ������״������������ӵ���Ͽ���
	if (m_CurStatus!=IDeviceClient::STATUS_DISCONNECTED)
	{
		closeAll();
	}
	m_CurStatus=DeviceClient::STATUS_CONNECTING;
	QVariantMap CurStatusParm;
	CurStatusParm.insert("CurrentStatus",m_CurStatus);
	eventProcCall("CurrentStatus",CurStatusParm);
	//��������
	int nStep=0;
	while(nStep!=5){
		switch(nStep){
			//����bubbleЭ������
		case 0:
			{
				if (NULL==m_DeviceConnectonBubble)
				{
					nStep=1;
					break;
				}
				m_DeviceConnecton=m_DeviceConnectonBubble;
                if (true==m_bCloseingFlags)
				{
					nStep=1;
					break;
				}
				if (1==m_DeviceConnectonBubble->setDeviceHost(sAddr)||1==m_DeviceConnectonBubble->setDevicePorts(m_ports)||1==m_DeviceConnectonBubble->setDeviceId(sEseeId))
				{
					nStep=1;
					break;
				}
				int nRet=1;

				nRet=m_DeviceConnectonBubble->connectToDevice();
				if (1==nRet)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<sAddr<<"bubble can't connect";
					nStep=1;
					break;
				}
				qDebug()<<__FUNCTION__<<__LINE__<<sAddr<<"bubble success";
				m_DeviceConnecton=m_DeviceConnectonBubble;

				nStep=3;
			}
			break;
			//���Դ�͸Э������
		case 1:
			{
				if (NULL==m_DeviceConnectonHole)
				{
					nStep=2;
					break;
				}
				if (1==m_DeviceConnectonHole->setDeviceHost(sAddr)||1==m_DeviceConnectonHole->setDevicePorts(m_ports)||1==m_DeviceConnectonHole->setDeviceId(sEseeId)||sEseeId=="0"||sEseeId=="")
				{
					nStep=2;
					break;
				}
				int nRet=1;
				m_DeviceConnecton=m_DeviceConnectonHole;
                if (true==m_bCloseingFlags)
				{
					nStep=2;
					break;
				}
				nRet=m_DeviceConnectonHole->connectToDevice();
				if (1==nRet)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"Hole can't connect";
					nStep=2;
					break;
				}
				qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"Hole success";
				m_DeviceConnecton=m_DeviceConnectonHole;
				nStep=3;
			}
			break;
			//����ת��Э������
		case 2:
			{
				if (NULL==m_DeviceConnectonTurn)
				{
					nStep=4;
					break;
				}
				if (1==m_DeviceConnectonTurn->setDeviceHost(sAddr)||1==m_DeviceConnectonTurn->setDevicePorts(m_ports)||1==m_DeviceConnectonTurn->setDeviceId(sEseeId)||sEseeId=="0"||sEseeId=="")
				{
					nStep=4;
					break;
				}
				int nRet=1;
				m_DeviceConnecton=m_DeviceConnectonTurn;
                if (true==m_bCloseingFlags)
				{
					nStep=4;
					break;
				}
				nRet=m_DeviceConnectonTurn->connectToDevice();
				if (1==nRet)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"turn can't connect";
					nStep=4;
					break;
				}
				qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"turn success";
				m_DeviceConnecton=m_DeviceConnectonTurn;
				nStep=3;
			}
			break;
			//���ӳɹ�
		case 3:
			{
                m_bCloseingFlags=false;
				if (IDeviceClient::STATUS_CONNECTED!=m_CurStatus)
				{
					m_CurStatus=IDeviceClient::STATUS_CONNECTED;
					QVariantMap CurStatusParm;
					CurStatusParm.insert("CurrentStatus",m_CurStatus);
					eventProcCall("CurrentStatus",CurStatusParm);
				}
				nStep=5;
			}
			break;
			//����ʧ��
		case 4:
			{
				qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<sAddr<<"connect fail";
                m_bCloseingFlags=false;
				if (IDeviceClient::STATUS_DISCONNECTED!=m_CurStatus)
				{
					m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
					QVariantMap CurStatusParm;
					CurStatusParm.insert("CurrentStatus",m_CurStatus);
					eventProcCall("CurrentStatus",CurStatusParm);
				}
				nStep=5;
			}
			break;
			//�˳�
		case 5:
			{
	
			}
			break;
		default:
			break;
		}
	}
	if (m_CurStatus==IDeviceClient::STATUS_DISCONNECTED)
	{
		return 1;
	}
	if (NULL != m_DeviceConnecton)
	{
		m_DeviceConnecton->QueryInterface(IID_IProtocolPTZ, (void**)&m_pProtocolPTZ);
	}

	return 0;
}

int DeviceClient::connectToDevice()
{
	return connectToDevice(m_sAddr,m_uiPort,m_sEseeId);
}

int DeviceClient::checkUser(const QString & sUsername,const QString &sPassword)
{
	m_sUserName = sUsername;
	m_sPassWord = sPassword;
	m_remotePlayback.checkUser(sUsername,sPassword);
	return 0;
}
int DeviceClient::setChannelName(const QString & sChannelName)
{
	m_sChannelName=sChannelName;
	return 0;
}
int DeviceClient::liveStreamRequire(int nChannel,int nStream,bool bOpen)
{
	IRemotePreview *n_IRemotePreview=NULL;
	if (NULL==m_DeviceConnecton)
	{
		return 1;
	}
	m_DeviceConnecton->QueryInterface(IID_IRemotePreview,(void**)&n_IRemotePreview);
	if (NULL==n_IRemotePreview)
	{
		return 1;
	}

	//��Ҫ�ж� �Ƿ��Ѿ�����
	//��Ҫ�ж� �Ƿ��Ѿ����������
	if (true==bOpen)
	{
		if (1==n_IRemotePreview->getLiveStream(nChannel,nStream))
		{
			n_IRemotePreview->Release();
			return 1;
		}
	}
	else if (false==bOpen)
	{
		n_IRemotePreview->pauseStream(true);
	}
	n_IRemotePreview->Release();
	return 0;
}

int DeviceClient::closeAll()
{
	m_csCloseAll.lock();
    m_bCloseingFlags=true;
	//�ͷ���̨���������Դ
	if (NULL != m_pProtocolPTZ)
	{
		m_pProtocolPTZ->Release();
		m_pProtocolPTZ = NULL;
	}
	//����Ѿ����ڶϿ�״̬��ֱ�ӷ���
	if (IDeviceClient::STATUS_DISCONNECTED==m_CurStatus)
	{
		m_csCloseAll.unlock();
        m_bCloseingFlags = false;
        return 0;
	}
	//�������ڶϿ���״̬�����׳�
	m_CurStatus=IDeviceClient::STATUS_DISCONNECTING;
	{
	QVariantMap CurStatusParm;
	CurStatusParm.insert("CurrentStatus",m_CurStatus);
	eventProcCall("CurrentStatus",CurStatusParm);
	}
	//����Ͽ��Ľӿ�
	IDeviceConnection *m_CloseAllConnect=NULL;
	if (NULL==m_DeviceConnecton)
	{
		m_csCloseAll.unlock();
        m_bCloseingFlags = false;
        return 0;
	}

	m_DeviceConnecton->QueryInterface(IID_IDeviceConnection,(void**)&m_CloseAllConnect);
	if (NULL!=m_CloseAllConnect)
	{
		m_CloseAllConnect->disconnect();
		m_CloseAllConnect->Release();
	}
	m_csCloseAll.unlock();

    m_bCloseingFlags=false;
	return 0;
}

QString DeviceClient::getVendor()
{
	QString sReturn;
	return sReturn;
}
int DeviceClient::getConnectStatus()
{
	return m_CurStatus;
}
int DeviceClient::ConnectStatusProc(QVariantMap evMap)
{
	QVariantMap::const_iterator it;
	for (it=evMap.begin();it!=evMap.end();++it)
	{
		QString sKey=it.key();
		QString sValue=it.value().toString();
		//Э�������ϣ�����豸�����״̬��Ϊ����״̬�����׳��ź�
		if (IDeviceConnection::CS_Connected==it.value().toInt())
		{
			if (IDeviceClient::STATUS_CONNECTED!=m_CurStatus)
			{
				m_CurStatus=IDeviceClient::STATUS_CONNECTED;
				QVariantMap CurStatusParm;
				CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_CONNECTED);
				eventProcCall("CurrentStatus",CurStatusParm);
			}
		}
		//Э��Ͽ�״̬������豸�����״̬��Ϊ�Ͽ�״̬�����׳��ź�
		if (IDeviceConnection::CS_Disconnected==it.value().toInt())
		{
			if (IDeviceClient::STATUS_DISCONNECTED!=m_CurStatus&&IDeviceClient::STATUS_CONNECTING!=m_CurStatus)
			{
				m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
				QVariantMap CurStatusParm;
				CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_DISCONNECTED);
				eventProcCall("CurrentStatus",CurStatusParm);
			}
		}
	}
	if (m_CurStatus==IDeviceClient::STATUS_CONNECTED)
	{
        m_bCloseingFlags=true;
	}
	return 0;
}
void DeviceClient::eventProcCall(QString sEvent,QVariantMap param)
{
	if (m_EventList.contains(sEvent))
	{
		DeviceClientInfoItem eventDes=m_EventMap.value(sEvent);
		if (NULL!=eventDes.proc)
		{
			eventDes.proc(sEvent,param,eventDes.puser);
		}
	}
}
int DeviceClient::cbInit()
{
	//ע��bubbЭ��Ļص�����
	if (NULL!=m_DeviceConnectonBubble)
	{
		IEventRegister *IEventReg=NULL;
		m_DeviceConnectonBubble->QueryInterface(IID_IEventRegister,(void**)&IEventReg);
		if (NULL!=IEventReg)
		{
			QString evName = QString("RecordStream");
			IEventReg->registerEvent(evName, cbXRecordStream, this);

			QMultiMap<QString,DeviceClientInfoItem>::const_iterator it;
			for (it=m_EventMapToPro.begin();it!=m_EventMapToPro.end();++it)
			{
				QString sKey=it.key();
				DeviceClientInfoItem sValue=it.value();
				IEventReg->registerEvent(sKey,sValue.proc,sValue.puser);
			}
			IEventReg->Release();
		}
	}
	//ע�ᴩ͸Э��Ļص�����
	if (NULL!=m_DeviceConnectonHole)
	{
		IEventRegister *IEventReg=NULL;
		m_DeviceConnectonHole->QueryInterface(IID_IEventRegister,(void**)&IEventReg);
		if (NULL!=IEventReg)
		{
			QMultiMap<QString,DeviceClientInfoItem>::const_iterator it;
			for (it=m_EventMapToPro.begin();it!=m_EventMapToPro.end();++it)
			{
				QString sKey=it.key();
				DeviceClientInfoItem sValue=it.value();
				IEventReg->registerEvent(sKey,sValue.proc,sValue.puser);
			}
			IEventReg->Release();
		}
	}
	//ע��ת��Э��Ļص�����
	if (NULL!=m_DeviceConnectonTurn)
	{
		IEventRegister *IEventReg=NULL;
		m_DeviceConnectonTurn->QueryInterface(IID_IEventRegister,(void**)&IEventReg);
		if (NULL!=IEventReg)
		{
			QMultiMap<QString,DeviceClientInfoItem>::const_iterator it;
			for (it=m_EventMapToPro.begin();it!=m_EventMapToPro.end();++it)
			{
				QString sKey=it.key();
				DeviceClientInfoItem sValue=it.value();
				IEventReg->registerEvent(sKey,sValue.proc,sValue.puser);
			}
			IEventReg->Release();
		}
	}
    m_bIsInitFlags=true;
	return 0;
}
int cbXStateChange(QString evName,QVariantMap evMap,void*pUser)
{
	if ("StateChangeed"==evName)
	{
		((DeviceClient*)pUser)->ConnectStatusProc(evMap);
		return 0;
	}
	return 1;
}

void DeviceClient::action(QString options, BufferManager *pBuffer)
{
	if ("StartPlay" == options)
	{
		QMap<int, WndPlay>::iterator it;
		for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
		{
			if (it->bufferManager == pBuffer)
			{
				it->playManager->setParamter(pBuffer, it->wnd);

				if (it->playManager->isRunning())
				{
					it->playManager->quit();
				}

				it->playManager->start();
			}
		}
	}
	else if ("Pause" == options)
	{
		m_pRemotePlayback->pausePlaybackStream(true);
	}
	else if ("Continue" == options)
	{
		m_pRemotePlayback->pausePlaybackStream(false);
	}
	else
	{
		//nothing
	}
}

void DeviceClient::bufferStatus(int persent, BufferManager* pBuff)
{
	if (m_groupMap.isEmpty() || NULL == pBuff)
	{
		return;
	}

	QMap<int, WndPlay>::iterator iter = m_groupMap.begin();
	while (iter != m_groupMap.end())
	{
		if (pBuff == iter->bufferManager)
		{
			break;
		}
		++iter;
	}
// 	void* wind = (void*)iter->wnd;
// 	int *wind = reinterpret_cast<int*>(iter->wnd);

    quintptr wind = (quintptr)iter->wnd;

	QVariantMap item;
	item.insert("Persent", persent);
	item.insert("wind", wind);

	eventProcCall(QString("bufferStatus"), item);
}
bool DeviceClient::removeRepeatWnd(QWidget *wndID)
{
	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (it->wnd == wndID)
		{
			int temp = ~m_nChannels;
			temp |= 1<<(it.key() - 1);
			m_nChannels = ~temp;
			m_groupMap.remove(it.key());

			return true;
		}
	}

	return false;
}

int DeviceClient::startSearchRecFile(int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime)
{
	m_remotePlayback.startSearchRecFile(nChannel,nTypes,startTime,endTime);
	return 0;
	if ( nTypes < 0 || nTypes > 15 || startTime >= endTime)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"fail";
		return 2;
	}
    if (false==m_bIsInitFlags)
	{
		cbInit();
	}
	int ret = 1;
	if (NULL == m_pRemotePlayback)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"fail";
		return 1;
	}

	IDeviceConnection *pDeviceConnection = NULL;
	m_pRemotePlayback->QueryInterface(IID_IDeviceConnection, (void**)&pDeviceConnection);
	if (NULL == pDeviceConnection)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"fail";
		return 1;
	}

	pDeviceConnection->setDeviceAuthorityInfomation(m_sUserName, m_sPassWord);
	pDeviceConnection->setDeviceHost(m_sAddr);
	pDeviceConnection->setDeviceId(m_sEseeId);
	QVariantMap ports;
	ports.insert("media",m_uiPort);
	pDeviceConnection->setDevicePorts(ports);
	pDeviceConnection->Release();
	ret = m_pRemotePlayback->startSearchRecFile(nChannel,nTypes, startTime, endTime);

	return ret;
}
//nChannel start from 0 to 31
int DeviceClient::AddChannelIntoPlayGroup(int nChannel,QWidget * wnd)
{
	if (4 <= m_groupMap.size() || nChannel < 0 || NULL == wnd)
	{
		return 1;
	}

	//insert new item into the map
	WndPlay wndPlay;
    if (1 != ((m_nChannels>>(nChannel))&1))
	{
		m_nChannels |= 1<<(nChannel);

		wndPlay.bufferManager = new BufferManager();
		wndPlay.playManager = new PlayManager();

		removeRepeatWnd(wnd);
		wndPlay.wnd = wnd;

		QObject::connect(wndPlay.bufferManager, SIGNAL(action(QString, BufferManager*)), this, SLOT(action(QString, BufferManager*)));
// 		QObject::connect(wndPlay.playManager, SIGNAL(action(QString, BufferManager*)), this, SLOT(action(QString, BufferManager*)));
		QObject::connect(wndPlay.bufferManager, SIGNAL(bufferStatus(int,BufferManager*)), this, SLOT(bufferStatus(int, BufferManager*)));

		m_groupMap.insert(nChannel, wndPlay);
	}
	else
	{
		m_groupMap[nChannel].wnd = wnd;
	}
	
	return 0;
}

int DeviceClient::GroupPlay(int nTypes,const QDateTime & start,const QDateTime & end)
{
	if (nTypes < 0 || nTypes > 15 || start >= end)
	{
		return 2;
	}
	if (NULL == m_pRemotePlayback || 0 != getConnectStatus())
	{
		return 1;
	}
	m_nStartTimeSeconds = start.toTime_t();
	int nRet = m_pRemotePlayback->getPlaybackStreamByTime(m_nChannels, nTypes, start, end);
	m_bGroupStop = false;

	if (!m_groupMap.isEmpty())
	{
		QMap<int, WndPlay>::iterator iter = m_groupMap.begin();
		while(iter != m_groupMap.end())
		{
            quintptr wind = (quintptr)iter->wnd;
			QVariantMap item;
			item.insert("Persent", 0);
			item.insert("wind", wind);

			eventProcCall(QString("bufferStatus"), item);
			iter++;
		}
	}

	return nRet;
}
QDateTime DeviceClient::GroupGetPlayedTime()
{
	QDateTime time;
	QTime secTime(0,0,0);

	if (m_groupMap.isEmpty())
	{
		return time;
	}

	QMap<int, WndPlay>::iterator it;
	it = m_groupMap.begin();

	if (NULL == it->playManager)
	{
		return time;
	}

	int seconds = 0;
	seconds = it->playManager->getPlayTime();
	seconds -= m_nStartTimeSeconds;
	time.setDate(QDate::currentDate());
	if (seconds < 0)
	{
		seconds = 0;
	}
	time.setTime(secTime.addSecs(seconds));
	/*time=QDateTime::fromTime_t(seconds);*/
	return time;
}
int DeviceClient::GroupPause()
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}

	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL == it->playManager)
		{
			continue;
		}

		it->playManager->pause(true);
	}
	return 0;
}
int DeviceClient::GroupContinue()
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}

	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL == it->playManager)
		{
			continue;
		}

		it->playManager->pause(false);
	}
	return 0;
}
int DeviceClient::GroupStop()
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}

	if (NULL == m_pRemotePlayback)
	{
		return 1;
	}
	//bCloseingFlags=true;
	//int nRet = m_pRemotePlayback->stopPlaybackStream();
	int nRet=closeAll();
	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL == it->bufferManager)
		{
			continue;
		}
		if (NULL == it->playManager)
		{
			continue;
		}
		g_pause.wakeOne();
		it->playManager->stop();
		it->bufferManager->emptyBuff();
		delete it->playManager;
		it->playManager = NULL;
		delete it->bufferManager;
		it->bufferManager = NULL;
	}
	m_nChannels = 0;
	m_groupMap.clear();
	m_bGroupStop = true;
	m_nStartTimeSeconds = 0;

	return nRet;
}
bool DeviceClient::GroupEnableAudio(bool bEnable)
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}
	QMap<int, WndPlay>::iterator it = m_groupMap.begin();
	if (NULL != it->playManager)
	{
		it->playManager->AudioSwitch(bEnable);
	}

	if (!bEnable&&m_channelWithAudio!=-1)
	{	
		it = m_groupMap.find(m_channelWithAudio);
		m_channelWithAudio = -1;
	}

	return true;
}
int DeviceClient::GroupSetVolume(unsigned int uiPersent, QWidget* pWnd)
{
    if ((int)uiPersent < 0)
	{
		return 1;
	}
	QMap<int, WndPlay>::iterator it = m_groupMap.begin();
	if (NULL == it->playManager)
	{
		return 1;
	}

	if (0xAECBCA == uiPersent)
	{

		while(it != m_groupMap.end())
		{
			if (it->wnd == pWnd)
			{
				break;
			}
			++it;
		}
		m_channelWithAudio = it.key();
		it->playManager->setCurAudioWnd(it->playManager);

	}
	else
	{
		it->playManager->setVolume(uiPersent);
	}
	return 0;
}

int DeviceClient::GroupSpeedFast()
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}

	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL == it->playManager)
		{
			continue;
		}
		m_nSpeedRate = 0;
		it->playManager->setPlaySpeed(1, 1);
	}
	return 0;
}
int DeviceClient::GroupSpeedSlow()
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}

	m_nSpeedRate++;

	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL == it->playManager)
		{
			continue;
		}

		it->playManager->setPlaySpeed(2, m_nSpeedRate);
	}
	return 0;
}
int DeviceClient::GroupSpeedNormal()
{
	if (m_groupMap.isEmpty())
	{
		return 1;
	}

	QMap<int, WndPlay>::iterator it;
	for (it = m_groupMap.begin(); it != m_groupMap.end(); it++)
	{
		if (NULL == it->playManager)
		{
			continue;
		}
		m_nSpeedRate = 0;
		it->playManager->setPlaySpeed(0, 1);
	}
	return 0;
}

int DeviceClient::recordFrame(QVariantMap &evMap)
{
	if (m_bGroupStop)
	{
		return 1;
	}

	int nRet = 0;
	int channel = evMap.value("channel").toInt();
	QMap<int, WndPlay>::iterator iter = m_groupMap.find(channel);
	BufferManager *pBuffer = iter->bufferManager;
	if (NULL == pBuffer)
	{
		return 1;
	}
	nRet = pBuffer->recordStream(evMap);

	return nRet;
}

int DeviceClient::startBackup(const QString &sAddr,unsigned int uiPort,const QString &sEseeId,
	int nChannel,
	int nTypes,
	const QString &sDeviceName,
	const QDateTime & startTime,
	const QDateTime & endTime,
	const QString & sbkpath)
{
	return m_RemoteBackup.StartByParam(sAddr,uiPort,sEseeId,nChannel,nTypes,sDeviceName,startTime,endTime,sbkpath);
}
int DeviceClient::stopBackup()
{
	return m_RemoteBackup.Stop();
}
float DeviceClient::getProgress()
{
	return m_RemoteBackup.getProgress();
}

int DeviceClient::cbFoundFile( QVariantMap &evmap )
{
	eventProcCall("foundFile",evmap);
	return 0;
}

int DeviceClient::cbRecFileSearchFinished( QVariantMap &evmap )
{
	eventProcCall("recFileSearchFinished",evmap);
	return 0;
}
int DeviceClient::cbRecFileSearchFail( QVariantMap &evmap )
{
	eventProcCall("recFileSearchFail",evmap);
	return 0;
}
int DeviceClient::cbLiveStream( QVariantMap &evmap )
{
	eventProcCall("LiveStream",evmap);
	eventProcCall("ForRecord",evmap);
	return 0;
}

int DeviceClient::cbSocketError(QVariantMap &evmap)
{
	eventProcCall("SocketError",evmap);
	return 0;
}

int DeviceClient::ControlPTZUp( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZUp(nChl, nSpeed);
}

int DeviceClient::ControlPTZDown( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZDown(nChl, nSpeed);
}

int DeviceClient::ControlPTZLeft( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZLeft(nChl, nSpeed);
}

int DeviceClient::ControlPTZRight( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZRight(nChl, nSpeed);
}

int DeviceClient::ControlPTZIrisOpen( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZIrisOpen(nChl, nSpeed);
}

int DeviceClient::ControlPTZIrisClose( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZIrisClose(nChl, nSpeed);
}

int DeviceClient::ControlPTZFocusFar( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZFocusFar(nChl, nSpeed);
}

int DeviceClient::ControlPTZFocusNear( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZFocusNear(nChl, nSpeed);
}

int DeviceClient::ControlPTZZoomIn( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZZoomIn(nChl, nSpeed);
}

int DeviceClient::ControlPTZZoomOut( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZZoomOut(nChl, nSpeed);
}

int DeviceClient::ControlPTZAuto( const int &nChl, bool bOpend )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZAuto(nChl, bOpend);
}

int DeviceClient::ControlPTZStop( const int &nChl, const int &nCmd )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nCmd < 0 || nCmd > 10)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZStop(nChl, nCmd);
}

int DeviceClient::setDeviceHost( const QString & sAddr )
{
	m_sAddr=sAddr;
	m_remotePlayback.setDeviceHost(sAddr);
	return 0;
}

int DeviceClient::setDevicePorts( unsigned int ports )
{
	m_uiPort=ports;
	m_remotePlayback.setDevicePorts(ports);
	return 0;
}

int DeviceClient::setDeviceId( const QString & isee )
{
	m_sEseeId=isee;
	m_remotePlayback.setDeviceEsee(isee);
	return 0;
}

int DeviceClient::cbConnectRefuse( QVariantMap &evMap )
{
	eventProcCall("ConnectRefuse",evMap);
	return 0;
}

void DeviceClient::setDeviceAuth( const QString & sUsername, const QString & sPassword )
{
	m_sUserName = sUsername;
	m_sPassWord = sPassword;
}

int cbXRecordStream(QString evName,QVariantMap evMap,void*pUser)
{
    Q_UNUSED(evName);
	int nRet = 0;
	DeviceClient *pClient = (DeviceClient*)pUser;

	nRet = pClient->recordFrame(evMap);

	return nRet;
}

int cbXFoundFile(QString evName,QVariantMap evMap,void*pUser)
{
	if ("foundFile"==evName)
	{
		((DeviceClient*)pUser)->cbFoundFile(evMap);
		return 0;
	}
	return 1;
}

int cbXRecFileSearchFinished(QString evName,QVariantMap evMap,void*pUser)
{
	if ("recFileSearchFinished"==evName)
	{
		((DeviceClient*)pUser)->cbRecFileSearchFinished(evMap);
		return 0;
	}
	return 1;
}
int cbXRecFileSearchFail(QString evName,QVariantMap evMap,void*pUser)
{
	if ("recFileSearchFail"==evName)
	{
		((DeviceClient*)pUser)->cbRecFileSearchFail(evMap);
		return 0;
	}
	return 1;
}
int cbXLiveStream( QString evName,QVariantMap evMap,void*pUser )
{
	if ("LiveStream"==evName)
	{
		((DeviceClient*)pUser)->cbLiveStream(evMap);
		return 0;
	}
	return 1;
}

int cbXSocketError( QString evName,QVariantMap evMap,void*pUser )
{
	if ("SocketError"==evName)
	{
		((DeviceClient*)pUser)->cbSocketError(evMap);
		return 0;
	}
	return 1;
}

int cbXConnectRefuse( QString evName,QVariantMap evMap,void*pUser )
{
	if ("ConnectRefuse"==evName)
	{
		((DeviceClient*)pUser)->cbConnectRefuse(evMap);
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"call back connectRefuse fail,as the evName is not collect";
		return 1;
	}
}
