#include "recordDatCore.h"


recordDatCore::recordDatCore(void):m_bStop(true),
	m_nPosition(0),
	m_pDataBuffer(NULL),
	m_pDataBuffer1(NULL),
	m_pDataBuffer2(NULL)
{
	for(int i=0;i<WNDMAXSIZE;i++)
	{
		tagRecordDatCoreWndInfo tWndInfo;
		m_tFileInfo.tWndInfo.insert(i,tWndInfo);
	}
	m_pDataBuffer1=(char*)malloc(BUFFERSIZE*1024*1024);
	m_pDataBuffer2=(char*)malloc(BUFFERSIZE*1024*1024);
	connect(&m_tCheckIsBlockTimer,SIGNAL(timeout()),this,SLOT(slcheckBlock()));
	m_tCheckIsBlockTimer.start(5000);
}


recordDatCore::~recordDatCore(void)
{
	m_bStop=true;
	int nCount=0;
	while(QThread::isRunning()){
		msleep(10);
		nCount++;
		if (nCount>10000&&nCount%100==0)
		{
			qDebug()<<__FUNCTION__<<__LINE__<<"stop the thread had cost over 10s";
		}
	}
	if (NULL!=m_pDataBuffer2)
	{
	}
}

void recordDatCore::run()
{
	bool bRunStop=false;
	int nRunStep=recordDat_init;
	int nWriteType=2;//0:����д��1����д�ļ���2��û���ļ���д
	QString sWriteFilePath;
	while(bRunStop==false){
		switch(nRunStep){
		case recordDat_init:{
			//���������ʼ��
			nRunStep=recordDat_filePath;
							}
							break;
		case recordDat_filePath:{
			//����д�ļ���·��
			nWriteType=obtainFilePath(sWriteFilePath);
			if (nWriteType!=OVERWRITE&&nWriteType!=ADDWRITE)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as nWriteType mode is unable";
				nRunStep=recordDat_error;
			}else{
				nRunStep=recordDat_initMemory;
			}
								}
								break;
		case recordDat_initMemory:{
			//��ʼ���ڴ��
			if (nWriteType==OVERWRITE)
			{

			}else{
				//nWriteType==ADDWRITE
			}
			nRunStep=recordDat_default;
								  }
								  break;
		case recordDat_writeMemory:{
			//����֡д���ڴ������ݿ���Ŀ����
								   }
								   break;
		case recordDat_writeDisk:{
			//�ڴ��д������
								 }
								 break;
		case recordDat_default:{
			//����Ƿ���Ҫд�����̣�����Ƿ�������֡����
							   }
							   break;
		case recordDat_error:{
			//����
							 }
							 break;
		case recordDat_end:{
			//����
						   }
						   break;
		}
	}
}

bool recordDatCore::setBufferQueue( int nWnd,BufferQueue &tBufferQueue )
{
	m_tBufferQueueMapLock.lock();
	m_tBufferQueueMap.insert(nWnd,&tBufferQueue);
	m_tBufferQueueMapLock.unlock();
	return true;
}

bool recordDatCore::removeBufferQueue( int nWnd )
{
	m_tBufferQueueMapLock.lock();
	m_tBufferQueueMap.remove(nWnd);
	m_tBufferQueueMapLock.unlock();
	return true;
}

void recordDatCore::registerEvent( QString sEventName,int(__cdecl *proc)(QString,QVariantMap,void*),void *pUser )
{
	if (m_tEventNameList.contains(sEventName))
	{
		tagRecordDatCoreProcInfo tProcInfo;
		tProcInfo.proc=proc;
		tProcInfo.pUser=pUser;
		m_tEventMap.insert(sEventName,tProcInfo);
		return;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"registerEvent fail as m_tEventNameList do not contains :"<<sEventName;
		return;
	}
}

void recordDatCore::eventCallBack( QString sEventName,QVariantMap tInfo )
{
	if (m_tEventNameList.contains(sEventName))
	{
		tagRecordDatCoreProcInfo tProcInfo=m_tEventMap.value(sEventName);
		if (NULL!=tProcInfo.proc)
		{
			tProcInfo.proc(sEventName,tInfo,tProcInfo.pUser);
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<sEventName<<" event is not register";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"not support:"<<sEventName;
	}
}

bool recordDatCore::setRecordType( int nWnd,int nType,bool bFlags )
{
	return false;
}

void recordDatCore::slcheckBlock()
{
	if (m_bIsBlock)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"block at:"<<m_nPosition;
	}else{
		//do nothing
	}
}

