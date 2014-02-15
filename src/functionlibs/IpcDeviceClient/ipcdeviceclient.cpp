#include "IpcDeviceClient.h"
#include <guid.h>


IpcDeviceClient::IpcDeviceClient(void):m_nRef(0),
	m_CurStatus(IDeviceClient::STATUS_DISCONNECTED),
	m_CurStream(0)
{
	//�������������ĳ�ʼ����״̬
	CurStatusInfo m_statusInfo;
	m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
	m_StreamCurStatus.insert(0,m_statusInfo);
	m_StreamCurStatus.insert(1,m_statusInfo);
	//���ûص��������ṹ��
	IpcDeviceClientToProcInfoItem m_IpcDevliInfo;
	//�������ص�����
	m_IpcDevliInfo.proc=cbLiveStreamFrompPotocol_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("LiveStream",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbStateChangeFrompPotocol_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("StateChangeed",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbSocketErrorFrompPotocol_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("SocketError",m_IpcDevliInfo);

	//�������ص�����
	m_IpcDevliInfo.proc=cbLiveStreamFrompPotocol_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("LiveStream",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbStateChangeFrompPotocol_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("StateChangeed",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbSocketErrorFrompPotocol_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("SocketError",m_IpcDevliInfo);
}


IpcDeviceClient::~IpcDeviceClient(void)
{
	closeAll();
}

long __stdcall IpcDeviceClient::QueryInterface( const IID & iid,void **ppv )
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
	else 
	{
		*ppv=NULL;
		return E_NOINTERFACE;
	}
	static_cast <IPcomBase*>(this)->AddRef();
	return S_OK;
}

unsigned long __stdcall IpcDeviceClient::AddRef()
{
	m_csRef.lock();
	m_nRef++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long __stdcall IpcDeviceClient::Release()
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

QStringList IpcDeviceClient::eventList()
{
	return m_EventList;
}

int IpcDeviceClient::queryEvent( QString eventName,QStringList& eventParams )
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
	return IEventRegister::OK;
}

int IpcDeviceClient::registerEvent( QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser )
{
	if (!m_EventList.contains(eventName))
	{
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}
	if (m_EventMap.contains(eventName))
	{
		IpcDeviceClientInfoItem ipcdeviceInfo;
		ipcdeviceInfo.proc=proc;
		ipcdeviceInfo.puser=pUser;
		m_EventMap.replace(eventName,ipcdeviceInfo);
		return IEventRegister::OK;
	}
	IpcDeviceClientInfoItem ipcdeviceInfo;
	ipcdeviceInfo.proc=proc;
	ipcdeviceInfo.puser=pUser;
	m_EventMap.insert(eventName,ipcdeviceInfo);
	return IEventRegister::OK;
}

int IpcDeviceClient::connectToDevice( const QString &sAddr,unsigned int uiPort,const QString &sEseeId )
{
	m_DeviceInfo.m_sAddr.clear();
	m_DeviceInfo.m_sAddr=sAddr;
	m_DeviceInfo.m_ports.clear();
	m_DeviceInfo.m_ports.insert("media",uiPort);
	m_DeviceInfo.m_sEseeId.clear();
	m_DeviceInfo.m_sEseeId=sEseeId;
	//�Ͽ���һ�ε�����
	closeAll();
	//make sure had been disconnect
	bCloseingFlags=true;
	int nStep=0;
	m_CurStatus=IDeviceClient::STATUS_CONNECTING;
	while(nStep!=5){
		switch(nStep){
			//����bubble
		case 0:
			{
				if (true==TryToConnectProtocol(CLSID_BubbleProtocol))
				{
					qDebug()<<"bubble connect success";
					nStep=3;
				}
				else{
					qDebug()<<"bubble connect fail";
					nStep=1;
					break;
				}
			}
			break;
			//���Դ�͸Э������
		case 1:
			{
				if (true==TryToConnectProtocol(CLSID_Hole))
				{
					qDebug()<<"Hole connect success";
					nStep=3;
				}
				else{
					qDebug()<<"Hole connect fail";
					nStep=2;
					break;
				}
			}
			break;
			//����ת��Э������
		case 2:
			{
				if (true==TryToConnectProtocol(CLSID_Turn))
				{
					qDebug()<<"Turn connect success";
					nStep=3;
				}
				else{
					qDebug()<<"Turn connect fail";
					nStep=4;
					break;
				}
			}
			break;
			//���ӳɹ�
		case 3:
			{
			//fixme
				bCloseingFlags=false;
				nStep=5;
			}
			break;
			//����ʧ��
		case 4:
			{
				qDebug()<<"connect fail";
				//fixme
				nStep =5;
				m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
			}
			break;
		case 5:
			{
				m_CurStatus=IDeviceClient::STATUS_CONNECTED;
			}
			break;
		default:
			break;
			}
		}
	return 0;
}

int IpcDeviceClient::checkUser( const QString & sUsername,const QString &sPassword )
{
	m_DeviceInfo.m_sUserName.clear();
	m_DeviceInfo.m_sUserName=sUsername;
	m_DeviceInfo.m_sPassword.clear();
	m_DeviceInfo.m_sPassword=sPassword;
	return 0;
}

int IpcDeviceClient::setChannelName( const QString & sChannelName )
{
	m_DeviceInfo.m_sChannelName.clear();
	m_DeviceInfo.m_sChannelName=sChannelName;
	return 0;
}

int IpcDeviceClient::liveStreamRequire( int nChannel,int nStream,bool bOpen )
{
	//����������
	if (m_DeviceClentMap.value(0).m_DeviceConnecton!=NULL)
	{
		IRemotePreview *m_LiveStreamRequire=NULL;
		m_DeviceClentMap.value(0).m_DeviceConnecton->QueryInterface(IID_IRemotePreview,(void**)&m_LiveStreamRequire);
		if (true==bOpen)
		{
			//Ĭ��������Ϊ0
			if (1==m_LiveStreamRequire->getLiveStream(nChannel,0))
			{
				m_LiveStreamRequire->Release();
				return 1;
			}
		}
		else if (false==bOpen)
		{
			m_LiveStreamRequire->pauseStream(true);
		}
		m_LiveStreamRequire->Release();
	}
	
	//���������
	if (m_DeviceClentMap.value(1).m_DeviceConnecton!=NULL)
	{
		IRemotePreview *m_LiveStreamRequire=NULL;
		m_DeviceClentMap.value(1).m_DeviceConnecton->QueryInterface(IID_IRemotePreview,(void**)&m_LiveStreamRequire);
		if (true==bOpen)
		{
			//Ĭ�ϴ�����Ϊ1
			if (1==m_LiveStreamRequire->getLiveStream(nChannel,1))
			{
				m_LiveStreamRequire->Release();
				return 1;
			}
		}
		else if (false==bOpen)
		{
			m_LiveStreamRequire->pauseStream(true);
		}
		m_LiveStreamRequire->Release();
	}
	return 0;
}

int IpcDeviceClient::closeAll()
{
	//�ж��������ӵ�״̬
	bCloseingFlags=true;
	m_CurStatus=IDeviceClient::STATUS_DISCONNECTING;
	QMultiMap<int,SingleConnect>::iterator it;
	for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
	{
		if (NULL!=it->m_DeviceConnecton)
		{
			//�Ͽ�����
			IDeviceConnection *m_CloseAllConnect=NULL;
			it->m_DeviceConnecton->QueryInterface(IID_IDeviceConnection,(void**)&m_CloseAllConnect);
			m_CloseAllConnect->disconnect();
			m_CloseAllConnect->Release();
			//�ͷ���Դ
			it->m_DeviceConnecton->Release();
			it->m_DeviceConnecton=NULL;
		}
	}
	m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
	m_CurStream=0;
	return 0;
}

QString IpcDeviceClient::getVendor()
{
	QString sReturn;
	return sReturn;
}

int IpcDeviceClient::getConnectStatus()
{
	return m_CurStatus;
}

int IpcDeviceClient::cbLiveStream( QVariantMap &evmap )
{
	if (0==m_CurStatus)
	{
		if ("Primary"==evmap.value("Stream"))
		{
			evmap.remove("Stream");
			eventProcCall("LiveStream",evmap);
			return 0;
		}
	}
	if (1==m_CurStatus)
	{
		if ("Minor"==evmap.value("Stream"))
		{
			evmap.remove("Stream");
			eventProcCall("LiveStream",evmap);
			return 0;
		}
	}
	return 0;
}

void IpcDeviceClient::eventProcCall( QString sEvent,QVariantMap param )
{
	if (m_EventList.contains(sEvent))
	{
		IpcDeviceClientInfoItem eventDes=m_EventMap.value(sEvent);
		if (NULL!=eventDes.proc)
		{
			eventDes.proc(sEvent,param,eventDes.puser);
		}
	}
}

int IpcDeviceClient::cbSocketError( QVariantMap &evmap )
{
	if (0==m_CurStatus)
	{
		if ("Primary"==evmap.value("Stream"))
		{
			evmap.remove("Stream");
			eventProcCall("SocketError",evmap);
			return 0;
		}
	}
	if (1==m_CurStatus)
	{
		if ("Minor"==evmap.value("Stream"))
		{
			evmap.remove("Stream");
			eventProcCall("SocketError",evmap);
			return 0;
		}
	}
	return 0;
}

int IpcDeviceClient::cbConnectStatusProc( QVariantMap evMap )
{
	//������������״̬��ֻ�������ӺͲ���������״̬
	if (0==m_CurStatus)
	{
		if ("Primary"==evMap.value("Stream"))
		{
			evMap.remove("Stream");
			QVariantMap::const_iterator it;
			for (it=evMap.begin();it!=evMap.end();++it)
			{
				QString sKey=it.key();
				if (IDeviceConnection::CS_Connected==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_CONNECTED;
					m_StreamCurStatus.insert(0,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Disconnected==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
					m_StreamCurStatus.insert(0,m_statusInfo);
				}
			}
		}
	}
		//������������״̬��ֻ�������ӺͲ���������״̬
	if (1==m_CurStatus)
	{
		if ("Minor"==evMap.value("Stream"))
		{
			evMap.remove("Stream");
			QVariantMap::const_iterator it;
			for (it=evMap.begin();it!=evMap.end();++it)
			{
				QString sKey=it.key();
				if (IDeviceConnection::CS_Connected==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_CONNECTED;
					m_StreamCurStatus.insert(1,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Disconnected==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
					m_StreamCurStatus.insert(1,m_statusInfo);
				}
			}
		}
	}
	//����IpcDeviceClient��״̬
	//����״̬����·������
	if (IDeviceClient::STATUS_CONNECTED==m_StreamCurStatus.value(0).m_CurStatus&&IDeviceClient::STATUS_CONNECTED==m_StreamCurStatus.value(1).m_CurStatus)
	{
		m_CurStatus=IDeviceClient::STATUS_CONNECTED;
		QVariantMap CurStatusParm;
		CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_CONNECTED);
		eventProcCall("CurrentStatus",CurStatusParm);
	}

	//�Ͽ�״̬����·���Ͽ�
	if (IDeviceClient::STATUS_DISCONNECTED==m_StreamCurStatus.value(0).m_CurStatus&&IDeviceClient::STATUS_DISCONNECTED==m_StreamCurStatus.value(1).m_CurStatus)
	{
		m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
		QVariantMap CurStatusParm;
		CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_DISCONNECTED);
		eventProcCall("CurrentStatus",CurStatusParm);
	}
	//ԭ��״̬Ϊ���ӣ�һ·�Ͽ������Ͽ����е�����
	if (IDeviceClient::STATUS_CONNECTED==m_CurStatus)
	{
		if (m_StreamCurStatus.value(0).m_CurStatus!=m_StreamCurStatus.value(1).m_CurStatus)
		{
			closeAll();
		}
	}

	//ԭ��״̬Ϊδ���ӣ�һ·���ӣ�do nothing
	return 0;
}

int IpcDeviceClient::SwitchStream( int StreamNum )
{
	m_CurStream=StreamNum;
	return 0;
}

bool IpcDeviceClient::TryToConnectProtocol( CLSID clsid )
{
	int mount=0;
	IDeviceConnection *m_DeviceConnectProtocol=NULL;
	while(mount<2){
		pcomCreateInstance(clsid,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnectProtocol);
		if (NULL==m_DeviceConnectProtocol)
		{
			//�ͷ���Դ
			QMultiMap<int,SingleConnect>::iterator it;
			for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
			{
				if (NULL!=it->m_DeviceConnecton)
				{
					it->m_DeviceConnecton->Release();
					it->m_DeviceConnecton=NULL;
				}
			}
			return false;
		}
		//save to struct 
		SingleConnect m_SingleConnect;
		m_SingleConnect.m_DeviceConnecton=m_DeviceConnectProtocol;
		m_DeviceClentMap.insert(mount,m_SingleConnect);
		//ע���¼�
		IEventRegister *m_RegisterProc=NULL;
		m_DeviceConnectProtocol->QueryInterface(IID_IEventRegister,(void**)&m_RegisterProc);
		if (NULL==m_RegisterProc)
		{
			QMultiMap<int,SingleConnect>::iterator it;
			for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
			{
				if (NULL!=it->m_DeviceConnecton)
				{
					it->m_DeviceConnecton->Release();
					it->m_DeviceConnecton=NULL;
				}
			}
			return false;
		}
		RegisterProc(m_RegisterProc,mount);
		m_RegisterProc->Release();

		//�����豸����
		if (1==m_DeviceConnectProtocol->setDeviceHost(m_DeviceInfo.m_sAddr)||1==m_DeviceConnectProtocol->setDevicePorts(m_DeviceInfo.m_ports)||1==m_DeviceConnectProtocol->setDeviceId(m_DeviceInfo.m_sEseeId))
		{
			//����ʧ�ܣ��ͷ���Դ��ֱ�ӷ���ʧ��
			QMultiMap<int,SingleConnect>::iterator it;
			for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
			{
				if (NULL!=it->m_DeviceConnecton)
				{
					it->m_DeviceConnecton->Release();
					it->m_DeviceConnecton=NULL;
				}
			}
			return false;
		}
		if (true==bCloseingFlags)
		{
			//Ҫ��ֹͣ����
			//�ͷ���Դ
			QMultiMap<int,SingleConnect>::iterator it;
			for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
			{
				if (NULL!=it->m_DeviceConnecton)
				{
					it->m_DeviceConnecton->Release();
					it->m_DeviceConnecton=NULL;
				}
			}
			return false;
		}
			//�����豸
		if (1==m_DeviceConnectProtocol->connectToDevice())
		{
			//����ʧ�ܣ��ͷ���Դ��ֱ�ӷ���
			QMultiMap<int,SingleConnect>::iterator it;
			for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
			{
				if (NULL!=it->m_DeviceConnecton)
				{
					it->m_DeviceConnecton->Release();
					it->m_DeviceConnecton=NULL;
				}
			}
			return false;
		}
		//���ӳɹ�
		mount++;
	}
	return true;
}

int IpcDeviceClient::RegisterProc(IEventRegister *m_RegisterProc,int m_Stream )
{
	QMultiMap<QString ,IpcDeviceClientToProcInfoItem>::const_iterator it;
	for(it=m_EventMapToProc.begin();it!=m_EventMapToProc.end();++it){
		QString sKey=it.key();
		IpcDeviceClientToProcInfoItem sValue=it.value();
		if (sValue.Stream==m_Stream)
		{
			m_RegisterProc->registerEvent(sKey,sValue.proc,sValue.puser);
		}
	}
	return 0;
}

int cbLiveStreamFrompPotocol_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("LiveStream"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbLiveStream(evMap);
		return 0;
	}
	return 1;
}

int cbSocketErrorFrompPotocol_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("SocketError"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbSocketError(evMap);
		return 0;
	}
	return 1;
}

int cbStateChangeFrompPotocol_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("StateChangeed"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbConnectStatusProc(evMap);
		return 0;
	}
	return 1;
}

int cbLiveStreamFrompPotocol_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("LiveStream"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbLiveStream(evMap);
		return 0;
	}
	return 1;
}

int cbSocketErrorFrompPotocol_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("SocketError"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbSocketError(evMap);
		return 0;
	}
	return 1;
}

int cbStateChangeFrompPotocol_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("StateChangeed"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbConnectStatusProc(evMap);
		return 0;
	}
	return 1;
}
