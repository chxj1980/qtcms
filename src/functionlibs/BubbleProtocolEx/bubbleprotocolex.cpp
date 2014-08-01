#include "bubbleprotocolex.h"
#include <guid.h>
#include <QtEndian>
BubbleProtocolEx::BubbleProtocolEx():m_nRef(0),
	m_nSleepSwitch(0),
	m_bStop(true),
	m_bBlock(false),
	m_tCurrentConnectStatus(BUBBLE_DISCONNECTED),
	m_tHistoryConnectStatus(BUBBLE_DISCONNECTED),
	m_pTcpSocket(NULL)
{
	m_sEventList<<"LiveStream"           <<"SocketError"<<"StateChangeed"<<"foundFile"
		<<"recFileSearchFinished"<<"RecordStream"  <<"SocketError"  <<"StateChanged"<<"recFileSearchFail"<<"ConnectRefuse";
	connect(this,SIGNAL(sgBackToMainThread(QVariantMap)),this,SLOT(slBackToMainThread(QVariantMap)),Qt::BlockingQueuedConnection);
	connect(&m_tCheckoutBlockTimer,SIGNAL(timeout()),this,SLOT(slCheckoutBlock()));
	m_tCheckoutBlockTimer.start(5000);
}

BubbleProtocolEx::~BubbleProtocolEx()
{
	m_bStop=true;
	int nCount=0;
	while(QThread::isRunning()){
		sleepEx(10);
		nCount++;
		if (nCount>500&&nCount%100==0)
		{
			qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<m_tDeviceInfo.sEseeId<<"terminate this thread had caused more time than 5s,there may be out of control,"<<"please check position at:"<<m_nPosition;
		}
	}
	m_tCheckoutBlockTimer.stop();
}

long __stdcall BubbleProtocolEx::QueryInterface( const IID & iid,void **ppv )
{
	if (IID_IPcomBase==iid)
	{
		*ppv=static_cast<IPcomBase *>(this);
	}else if (IID_IEventRegister==iid)
	{
		*ppv=static_cast<IEventRegister *>(this);
	}else{
		*ppv=NULL;
		return E_NOINTERFACE;
	}
	static_cast<IPcomBase *>(this)->AddRef();
	return S_OK;
}

