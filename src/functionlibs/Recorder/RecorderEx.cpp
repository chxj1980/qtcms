#include "RecorderEx.h"
#include <guid.h>
#include "StorageMgrEx.h"
#include "netlib.h"
#pragma comment(lib,"netlib.lib")
unsigned int g_uiStorageMgrCount=0;
QMutex g_tStorageMgrLock;
StorageMgrEx * g_pStorageMgrEx=NULL;

StorageMgrEx * applyStorageMgrEX()
{
	if (g_uiStorageMgrCount==0)
	{
		if (g_pStorageMgrEx!=NULL)
		{
			delete g_pStorageMgrEx;
			g_pStorageMgrEx=NULL;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"g_pStorageMgrEx should be null,please check";
		}
		g_pStorageMgrEx=new StorageMgrEx;
		g_pStorageMgrEx->startMgr();
	}else{
		// do nothing
	}
	g_uiStorageMgrCount++;
	return g_pStorageMgrEx;
}

void releaseStorageMgrEx()
{
	g_uiStorageMgrCount--;
	if (g_uiStorageMgrCount==0)
	{
		if (g_pStorageMgrEx!=NULL)
		{
			g_pStorageMgrEx->stopMgr();
			delete g_pStorageMgrEx;
			g_pStorageMgrEx=NULL;
		}else{
			//do nothing
		}
	}else{
		//do nothing
	}
}

RecorderEx::RecorderEx(void):m_nRef(0),
	m_iPosition(0),
	m_iSleepSwitch(0),
	m_iVideoHeight(0),
	m_iVideoWidth(0),
	m_iFrameCount(0),
	m_iLastTicket(0),
	m_iCheckBlockCount(0),
	m_bStop(true),
	m_bBlock(false),
	m_bFull(false),
	m_bCheckDiskSize(false),
	m_bChecKFileSize(false),
	m_bUpdateDatabase(false)
{
	connect(&m_tCheckBlockTimer,SIGNAL(timeout()),this,SLOT(slCheckBlock()));
	m_tCheckBlockTimer.start(1000);
}


RecorderEx::~RecorderEx(void)
{
}