int  recordDatCore::obtainFilePath(QString &sWriteFilePath)
{
	//0:����д��1����д�ļ���2��û���ļ���д
	int nStep=obtainFilePath_getDrive;
	bool bStop=false;
	int bFlag=2;
	QString sDiskList;//���ݿ��п��õ��̷���
	QString sUsableDiks;//���õ��̷�
	QString sFilePath;//�ļ�·��
	while(bStop==false){
		switch(nStep){
		case obtainFilePath_getDrive:{
			//��ȡ��¼���̷�
			sUsableDiks=getUsableDisk(sDiskList);
			if (!(sUsableDiks.isEmpty()&&sDiskList.isEmpty()))
			{
				if (sUsableDiks.isEmpty())
				{
					nStep=obtainFilePath_diskFull;
				}else{
					nStep=obtainFilePath_diskUsable;
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"obtainFilePath fail as there is no disk for record";
				nStep=obtainFilePath_fail;
			}
									 }
									 break;
		case obtainFilePath_diskUsable:{
			//��ʣ��ռ�Ŀ�¼����̷�
			sFilePath=getLatestItem(sUsableDiks);
			bFlag=1;
			nStep=obtainFilePath_createFile;
									   }
									   break;
		case obtainFilePath_diskFull:{
			//ÿ���̷����Ѿ�¼��
			QStringList sDiskListInDatabase=sDiskList.split(":");
			QList<QString> tFilePathList;
			foreach(QString sDiskEx,sDiskListInDatabase){
				QString sFilePathItem=m_tOperationDatabase.getLatestItem(sDiskEx);
				if (!sFilePathItem.isEmpty())
				{
					tFilePathList.append(sFilePathItem);
				}else{
					//do nothing
				}
			}
			if (!tFilePathList.isEmpty())
			{
				//���ҳ������̷�������ļ�
				//�Ѳ��ҵ������ļ������ݿ��ڵ������Ϣɾ��
				//fix me
				quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();
				QString sOldestFile;
				for (int i=0;i<tFilePathList.size();i++)
				{
					QString sFilePathItem=tFilePathList.at(i);
					if (QFile::exists(sFilePathItem))
					{
						QFile tFile;
						tFile.setFileName(sFilePathItem);
						if (tFile.open(QIODevice::ReadOnly|QIODevice::Text))
						{
							if (tFile.size()>=sizeof(tagFileHead))
							{
								m_tFileHead.clear();
								m_tFileHead=tFile.read(sizeof(tagFileHead));
								tagFileHead *pFileHead=(tagFileHead*)m_tFileHead.data();
								if (NULL!=pFileHead)
								{
									if (m_tFileHead.contains("JUAN"))
									{
										if (pFileHead->uiStart<uiStartTime)
										{
											uiStartTime=pFileHead->uiStart;
											sOldestFile=sFilePathItem;
										}else{
											//do nothing
										}
									}else{
										//do nothing
									}
								}else{
									//do nothing
								}
								tFile.close();
							}else{
								sOldestFile=sFilePathItem;
								tFile.close();
								break;
							}
						}else{
							//do nothing
						}
					}else{
						//do nothing
					}	
				}
				if (sOldestFile.isEmpty())
				{
					sOldestFile=tFilePathList.at(0);
				}else{
					//do nothing
				}
				sFilePath=sOldestFile;
				m_tOperationDatabase.clearInfoInDatabase(sFilePath);
				bFlag=0;
				nStep=obtainFilePath_createFile;
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no disk space";
				nStep=obtainFilePath_fail;
			}
									 }
									 break;
		case obtainFilePath_createFile:{
			//����ļ������ڣ��򴴽��ļ�
			if (createNewFile(sFilePath))
			{
				nStep=obtainFilePath_success;
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as createNewFile fail";
				nStep=obtainFilePath_fail;
			}
									   }
									   break;
		case obtainFilePath_success:{
			//��ȡ¼���ļ�·���ɹ�
			m_tOperationDatabase.setFileIsLock(sFilePath,true);
			nStep=obtainFilePath_end;
									}
									break;
		case obtainFilePath_fail:{
			//��ȡ¼���ļ�·��ʧ��
			bFlag=2;
			nStep=obtainFilePath_end;
								 }
								 break;
		case obtainFilePath_end:{
			//����
			bStop=true;
								}
		}
	}
	sWriteFilePath=sFilePath;
	return bFlag;
}

QString recordDatCore::getUsableDisk( QString &sDiskLisk )
{
	return "";
}

QString recordDatCore::getLatestItem(QString sDisk)
{
	//��ȡ���µ��ļ�·��
	//step1:�������ݿ������µ��ļ�·��,û�еĻ���ֱ�Ӵ���
	//step2:�������ݿ����µ��ļ�·�����ж��ļ��Ƿ�д����Ϊ������д�����Ļ�������һ���ļ�
	QString sFilePath=m_tOperationDatabase.getLatestItem(sDisk);
	if (sFilePath.isEmpty())
	{
		sFilePath=m_tOperationDatabase.createLatestItem(sDisk);
	}else{
		if (checkFileIsFull(sFilePath))
		{
			sFilePath=m_tOperationDatabase.createLatestItem(sDisk);
		}else{
			//do nothing
		}
	}
	return sFilePath;
}

bool recordDatCore::checkFileIsFull( QString sFilePath )
{
	return false;
}

bool recordDatCore::createNewFile( QString sFilePath )
{
	return false;
}

bool recordDatCore::startRecord()
{
	if (!QThread::isRunning())
	{
		m_bStop=false;
		QThread::start();
	}else{
		//do nothing
	}
	return false;
}