unsigned long __stdcall BubbleProtocolEx::AddRef()
{
	m_csRef.lock();
	m_nRef ++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long __stdcall BubbleProtocolEx::Release()
{
	int nRet = 0;
	m_csRef.lock();
	m_nRef -- ;
	nRet = m_nRef;
	m_csRef.unlock();
	if (0 == nRet)
	{
		delete this;
	}
	return nRet;
}

QStringList BubbleProtocolEx::eventList()
{
	return m_sEventList;
}

int BubbleProtocolEx::queryEvent( QString eventName,QStringList& eventParams )
{
	if (!m_sEventList.contains(eventName))
	{
		qDebug()<<__FUNCTION__<<__LINE__<<eventName<<"is undefined";
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}else{
		//fix eventParams
		return IEventRegister::OK;
	}
}

int BubbleProtocolEx::registerEvent( QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser )
{
	if (!m_sEventList.contains(eventName))
	{
		qDebug()<<__FUNCTION__<<__LINE__<<eventName<<"is undefined";
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}else{
		tagBubbleProInfo tProInfo;
		tProInfo.proc=proc;
		tProInfo.pUser=pUser;
		m_tEventMap.insert(eventName,tProInfo);
		return IEventRegister::OK;
	}
}

void BubbleProtocolEx::eventProcCall( QString sEvent,QVariantMap tInfo )
{
	if (m_sEventList.contains(sEvent))
	{
		tagBubbleProInfo tProInfo=m_tEventMap.value(sEvent);
		if (NULL!=tProInfo.proc)
		{
			tProInfo.proc(sEvent,tInfo,tProInfo.pUser);
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<sEvent<<"is not register";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<sEvent<<"is  undefined";
	}
}

void BubbleProtocolEx::run()
{
	int nRunStep=BUBBLE_RUN_CONNECT;
	bool bRunStop=false;
	int nHeartBeat=0;
	QVariantMap evMap;
	m_tCurrentConnectStatus=BUBBLE_CONNECTTING;
	evMap.insert("status",m_tCurrentConnectStatus);
	sgBackToMainThread(evMap);
	while(bRunStop==false){
		switch(nRunStep){
		case BUBBLE_RUN_CONNECT:{
			//���ӵ��豸
			if (NULL!=m_pTcpSocket)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"m_pTcpSocket should be null on here,please check";
				delete m_pTcpSocket;
				m_pTcpSocket=NULL;
			}else{
				//keep going  
			}
			m_pTcpSocket=new QTcpSocket;
			m_pTcpSocket->connectToHost(m_tDeviceInfo.tIpAddr.toString(),m_tDeviceInfo.tPorts["media"].toInt());
			//�ȴ�5s���������5sû�з��أ�����Ϊ����ʧ��
			if (m_pTcpSocket->waitForConnected(5000))
			{
				//�׽������ӳɹ�
				//��һ����֤bubble�Ƿ�֧��ʹ��httpЭ���������
				QString sBlock="GET /bubble/live?ch=0&stream=0 HTTP/1.1\r\n\r\n";
				m_pTcpSocket->write(sBlock.toAscii());
				//�ȴ�2s���������2sд���׽���ʧ�ܣ���Ͽ�����
				if (m_pTcpSocket->waitForBytesWritten(2000))
				{
					//�ȴ���֤����,���һֱ��������״̬����û���κ���Ϣ���أ���δ���
					nRunStep=BUBBLE_RUN_DEFAULT;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<m_pTcpSocket->state()<<m_pTcpSocket->error()<<"write block to m_pTcpSocket fail";
					nRunStep=BUBBLE_RUN_DISCONNECT;
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<m_pTcpSocket->state()<<m_pTcpSocket->error()<<"connect to device fail";
				nRunStep=BUBBLE_RUN_DISCONNECT;
			}
								}
								break;
		case BUBBLE_RUN_RECEIVE:{
			//���ܽ�������
			int nReceiveStep=BUBBLE_RECEIVE_HTTP;
			bool bReceiveStop=false;
			m_tBuffer+=m_pTcpSocket->readAll();
			while(bReceiveStop==false){
				switch(nReceiveStep){
				case BUBBLE_RECEIVE_HTTP:{
					//���bubble�Ƿ���ͨ��http����
					//���m_tBuffer �Ƿ����HTTP/1.1 200 ���� HTTP/1.1 404
					//���� m_tBuffer ��СС��12�����
					if (m_tBuffer.contains("HTTP/1.1 200")||m_tBuffer.contains("HTTP/1.1 404"))
					{
						//����Ƿ�1024���ֽ�
						if (m_tBuffer.size()<1024)
						{
							//���Ž���
							nReceiveStep=BUBBLE_RECEIVE_WAITMOREBUFFER;
						}else{
							//����
							if (analyzeBubbleInfo())
							{
								//keep going
								nReceiveStep=BUBBLE_RECEIVE_FRAME;
							}else{
								//����ʧ�ܣ��Ͽ�����
								nReceiveStep=BUBBLE_RECEIVE_END;
							}
						}
					}else{
						nReceiveStep=BUBBLE_RECEIVE_FRAME;
					}
					   }
					   break;
				case BUBBLE_RECEIVE_FRAME:{
					//��������֡,��һ�ν�����buffer ��С��С��11
					//step1������֡ͷ
					//step2:�ж�����֡��С������500k����Ϊ���ݲ��Ϸ����Ͽ����ӣ��ȴ�buffer������������֡�Ĵ�С
					//step3:����ָ���룬���û����Ӧ��ָ���룬��Ϊ���ݲ��Ϸ����Ͽ�����
					int nFrameStep=0;
					bool bFrameStop=false;
					while(bFrameStop==false){
						switch(nFrameStep){
						case 0:{
							//�ж�
							if (m_tBuffer.size()>11)
							{
								if (m_tBuffer.startsWith("\xab")||m_tBuffer.startsWith("\xaa"))
								{
									//step1:find֡ͷ
									tagBubbleInfo *pBubbleInfo=(tagBubbleInfo *)m_tBuffer.data();
									int nBubbleSize=qToBigEndian(pBubbleInfo->uiLength)+sizeof(pBubbleInfo->cHead)+sizeof(pBubbleInfo->uiLength);
									if (nBubbleSize<1024*500)
									{
										//�ж�ָ����
										if (m_tBuffer.startsWith("\xab"))
										{
											//�ط�
											if (m_tRemoteCode.contains(pBubbleInfo->cCmd))
											{
												if (m_tBuffer.size()>=nBubbleSize)
												{
													nFrameStep=1;
												}else{
													nFrameStep=2;
												}
											}else{
												//û�в�ѯ����ص�ָ���룬�ж�Ϊ����ʧ��
												qDebug()<<__FUNCTION__<<__LINE__<<"analysis data fail as it(pBubbleInfo->cCmd) do not match any cmd code";
												nFrameStep=3;
											}	
										}else{
											//Ԥ��
											if (m_tPreviewCode.contains(pBubbleInfo->cCmd))
											{
												if (m_tBuffer.size()>=nBubbleSize)
												{
													nFrameStep=1;
												}else{
													nFrameStep=2;
												}
											}else{
												//û�в�ѯ����ص�ָ���룬�ж�Ϊ����ʧ��
												qDebug()<<__FUNCTION__<<__LINE__<<"analysis data fail as it(pBubbleInfo->cCmd) do not match any cmd code";
												nFrameStep=3;
											}
										}
										pBubbleInfo->cCmd;
									}else{
										//����֡����500k������Ϊ�������ݣ��Ͽ�����
										qDebug()<<__FUNCTION__<<__LINE__<<"analysis data fail as the buffer size is over 500k,we will reConnect the device";
										nFrameStep=3;
									}
								}else{
									m_tBuffer.remove(0,1);
									nFrameStep=0;
								}
							}else{
								nFrameStep=2;
							}
							   }
							   break;
						case 1:{
							//find
							if (m_tBuffer.startsWith("\xab"))
							{
								//�����ط�����
								if (analyzeRemoteInfo())
								{
									nFrameStep=2;
								}else{
									nFrameStep=3;
								}
							}else{
								//����Ԥ������
								if (analyzePreviewInfo())
								{
									nFrameStep=2;
								}else{
									nFrameStep=3;
								}
							}
							   }
							   break;
						case 2:{
							//��Ҫ���Ž�������
							nFrameStep=4;
							nReceiveStep=BUBBLE_RECEIVE_WAITMOREBUFFER;
							   }
							   break;
						case 3:{
							//����ʧ��
							nFrameStep=4;
							nReceiveStep=BUBBLE_RECEIVE_END;
							   }
							   break;
						case 4:{
							//end
							bFrameStop=true;
							   }
							   break;
						}
					}
					   }
					   break;
				case BUBBLE_RECEIVE_WAITMOREBUFFER:{
					//�ȴ����������
					nRunStep=BUBBLE_RUN_DEFAULT;
					bReceiveStop=true;
					   }
					   break;
				case BUBBLE_RECEIVE_END:{
					//ֹͣ��������
					nRunStep=BUBBLE_RUN_DISCONNECT;
					bReceiveStop=true;
										}
										break;
				}
			}
								}
								break;
		case BUBBLE_RUN_CONTROL:{
			//����ָ��
			m_csStepCode.lock();
			int nRunControlStep=m_tStepCode.dequeue();
			m_csStepCode.unlock();
			bool bFlag=false;
			switch(nRunControlStep){
			case BUBBLE_AUTHORITY:{
				//�û���֤
				bFlag=cmdAuthority();
								  }
								  break;
			case BUBBLE_DISCONNECT:{
				//�Ͽ�����
				bFlag=cmdDisConnect();
								   }
								   break;
			case BUBBLE_GETLIVESTREAM:{
				//��ȡԤ������
				if (m_bIsSupportHttp)
				{
					bFlag=cmdGetLiveStreamEx();
				}else{
					bFlag=cmdGetLiveStream();
				}
				
									  }
									  break;
			case BUBBLE_STOPSTREAM:{
				//ֹͣԤ��
				bFlag=cmdStopStream();
								   }
								   break;
			case BUBBLE_PAUSESTREAM:{
				//��ͣԤ��
				bFlag=cmdPauseStream();
									}
									break;
			case BUBBLE_HEARTBEAT:{
				//����ָ��
				bFlag=cmdHeartBeat();
								  }
								  break;
			case BUBBLE_GETPLAYBACKSTREAMBYTIME:{
				//��ȡ�ط�����
				bFlag=cmdGetPlayBackStreamByTime();
												}
												break;
			case BUBBLE_GETPLAYBACKSTREAMBYFILENAME:{
				//��ȡ�ط�����
				bFlag=cmdGetPlayBackStreamByFileName();
													}
													break;
			case BUBBLE_PAUSEPLAYBACKSTREAM:{
				//��ͣ�ط�����
				bFlag=cmdPausePlayBackStream();
											}
											break;
			case BUBBLE_STOPPLAYBACKSTREAM:{
				//ֹͣ�ط�����
				bFlag=cmdStopPlayBackStream();
										   }
										   break;
			case BUBBLE_PTZ:{
				//��̨����
				bFlag=cmdPtz();
							}
							break;
			default:{
				qDebug()<<__FUNCTION__<<__LINE__<<"there is an undefined cmd,please check! it will cause device reConnect";
					}
			}
			if (bFlag)
			{
				nRunStep=BUBBLE_RUN_DISCONNECT;
			}else{
				nRunStep=BUBBLE_RUN_DEFAULT;
			}
			
								}
								break;
		case BUBBLE_RUN_DISCONNECT:{
			//�Ͽ�����
			if (m_tCurrentConnectStatus==BUBBLE_CONNECTTING||m_tCurrentConnectStatus==BUBBLE_CONNECTED)
			{
				m_tCurrentConnectStatus=BUBBLE_DISCONNECTING;
				QVariantMap evMap;
				evMap.insert("status",evMap);
				sgBackToMainThread(evMap);
			}else{
				//do nothing
			}
			//�Ͽ��׽������ӵ��ź�

			//�Ͽ��׽��ֵ���������
			if (NULL!=m_pTcpSocket)
			{
				if (QAbstractSocket::UnconnectedState!=m_pTcpSocket->state())
				{
					m_pTcpSocket->disconnectFromHost();
					//�ȴ�2s������2sû�жϿ����أ������������ʾ��Ϣ
					if (m_pTcpSocket->waitForDisconnected(2000))
					{
						//�ɹ��Ͽ�
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<"try to disconnectFromHost,but it had cost 2s,please check";
					}
				}else{
					//do nothing
				}
				
			}else{
				//do nothing
			}
			nRunStep=BUBBLE_RUN_END;
								   }
								   break;
		case BUBBLE_RUN_DEFAULT:{
			//ȱʡ����
			//����Ƿ���Ҫֹͣ�߳�
			if (!m_bStop&&NULL!=m_pTcpSocket)
			{
				//�������״̬
				if (QAbstractSocket::ConnectedState==m_pTcpSocket->state())
				{
					//ָ�������ڽ�������
					if (m_tStepCode.size()>0)
					{
						nRunStep=BUBBLE_RUN_CONTROL;
						sleepEx(10);
					}else{
						if (m_pTcpSocket->bytesAvailable()>0)
						{
							nRunStep=BUBBLE_RUN_RECEIVE;
						}else{
							sleepEx(10);
							nRunStep=BUBBLE_RUN_DEFAULT;
						}
					}
					//�����ӷ���һ������ָ��
					nHeartBeat++;
					if (nHeartBeat>3000)
					{
						nHeartBeat=0;
						m_csStepCode.lock();
						m_tStepCode.enqueue(BUBBLE_HEARTBEAT);
						m_csStepCode.unlock();
					}else{
						//do nothing
					}
				}else{
					nRunStep=BUBBLE_RUN_DISCONNECT;
					qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<"turn to disconnect as current state::"<<m_pTcpSocket->state()<<m_pTcpSocket->error();
				}
			}else{
				nRunStep=BUBBLE_RUN_DISCONNECT;
				qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<"turn to disconnect as m_bStop::"<<m_bStop;
			}
								}
								break;
		case BUBBLE_RUN_END:{
			//����
			bRunStop=true;
			if (NULL!=m_pTcpSocket)
			{
				m_pTcpSocket->deleteLater();
			}else{
				//do nothing
			}
							}
							break;
		}
	}
	evMap.clear();
	m_tCurrentConnectStatus=BUBBLE_DISCONNECTED;
	evMap.insert("status",m_tCurrentConnectStatus);
	sgBackToMainThread(evMap);
}

int BubbleProtocolEx::setDeviceHost( const QString &sIpAddr )
{
	//0�����óɹ�
	//1������ʧ��
	if (!QThread::isRunning())
	{
		if (m_tDeviceInfo.tIpAddr.setAddress(sIpAddr))
		{
			return 0;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"setDeviceHost fail as the input ip addr is illegal:"<<sIpAddr;
			return 1;
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"setDeviceHost fail as the thread had been running,you should set it before the thread start";
		return 1;
	}
}

int BubbleProtocolEx::setDevicePorts( const QVariantMap &tPorts )
{
	//0�����óɹ�
	//1������ʧ��
	if (!QThread::isRunning())
	{
		if (tPorts["media"].toInt()<=0||tPorts["media"].toInt()>=65535)
		{
			qDebug()<<__FUNCTION__<<__LINE__<<"setDevicePorts fail as the input ports is illegal :"<<tPorts["media"].toInt();
			return 1;
		}else{
			m_tDeviceInfo.tPorts=tPorts;
			return 0;
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"setDevicePorts fail as the thread had been running,you should set it before the thread start";
		return 1;
	}
}

int BubbleProtocolEx::setDeviceId( const QString &sEseeId )
{
	//0�����óɹ�
	//1������ʧ��
	if (!QThread::isRunning())
	{
		m_tDeviceInfo.sEseeId=sEseeId;
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"setDeviceId fail as the thread had been running,you should set it before the thread start";
		return 1;
	}
}

int BubbleProtocolEx::setDeviceAuthorityInfomation( QString sUserName,QString sPassword )
{
	//0�����óɹ�
	//1������ʧ��
	if (!QThread::isRunning())
	{
		m_tDeviceInfo.sPassword=sPassword;
		m_tDeviceInfo.sUserName=sUserName;
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"setDeviceAuthorityInfomation fail as the thread had been running,you should set it before the thread start";
		return 1;
	}
}

int BubbleProtocolEx::connectToDevice()
{
	//0�����ӳɹ�
	//1������ʧ��
	if (!QThread::isRunning())
	{
		m_csStepCode.lock();
		m_tStepCode.clear();
		m_csStepCode.unlock();
		QThread::start();

	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"connectToDevice fail ,if you want to connect to device ,please wait last connect thread terminate";
		return 1;
	}
	return 0;
}

int BubbleProtocolEx::authority()
{
	//	0:У��ɹ�
	//	1:У��ʧ��
	if (m_tCurrentConnectStatus==BUBBLE_CONNECTED)
	{
		m_csStepCode.lock();
		m_tStepCode.enqueue(BUBBLE_AUTHORITY);
		m_csStepCode.unlock();
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"authority fail as the current connect status is not in connected";
		return 1;
	}
	return 0;
}

int BubbleProtocolEx::disconnect()
{
	//0���Ͽ��ɹ�
	//1���Ͽ�ʧ��
	if (QThread::isRunning())
	{
		m_csStepCode.lock();
		m_tStepCode.enqueue(BUBBLE_DISCONNECT);
		m_csStepCode.unlock();
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"it had been in disconnect status,there is no need to call this function again";
	}
	return 0;
}

int BubbleProtocolEx::getCurrentStatus()
{
	return m_tCurrentConnectStatus;
}

QString BubbleProtocolEx::getDeviceHost()
{
	return m_tDeviceInfo.tIpAddr.toString();
}

QString BubbleProtocolEx::getDeviceid()
{
	return m_tDeviceInfo.sEseeId;
}

QVariantMap BubbleProtocolEx::getDevicePorts()
{
	return m_tDeviceInfo.tPorts;
}

int BubbleProtocolEx::getLiveStream( int nChannel,int nStream )
{
	//0����ȡ�ɹ�
	//1����ȡʧ��
	if (m_tCurrentConnectStatus==BUBBLE_CONNECTED)
	{
		m_tDeviceInfo.nPreChannel=nChannel;
		m_tDeviceInfo.nPreStream=nStream;
		//fix me
		return 0;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"getLiveStream fail as the current connect status is not in connected";
		return 1;
	}
}

int BubbleProtocolEx::stopStream()
{
	//0���Ͽ��ɹ�
	//1���Ͽ�ʧ��
	if (QThread::isRunning())
	{
		//fix me
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"the thread had beed terminte,there is no need to call this function";
	}
	return 0;
}

int BubbleProtocolEx::pauseStream( bool bPause )
{
	//0����ͣ�ɹ�
	//1����ͣʧ��
	//fix me
	return 0;
}

int BubbleProtocolEx::getStreamCount()
{
	//fix me
	return 0;
}

int BubbleProtocolEx::getStreamInfo( int nStreamId,QVariantMap &tStreamInfo )
{
	//fix me
	return 0;
}

int BubbleProtocolEx::startSearchRecFile( int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime )
{
	//	0:���óɹ�
	//	1:����ʧ��
	//	2:��������
	//fix me
	return 0;
}

int BubbleProtocolEx::getPlaybackStreamByTime( int nChannel,int nTypes,const QDateTime & startTime,const QDateTime & endTime )
{
	//	0:���óɹ�
	//	1:����ʧ��
	//	2:��������
	//fix me
	return 0;
}

int BubbleProtocolEx::getPlaybackStreamByFileName( int nChannel,const QString &sFileName )
{
	//	0:���óɹ�
	//	1:����ʧ��
	//	2:��������
	//fix me
	return 0;
}

int BubbleProtocolEx::pausePlaybackStream( bool bPause )
{
	//	0:���óɹ�
	//	1:����ʧ��
	//fix me
	return 0;
}

int BubbleProtocolEx::stopPlaybackStream()
{
	//	0:���óɹ�
	//	1:����ʧ��
	//fix me
	return 0;
}

void BubbleProtocolEx::sleepEx( int nTime )
{
	if (m_nSleepSwitch<100)
	{
		msleep(nTime);
		m_nSleepSwitch++;
	}else{
		QEventLoop tEventLoop;
		QTimer::singleShot(2,&tEventLoop,SLOT(quit()));
		tEventLoop.exec();
		m_nSleepSwitch=0;
	}
	return;
}

void BubbleProtocolEx::slCheckoutBlock()
{
	if (m_bBlock)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<m_tDeviceInfo.tIpAddr.toString()<<m_tDeviceInfo.sEseeId<<"block at ::"<<m_nPosition<<"please check!";
	}else{
		//keep going
	}
}

void BubbleProtocolEx::slBackToMainThread( QVariantMap evMap )
{
	if (evMap.contains("status"))
	{
		//״̬�ı䣬�׳�״̬
		if (evMap.value("status").toInt()!=m_tHistoryConnectStatus)
		{
			eventProcCall("StateChangeed",evMap);
			m_tHistoryConnectStatus=(tagBubbleConnectStatusInfo)evMap.value("status").toInt();
		}else{
			//do nothing
		}
	}else{
		//do nothing
	}
}

bool BubbleProtocolEx::analyzeBubbleInfo()
{
	return true;
}

bool BubbleProtocolEx::analyzePreviewInfo()
{
	return true;
}

bool BubbleProtocolEx::analyzeRemoteInfo()
{
	return true;
}

bool BubbleProtocolEx::cmdAuthority()
{
	char cBuffer[100];
	QByteArray tBlock;
	qint64 nLen=0;
	tagBubbleMessageInfo *pMessageInfo=NULL;
	tagBubbleAuthoritySend *pAuthoritySend=NULL;
	memset(cBuffer,0,100);
	tagBubbleInfo *pBubbleInfo=(tagBubbleInfo*)cBuffer;
	pBubbleInfo->cHead=(char)0xaa;

	QDateTime tTime=QDateTime::currentDateTime();
	pBubbleInfo->uiTicket=qToBigEndian((unsigned int)tTime.toMSecsSinceEpoch()*1000);
	pBubbleInfo->cCmd=(char)0x00;

	pMessageInfo=(tagBubbleMessageInfo*)pBubbleInfo->pLoad;
	pMessageInfo->cMessage=(char)0x00;
	memset(pMessageInfo->cReverse,(char)0x00,3);

	pAuthoritySend=(tagBubbleAuthoritySend*)pMessageInfo->cParameters;
	qstrcpy(pAuthoritySend->cUserName,m_tDeviceInfo.sUserName.toUtf8().data());
	qstrcpy(pAuthoritySend->cPassWord,m_tDeviceInfo.sPassword.toUtf8().data());

	return true;
}

bool BubbleProtocolEx::cmdDisConnect()
{
	return true;
}

bool BubbleProtocolEx::cmdGetLiveStream()
{
	return true;
}

bool BubbleProtocolEx::cmdGetLiveStreamEx()
{
	return true;
}

bool BubbleProtocolEx::cmdStopStream()
{
	return true;
}

bool BubbleProtocolEx::cmdPauseStream()
{
	return true;
}

bool BubbleProtocolEx::cmdHeartBeat()
{
	return true;
}

bool BubbleProtocolEx::cmdGetPlayBackStreamByTime()
{
	return true;
}

bool BubbleProtocolEx::cmdGetPlayBackStreamByFileName()
{
	return true;
}

bool BubbleProtocolEx::cmdPausePlayBackStream()
{
	return true;
}

bool BubbleProtocolEx::cmdPtz()
{
	return true;
}

bool BubbleProtocolEx::cmdStopPlayBackStream()
{
	return true;
}