void RecorderEx::run()
{
	g_tStorageMgrLock.lock();
	StorageMgrEx *pStorageMgrEx=applyStorageMgrEX();
	if (pStorageMgrEx==NULL)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"run fail as applyStorageMgrEX fail";
		g_tStorageMgrLock.unlock();
		return;
	}else{
		//keep going
	}
	g_tStorageMgrLock.unlock();
	int iRecStep=REC_INIT;
	avi_t *pAviFile=NULL;
	bool bRunStop=false;
	QVariantMap tInfo;
	int iSleepTime=0;
	bool bAudioSet=false;
	tInfo.insert("RecordState",true);
	eventProcCall("RecordState",tInfo);
	while(bRunStop==false){
		if (m_tDataQueue.size()<2||iSleepTime>100)
		{
			msleep(10);
			iSleepTime=0;
		}
		iSleepTime++;
		switch(iRecStep){
		case REC_INIT:{
			//���ļ��ĸ��������ʼ��
			m_csDataLock.lock();
			//ͳ��֡����صĲ���
			memset(m_uiFrameCountArray,0,sizeof(m_uiFrameCountArray));
			m_iFrameCount=0;
			m_iLastTicket=0;
			m_iLastPts=0;
			m_iFristPts=0;
			m_csDataLock.unlock();
			//��Ƶ�Ĳ���
			bAudioSet=false;
			if (m_bStop)
			{
				iRecStep=REC_END;
			}else{
				iRecStep=REC_FRIST_I_FRAME;
			}
					  }
					  break;
		case REC_FRIST_I_FRAME:{
			//�ȴ���һ��I֡
			m_csDataLock.lock();
			if (m_tDataQueue.size()>0)
			{
				tagRecorderExNode tNodeTemp=m_tDataQueue.front();
				if (AVENC_IDR==tNodeTemp.uiDataType)
				{
					m_iFristPts=tNodeTemp.uiTicketCount;
					iRecStep=REC_CREATE_PATH;
				}else{
					delete []tNodeTemp.pBuffer;
					tNodeTemp.pBuffer=NULL;
					m_tDataQueue.pop_front();
					iRecStep=REC_FRIST_I_FRAME;
				}
			}else{
				iRecStep=REC_FRIST_I_FRAME;
			}
			m_csDataLock.unlock();
			if (m_bStop)
			{
				iRecStep=REC_END;
			}else{
				//keep going
			}
							   }
							   break;
		case REC_CREATE_PATH:{
			//�����ļ�·��������ռ�,����¼�����ݱ�¼�������������ļ���
			if (createFilePath())
			{
				iRecStep=REC_OPEN_FILE;
			}else{
				iRecStep=REC_END;
				qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as createFilePath fail";
			}
			if (m_bStop)
			{
				iRecStep=REC_END;
			}else{
				//do nothing
			}
							 }
							 break;
		case REC_OPEN_FILE:{
			m_csDataLock.lock();
			if (m_tDataQueue.size()>0)
			{
				pAviFile=AVI_open_output_file(m_tRecorderInfo.sFilePath.toAscii().data());
				if (NULL!=pAviFile)
				{
					iRecStep=REC_SET_VIDEO_PARM;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as AVI_open_output_file fail ";
					iRecStep=REC_END;
				}
			}else{
				//keep going
				qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as it should not be here ,i will exit recorder";
				iRecStep=REC_END;
			}
			m_csDataLock.lock();
						   }
						   break;
		case REC_SET_VIDEO_PARM:{
			// �����ļ�����Ƶ���ĸ������
			m_csDataLock.lock();
			AVI_set_video(pAviFile,m_iVideoWidth,m_iVideoHeight,25,"X264");
			m_csDataLock.unlock();
			iRecStep=REC_WRITE_FRAME;
								}
								break;
		case REC_SET_AUDIO_PARM:{
			//�����ļ�����Ƶ���ĸ������,���ã����ڴ˴�ʹ��
								}
								break;
		case REC_WRITE_FRAME:{
			//д�ļ�
			m_csDataLock.lock();
			if (m_tDataQueue.size()>0)
			{
				iSleepTime--;
				tagRecorderExNode tNodeTemp=m_tDataQueue.front();
				if (AVENC_IDR==tNodeTemp.uiDataType||AVENC_PSLICE==tNodeTemp.uiDataType)
				{
					m_iLastPts=tNodeTemp.uiTicketCount;
					AVI_write_frame(pAviFile,tNodeTemp.pBuffer,tNodeTemp.uiBufferSize,(AVENC_IDR==tNodeTemp.uiDataType));
				}else if(AVENC_AUDIO==tNodeTemp.uiDataType){
					if (!bAudioSet)
					{
						int iAudioFormat=WAVE_FORMAT_ALAW;
						AVI_set_audio(pAviFile,1,tNodeTemp.iSampleRate,tNodeTemp.iSampleWidth,iAudioFormat,64);
						bAudioSet=true;
					}else{
						//do nothing
					}
					AVI_write_audio(pAviFile,tNodeTemp.pBuffer,tNodeTemp.uiBufferSize);
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"this is a undefined frame,please check,keep recorder";
				}
				delete[] tNodeTemp.pBuffer;
				tNodeTemp.pBuffer=NULL;
				m_tDataQueue.pop_front();
			}else{
				//keep going
				if (m_bFull)
				{
					iRecStep=REC_PACK;
				}else{
					//do nothing
				}
			}
			if (m_bStop)
			{
				iRecStep=REC_WAIT_FOR_PACK;
			}else{
				iRecStep=REC_CHECK_AND_UPDATE;
			}
							 }
							 break;
		case REC_CHECK_AND_UPDATE:{
			//���Ӳ�̿ռ�,����ļ���С,�������ݿ�
			int iRet=checkALL();
			if (iRet==0)
			{
				iRecStep=REC_WRITE_FRAME;
			}else if (iRet==1)
			{
				iRecStep=REC_WAIT_FOR_PACK;
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as checkALL fail";
				iRecStep=REC_WAIT_FOR_PACK;
				m_bStop=true;
			}
								  }
								  break;
		case REC_WAIT_FOR_PACK:{
			int iCount=0;
			bool bWait=true;
			while(bWait&&iCount<300&&!m_bStop){//û�еȵ�I֡&&ѭ����������300&&û��ֹͣ¼��
				m_csDataLock.lock();
				if (m_tDataQueue.size()>0)
				{
					tagRecorderExNode tNodeTemp=m_tDataQueue.front();
					if (AVENC_IDR==tNodeTemp.uiDataType)
					{
						bWait=false;
					}else if (AVENC_PSLICE==tNodeTemp.uiDataType)
					{
						AVI_write_frame(pAviFile,tNodeTemp.pBuffer,tNodeTemp.uiBufferSize,(AVENC_IDR==tNodeTemp.uiDataType));
						m_iLastPts=tNodeTemp.uiTicketCount;
						delete[] tNodeTemp.pBuffer;
						tNodeTemp.pBuffer=NULL;
						m_tDataQueue.pop_front();
					}else if (AVENC_AUDIO==tNodeTemp.uiDataType)
					{
						AVI_write_audio(pAviFile,tNodeTemp.pBuffer,tNodeTemp.uiBufferSize);
						delete[] tNodeTemp.pBuffer;
						tNodeTemp.pBuffer=NULL;
						m_tDataQueue.pop_front();
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"there is a undefined frame ,please check";
					}
				}else{
					msleep(10);
				}
				iCount++;
				m_csDataLock.unlock();
			}
			iRecStep=REC_PACK;
							   }
							   break;
		case REC_PACK:{
			//����ļ������ʧ�ܣ���ת��end
			//�������ݿ⣬���ʧ�ܣ���ת��end
			//����Ƿ�����������������ת��end��������ͷ��ʼ
			int iRet=0;//0:��ʾ�ɹ�������¼��1����ʾʧ�ܣ�ֹͣ¼��
			if (packFile(pAviFile))
			{
				//keep going
			}else{
				iRet=1;
				qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as packFile fail";
			}
			if (upDateDataBase())
			{
				//keep going
			}else{
				iRet=1;
				qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as upDateDataBase fail";
			}
			if (iRet!=1)
			{
				if (!m_bStop)
				{
					if (m_bFull)
					{
						if (m_tDataQueue.size()>0)
						{
							m_iLastPts=0;
							m_iFristPts=0;
							bAudioSet=false;
							tagRecorderExNode tNodeTemp=m_tDataQueue.front();
							if (tNodeTemp.uiDataType==AVENC_IDR)
							{
								iRecStep=REC_FRIST_I_FRAME;
							}else{
								clearData();
								iRecStep=REC_INIT;
								m_bFull=false;
							}
						}else{
							iRecStep=REC_INIT;
							m_bFull=false;
						}
					}else{
						//do nothing
						iRecStep=REC_INIT;
					}
				}else{
					iRecStep=REC_END;
					qDebug()<<__FUNCTION__<<__LINE__<<"recorder stop as m_bStop is ture";
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as REC_PACK step fail";
				iRecStep=REC_END;
			}
					  }
					  break;
		case REC_ERROR:{
			//do nothing
					   }
					   break;
		case REC_END:{
			//��ն����е�����
			//ɾ�������������ɵ���Դ
			//ɾ���������ļ�
			//ɾ�����ݿ������õ���Ŀ
			//end
					 }
					 break;
		}
	}
	g_tStorageMgrLock.lock();
	releaseStorageMgrEx();
	g_tStorageMgrLock.unlock();
}

