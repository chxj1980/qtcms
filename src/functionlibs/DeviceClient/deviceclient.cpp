#include "deviceclient.h"
#include <guid.h>

DeviceClient::DeviceClient():m_nRef(0),
	m_DeviceConnecton(NULL)
{
	pcomCreateInstance(CLSID_RemotePreview,NULL,IID_IDeviceConnection,(void**)&m_DeviceConnecton);
	m_EventList<<"LiveStream";
}

DeviceClient::~DeviceClient()
{

}

long _stdcall DeviceClient::QueryInterface(const IID & iid,void **ppv)
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

unsigned long _stdcall DeviceClient::AddRef()
{
	m_csRef.lock();
	m_nRef++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long _stdcall DeviceClient::Release()
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
	return IEventRegister::OK;
}

int DeviceClient::registerEvent(QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser)
{
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
	//������������Э�飺bubble����͸��ת��
	IDeviceConnection *n_IDeviceConnection;
	pcomCreateInstance(CLSID_RemotePreview,NULL,IID_IDeviceConnection,(void**)&n_IDeviceConnection);
	if (NULL==n_IDeviceConnection)
	{
		return 1;
	}
	//��Ҫ�����Ƿ����óɹ������巵��ֵ�ĺ���
	m_ports.insert("media",uiPort);
	n_IDeviceConnection->setDeviceHost(sAddr);
	n_IDeviceConnection->setDeviceId(sEseeId);
	n_IDeviceConnection->setDevicePorts(m_ports);

	//����
	int nRet=1;
	nRet=n_IDeviceConnection->connectToDevice();
	if (1==nRet)
	{
		return 1;
	}
	//ע��ص�����
	IEventRegister *IEventReg=NULL;
	n_IDeviceConnection->QueryInterface(IID_IEventRegister,(void**)&IEventReg);
	if (NULL==IEventReg)
	{
		n_IDeviceConnection->Release();
		n_IDeviceConnection=NULL;
		return 1;
	}
	QMultiMap<QString,DeviceClientInfoItem>::const_iterator it;
	for (it=m_EventMap.begin();it!=m_EventMap.end();++it)
	{
		QString sKey=it.key();
		DeviceClientInfoItem sValue=it.value();
		IEventReg->registerEvent(sKey,sValue.proc,sValue.puser);
	}

	if (NULL!=m_DeviceConnecton)
	{
		m_DeviceConnecton->Release();
		m_DeviceConnecton=NULL;
	}

	m_DeviceConnecton=n_IDeviceConnection;
	m_DeviceConnecton->AddRef();
	n_IDeviceConnection->Release();

	//fix me
	return 0;
}
int DeviceClient::checkUser(const QString & sUsername,const QString &sPassword)
{
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
		n_IRemotePreview->getLiveStream(nChannel,nStream);
	}
	else if (false==bOpen)
	{
		n_IRemotePreview->pauseStream(false);
	}
	n_IRemotePreview->Release();
	return 0;
}

int DeviceClient::closeAll()
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
	n_IRemotePreview->stopStream();
	n_IRemotePreview->Release();
	return 0;
}

QString DeviceClient::getVendor()
{
	QString sReturn;
	return sReturn;
}
int DeviceClient::getConnectStatus()
{
	if (NULL==m_DeviceConnecton)
	{
		return 1;
	}
	int nRet=m_DeviceConnecton->getCurrentStatus();
	return nRet;
}
int DeviceClient::cbInit()
{
	if (NULL==m_DeviceConnecton)
	{
		return 1;
	}
	IEventRegister *pRegist=NULL;
	m_DeviceConnecton->QueryInterface(IID_IEventRegister,(void**)&pRegist);
	if (NULL==pRegist)
	{
		return 1;
	}
	QMultiMap<QString,DeviceClientInfoItem> ::const_iterator it;
	for (it=m_EventMap.begin();it!=m_EventMap.end();++it)
	{
		QString sKey=it.key();
		DeviceClientInfoItem sValue=it.value();
		pRegist->registerEvent(sKey,sValue.proc,sValue.puser);
	}
	pRegist->Release();
	return 0;
}