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
		tWndInfo.uiCurrentRecordType=0;
		tWndInfo.uiHistoryRecordType=0;
		tWndInfo.uiChannelInDatabaseId=0;
		tWndInfo.uiPreFrame=0;
		tWndInfo.uiPreIFrame=0;
		m_tFileInfo.tWndInfo.insert(i,tWndInfo);
	}
	m_pDataBuffer1=(char*)malloc(BUFFERSIZE*1024*1024);
	m_pDataBuffer2=(char*)malloc(BUFFERSIZE*1024*1024);
	if (m_pDataBuffer2==NULL||m_pDataBuffer1==NULL)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"malloc fail";
		abort();
	}else{
		//do nothing
	}
	memset(m_pDataBuffer1,0,BUFFERSIZE*1024*1024);
	memset(m_pDataBuffer2,0,BUFFERSIZE*1024*1024);
	connect(&m_tCheckIsBlockTimer,SIGNAL(timeout()),this,SLOT(slcheckBlock()));
	connect(&m_tWriteDiskTimer,SIGNAL(timeout()),this,SLOT(slsetWriteDiskFlag()));
	m_tCheckIsBlockTimer.start(5000);//5s
	
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
		free(m_pDataBuffer2);
		m_pDataBuffer2=NULL;
	}else{
		//do nothing
	}
	if (NULL!=m_pDataBuffer1)
	{
		free(m_pDataBuffer1);
		m_pDataBuffer1=NULL;
	}else{
		//do nothing
	}
	m_tCheckIsBlockTimer.stop();
	m_pDataBuffer=NULL;
}