int RecorderEx::Start()
{
	if (!QThread::isRunning())
	{
		m_bStop=false;
		QThread::start();
		return IRecorder::OK;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"the thread had been running,there is no need to call this func again";
		return IRecorder::OK;
	}
}

int RecorderEx::Stop()
{
	if (QThread::isRunning())
	{
		m_bStop=true;
	}else{
		clearData();
	}
	return IRecorder::OK;
}

int RecorderEx::InputFrame( QVariantMap& frameinfo )
{
	int iType=frameinfo["frametype"].toInt();
	int iDataSize=frameinfo["length"].toInt();
	if (!(m_bStop==true||m_bFull==true))
	{
		if (iType==AVENC_IDR||iType==AVENC_AUDIO||iType==AVENC_PSLICE)
		{
			tagRecorderExNode tBufferTemp;
			tBufferTemp.uiDataType=iType;
			tBufferTemp.uiBufferSize=iDataSize;
			tBufferTemp.uiTicketCount=(unsigned int)(frameinfo["pts"].toULongLong()/1000);
			tBufferTemp.iChannel=frameinfo["channel"].toInt();
			tBufferTemp.pBuffer=NULL;
			tBufferTemp.pBuffer=new char[iDataSize];
			if (tBufferTemp.pBuffer!=NULL)
			{
				char *pData=(char*)frameinfo["data"].toUInt();
				memcpy(tBufferTemp.pBuffer,pData,iDataSize);
				if (m_tDataQueue.size()<=DATA_QUEUE_MAX_SIZE)
				{
					if (AVENC_IDR==iType||AVENC_PSLICE==iType)
					{
						//��Ƶ
						if (AVENC_IDR==iType)
						{
							m_iVideoWidth=frameinfo["width"].toInt();
							m_iVideoHeight=frameinfo["height"].toInt();
						}else{
							//do nothing
						}
						m_iFrameCount++;
						if (0==m_iLastTicket)
						{
							m_iLastTicket=tBufferTemp.uiTicketCount;
						}else{
							if (tBufferTemp.uiTicketCount-m_iLastTicket>=1000)
							{
								if (m_iFrameCount<31)
								{
									m_uiFrameCountArray[m_iFrameCount]++;
								}else{
									//do nothing
								}
								m_iFrameCount=0;
								m_iLastTicket=tBufferTemp.uiTicketCount;
							}else{
								//do nothing
							}
						}
					}else{
						//��Ƶ
						tBufferTemp.iSampleRate=frameinfo["samplerate"].toInt();
						tBufferTemp.iSampleWidth=frameinfo["samplewidth"].toInt();
					}
					if (m_tDataQueue.size()>10)
					{
						msleep(m_tDataQueue.size());
						if (m_tDataQueue.size()>20&&m_tDataQueue.size()%10==0)
						{
							qDebug()<<__FUNCTION__<<__LINE__<<"size:"<<m_tDataQueue.size()<<m_tRecorderInfo.sDeviceName<<"record cause sleep!!!";
						}
					}else{
						//keep going
					}
					//�����ݷŽ�����
					m_csDataLock.lock();
					m_tDataQueue.enqueue(tBufferTemp);
					m_csDataLock.unlock();
					return IRecorder::OK;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"InputFrame fail as the m_tDataQueue.size had been full,it must wait for more space";
					return IRecorder::E_SYSTEM_FAILED;
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"InputFrame fail as malloc buffer fail";
				m_bFull=true;
				return IRecorder::E_SYSTEM_FAILED;
			}
		}else{
			return IRecorder::E_PARAMETER_ERROR;
		}
	}else{
		// do nothing
		return IRecorder::OK;
	}
}

int RecorderEx::SetDevInfo( const QString& devname,int nChannelNum )
{
	if (!QThread::isRunning())
	{
		if (!(nChannelNum<0))
		{
			m_tRecorderInfo.sDeviceName=devname;
			m_tRecorderInfo.iChannel=nChannelNum;
			return IRecorder::OK;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"SetDevInfo fail as the input paras are unCorrect";
			return IRecorder::E_PARAMETER_ERROR;
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"SetDevInfo fail as the thread had been running";
		return IRecorder::E_SYSTEM_FAILED;
	}
	return 0;
}

long __stdcall RecorderEx::QueryInterface( const IID & iid,void **ppv )
{
	if (IID_IRecorder == iid)
	{
		*ppv = static_cast<IRecorder *>(this);
	}
	else if (IID_IPcomBase == iid)
	{
		*ppv = static_cast<IPcomBase *>(this);
	}
	else if (IID_IEventRegister==iid)
	{
		*ppv=static_cast<IEventRegister*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IPcomBase *>(this)->AddRef();

	return S_OK;
}

unsigned long __stdcall RecorderEx::AddRef()
{
	m_csRef.lock();
	m_nRef ++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long __stdcall RecorderEx::Release()
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

QStringList RecorderEx::eventList()
{
	return m_tEventList;
}

int RecorderEx::queryEvent( QString eventName,QStringList& eventParams )
{
	if (!m_tEventList.contains(eventName))
	{
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}
	if ("RecordState" == eventName)
	{
		eventParams<<"RecordState";
	}
	return IEventRegister::OK;
}

int RecorderEx::registerEvent( QString eventName,int (__cdecl *proc)(QString,QVariantMap,void *),void *pUser )
{
	if (m_tEventList.contains(eventName))
	{
		tagRecorderExProcInfo proInfo;
		proInfo.proc=proc;
		proInfo.pUser=pUser;
		m_tEventMap.insert(eventName,proInfo);
		return IEventRegister::OK;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"registerEvent fail as m_tEventList do not contains::"<<eventName;
		return IEventRegister::E_EVENT_NOT_SUPPORT;
	}
}

void RecorderEx::clearData()
{
	m_csDataLock.lock();
	while(m_tDataQueue.size()>0){
		tagRecorderExNode tNode=m_tDataQueue.dequeue();
		delete[] tNode.pBuffer;
	}
	m_csDataLock.unlock();
}

void RecorderEx::sleepEx(int iTime)
{
	if (m_iSleepSwitch<100)
	{
		msleep(iTime);
		m_iSleepSwitch++;
	}else{
		m_iSleepSwitch=0;
		QEventLoop tEventLoop;
		QTimer::singleShot(2,&tEventLoop,SLOT(quit));
		tEventLoop.exec();
	}
}

void RecorderEx::eventProcCall( QString sEvent,QVariantMap tInfo )
{
	if (m_tEventList.contains(sEvent))
	{
		tagRecorderExProcInfo tEventProc=m_tEventMap.value(sEvent);
		if (NULL!=tEventProc.proc)
		{
			tEventProc.proc(sEvent,tInfo,tEventProc.pUser);
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"eventProcCall fail as event::"<<sEvent<<"do not register";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"eventProcCall fail as m_tEventList do not contains::"<<sEvent;
	}
}

void RecorderEx::slCheckBlock()
{
	//�����Ӽ��һ��run�����Ƿ�����
	m_iCheckBlockCount++;
	if (m_bBlock&&m_iCheckBlockCount>3)
	{
		m_iCheckBlockCount=0;
		qDebug()<<__FUNCTION__<<__LINE__<<"thread block at position::"<<m_iPosition<<"please check";
	}else{
		//do nothing
		if (m_iCheckBlockCount>100)
		{
			m_iCheckBlockCount=0;
		}else{
			//do nothing
		}
	}
	//һ���Ӽ��һ�δ��̿ռ�
	//һ���Ӽ��һ���ļ���С
	//һ���Ӹ���һ�����ݿ�
	m_bCheckDiskSize=true;
	m_bChecKFileSize=true;
	m_bUpdateDatabase=true;
}

bool RecorderEx::createFilePath()
{
	//�����ļ�·��������ռ�,����¼�����ݱ�¼��������
	if (applyDiskSpace())
	{
		if (createRecordItem())
		{
			if (createSearchItem())
			{
				return true;
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"createFilePath fail as createSearchItem fail";
			}
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"createFilePath fail as createRecordItem fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"createFilePath fail as applyDiskSpace fail";
	}
	return false;
}

int RecorderEx::checkALL()
{
	//0:����¼��1��¼������2����������ֹͣ¼��
	int iFlags=2;
	int iCheckStep=0;
	bool bCheckStop=false;
	while(bCheckStop==false){
		switch(iCheckStep){
		case 0:{
			//����ļ���С
			if (m_bChecKFileSize)
			{
				m_bChecKFileSize=false;
				int iRet=checkFileSize();
				if (iRet==0)
				{
					iCheckStep=4;
				}else if(iRet==1){
					iCheckStep=4;
				}else{
					iCheckStep=5;
					qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as checkFileSize fail";
				}
			}else{
				//
				iCheckStep=1;
			}
			   }
			   break;
		case 1:{
			//���Ӳ�̴�С
			if (m_bCheckDiskSize)
			{
				m_bCheckDiskSize=false;
				int iRec=checkDiskSize();
				if (iRec==0)
				{
					iCheckStep=2;
				}else if(iRec==1){
					iCheckStep=5;
					qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as checkDiskSize fail,there is not disk space for recorder";
				}else{

				}
			}else{
				iCheckStep=2;
			}
			   }
			   break;
		case 2:{
			//�������ݿ�
			if (m_bUpdateDatabase)
			{
				m_bUpdateDatabase=false;
				bool bRet=upDateDataBase();
				if (bRet)
				{
					iCheckStep=3;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"recorder fail as upDateDataBase fail";
					iCheckStep=5;
				}
			}else{
				iCheckStep=3;
			}
			   }
			   break;
		case 3:{
			//����¼��
			iCheckStep=6;
			iFlags=0;
			   }
			   break;
		case 4:{
			//ֹͣ¼��
			iCheckStep=6;
			iFlags=1;
			   }
			   break;
		case 5:{
			//��������
			iCheckStep=6;
			iFlags=2;
			   }
			   break;
		case 6:{
			//end
			bCheckStop=true;
			   }
			   break;
		}
	}
	return iFlags;
}

bool RecorderEx::packFile( avi_t *pAviFile )
{
	return false;
}

bool RecorderEx::upDateDataBase()
{
	//�������ݿ⣬0����ʾ���³ɹ���1����ʾ����ʧ��
	return false;
}

int RecorderEx::checkDiskSize()
{
	//0:��ʾ���̿ռ��㹻������¼��1����ʾ���̿ռ䲻��;2 :��ʾϵͳ����
	StorageMgrEx *pStorageMgrEx=applyStorageMgrEX();
	if (pStorageMgrEx!=NULL)
	{
		tagStorageMgrExInfo tMgrExInfo=pStorageMgrEx->getStorageMgrExInfo();
		QString sDisk=m_tRecorderInfo.sApplyDisk;
		quint64 uiFreeByteAvailable;
		quint64 uiTotalNumberOfbytes;
		quint64 uiTotalNumberOfFreeBytes;
		if (GetDiskFreeSpaceExQ(sDisk.toAscii().data(),&uiFreeByteAvailable,&uiTotalNumberOfbytes,&uiTotalNumberOfFreeBytes))
		{
			if (uiTotalNumberOfFreeBytes<=tMgrExInfo.iDiskReservedSize*1024*1024)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"disk had been full,turn to pack";
				return 1;
			}else{
				return 0;
			}
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"checkFileSize fail as GetDiskFreeSpaceExQ fail";
			return 2;
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"checkFileSize fail as pStorageMgrEx is null";
		return 2;
	}
	return 0;
}

int RecorderEx::checkFileSize()
{
	//0:�ļ���С�㹻�ˣ����Դ����1����ʾ�ļ���С�����㣬����¼��2����ʾ�д���
	return 0;
}

bool RecorderEx::createRecordItem()
{
	m_tRecorderInfo.uiRecorderId=0;
	m_tRecorderInfo.sFilePath.clear();
	StorageMgrEx *pStorageMgrEx=applyStorageMgrEX();
	if (NULL!=pStorageMgrEx)
	{
		if (pStorageMgrEx->createRecordItem(m_tRecorderInfo.sApplyDisk,m_tRecorderInfo.sDeviceName,m_tRecorderInfo.iWindId,m_tRecorderInfo.iChannel,m_tRecorderInfo.iRecordType,m_tRecorderInfo.uiRecorderId,m_tRecorderInfo.sFilePath))
		{
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"createRecordItem fail as pStorageMgrEx->createRecordItem fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"createRecordItem fail as pStorageMgrEx is null";
	}
	return false;
}

bool RecorderEx::createSearchItem()
{
	m_tRecorderInfo.uiSearchId=0;
	StorageMgrEx *pStorageMgrEX=applyStorageMgrEX();
	if (NULL!=pStorageMgrEX)
	{
		QString sDate=QDate::currentDate().toString("yyyy-MM-dd");
		QString sStartTime=QTime::currentTime().toString("hh:mm:ss");
		if (pStorageMgrEX->createSearchItem(m_tRecorderInfo.uiSearchId,m_tRecorderInfo.iWindId,m_tRecorderInfo.iRecordType,sDate,sStartTime,QString("00:00:00")))
		{
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"createSearchItem fail as pStorageMgrEX->createSearchItem fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"createSearchItem fail as pStorageMgrEx is null";
	}
	return false;
}

bool RecorderEx::applyDiskSpace()
{
	m_tRecorderInfo.sApplyDisk.clear();
	StorageMgrEx *pStorageMgrEx=applyStorageMgrEX();
	if (NULL!=pStorageMgrEx)
	{
		if (pStorageMgrEx->applyDiskSpace(m_tRecorderInfo.sApplyDisk))
		{
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"applyDiskSpace fail as pStorageMgrEx->applyDiskSpace fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"applyDiskSpace fail as pStorageMgrEx is null";
	}
	return false;
}

int RecorderEx::SetDevInfoEx( const int &nWindId, const int &nRecordType )
{
	if (!(nWindId<0||nRecordType>3||nRecordType<0))
	{
		m_tRecorderInfo.iWindId=nWindId;
		m_tRecorderInfo.iRecordType=nRecordType;
		return IRecorderEx::OK;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"SetDevInfoEx fail as the input param is unCorrect";
		return IRecorderEx::E_PARAMETER_ERROR;
	}
}

int RecorderEx::FixExceptionalData()
{
	return 0;
}



