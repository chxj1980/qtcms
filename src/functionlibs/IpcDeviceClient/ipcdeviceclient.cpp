#include "IpcDeviceClient.h"
#include <guid.h>
#include "ILocalSetting.h"
#include <QDateTime>
#include <QHostAddress>
#include <QUrl>
#include <IRemoteMotionDetection.h>

int mdsignal_proc(QString sEvent,QVariantMap param,void * pUser)
{
	/*qDebug() << "Md" << param["signal"];*/
	//qDebug()<<__LINE__<<__LINE__<<"Md"<<param;
	if (sEvent=="MDSignal")
	{
		((IpcDeviceClient*)pUser)->cbMotionDetection(param);
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"mdsignal_proc callBack event fail as the eventName is not collect";
		return 1;
	}
	return 0;
}
int cbXAuthority(QString evName,QVariantMap evMap,void*pUser){
	if (evName=="Authority")
	{
		((IpcDeviceClient*)pUser)->cbAuthority(evMap);
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"cbXAuthority callBack event fail as the eventName is not collect";
		return 1;
	}
	return 0;
}
IpcDeviceClient::IpcDeviceClient(void):m_nRef(0),
	m_CurStatus(IDeviceClient::STATUS_DISCONNECTED),
	m_CurStream(0),
	m_csRefDelete(QMutex::Recursive),
	bHadCallCloseAll(false),
	bCloseingFlags(false),
	m_bIsSycTime(false),
	//m_tcpSocket(NULL),
	m_pProtocolPTZ(NULL),
	m_steps(0),
	m_IfSwithStream(0)
{
	//���ñ����֧�ֵĻص������¼�����
	m_EventList<<"LiveStream"<<"SocketError"<<"StateChangeed"<<"CurrentStatus"<<"foundFile"<<"recFileSearchFinished"<<"ForRecord"<<"SyncTimeMsg"<<"ConnectRefuse"<<"MDSignal"<<"Authority";
	//�������������ĳ�ʼ����״̬
	CurStatusInfo m_statusInfo;
	m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
	m_StreamCurStatus.insert(0,m_statusInfo);
	m_StreamCurStatus.insert(1,m_statusInfo);
	//���ûص��������ṹ��
	IpcDeviceClientToProcInfoItem m_IpcDevliInfo;
	//�������ص�����
	m_IpcDevliInfo.proc=cbXConnectRefuse_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("ConnectRefuse",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbXLiveStream_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("LiveStream",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbXStateChange_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("StateChangeed",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbXSocketError_Primary;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=0;
	m_EventMapToProc.insert("SocketError",m_IpcDevliInfo);

	//�������ص�����
	m_IpcDevliInfo.proc=cbXConnectRefuse_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("ConnectRefuse",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbXLiveStream_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("LiveStream",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbXStateChange_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("StateChangeed",m_IpcDevliInfo);
	m_IpcDevliInfo.proc=cbXSocketError_Minor;
	m_IpcDevliInfo.puser=this;
	m_IpcDevliInfo.Stream=1;
	m_EventMapToProc.insert("SocketError",m_IpcDevliInfo);
}


IpcDeviceClient::~IpcDeviceClient(void)
{
	if (false==bHadCallCloseAll)
	{
		closeAll();
	}
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
	else if (IID_ISwitchStream==iid)
	{
		*ppv=static_cast<ISwitchStream*>(this);
	}
	else if (IID_IAutoSycTime == iid)
	{
		*ppv=static_cast<IAutoSycTime*>(this);
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
	if ("SyncTimeMsg")
	{
		eventParams<<"statusCode"<<"statusMsg";
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
	m_DeviceInfo.m_ports.insert("http",uiPort);
	m_DeviceInfo.m_sEseeId.clear();
	m_DeviceInfo.m_sEseeId=sEseeId;

	//if (m_bIsSycTime)
	//{
	//	connect(this, SIGNAL(sigSyncTime()), this, SLOT(SyncTime()));
	//	emit sigSyncTime();//ͬ��ʱ��
	//}
	//�Ͽ���һ�ε�����
	if (m_CurStatus==IDeviceClient::STATUS_CONNECTED||m_CurStatus==IDeviceClient::STATUS_DISCONNECTING||m_CurStatus==IDeviceClient::STATUS_CONNECTING)
	{
		closeAll();
	}
	//make sure had been disconnect
	bCloseingFlags=false;
	int nStep=0;
	m_CurStatus=IDeviceClient::STATUS_CONNECTING;
	QVariantMap CurStatusParm;
	CurStatusParm.insert("CurrentStatus",m_CurStatus);
	eventProcCall("CurrentStatus",CurStatusParm);
	while(nStep!=5){
		switch(nStep){
			//����bubble
		case 0:
			{
				if (true==TryToConnectProtocol(CLSID_Bubble))
				{
					qDebug()<<__FUNCTION__<<__LINE__<<sAddr<<"bubble connect success";
					nStep=3;
				}
				else{
					qDebug()<<__FUNCTION__<<__LINE__<<sAddr<<"bubble connect fail";
					nStep=1;
					break;
				}
			}
			break;
			//���Դ�͸Э������
		case 1:
			{
				if (m_DeviceInfo.m_sEseeId!=""&&m_DeviceInfo.m_sEseeId!="0"&&true==TryToConnectProtocol(CLSID_Hole))
				{
					qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"Hole connect success";
					nStep=3;
				}
				else{
					qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"Hole connect fail";
					nStep=2;
					break;
				}
			}
			break;
			//����ת��Э������
		case 2:
			{
				if (m_DeviceInfo.m_sEseeId!=""&&m_DeviceInfo.m_sEseeId!="0"&&true==TryToConnectProtocol(CLSID_Turn))
				{
					qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"Turn connect success";
					nStep=3;
				}
				else{
					qDebug()<<__FUNCTION__<<__LINE__<<sEseeId<<"Turn connect fail";
					nStep=4;
					break;
				}
			}
			break;
			//���ӳɹ�
		case 3:
			{
				// ��֤��Ϣ
				int i;
				for (i = 0; i < 2; i ++)
				{
					m_csDeInit.lock();
					if (m_DeviceClentMap.contains(i))
					{
						m_DeviceClentMap.value(i).m_DeviceConnecton->setDeviceAuthorityInfomation(m_DeviceInfo.m_sUserName,m_DeviceInfo.m_sPassword);
					}
					m_csDeInit.unlock();
				}

				// ע���ƶ�����ź�
				//ע���û���֤�ص�����
				IEventRegister * iEg;
				m_csDeInit.lock();
				m_DeviceClentMap.value(0).m_DeviceConnecton->QueryInterface(IID_IEventRegister,(void **)&iEg);
				m_csDeInit.unlock();
				if (NULL != iEg)
				{
					iEg->registerEvent("MDSignal",mdsignal_proc,this);
					iEg->registerEvent("Authority",cbXAuthority,this);
					iEg->Release();
					iEg = NULL;
				}

				// �����ƶ����
				IRemoteMotionDetection * iMd;
				m_csDeInit.lock();
				m_DeviceClentMap.value(0).m_DeviceConnecton->QueryInterface(IID_IRemoteMotionDetection,(void **)&iMd);
				m_csDeInit.unlock();
				if (NULL != iMd)
				{
					iMd->startMotionDetection();
					iMd->Release();
					iMd = NULL;
				}

			//fixme
				bCloseingFlags=false;
				nStep=5;
			}
			break;
			//����ʧ��
		case 4:
			{
				qDebug()<<__FUNCTION__<<__LINE__<<sAddr<<sEseeId<<"connect fail";
				//fixme
				nStep =5;
				if (m_CurStatus!=IDeviceClient::STATUS_DISCONNECTED)
				{
					m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
					QVariantMap CurStatusParm;
					CurStatusParm.insert("CurrentStatus",m_CurStatus);
					eventProcCall("CurrentStatus",CurStatusParm);
				}
				return 1;
			}
			break;
		case 5:
			{
				//m_CurStatus=IDeviceClient::STATUS_CONNECTED;
				//QVariantMap CurStatusParm;
				//CurStatusParm.insert("CurrentStatus",m_CurStatus);
				//eventProcCall("CurrentStatus",CurStatusParm);
			}
			break;
		default:
			break;
			}
		}
	return 0;
}

int IpcDeviceClient::connectToDevice()
{
	return connectToDevice(m_sAddr,m_uiPort,m_sEseeId);
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
		m_csDeInit.lock();
		m_DeviceClentMap.value(0).m_DeviceConnecton->QueryInterface(IID_IRemotePreview,(void**)&m_LiveStreamRequire);
		m_csDeInit.unlock();
		if (true==bOpen)
		{
			//Ĭ��������Ϊ0
			IDeviceConnection *pDeviceConnection=NULL;
			m_csDeInit.lock();
			m_DeviceClentMap.value(0).m_DeviceConnecton->QueryInterface(IID_IDeviceConnection,(void**)&pDeviceConnection);
			m_csDeInit.unlock();
			if (NULL!=pDeviceConnection)
			{
				pDeviceConnection->setDeviceAuthorityInfomation(m_DeviceInfo.m_sUserName,m_DeviceInfo.m_sPassword);
				if (0==pDeviceConnection->authority())
				{
					if (1==m_LiveStreamRequire->getLiveStream(nChannel,0))
					{
						m_LiveStreamRequire->Release();
						pDeviceConnection->Release();
						return 1;
					}else{
						pDeviceConnection->Release();
					}
				}else{
					m_LiveStreamRequire->Release();
					pDeviceConnection->Release();
					return 1;
				}
			}else{
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
		m_csDeInit.lock();
		m_DeviceClentMap.value(1).m_DeviceConnecton->QueryInterface(IID_IRemotePreview,(void**)&m_LiveStreamRequire);
		m_csDeInit.unlock();
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
	// ֹͣmd
	IRemoteMotionDetection * iMd;
	m_csDeInit.lock();
	if (m_DeviceClentMap.contains(0))
	{
		m_DeviceClentMap.value(0).m_DeviceConnecton->QueryInterface(IID_IRemoteMotionDetection,(void **)&iMd);
		if (NULL != iMd)
		{
			iMd->stopMotionDetection();
			iMd->Release();
			iMd = NULL;
		}
	}
	m_csDeInit.unlock();
	//�ж��������ӵ�״̬
	bHadCallCloseAll=true;
	m_csRefDelete.lock();
	//�ͷ���̨���������Դ
	if (NULL != m_pProtocolPTZ)
	{
		m_pProtocolPTZ->Release();
		m_pProtocolPTZ = NULL;
	}
	//����Ѿ����ڶϿ���״̬�����ͷ���Դ
	if (m_CurStatus==IDeviceClient::STATUS_DISCONNECTED)
	{
		DeInitProtocl();
		m_csRefDelete.unlock();
		bHadCallCloseAll=false;
		return 0;
	}
	bCloseingFlags=true;
	m_CurStatus=IDeviceClient::STATUS_DISCONNECTING;
	QVariantMap CurStatusParm;
	CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_DISCONNECTING);
	eventProcCall("CurrentStatus",CurStatusParm);


	QMultiMap<int,SingleConnect>::iterator it;
	m_csDeInit.lock();
	for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
	{
		
		if (NULL!=it->m_DeviceConnecton)
		{
			//�Ͽ�����
			IDeviceConnection *m_CloseAllConnect=NULL;
			it->m_DeviceConnecton->QueryInterface(IID_IDeviceConnection,(void**)&m_CloseAllConnect);
			m_CloseAllConnect->disconnect();
			msleep(20);
			m_CloseAllConnect->Release();
		}
	}
	m_csDeInit.unlock();
	//�ͷ���Դ
	DeInitProtocl();

	m_CurStream=0;
	m_csRefDelete.unlock();
	bHadCallCloseAll=false;
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
	m_cscbLiveStream.lock();
	//¼������
	if ("Primary"==evmap.value("Stream"))
	{
		evmap.remove("Stream");
		eventProcCall("ForRecord",evmap);
		evmap.insert("Stream","Primary");
	}

	// �л�����
	if (m_IfSwithStream != m_CurStream)
	{
		if (1 == evmap.value("frametype"))
		{
			if ("Primary" == evmap.value("Stream") && 0 == m_IfSwithStream)
			{
				m_CurStream = m_IfSwithStream;
			}
			else if ("Minor" == evmap.value("Stream") && 1 == m_IfSwithStream)
			{
				m_CurStream = m_IfSwithStream;
			}
		}
	}

	//Ԥ������
	if (m_IfSwithStream==m_CurStream)
	{
		if (0==m_CurStream)
		{
			if ("Primary"==evmap.value("Stream"))
			{
				evmap.remove("Stream");
				eventProcCall("LiveStream",evmap);
				m_cscbLiveStream.unlock();
				return 0;
			}
		}
		if (1==m_CurStream)
		{
			if ("Minor"==evmap.value("Stream"))
			{
				evmap.remove("Stream");
				eventProcCall("LiveStream",evmap);
			}
		}
		m_cscbLiveStream.unlock();
		return 0;
	}
	m_cscbLiveStream.unlock();
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
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<sEvent<<"has not register ye";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"eventProcCall fail as m_EventList do not contain :"<<sEvent;
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
					m_StreamCurStatus.replace(0,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Disconnected==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
					m_StreamCurStatus.replace(0,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Disconnecting==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTING;
					m_StreamCurStatus.replace(0,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Connectting==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_CONNECTING;
					m_StreamCurStatus.replace(0,m_statusInfo);
				}
			}
		}
		//������������״̬��ֻ�������ӺͲ���������״̬
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
					m_StreamCurStatus.replace(1,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Disconnected==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
					m_StreamCurStatus.replace(1,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Disconnecting==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_DISCONNECTING;
					m_StreamCurStatus.replace(1,m_statusInfo);
				}
				else if (IDeviceConnection::CS_Connectting==it.value().toInt())
				{
					CurStatusInfo m_statusInfo;
					m_statusInfo.m_CurStatus=IDeviceClient::STATUS_CONNECTING;
					m_StreamCurStatus.replace(1,m_statusInfo);
				}
			}
		}

	//����IpcDeviceClient��״̬
	//����״̬����·������
	if (IDeviceClient::STATUS_CONNECTED==m_StreamCurStatus.value(0).m_CurStatus&&IDeviceClient::STATUS_CONNECTED==m_StreamCurStatus.value(1).m_CurStatus)
	{
		//���ԭ��״̬��������״̬�����׳��ź�
		if (m_CurStatus!=IDeviceClient::STATUS_CONNECTED)
		{
			QVariantMap CurStatusParm;
			CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_CONNECTED);
			eventProcCall("CurrentStatus",CurStatusParm);
		}
			m_CurStatus=IDeviceClient::STATUS_CONNECTED;
	}

	//�Ͽ�״̬����·���Ͽ�
	if (IDeviceClient::STATUS_DISCONNECTED==m_StreamCurStatus.value(0).m_CurStatus&&IDeviceClient::STATUS_DISCONNECTED==m_StreamCurStatus.value(1).m_CurStatus)
	{
		//������������ӹ��̣���û�����ӣ��źţ���connect�����׳����˴���������
		if (IDeviceClient::STATUS_CONNECTING==m_CurStatus)
		{
			return 0;
		}
		//���ԭ�����ǶϿ�״̬�����׳��Ͽ����ź�
		if (IDeviceClient::STATUS_DISCONNECTED!=m_CurStatus)
		{
			QVariantMap CurStatusParm;
			CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_DISCONNECTED);
			eventProcCall("CurrentStatus",CurStatusParm);
		}
		m_CurStatus=IDeviceClient::STATUS_DISCONNECTED;
	}
	//ԭ��״̬Ϊ���ӣ�һ·�Ͽ������Ͽ����е�����
	if (IDeviceClient::STATUS_CONNECTED==m_CurStatus)
	{
		if (m_StreamCurStatus.value(0).m_CurStatus==IDeviceClient::STATUS_DISCONNECTED||m_StreamCurStatus.value(1).m_CurStatus==IDeviceClient::STATUS_DISCONNECTED)
		{
			if (false==bHadCallCloseAll)
			{
				QVariantMap CurStatusParm;
				CurStatusParm.insert("CurrentStatus",IDeviceClient::STATUS_DISCONNECTED);
				eventProcCall("CurrentStatus",CurStatusParm);
			}
		}
	}

	//ԭ��״̬Ϊδ���ӣ�һ·���ӣ�do nothing
	return 0;
}

int IpcDeviceClient::SwitchStream( int StreamNum )
{
	m_IfSwithStream=StreamNum;
	return 0;
}
int IpcDeviceClient::setAutoSycTime(bool bEnabled)
{
	m_bIsSycTime = bEnabled;
	//change 
	if (m_bIsSycTime)
	{
		//connect(this, SIGNAL(sigSyncTime()), this, SLOT(SyncTime()));
		//emit sigSyncTime();//ͬ��ʱ��
		m_tSetAutoSycTime.setAutoSycTime(m_sAddr,(quint16)m_DeviceInfo.m_ports["media"].toUInt(),m_DeviceInfo.m_sUserName,m_DeviceInfo.m_sPassword);
	}
	return 0;
}
bool IpcDeviceClient::TryToConnectProtocol( CLSID clsid )
{
	int mount=0;
	IDeviceConnection *m_DeviceConnectProtocol=NULL;
	if (true==bCloseingFlags)
	{
		return false;
	}
	while(mount<2){
		pcomCreateInstance(clsid,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnectProtocol);
		if (NULL==m_DeviceConnectProtocol)
		{
			//�ͷ���Դ
			DeInitProtocl();
			return false;
		}

		//save to struct 
		SingleConnect m_SingleConnect;
		m_SingleConnect.m_DeviceConnecton=NULL;
		m_DeviceConnectProtocol->QueryInterface(IID_IDeviceConnection,(void**)&m_SingleConnect.m_DeviceConnecton);
		m_csDeInit.lock();
		m_DeviceClentMap.insert(mount,m_SingleConnect);
		m_csDeInit.unlock();
		//ע���¼�
		IEventRegister *m_RegisterProc=NULL;
		m_DeviceConnectProtocol->QueryInterface(IID_IEventRegister,(void**)&m_RegisterProc);
		if (NULL==m_RegisterProc)
		{
			DeInitProtocl();
			m_DeviceConnectProtocol->Release();
			m_DeviceConnectProtocol=NULL;
			return false;
		}
		RegisterProc(m_RegisterProc,mount);
		m_RegisterProc->Release();

		// �û�У����Ϣ
		m_DeviceConnectProtocol->setDeviceAuthorityInfomation(m_DeviceInfo.m_sUserName,m_DeviceInfo.m_sPassword);

		//�����豸����
		if (1==m_DeviceConnectProtocol->setDeviceHost(m_DeviceInfo.m_sAddr)||1==m_DeviceConnectProtocol->setDevicePorts(m_DeviceInfo.m_ports)||1==m_DeviceConnectProtocol->setDeviceId(m_DeviceInfo.m_sEseeId))
		{
			//����ʧ�ܣ��ͷ���Դ��ֱ�ӷ���ʧ��
			DeInitProtocl();
			m_DeviceConnectProtocol->Release();
			m_DeviceConnectProtocol=NULL;
			return false;
		}
		if ("0"==m_DeviceInfo.m_sEseeId&&(clsid==CLSID_Hole||clsid==CLSID_Turn))
		{
			DeInitProtocl();
			m_DeviceConnectProtocol->Release();
			m_DeviceConnectProtocol=NULL;
			return false;
		}
		if (true==bCloseingFlags)
		{
			//Ҫ��ֹͣ����
			//�ͷ���Դ
			DeInitProtocl();
			m_DeviceConnectProtocol->Release();
			m_DeviceConnectProtocol=NULL;
			return false;
		}
			//�����豸
		if (1==m_DeviceConnectProtocol->connectToDevice())
		{
			//����ʧ�ܣ��ͷ���Դ��ֱ�ӷ���
			DeInitProtocl();
			m_DeviceConnectProtocol->Release();
			m_DeviceConnectProtocol=NULL;
			return false;
		}
		//ʹ�ô����������ӷ�����̨��������
		if (1 == mount)
		{
			m_DeviceConnectProtocol->QueryInterface(IID_IProtocolPTZ, (void**)&m_pProtocolPTZ);
		}

		//���ӳɹ�
		m_DeviceConnectProtocol->Release();
		m_DeviceConnectProtocol=NULL;
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

void IpcDeviceClient::DeInitProtocl()
{
	m_csDeInit.lock();
	QMultiMap<int,SingleConnect>::iterator it;
	for (it=m_DeviceClentMap.begin();it!=m_DeviceClentMap.end();it++)
	{
		if (NULL!=it->m_DeviceConnecton)
		{
			it->m_DeviceConnecton->Release();
			it->m_DeviceConnecton=NULL;
		}
	}
	m_DeviceClentMap.clear();
	m_csDeInit.unlock();
}


int IpcDeviceClient::ControlPTZUp( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZUp(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZDown( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZDown(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZLeft( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZLeft(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZRight( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZRight(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZIrisOpen( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZIrisOpen(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZIrisClose( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZIrisClose(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZFocusFar( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZFocusFar(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZFocusNear( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZFocusNear(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZZoomIn( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZZoomIn(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZZoomOut( const int &nChl, const int &nSpeed )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nSpeed < 0 || nSpeed > 7)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZZoomOut(nChl, nSpeed);
}

int IpcDeviceClient::ControlPTZAuto( const int &nChl, bool bOpend )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZAuto(nChl, bOpend);
}

int IpcDeviceClient::ControlPTZStop( const int &nChl, const int &nCmd )
{
	if (NULL == m_pProtocolPTZ || nChl < 0 || nChl > 64 || nCmd < 0 || nCmd > 10)
	{
		return 1;
	}
	return m_pProtocolPTZ->PTZStop(nChl, nCmd);
}

int IpcDeviceClient::setDevicePorts( unsigned int ports )
{
	m_uiPort=ports;
	return 0;
}

int IpcDeviceClient::setDeviceId( const QString & isee )
{
	m_sEseeId=isee;
	return 0;
}

int IpcDeviceClient::setDeviceHost( const QString & sAddr )
{
	m_sAddr=sAddr;
	return 0;
}

int IpcDeviceClient::cbConnectRefuse( QVariantMap evMap )
{
	if ("Primary"==evMap.value("Stream")||"Minor"==evMap.value("Stream"))
	{
		evMap.remove("Stream");
		eventProcCall("ConnectRefuse",evMap);
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"call back cbConnectRefuse fail,as evMap is error";
		return 1;
	}
}

void IpcDeviceClient::setDeviceAuth( const QString & sUsername, const QString & sPassword )
{
	m_DeviceInfo.m_sUserName = sUsername;
	m_DeviceInfo.m_sPassword = sPassword;
}

int IpcDeviceClient::cbMotionDetection( QVariantMap evMap )
{
	eventProcCall("MDSignal",evMap);
	return 0;
}

int IpcDeviceClient::cbAuthority( QVariantMap evMap )
{
	eventProcCall("Authority",evMap);
	return 0;
}


int cbXLiveStream_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("LiveStream"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbLiveStream(evMap);
		return 0;
	}
	return 1;
}

int cbXSocketError_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("SocketError"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbSocketError(evMap);
		return 0;
	}
	return 1;
}

int cbXStateChange_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("StateChangeed"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbConnectStatusProc(evMap);
		return 0;
	}
	return 1;
}

int cbXLiveStream_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("LiveStream"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbLiveStream(evMap);
		return 0;
	}
	return 1;
}

int cbXSocketError_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("SocketError"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbSocketError(evMap);
		return 0;
	}
	return 1;
}

int cbXStateChange_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("StateChangeed"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbConnectStatusProc(evMap);
		return 0;
	}
	return 1;
}

int cbXConnectRefuse_Primary( QString evName,QVariantMap evMap,void*pUser )
{
	if ("ConnectRefuse"==evName)
	{
		evMap.insert("Stream","Primary");
		((IpcDeviceClient*)pUser)->cbConnectRefuse(evMap);
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"connectRefuse callBack event fail as the eventName is not collect";
		return 1;
	}
}

int cbXConnectRefuse_Minor( QString evName,QVariantMap evMap,void*pUser )
{
	if ("ConnectRefuse"==evName)
	{
		evMap.insert("Stream","Minor");
		((IpcDeviceClient*)pUser)->cbConnectRefuse(evMap);
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"connectRefuse callBack event fail as the eventName is not collect";
		return 1;
	}
}