void recordDatCore::run()
{
	bool bRunStop=false;
	int nRunStep=recordDat_init;
	int nSleepCount=0;
	int nWriteType=2;//0:����д��1����д�ļ���2��û���ļ���д
	QString sWriteFilePath;
	m_tWriteDiskTimer.start(60000);
	while(bRunStop==false){
		switch(nRunStep){
		case recordDat_init:{
			//���������ʼ��
			m_tToDiskType=recordDatToDiskType_null;
			m_nWriteMemoryChannel=0;
			nRunStep=recordDat_filePath;
			m_pDataBuffer=m_pDataBuffer1;
							}
							break;
		case recordDat_filePath:{
			//����д�ļ���·��
			//step1:�����ļ������߻�ȡ�����е��ļ���
			//step2:���ݿ��������ļ�
			nWriteType=obtainFilePath(sWriteFilePath);
			if (nWriteType!=OVERWRITE&&nWriteType!=ADDWRITE)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as nWriteType mode is unable";
				m_tResetType=recordDatReset_outOfDisk;
				nRunStep=recordDat_reset;
			}else{
				nRunStep=recordDat_initMemory;
			}
								}
								break;
		case recordDat_initMemory:{
			//��ʼ���ڴ��
			if (nWriteType==OVERWRITE)
			{
				//����д�ļ�
				memset(m_pDataBuffer,0,BUFFERSIZE*1024*1024);
			}else{
				//nWriteType==ADDWRITE
				//��ԭ�ļ������ݶ����ڴ��У�������д
				memset(m_pDataBuffer,0,BUFFERSIZE*1024*1024);
				QFile tFile;
				tFile.setFileName(sWriteFilePath);
				if (tFile.open(QIODevice::ReadOnly))
				{
					m_tFileHead.clear();
					m_tFileHead=tFile.read(sizeof(tagFileHead));
					if (m_tFileHead.size()==sizeof(tagFileHead))
					{
						tagFileHead *pFileHead=(tagFileHead*)m_tFileHead.data();
						if (m_tFileHead.contains("JUAN"))
						{
							if (pFileHead->uiIndex<=BUFFERSIZE*1024*1024)
							{
								(QByteArray)m_pDataBuffer=tFile.read(pFileHead->uiIndex);
							}else{
								//do nothing
								qDebug()<<__FUNCTION__<<__LINE__<<"this file is out of length,i will over write it";
							}		
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"this file is undefined type,i will over write it";
							//do nothing
						}
					}else{
						//do nothing
						qDebug()<<__FUNCTION__<<__LINE__<<"this file is undefined type,i will over write it";
					}
					tFile.close();
				}else{
					//�����ļ��򿪴�����������
					qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as open file fail";
					m_tResetType=recordDatReset_fileError;
					nRunStep=recordDat_reset;
					break;
				}
			}
			//д�ļ�ͷ���ڴ���
			tagFileHead *pFileHead=(tagFileHead*)m_pDataBuffer;
			char *pMagic="JUAN";
			if (memcmp(pMagic,pFileHead->ucMagic,4)==0)
			{
				//do nothing

			}else{
				memcpy(pFileHead->ucMagic,pMagic,4);
				pFileHead->uiChannels[0]=0;
				pFileHead->uiChannels[1]=0;
				pFileHead->uiEnd=0;
				pFileHead->uiStart=0;
				pFileHead->uiVersion=0;
				pFileHead->uiIndex=sizeof(tagFileHead);
				for(int i=0;i<sizeof(tagIFrameIndex);i++){
					pFileHead->tIFrameIndex.uiFirstIFrame[i]=0;
				}
				m_tFileInfo.tFristIFrameIndex.clear();
				m_tFileInfo.tHistoryFrameIndex.clear();
				m_tFileInfo.tHistoryIFrameIndex.clear();
			}
			//����buffer������ͨ����λ������
			unsigned int uiCurrentIndex=sizeof(tagFileHead);
			bool bStop=false;
			while(bStop==false){
				if (uiCurrentIndex<pFileHead->uiIndex)
				{
					tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)(pFileHead+uiCurrentIndex);
					int nChannel=pFileFrameHead->tFrameHead.uiChannel;

					if (pFileFrameHead->tFrameHead.uiType==FT_VideoConfig)
					{
						//ͨ���ĵ�һ��I֡
						if (!m_tFileInfo.tFristIFrameIndex.contains(nChannel))
						{
							m_tFileInfo.tFristIFrameIndex.insert(nChannel,uiCurrentIndex);
						}else{
							//do nothing
						}
						//ͨ�������һ��I֡��λ��
						m_tFileInfo.tHistoryIFrameIndex[nChannel]=uiCurrentIndex;
					}else{
						//do nothing
					}
					//ͨ�����һ��֡��λ��
					m_tFileInfo.tHistoryFrameIndex.insert(nChannel,uiCurrentIndex);

					uiCurrentIndex=uiCurrentIndex+sizeof(tagFileFrameHead)+pFileFrameHead->tFrameHead.uiLength-sizeof(pFileFrameHead->tFrameHead.pBuffer);
				}else{
					bStop=true;
				}
			}
			nRunStep=recordDat_default;
								  }
								  break;
		case recordDat_writeMemory:{
			//����֡д���ڴ������ݿ���Ŀ����
			//step 1�����ҳ���Ҫд��ͨ��
			m_tBufferQueueMapLock.lock();
			QMap<int,BufferQueue*>::Iterator tItem=m_tBufferQueueMap.begin();
			int nMinChannel=m_nWriteMemoryChannel;
			int nMinChannelEx=0;
			bool bFlag=false;
			bool bFlagNext=false;
			while(tItem!=m_tBufferQueueMap.end()){
				BufferQueue *pBuffer=tItem.value();
				int nKey=tItem.key();
					bFlag=true;
					if (nKey>m_nWriteMemoryChannel)
					{
						if (bFlagNext==false)
						{
							nMinChannelEx=nKey;
						}else{
							//do nothing
						}
						bFlagNext=true;
						if (nKey<=nMinChannelEx)
						{
							nMinChannelEx=nKey;
						}else{
							//keep going
						}
					}else{
						if (nKey<=nMinChannel)
						{
							nMinChannel=nKey;
						}else{
							//keep going
						}
					}
				tItem++;
			}
			int nWriteToBuffer=0;
			if (bFlag)
			{
				if (!bFlagNext)
				{
					m_nWriteMemoryChannel=nMinChannel;
				}else{
					//do nothing
					m_nWriteMemoryChannel=nMinChannelEx;
				}
				//������д��buffer��
				nWriteToBuffer=writeToBuffer(m_nWriteMemoryChannel,sWriteFilePath);
				if (nWriteToBuffer==0)
				{
					//00��bufferδ��&&ûд��buffer
					nRunStep=recordDat_default;
				}else if (nWriteToBuffer==1)
				{
					//01��bufferδ��&&д��buffer 
					nRunStep=recordDat_default;
					nSleepCount=0;
				}else if (nWriteToBuffer==2)
				{
					//10��buffer����&&δд��buffer��
					m_tToDiskType=recordDatToDiskType_bufferFull;
					nRunStep=recordDat_default;
				}else if (nWriteToBuffer==3)
				{
					//11��buffer����&&д��buffer
					m_tToDiskType=recordDatToDiskType_bufferFull;
					nRunStep=recordDat_default;
					nSleepCount=0;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as nWriteToBuffer is undefined";
					abort();
				}
			}else{
				nRunStep=recordDat_default;
				sleepEx(10);
			}
			m_tBufferQueueMapLock.unlock();
			nSleepCount++;
			if (nSleepCount>64)
			{
				sleepEx(10);
				nSleepCount=0;
			}else{
				//do nothing
			}
								   }
								   break;
		case recordDat_writeDisk:{
			//�ڴ��д������
			//step1:�����ļ�(���ݿ���λ)--��������recordDatToDiskType_bufferFull����
			//step2:����������
			//step3:����¼���
			//step4:֪ͨ�߳�д����
			//step5:�л�bufferָ��--��ת��recordDat_default��buffer���ݿ�������һ��bufferָ�루recordDatToDiskType_outOfTime�����л�bufferָ��--��ת��recordDat_filePath��recordDatToDiskType_bufferFull��
			bool bFlags=false;
			if (m_tToDiskType==recordDatToDiskType_outOfTime||m_tToDiskType==recordDatToDiskType_bufferFull)
			{
				//step2:����������
				if (updateSearchDatabase())
				{
					//step3:����¼���
					if (updateRecordDatabase())
					{
						//step4:֪ͨ�߳�д����
						if (writeTodisk())
						{
							if (m_tToDiskType==recordDatToDiskType_outOfTime)
							{
								//step5:�л�bufferָ��--��ת��recordDat_default��buffer���ݿ�������һ��bufferָ��
								if (m_pDataBuffer!=m_pDataBuffer1)
								{
									memcpy(m_pDataBuffer1,m_pDataBuffer,BUFFERSIZE*1024*1024);
									m_pDataBuffer=m_pDataBuffer1;
								}else{
									memcpy(m_pDataBuffer2,m_pDataBuffer,BUFFERSIZE*1024*1024);
									m_pDataBuffer=m_pDataBuffer2;
								}
								nRunStep=recordDat_default;
							}else{
								//step5:�л�bufferָ��--��ת��recordDat_filePath
								if (m_pDataBuffer!=m_pDataBuffer1)
								{
									m_pDataBuffer=m_pDataBuffer1;
								}else{
									m_pDataBuffer=m_pDataBuffer2;
								}
								//step1:�����ļ�(���ݿ���λ)
								m_tOperationDatabase.setFileIsLock(sWriteFilePath,false);
								nRunStep=recordDat_filePath;
							}
							bFlags=true;
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"writeTodisk fail";
							m_tResetType=rrecordDatReset_writeToDisk;
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"updateRecordDatabase fail";
						m_tResetType=rrecordDatReset_recordDatabase;
					}
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"updateSearchRecord fail";
					m_tResetType=recordDatReset_searchDatabase;
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as m_tToDiskType is undefined";
				abort();
			}
			m_tToDiskType=recordDatToDiskType_null;
			m_nWriteDiskTimeCount=0;
			if (bFlags)
			{
				//do nothing
			}else{
				nRunStep=recordDat_reset;
			}
								 }
								 break;
		case recordDat_default:{
			//����Ƿ���Ҫд�����̣�����Ƿ�������֡����
			//step 1:��¼��ʱд��Ӳ�̵ļ���
			//step 2:����Ƿ���Ҫ���ļ�д������
			//step 3:����Ƿ����µ�buffer��Ҫд���ڴ�
			if (m_bWriteDiskTimeFlags)
			{
				m_nWriteDiskTimeCount++;
				m_bWriteDiskTimeFlags=false;
			}else{
				//do nothing
			}
			if (m_nWriteDiskTimeCount>3)
			{
				m_tToDiskType=recordDatToDiskType_outOfTime;
			}else{
				//do nothing
			}
			if (m_tToDiskType==recordDatToDiskType_null)
			{
				//step frist:����Ƴ�¼���ͨ������д���ݿ�
				//step second:дbuffer
				m_tBufferQueueMapLock.lock();
				for (int i=0;i<m_tDatabaseInfo.tRemoveChannel.size();i++)
				{
					int nChannel=m_tDatabaseInfo.tRemoveChannel.at(i);
					//��д������¼���
					if (m_tDatabaseInfo.tChannelInRecordDatabaseId.contains(nChannel))
					{
						m_tOperationDatabase.updateRecordDatabase(m_tDatabaseInfo.tChannelInRecordDatabaseId.value(nChannel));
						m_tDatabaseInfo.tChannelInRecordDatabaseId.remove(nChannel);
					}else{
						//do nothing
					}
					if (m_tDatabaseInfo.tChannelInSearchDatabaseId.contains(nChannel))
					{
						m_tOperationDatabase.updateSearchDatabase(m_tDatabaseInfo.tChannelInSearchDatabaseId.value(nChannel));
						m_tDatabaseInfo.tChannelInSearchDatabaseId.remove(nChannel);
					}else{
						//do nothing
					}
					m_tFileInfo.tWndInfo[nChannel].uiHistoryRecordType=0;
				}
				m_tBufferQueueMapLock.unlock();
				nRunStep=recordDat_writeMemory;
			}else
			{
				//��bufferд��Ӳ����
				nRunStep=recordDat_writeDisk;
			}
			if (m_bStop)
			{
				nRunStep=recordDat_end;
			}else{
				//do nothing
			}
							   }
							   break;
		case recordDat_reset:{
			//���������
			if (m_tResetType==recordDatReset_fileError)
			{
				//fix me
			}else if (m_tResetType==recordDatReset_outOfDisk)
			{
				//fix me
			}else{
				//fix me
			}
			if (m_bStop)
			{
				nRunStep=recordDat_end;
			}else{
				//do nothing
			}
							 }
							 break;
		case recordDat_error:{
			//���ɻָ��ĳ���
							 }
							 break;
		case recordDat_end:{
			//����
						   }
						   break;
		}
	}
	m_tWriteDiskTimer.stop();
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
	if (!m_tDatabaseInfo.tRemoveChannel.contains(nWnd))
	{
		m_tDatabaseInfo.tRemoveChannel.append(nWnd);
	}else{
		//do nothing
	}
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
	m_tBufferQueueMapLock.lock();
	int nHisRecordType=m_tFileInfo.tWndInfo.value(nWnd).uiHistoryRecordType;
	int nTotal=MANUALRECORD+TIMERECORD+MOTIONRECORD;
	if (nType==MANUALRECORD||nType==MOTIONRECORD||TIMERECORD)
	{
		if (bFlags)
		{
			nHisRecordType=nType|nHisRecordType;
		}else{
			nHisRecordType=(nTotal-nType)&nHisRecordType;
		}
		m_tFileInfo.tWndInfo[nWnd].uiCurrentRecordType=nHisRecordType;
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"setRecordType Not working as nType is undefined";
	}
	m_tBufferQueueMapLock.unlock();
	return true;
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
			//�ж��Ƿ񸲸�¼��
			bool bIsRecover=getIsRecover();
			if (bIsRecover)
			{
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
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no disk space and bIsRecover is false";
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

bool recordDatCore::getIsRecover()
{
	return false;
}

void recordDatCore::sleepEx( quint64 uiTime )
{
	if (m_nSleepSwitch<100)
	{
		msleep(uiTime);
		m_nSleepSwitch++;
	}else{
		QEventLoop eventloop;
		QTimer::singleShot(2, &eventloop, SLOT(quit()));
		eventloop.exec();
		m_nSleepSwitch=0;
	}
	return;
}
int recordDatCore::writeToBuffer( int nChannel,QString sFilePath )
{
	//����ֵ����λ����00(0)��bufferδ��&&ûд��buffer��01(1)��bufferδ��&&д��buffer��10(2)��buffer����&&δд��buffer��11(3)��buffer����&&д��buffer
	int nFlags=0;
	if (m_tBufferQueueMap.contains(nChannel))
	{
		BufferQueue *pBufferQueue=m_tBufferQueueMap.value(nChannel);
		pBufferQueue->enqueueDataLock();
		tagFileHead *pFileHead=(tagFileHead*)m_pDataBuffer;
		int nHistoryType=m_tFileInfo.tWndInfo.value(nChannel).uiHistoryRecordType;
		int nCurrentType=m_tFileInfo.tWndInfo.value(nChannel).uiCurrentRecordType;
		uint nRecType=0;
		quint64 uiTotalLength=BUFFERSIZE*1024*1024;
		quint64 uiFrameSize=0;
		uint nSearchItemId=0;
		uint nRecordItemId=0;
		int nStep=WriteToBuffer_Init;
		bool bStop=false;
		while(bStop==false){
			switch(nStep){
			case WriteToBuffer_Init:{
				if (nHistoryType==nCurrentType&&nCurrentType==0)
				{
					nStep=WriteToBuffer_00;
				}else if (nHistoryType==0&&nCurrentType!=0)
				{
					nStep=WriteToBuffer_01;
				}else if (nHistoryType!=0&&nCurrentType==0)
				{
					nStep=WriteToBuffer_10;
				}else if (nHistoryType==nCurrentType&&nCurrentType!=0)
				{
					nStep=WriteToBuffer_011;
				}else if (nHistoryType!=0&&nCurrentType!=0)
				{
					nStep=WriteToBuffer_111;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"undefined record type change,terminate the thread";
					abort();
				}
									}
									break;
			case WriteToBuffer_00:{
				//historyType==currentType==0,���κβ���
				nStep=WriteToBuffer_end;
				nFlags=0;
								  }
								  break;
			case WriteToBuffer_01:{
				//historyType==0,currentType!=0,��ʼ¼����Ҫ�ȴ�I֡
				//step 1:�ȴ�I֡
				//step 2:�������ݿ���Ŀ
				//step 3:д֡
				bool bFindIFrame=false;
				RecBufferNode *pRecBufferNodeTemp=NULL;
				//step 1:�ȴ�I֡
				while(pBufferQueue->isEmpty()==false&&bFindIFrame==false){
					pRecBufferNodeTemp=pBufferQueue->front();
					if (NULL!=pRecBufferNodeTemp)
					{
						tagFrameHead *pFrameHead=NULL;
						pRecBufferNodeTemp->getDataPointer(&pFrameHead);
						if (NULL!=pFrameHead)
						{
							if (pFrameHead->uiType==IFRAME)
							{
								bFindIFrame=true;
							}else{
								RecBufferNode *pRecBufferNodeTakeOut=NULL;
								pRecBufferNodeTakeOut=pBufferQueue->dequeue();
								pRecBufferNodeTakeOut->release();
								pRecBufferNodeTakeOut=NULL;
							}
							pRecBufferNodeTemp->release();
							pFrameHead=NULL;
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
							abort();
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
						abort();
					}
				}
				//step 2:�������ݿ���Ŀ
				if (bFindIFrame)
				{
					quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();
					quint64 uiEndTime=uiStartTime;
					//step1:����¼�����ݿ���Ŀ
					//step2:�����������ݿ���Ŀ
					if (createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,nCurrentType,nSearchItemId))
					{
						if (createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,nCurrentType,sFilePath,nRecordItemId))
						{
							m_tDatabaseInfo.tChannelInRecordDatabaseId.insert(nChannel,nRecordItemId);
							m_tDatabaseInfo.tChannelInSearchDatabaseId.insert(nChannel,nSearchItemId);
							m_tFileInfo.tWndInfo[nChannel].uiHistoryRecordType=nCurrentType;
							nHistoryType=nCurrentType;
							if (nHistoryType&MOTIONRECORD)
							{
								nRecType=MOTIONRECORD;
							}else if (nHistoryType&MANUALRECORD)
							{
								nRecType=MANUALRECORD;
							}else if (nHistoryType&TIMERECORD)
							{
								nRecType=TIMERECORD;
							}else{
								//do nothing
							}
							nStep=WriteToBuffer_Write;
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"createRecordDatabaseItem fail,there must exist a error,i will terminate the thread";
							abort();
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"createSearchDatabaseItem fail,there must exist a error,i will terminate the thread";
						abort();
					}
				}else{
					nFlags=0;
					nStep=WriteToBuffer_end;
				}
								  }
								  break;
			case WriteToBuffer_10:{
				//historyType!=0,currentType==0,ֹͣ¼��
				//step 1:�������ݿ�
								  }
								  break;
			case WriteToBuffer_011:{
				//historyType==currentType!=0,����û��ת�䣬����¼��
				//step 1:д֡
								   }
								   break;
			case WriteToBuffer_111:{
				//historyType!=currentType!=0,����ת��������¼��
				//step1���������ݿ�
				//step 2:�������ݿ���Ŀ
								   }
								   break;
			case WriteToBuffer_Write:{
				//дһ֡����
				bool bStopWrite=false;
				RecBufferNode *pRecBufferNodeTemp=NULL;
				while(bStopWrite==false&&pBufferQueue->isEmpty()==false){
					nCurrentType=m_tFileInfo.tWndInfo.value(nChannel).uiCurrentRecordType;
					if (nCurrentType==nHistoryType)
					{
						quint64 uiUnusedLength=uiTotalLength-pFileHead->uiIndex;
						pRecBufferNodeTemp=pBufferQueue->front();
						tagFrameHead *pFrameHead=NULL;
						pRecBufferNodeTemp->getDataPointer(&pFrameHead);
						if (NULL!=pFrameHead)
						{
							//step1:�ж�buffer�ĳ����Ƿ񻹹�
							if (pFrameHead->uiType==IFRAME)
							{
								//��������֡
								uiFrameSize=sizeof(tagFileFrameHead)+pFrameHead->uiLength-sizeof(pFrameHead->pBuffer)+sizeof(tagFileFrameHead)+sizeof(tagVideoConfigFrame)-sizeof(pFrameHead->pBuffer);
							}else if (pFrameHead->uiType==PFRAME)
							{
								uiFrameSize=sizeof(tagFileFrameHead)+pFrameHead->uiLength-sizeof(pFrameHead->pBuffer);
							}else if (pFrameHead->uiType==AFRMAE)
							{
								//��������֡
								uiFrameSize=sizeof(tagFileFrameHead)+pFrameHead->uiLength-sizeof(pFrameHead->pBuffer)+sizeof(tagFileFrameHead)+sizeof(tagAudioConfigFrame)-sizeof(pFrameHead->pBuffer);
							}else{
								qDebug()<<__FUNCTION__<<__LINE__<<"there is a undefined frame type,i will terminate the thread";
								abort();
							}
							//step 2;д֡
							if (uiUnusedLength<uiFrameSize)
							{
								//step1:д������֡			
								if (pFrameHead->uiType==FT_IFrame)
								{
									tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)(pFileHead+pFileHead->uiIndex);
									tagFrameHead *pFrameHeadBuffer=(tagFrameHead*)(pFileFrameHead);
									pFrameHeadBuffer->uiChannel=pFrameHead->uiChannel;
									pFrameHeadBuffer->uiExtension=pFrameHead->uiExtension;
									pFrameHeadBuffer->uiGentime=pFrameHead->uiGentime;
									pFrameHeadBuffer->uiLength=sizeof(tagVideoConfigFrame);
									pFrameHeadBuffer->uiPts=pFrameHead->uiPts;
									pFrameHeadBuffer->uiRecType=nRecType;
									pFrameHeadBuffer->uiSessionId=nRecordItemId;
									pFrameHeadBuffer->uiType=FT_VideoConfig;

									tagVideoConfigFrame *pVideoConfigFrame=(tagVideoConfigFrame*)(pFileFrameHead+sizeof(tagFileFrameHead)+sizeof(tagVideoConfigFrame)-sizeof(pFrameHeadBuffer->pBuffer));
									tagVideoConfigFrame *pVideoConfigFrameTemp=(tagVideoConfigFrame*)(pFrameHead+sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(char*));
									pVideoConfigFrame->uiHeight=pVideoConfigFrameTemp->uiHeight;
									pVideoConfigFrame->uiWidth=pVideoConfigFrameTemp->uiWidth;
									memcpy(pVideoConfigFrame->ucReversed,pVideoConfigFrameTemp->ucReversed,4);
									memcpy(pVideoConfigFrame->ucVideoDec,pVideoConfigFrameTemp->ucVideoDec,4);
									//tFristIFrameIndex
									if (!m_tFileInfo.tFristIFrameIndex.contains(pFrameHead->uiChannel))
									{
										m_tFileInfo.tFristIFrameIndex.insert(pFrameHead->uiChannel,pFileHead->uiIndex);
										pFileHead->tIFrameIndex.uiFirstIFrame[pFrameHead->uiChannel]=pFileHead->uiIndex;
									}else{
										//do nothing
									}
									//tHistoryIFrameIndex
									//set uiPreIFrame
									//set uiNextIFrame
									if (!m_tFileInfo.tHistoryIFrameIndex.contains(nChannel))
									{
										m_tFileInfo.tHistoryIFrameIndex.insert(nChannel,pFileHead->uiIndex);
										pFileFrameHead->tPerFrameIndex.uiPreIFrame=0;
									}else{
										pFileFrameHead->tPerFrameIndex.uiPreIFrame=m_tFileInfo.tHistoryIFrameIndex.value(nChannel);
										m_tFileInfo.tHistoryIFrameIndex.insert(nChannel,pFileHead->uiIndex);
										tagFileFrameHead *pLastFrameHead=(tagFileFrameHead*)(pFileHead+pFileFrameHead->tPerFrameIndex.uiPreIFrame);
										pLastFrameHead->tPerFrameIndex.uiNextIFrame=pFileHead->uiIndex;
									}
									//tHistoryFrameIndex
									//set uiPreFrame
									//set uiNextFrame
									if (!m_tFileInfo.tHistoryFrameIndex.contains(nChannel))
									{
										m_tFileInfo.tHistoryFrameIndex.insert(nChannel,pFileHead->uiIndex);
										pFileFrameHead->tPerFrameIndex.uiPreFrame=0;
									}else{
										pFileFrameHead->tPerFrameIndex.uiPreFrame=m_tFileInfo.tHistoryFrameIndex.value(nChannel);
										m_tFileInfo.tHistoryFrameIndex.insert(nChannel,pFileHead->uiIndex);
										tagFileFrameHead *pLastFrameHead=(tagFileFrameHead*)(pFileHead+pFileFrameHead->tPerFrameIndex.uiPreFrame);
										pLastFrameHead->tPerFrameIndex.uiNextFrame=pFileHead->uiIndex;
									}

									pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagFileFrameHead)+sizeof(tagVideoConfigFrame)-sizeof(pFileFrameHead->tFrameHead.pBuffer);
								}else if (pFrameHead->uiType==FT_Audio)
								{

								}else if (pFrameHead->uiType==FT_PFrame)
								{
									//do nothing
								}else{
									qDebug()<<__FUNCTION__<<__LINE__<<"there is a undefined frame type,i will terminate the thread";
									abort();
								}
								//step2:д��֡

								//step 2:д��֡����
								tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)(pFileHead+pFileHead->uiIndex);
								tagFrameHead *pFrameHeadBuffer=(tagFrameHead*)(pFileFrameHead);
								pFrameHeadBuffer->uiChannel=pFrameHead->uiChannel;
								pFrameHeadBuffer->uiExtension=pFrameHead->uiExtension;
								pFrameHeadBuffer->uiGentime=pFrameHead->uiGentime;
								pFrameHeadBuffer->uiLength=pFrameHead->uiLength;
								pFrameHeadBuffer->uiPts=pFrameHead->uiPts;
								pFrameHeadBuffer->uiRecType=nRecType;
								pFrameHeadBuffer->uiSessionId=nRecordItemId;
								pFrameHeadBuffer->uiType=pFrameHead->uiType;
								memcpy(pFrameHeadBuffer->pBuffer,pFrameHead->pBuffer,pFrameHead->uiLength);

								//set uiPreIFrame
								//set uiNextIFrame
								if (m_tFileInfo.tHistoryIFrameIndex.contains(nChannel))
								{
									pFileFrameHead->tPerFrameIndex.uiPreIFrame=m_tFileInfo.tHistoryIFrameIndex.value(nChannel);
								}else{
									pFileFrameHead->tPerFrameIndex.uiPreIFrame=0;
								}
								pFileFrameHead->tPerFrameIndex.uiNextIFrame=0;

								//set uiPreFrame
								//set uiNextFrame
								if (m_tFileInfo.tHistoryFrameIndex.contains(nChannel))
								{
									pFileFrameHead->tPerFrameIndex.uiPreFrame=m_tFileInfo.tHistoryFrameIndex.value(nChannel);
									tagFileFrameHead *pLastFileFrameHead=(tagFileFrameHead*)(pFileHead+pFileFrameHead->tPerFrameIndex.uiPreFrame);
									pLastFileFrameHead->tPerFrameIndex.uiNextFrame=pFileHead->uiIndex;
								}else{
									pFileFrameHead->tPerFrameIndex.uiPreFrame=0;
								}
								pFileFrameHead->tPerFrameIndex.uiNextFrame=0;
								m_tFileInfo.tHistoryFrameIndex.insert(nChannel,pFileHead->uiIndex);

								pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagFileFrameHead)+pFileFrameHead->tFrameHead.uiLength-sizeof(pFrameHead->pBuffer);
								nFlags=nFlags|1;

								//ɾ���ڵ�����
								RecBufferNode *pRecBufferNodeToken=pBufferQueue->dequeue();
								pRecBufferNodeToken->release();
								pRecBufferNodeToken=NULL;
							}else{
								nStep=WriteToBuffer_end;
								bStopWrite=true;
								nFlags=nFlags|2;
							}
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
							abort();
						}
						pRecBufferNodeTemp->release();
						pRecBufferNodeTemp=NULL;
					}else{
						nStep=WriteToBuffer_end;
						bStopWrite=true;
					}
				}
									 }
									 break;
			case WriteToBuffer_end:{

								   }
			}
		}
		return nFlags;
	}else{
		return nFlags;
	}
}
int recordDatCore::writeToBufferEx( int nChannel ,QString sFilePath)
{
	//����ֵ����λ����00(0)��bufferδ��&&ûд��buffer��01(1)��bufferδ��&&д��buffer��10(2)��buffer����&&δд��buffer��11(3)��buffer����&&д��buffer
	int nHistoryType=m_tFileInfo.tWndInfo.value(nChannel).uiHistoryRecordType;
	int nCurrentType=m_tFileInfo.tWndInfo.value(nChannel).uiCurrentRecordType;
	uint nRecType=0;
	if (m_tBufferQueueMap.contains(nChannel))
	{
		BufferQueue *pBufferQueue=m_tBufferQueueMap.value(nChannel);
		pBufferQueue->enqueueDataLock();
		tagFileHead *pFileHead=(tagFileHead*)m_pDataBuffer;
		quint64 uiTotalLength=BUFFERSIZE*1024*1024;
		quint64 uiFrameSize=0;
		if (nHistoryType==nCurrentType&&nHistoryType==0)
		{
			//historyType==currentType==0,���κβ���
			pBufferQueue->enqueueDataUnLock();
			return 0;
		}else if (nHistoryType==0&&nCurrentType!=0)
		{
			//historyType==0,currentType!=0,��ʼ¼����Ҫ�ȴ�I֡
			//����ȵ�I֡�������״̬ת��������¼�����ݿ���Ŀ���������ݿ���Ŀ
			int nStep=0;
			bool bStop=false;
			int nFlags=0;
			RecBufferNode *pRecBufferNodeTemp=NULL;
			while(!pBufferQueue->isEmpty()&&bStop==false){
				switch(nStep){
				case 0:{
					//ȥ����һ��I֡ǰ��P֡
					pRecBufferNodeTemp=pBufferQueue->front();
					if (pRecBufferNodeTemp!=NULL)
					{
						tagFrameHead *pFrameHead=NULL;
						pRecBufferNodeTemp->getDataPointer(&pFrameHead);
						if (pFrameHead!=NULL)
						{
							if (pFrameHead->uiType==IFRAME)
							{
								nStep=1;
							}else{
								RecBufferNode *pRecBufferNodeTakeOut=NULL;
								pRecBufferNodeTakeOut=pBufferQueue->dequeue();
								pRecBufferNodeTakeOut->release();
								pRecBufferNodeTakeOut=NULL;
								nStep=0;
							}
							pRecBufferNodeTemp->release();
							pFrameHead=NULL;
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
							abort();
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
						abort();
					}
					   }
					   break;
				case 1:{
					//�ҵ���һ��I֡
					//step1:����¼�����ݿ���Ŀ
					//step2:�����������ݿ���Ŀ
					uint nSearchItemId=0;
					uint nRecordItemId=0;
					quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();
					quint64 uiEndTime=uiStartTime;
					if (createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,nCurrentType,nSearchItemId))
					{
						if (createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,nCurrentType,sFilePath,nRecordItemId))
						{
							m_tDatabaseInfo.tChannelInRecordDatabaseId.insert(nChannel,nRecordItemId);
							m_tDatabaseInfo.tChannelInSearchDatabaseId.insert(nChannel,nSearchItemId);
							m_tFileInfo.tWndInfo[nChannel].uiHistoryRecordType=nCurrentType;
							nHistoryType=nCurrentType;
							if (nHistoryType&MOTIONRECORD)
							{
								nRecType=MOTIONRECORD;
							}else if (nHistoryType&MANUALRECORD)
							{
								nRecType=MANUALRECORD;
							}else if (nHistoryType&TIMERECORD)
							{
								nRecType=TIMERECORD;
							}else{
								//do nothing
							}
							nStep=2;
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
							abort();
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
						abort();
					}
					   }
					   break;
				case 2:{
					//д��֡
					nCurrentType=m_tFileInfo.tWndInfo.value(nChannel).uiCurrentRecordType;
					if (nCurrentType==nHistoryType)
					{
						//�ж�bubbfer�����Ƿ񻹹�д��һ֡
						quint64 uiUnusedLength=uiTotalLength-pFileHead->uiIndex;
						pRecBufferNodeTemp=NULL;
						pRecBufferNodeTemp=pBufferQueue->front();
						tagFrameHead *pFrameHead=NULL;
						pRecBufferNodeTemp->getDataPointer(&pFrameHead);
						if (pFrameHead!=NULL)
						{
							if (pFrameHead->uiType==IFRAME)
							{
								//I֡ǰ��Ҫ����һ������֡
								uiFrameSize=sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(pFrameHead->pBuffer)+sizeof(tagVideoConfigFrame)+sizeof(tagPerFrameIndex);
							}else if (pFrameHead->uiType==PFRAME)
							{
								uiFrameSize=sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(pFrameHead->pBuffer)+sizeof(tagPerFrameIndex);
							}else{
								//��Ƶ֡ǰ��Ҫ����һ������֡
								uiFrameSize=sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(pFrameHead->pBuffer)+sizeof(tagAudioConfigFrame)+sizeof(tagPerFrameIndex);
							}
							
							if (uiFrameSize<uiUnusedLength)
							{
								//buffer�ĳ��Ȼ��㹻
								pRecBufferNodeTemp->release();
								pRecBufferNodeTemp=NULL;
								pRecBufferNodeTemp=pBufferQueue->dequeue();
								pFrameHead=NULL;
								pRecBufferNodeTemp->getDataPointer(&pFrameHead);
								if (pFrameHead->uiType==IFRAME)
								{
									//step1:д������֡
									tagVideoConfigFrame *pVideoConfigFrame=(tagVideoConfigFrame*)(pFileHead+pFileHead->uiIndex);
									tagVideoConfigFrame *pVideoConfigFramTemp=(tagVideoConfigFrame*)(pFrameHead+sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(char*));
									pVideoConfigFrame->uiHeight=pVideoConfigFramTemp->uiHeight;
									pVideoConfigFrame->uiWidth=pVideoConfigFramTemp->uiWidth;

									//ƫ���ļ����д��λ�õ�����
									pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagVideoConfigFrame);
								}else if (pFrameHead->uiType==AFRMAE)
								{
									//step1:д������֡
									tagAudioConfigFrame *pAudioConfigFrame=(tagAudioConfigFrame*)(pFileHead+pFileHead->uiIndex);
									tagAudioConfigFrame *pAudioConfigFrameTemp=(tagAudioConfigFrame*)(pFrameHead+sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(char*));
									pAudioConfigFrame->uiChannels=pAudioConfigFrameTemp->uiChannels;
									pAudioConfigFrame->uiSamplebit=pAudioConfigFrameTemp->uiSamplebit;
									pAudioConfigFrame->uiSamplerate=pAudioConfigFrameTemp->uiSamplerate;

									//ƫ���ļ����д��λ�õ�����
									pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagAudioConfigFrame);
								}else {
									// do nothing
								}
								if (pFrameHead->uiType==IFRAME||pFrameHead->uiType==PFRAME||pFrameHead->uiType==AFRMAE)
								{				
									//step 2:д��֡����
									tagFrameHead *pFrameHeadBuffer=(tagFrameHead*)(pFileHead+pFileHead->uiIndex);
									pFrameHeadBuffer->uiChannel=pFrameHead->uiChannel;
									pFrameHeadBuffer->uiGentime=pFrameHead->uiGentime;
									pFrameHeadBuffer->uiLength=pFrameHead->uiLength;
									pFrameHeadBuffer->uiPts=pFrameHead->uiPts;
									pFrameHeadBuffer->uiRecType=nRecType;
									pFrameHeadBuffer->uiSessionId=m_tDatabaseInfo.tChannelInRecordDatabaseId.value(nChannel);
									pFrameHeadBuffer->uiType=pFrameHead->uiType;
									memcpy(pFrameHeadBuffer->pBuffer,pFrameHead->pBuffer,pFrameHead->uiLength);

									//ƫ���ļ����д��λ�õ�����
									pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(char*);

									//step 3:д��֮֡���λ��������ϵ
									
								}else{
									qDebug()<<__FUNCTION__<<__LINE__<<"undefined frame type ,i will terminate the thread";
									abort();
								}
								nStep=2;
								nFlags=nFlags|1;
								pRecBufferNodeTemp->release();
								pRecBufferNodeTemp=NULL;
							}else{
								//buffer���Ȳ���
								pRecBufferNodeTemp->release();
								pRecBufferNodeTemp=NULL;
								nStep=3;
								nFlags=nFlags|2;
							}
							
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
							abort();
						}
					}else{
						nStep=3;
					}
					   }
					   break;
				case 3:{
					//����
					   }
					   break;
				}
			}
		}else if (nHistoryType!=0&&nCurrentType==0)
		{
			//historyType!=0,currentType==0,ֹͣ¼��
			//step1:��д¼�����ݿ⣬��д�������ݿ�
		}else if (nHistoryType==nCurrentType&&nCurrentType!=0)
		{
			//historyType==currentType!=0,����û��ת�䣬����¼��
		}else if (nHistoryType!=nCurrentType&&nCurrentType!=0&&nCurrentType!=0)
		{
			//historyType!=currentType!=0,����ת��������¼��
			//step1:��д¼�����ݿ⣬�����µ�һ��¼���¼
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as tagRecordDatTurnType is undefined";
			abort();
		}
	}else{
		return 0;
	}
	return 0;
}

void recordDatCore::slsetWriteDiskFlag()
{
	m_bWriteDiskTimeFlags=true;
}

bool recordDatCore::updateSearchDatabase()
{
	return false;
}

bool recordDatCore::updateRecordDatabase()
{
	return false;
}

bool recordDatCore::writeTodisk()
{
	return false;
}

bool recordDatCore::createSearchDatabaseItem( int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,uint &uiItemId )
{
	return m_tOperationDatabase.createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,uiType, uiItemId);
}

bool recordDatCore::createRecordDatabaseItem( int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,uint &uiItemId )
{
	return m_tOperationDatabase.createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,uiType,sFileName, uiItemId);
}





