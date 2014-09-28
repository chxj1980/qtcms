#include "recordDatCore.h"


recordDatCore::recordDatCore(void):m_bStop(true),
	m_bReloadSystemDatabase(false),
	m_bIsBlock(false),
	m_bWriteDiskTimeFlags(false),
	m_nPosition(0),
	m_nSleepSwitch(0),
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
	m_tWriteDiskTimer.start(60000);
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
	m_tWriteToDisk.stopWriteToDisk();
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
	m_tWriteDiskTimer.stop();
	m_tCheckIsBlockTimer.stop();
	m_pDataBuffer=NULL;
}

void recordDatCore::run()
{
	bool bRunStop=false;
	int nRunStep=recordDat_init;
	int nSleepCount=0;
	QString sHisRecordDiks;
	int nWriteType=2;//0:����д��1����д�ļ���2��û���ļ���д
	QString sWriteFilePath;
	
	while(bRunStop==false){
		switch(nRunStep){
		case recordDat_init:{
			//���������ʼ��
			m_tToDiskType=recordDatToDiskType_null;
			m_nWriteMemoryChannel=0;
			m_nWriteDiskTimeCount=0;
			m_bIsBlock=true;
			m_nPosition=__LINE__;
			m_tOperationDatabase.startOperationDatabase();
			m_tOperationDatabase.reloadSystemDatabase();
			m_tRecordDatabaseMaxId.clear();
			m_bIsBlock=false;
			nRunStep=recordDat_filePath;
			m_pDataBuffer=m_pDataBuffer1;
							}
							break;
		case recordDat_filePath:{
			//����д�ļ���·��
			//step1:�����ļ������߻�ȡ�����е��ļ���
			//step2:���ݿ��������ļ�
			m_bIsBlock=true;
			m_nPosition=__LINE__;
 			nWriteType=obtainFilePath(sWriteFilePath);
			m_bIsBlock=false;
			m_tFileInfo.sFilePath=sWriteFilePath;
			qDebug()<<__FUNCTION__<<__LINE__<<"file path:"<<sWriteFilePath;
			if (nWriteType!=OVERWRITE&&nWriteType!=ADDWRITE)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as nWriteType mode is unable";
				m_tResetType=recordDatReset_outOfDisk;
				nRunStep=recordDat_reset;
			}else{
				QString sDisk=sWriteFilePath.left(1);
				if (!m_tRecordDatabaseMaxId.contains(sDisk))
				{
					quint64 uiMaxRecord=0;
					quint64 uiMaxSearch=0;
					if (m_tOperationDatabase.getMaxDatabaseId(uiMaxRecord,uiMaxSearch,sWriteFilePath))
					{
						tagRecordDatabaseMaxID tInfo;
						tInfo.uiMaxRecordId=uiMaxRecord;
						tInfo.uiMaxSearchId=uiMaxSearch;
						m_tRecordDatabaseMaxId.insert(sDisk,tInfo);
					}else{
						//do nothing
						abort();
					}
				}else{
					//do nothing
				}
				if (sHisRecordDiks!=sDisk)
				{
					if (m_tDatabaseInfo.tChannelInSearchDatabaseId.size()>0)
					{
						updateSearchDatabase();
						m_tDatabaseInfo.tChannelInSearchDatabaseId.clear();
					}else{
						//do nothing
					}
				}else{
					//do nothing
				}
				sHisRecordDiks=sDisk;
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
				if (tFile.open(QIODevice::ReadWrite))
				{
					m_tFileHead.clear();
					m_tFileHead=tFile.readAll();
					if (m_tFileHead.size()>=sizeof(tagFileHead))
					{
						tagFileHead *pFileHead=(tagFileHead*)m_tFileHead.data();
						if (m_tFileHead.contains("JUAN"))
						{
							if (pFileHead->uiIndex<=BUFFERSIZE*1024*1024)
							{
								if (tFile.reset())
								{
								}else{
									qDebug()<<__FUNCTION__<<__LINE__<<"tFile.reset() fail";
									abort();
								}
								if (m_tFileHead.size()>pFileHead->uiIndex)
								{
									memcpy(m_pDataBuffer,m_tFileHead.data(),pFileHead->uiIndex);
								}else{
									memcpy(m_pDataBuffer,m_tFileHead.data(),m_tFileHead.size());
								}
								
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
			m_tFileInfo.tFristIFrameIndex.clear();
			m_tFileInfo.tHistoryFrameIndex.clear();
			m_tFileInfo.tHistoryIFrameIndex.clear();
			unsigned int uiCurrentIndex=sizeof(tagFileHead);
			bool bStop=false;
			m_bIsBlock=true;
			m_nPosition=__LINE__;
			while(bStop==false){
				if (uiCurrentIndex<pFileHead->uiIndex)
				{
					tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiCurrentIndex);
					int nChannel=pFileFrameHead->tFrameHead.uiChannel;
					//test file first: 
					if (pFileFrameHead->tFrameHead.uiChannel>=0&&pFileFrameHead->tFrameHead.uiChannel<64)
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pFileFrameHead->tFrameHead.uiChannel is out of range"<<pFileFrameHead->tFrameHead.uiChannel;
						abort();
					}
					if (pFileFrameHead->tFrameHead.uiRecType==MOTIONRECORD||pFileFrameHead->tFrameHead.uiRecType==TIMERECORD||pFileFrameHead->tFrameHead.uiRecType==MANUALRECORD)
					{
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pFileFrameHead->tFrameHead.uiRecType is undefined";
						abort();
					}
					if (pFileFrameHead->tFrameHead.uiSessionId>0)
					{
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pFileFrameHead->tFrameHead.uiSessionId is out of range"<<pFileFrameHead->tFrameHead.uiSessionId;
						abort();
					}
					if (pFileFrameHead->tFrameHead.uiType==FT_VideoConfig||pFileFrameHead->tFrameHead.uiType==FT_AudioConfig||pFileFrameHead->tFrameHead.uiType==FT_PFrame||pFileFrameHead->tFrameHead.uiType==FT_Audio||pFileFrameHead->tFrameHead.uiType==FT_IFrame)
					{
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pFileFrameHead->tFrameHead.uiType is undefined"<<pFileFrameHead->tFrameHead.uiType;
						abort();
					}
					//test file first: over

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
						m_tFileInfo.tHistoryIFrameIndex.insert(nChannel,uiCurrentIndex);
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
			//test file second:
			//test step 1:I frame
			QMap<int ,int >::const_iterator tItem=m_tFileInfo.tFristIFrameIndex.constBegin();
			while(tItem!=m_tFileInfo.tFristIFrameIndex.constEnd()){
				unsigned int uiChannel=tItem.key();
				unsigned int uiCurrentIndex=tItem.value();
				if (uiCurrentIndex>=BUFFERSIZE*1024*1024)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<"uiCurrentIndex out of range"<<uiCurrentIndex;
					abort();
				}else{
					//do nothing
				}
				tagFileFrameHead *pCurFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiCurrentIndex);
				unsigned int uiNextIndex=pCurFileFrameHead->tPerFrameIndex.uiNextIFrame;
				if (uiNextIndex>=BUFFERSIZE*1024*1024)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<"uiNextIndex out of range"<<uiNextIndex;
					abort();
				}else{
					//do nothing
				}
				if (pFileHead->uiIndex>BUFFERSIZE*1024*1024)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<"pFileHead->uiIndex out of range"<<pFileHead->uiIndex;
					abort();
				}else{
					//do nothing
				}
				tagFileFrameHead *pNextFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiNextIndex);
				while(uiNextIndex!=0&&uiCurrentIndex<pFileHead->uiIndex&&uiNextIndex<pFileHead->uiIndex){
					if (pNextFileFrameHead->tFrameHead.uiType==FT_VideoConfig)
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pNextFileFrameHead->tFrameHead.uiType!=FT_VideoConfig";
						abort();
					}
					if (pNextFileFrameHead->tFrameHead.uiChannel==uiChannel)
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pNextFileFrameHead->tFrameHead.uiChannel!=uiChannel"<<uiChannel<<pNextFileFrameHead->tFrameHead.uiChannel;
						abort();
					}
					uiCurrentIndex=uiNextIndex;
					pCurFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiCurrentIndex);
					uiNextIndex=pCurFileFrameHead->tPerFrameIndex.uiNextIFrame;
					pNextFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiNextIndex);
					if (uiNextIndex==0)
					{
						if (m_tFileInfo.tHistoryIFrameIndex.contains(uiChannel))
						{
							if (uiCurrentIndex==m_tFileInfo.tHistoryIFrameIndex.value(uiChannel))
							{
							}else{
								qDebug()<<__FUNCTION__<<__LINE__<<"please checkout ,there must some error while write file";
								abort();
							}
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"please checkout ,there must some error while write file";
							abort();
						}
					}else{
						//do nothing
					}
				}
				++tItem;
			}
			//test setp 2:per frame
			QMap<int ,int >::const_iterator tItemPerFrame=m_tFileInfo.tHistoryFrameIndex.constBegin();
			while(tItemPerFrame!=m_tFileInfo.tHistoryFrameIndex.constEnd()){
				unsigned int uiChannel=tItemPerFrame.key();
				unsigned int uiCurrentIndex=tItemPerFrame.value();
				if (uiCurrentIndex>=BUFFERSIZE*1024*1024)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<"uiCurrentIndex out of range"<<uiCurrentIndex;
					abort();
				}else{
					//do nothing
				}
				tagFileFrameHead *pCurFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiCurrentIndex);
				unsigned int uiNextIndex=pCurFileFrameHead->tPerFrameIndex.uiNextFrame;
				if (uiNextIndex>=BUFFERSIZE*1024*1024)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<"uiNextIndex out of range"<<uiNextIndex;
					abort();
				}else{
					//do nothing
				}
				if (pFileHead->uiIndex>BUFFERSIZE*1024*1024)
				{
					qDebug()<<__FUNCTION__<<__LINE__<<"pFileHead->uiIndex out of range"<<pFileHead->uiIndex;
					abort();
				}else{
					//do nothing
				}
				tagFileFrameHead *pNextFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiNextIndex);
				bool bIframe=false;
				if (m_tFileInfo.tFristIFrameIndex.contains(uiChannel))
				{
					bIframe=true;
				}else{
					//do nothing
				}
				while(uiNextIndex!=0&&uiNextIndex<pFileHead->uiIndex&&uiCurrentIndex<pFileHead->uiIndex){
					if (pNextFileFrameHead->tFrameHead.uiChannel==uiChannel)
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pNextFileFrameHead->tFrameHead.uiChannel!=uiChannel"<<uiChannel<<pNextFileFrameHead->tFrameHead.uiChannel;
						abort();
					}
					if (bIframe)
					{
						uint uiPerIFrameIndex=pNextFileFrameHead->tPerFrameIndex.uiPreIFrame;
						if (uiPerIFrameIndex!=0)
						{
							tagFileFrameHead *pPerIFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiPerIFrameIndex);
							if (pPerIFrameHead->tFrameHead.uiChannel==uiChannel)
							{
								//keep going
							}else{
								qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pPerIFrameHead->tFrameHead.uiChannel!=uiChannel";
								abort();
							}
							if (pPerIFrameHead->tFrameHead.uiType==FT_VideoConfig)
							{
							}else{
								qDebug()<<__FUNCTION__<<__LINE__<<"illegal file as pPerIFrameHead->tFrameHead.uiType!=FT_VideoConfig";
								abort();
							}
						}else{
							//do nothing
						}
					}else{
						//do nothing
					}
					uiCurrentIndex=uiNextIndex;
					pCurFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiCurrentIndex);
					uiNextIndex=pCurFileFrameHead->tPerFrameIndex.uiNextIFrame;
					pNextFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+uiNextIndex);
				}
				++tItemPerFrame;
			}
			m_bIsBlock=false;
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
			m_bIsBlock=true;
			m_nPosition=__LINE__;
			while(tItem!=m_tBufferQueueMap.end()){
				BufferQueue *pBuffer=tItem.value();
				int nKey=tItem.key();
				int uiCurrentRecordType=m_tFileInfo.tWndInfo.value(nKey).uiCurrentRecordType;
				int uiHistoryRecordType=m_tFileInfo.tWndInfo.value(nKey).uiHistoryRecordType;
				pBuffer->enqueueDataLock();
				if (!(uiHistoryRecordType==0&&uiCurrentRecordType==0))
				{
					//keep going
					if (!pBuffer->isEmpty())
					{
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
					}else{
						//do nothing
					}
				}else{
					//do nothing	
				}
				pBuffer->enqueueDataUnLock();
				tItem++;
			}
			m_bIsBlock=false;
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
				m_bIsBlock=true;
				m_nPosition=__LINE__;
				nWriteToBuffer=writeToBuffer(m_nWriteMemoryChannel,sWriteFilePath);
				m_bIsBlock=false;
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
				sleepEx(100);
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
			//step0:���ļ���������ͨ��д���ļ�ͷ�ļ���
			//step1:�����ļ�(���ݿ���λ)--��������recordDatToDiskType_bufferFull����
			//step2:����������
			//step3:����¼���
			//step4:֪ͨ�߳�д����
			//step5:�л�bufferָ��--��ת��recordDat_default��buffer���ݿ�������һ��bufferָ�루recordDatToDiskType_outOfTime�����л�bufferָ��--��ת��recordDat_filePath��recordDatToDiskType_bufferFull��
			
			//step 0:���ļ���������ͨ��д���ļ�ͷ�ļ���
			m_bIsBlock=true;
			m_nPosition=__LINE__;
			setChannelNumInFileHead();
			setFileStartTime();
			m_bIsBlock=false;
			bool bFlags=false;
			if (m_tToDiskType==recordDatToDiskType_outOfTime||m_tToDiskType==recordDatToDiskType_bufferFull)
			{
				//step2:����������
				m_bIsBlock=true;
				m_nPosition=__LINE__;
				if (updateSearchDatabase())
				{
					//step3:����¼���
					m_nPosition=__LINE__;
					if (updateRecordDatabase(m_tToDiskType))
					{
						//step4:֪ͨ�߳�д����
						m_nPosition=__LINE__;
						if (writeTodisk())
						{
							QVariantMap tInfo;
							tInfo.insert("nInUse",1);
							m_tOperationDatabase.setRecordFileStatus(sWriteFilePath,tInfo);
							m_nPosition=__LINE__;
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
								QVariantMap tInfo;
								tInfo.insert("nLock",0);
								m_tOperationDatabase.setRecordFileStatus(sWriteFilePath,tInfo);
								nRunStep=recordDat_filePath;
							}
							bFlags=true;
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"writeTodisk fail";
							m_tResetType=recordDatReset_writeToDisk;
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"updateRecordDatabase fail";
						m_tResetType=recordDatReset_recordDatabase;
					}
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"updateSearchRecord fail";
					m_tResetType=recordDatReset_searchDatabase;
				}
				m_bIsBlock=false;
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as m_tToDiskType is undefined";
				abort();
			}
			m_tToDiskType=recordDatToDiskType_null;
			m_nWriteDiskTimeCount=0;
			if (m_bStop)
			{
				nRunStep=recordDat_end;
			}else{
				if (bFlags)
				{
					//do nothing
				}else{
					nRunStep=recordDat_reset;
				}
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
						quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
						QVariantMap tInfo;
						tInfo.insert("nEndTime",uiEndTime);
						QList<int > tIdlist;
						tIdlist.append(m_tDatabaseInfo.tChannelInRecordDatabaseId.value(nChannel));
						m_tOperationDatabase.updateRecordDatabase(tIdlist,tInfo,sWriteFilePath);
						m_tDatabaseInfo.tChannelInRecordDatabaseId.remove(nChannel);
					}else{
						//do nothing
					}
					if (m_tDatabaseInfo.tChannelInSearchDatabaseId.contains(nChannel))
					{
						quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
						QVariantMap tInfo;
						tInfo.insert("nEndTime",uiEndTime);
						QList<int > tIdList;
						tIdList.append(m_tDatabaseInfo.tChannelInSearchDatabaseId.value(nChannel));
						m_tOperationDatabase.updateSearchDatabase(tIdList,tInfo,sWriteFilePath);
						m_tDatabaseInfo.tChannelInSearchDatabaseId.remove(nChannel);
					}else{
						//do nothing
					}
					m_tFileInfo.tWndInfo[nChannel].uiHistoryRecordType=0;
				}
				m_tDatabaseInfo.tRemoveChannel.clear();
				m_tBufferQueueMapLock.unlock();
				nRunStep=recordDat_writeMemory;	
			}else
			{
				//��bufferд��Ӳ����
				nRunStep=recordDat_writeDisk;
			}
			if (m_bReloadSystemDatabase)
			{
				m_bIsBlock=true;
				m_nPosition=__LINE__;
				m_tOperationDatabase.reloadSystemDatabase();
				m_bIsBlock=false;
				m_bReloadSystemDatabase=false;
			}else{
				//do nothing
			}
			if (m_bStop)
			{
				nRunStep=recordDat_writeDisk;
				m_tToDiskType=recordDatToDiskType_bufferFull;
			}else{
				//do nothing
			}
							   }
							   break;
		case recordDat_reset:{
			//���������
			if (m_tResetType==recordDatReset_fileError)
			{
				//�ļ�����
				QVariantMap tInfo;
				tInfo.insert("nLock",1);
				m_bIsBlock=true;
				m_nPosition=__LINE__;
				m_tOperationDatabase.setRecordFileStatus(sWriteFilePath,tInfo);
				m_bIsBlock=false;
				nRunStep=recordDat_init;
				qDebug()<<__FUNCTION__<<__LINE__<<"recordDat_reset to add sWriteFilePath to lock status"<<sWriteFilePath;
				msleep(10);
			}else if (m_tResetType==recordDatReset_outOfDisk)
			{
				//���̿ռ䲻��
				//�Ƿ�ѭ��¼��
				//�Ƿ����̷�
				//�Ƿ��д��̿ռ�
				//1.1.13�汾֮ǰ�����ݿ��Ƿ�����Ŀ
				//1.1.13�汾֮��������Ƿ�����Ŀ
				m_bIsBlock=true;
				m_nPosition=__LINE__;
				tagSystemDatabaseInfo tSystemDatabaseInfo=m_tOperationDatabase.getSystemDatabaseInfo();
				m_nPosition=__LINE__;
				if (!tSystemDatabaseInfo.sAllRecordDisk.isEmpty())
				{
					m_nPosition=__LINE__;
					if (m_tOperationDatabase.isDiskSpaceOverReservedSize())
					{
						nRunStep=recordDat_init;
					}else{
						if (tSystemDatabaseInfo.bIsRecover==true)
						{
							if (m_tOperationDatabase.isRecordDataExistItem())
							{
								nRunStep=recordDat_init;
							}else{
								//do nothing
								qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no space for relase to record";
							}
						}else{
							//do nothing
							qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as diskSpace is full and IsRecover is false";
						}
					}
				}else{
					//do nothing
					qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no disk for record";
				}
				m_bIsBlock=false;
				if (nRunStep==recordDat_reset)
				{
					sleepEx(2000);
				}else{
					//keep going
					msleep(10);
				}
			}else if(m_tResetType==recordDatReset_searchDatabase){
				//�������ݿ����ʧ��
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread ,as m_tResetType is recordDatReset_searchDatabase,There is no way to recover";
				abort();
			}else if (m_tResetType==recordDatReset_recordDatabase)
			{
				//¼�����ݿ����ʧ��
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread ,as m_tResetType is rrecordDatReset_recordDatabase,There is no way to recover";
				abort();
			}else if (m_tResetType==recordDatReset_writeToDisk)
			{
				//д����ʧ��
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread ,as m_tResetType is rrecordDatReset_writeToDisk,There is no way to recover";
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread,as m_tResetType is undefined";
				abort();
			}
			if (m_bReloadSystemDatabase)
			{
				m_bIsBlock=true;
				m_nPosition=__LINE__;
				m_tOperationDatabase.reloadSystemDatabase();
				m_bIsBlock=false;
				m_bReloadSystemDatabase=false;
			}else{
				// do nothing
			}
			m_tRecordDatabaseMaxId.clear();
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
			//step 1:ȷ��дӲ���߳̽���
			m_bIsBlock=true;
			m_nPosition=__LINE__;
			m_tWriteToDisk.stopWriteToDisk();
			m_bIsBlock=false;
			//step 2:����¼�����ݿ�
			m_tDatabaseInfo.tChannelInRecordDatabaseId.clear();
			//step 3:�����������ݿ�
			m_tDatabaseInfo.tChannelInSearchDatabaseId.clear();
			m_tRecordDatabaseMaxId.clear();
			m_tOperationDatabase.stopOperationDatabase();
			bRunStop=true;
						   }
						   break;
		}
	}
	
}

bool recordDatCore::setBufferQueue( int nWnd,BufferQueue &tBufferQueue )
{
	m_tBufferQueueMapLock.lock();
	m_tBufferQueueMap.insert(nWnd,&tBufferQueue);
	int nRemoveWnd=-1;
	for (int i=0;i<m_tDatabaseInfo.tRemoveChannel.size();i++)
	{
		int nRemoveWnd=m_tDatabaseInfo.tRemoveChannel.at(i);
		if (nRemoveWnd==nWnd)
		{
			break;
		}else{
			//do nothing
		}
	}
	if (nRemoveWnd!=-1)
	{
		m_tDatabaseInfo.tRemoveChannel.removeAt(nRemoveWnd);
	}else{
		//do nothing
	}
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
	if (nType==MANUALRECORD||nType==MOTIONRECORD||nType==TIMERECORD)
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
	return m_tOperationDatabase.obtainFilePath(sWriteFilePath);
	//int nStep=obtainFilePath_getDrive;
	//bool bStop=false;
	//int bFlag=2;
	//QString sDiskList;//���ݿ��п��õ��̷���
	//QString sUsableDiks;//���õ��̷�
	//QString sFilePath;//�ļ�·��
	//while(bStop==false){
	//	switch(nStep){
	//	case obtainFilePath_getDrive:{
	//		//��ȡ��¼���̷�
	//		sUsableDiks=getUsableDisk(sDiskList);
	//		if (!(sUsableDiks.isEmpty()&&sDiskList.isEmpty()))
	//		{
	//			if (sUsableDiks.isEmpty())
	//			{
	//				nStep=obtainFilePath_diskFull;
	//			}else{
	//				nStep=obtainFilePath_diskUsable;
	//			}
	//		}else{
	//			qDebug()<<__FUNCTION__<<__LINE__<<"obtainFilePath fail as there is no disk for record";
	//			nStep=obtainFilePath_fail;
	//		}
	//								 }
	//								 break;
	//	case obtainFilePath_diskUsable:{
	//		//��ʣ��ռ�Ŀ�¼����̷�
	//		sFilePath=getLatestItem(sUsableDiks);
	//		bFlag=1;
	//		nStep=obtainFilePath_createFile;
	//								   }
	//								   break;
	//	case obtainFilePath_diskFull:{
	//		//�ж��Ƿ񸲸�¼��
	//		bool bIsRecover=getIsRecover();
	//		if (bIsRecover)
	//		{
	//			//ÿ���̷����Ѿ�¼��
	//			QStringList sDiskListInDatabase=sDiskList.split(":");
	//			QList<QString> tFilePathList;
	//			qDebug()<<__FUNCTION__<<__LINE__<<"getOldestItem in";
	//			foreach(QString sDiskEx,sDiskListInDatabase){
	//				sDiskEx=sDiskEx+":";
	//				QString sFilePathItem=m_tOperationDatabase.getOldestItem(sDiskEx);
	//				if (!sFilePathItem.isEmpty())
	//				{
	//					tFilePathList.append(sFilePathItem);
	//				}else{
	//					//do nothing
	//				}
	//			}
	//			qDebug()<<__FUNCTION__<<__LINE__<<"getOldestItem out";
	//			if (!tFilePathList.isEmpty())
	//			{
	//				//���ҳ������̷�������ļ�
	//				//�Ѳ��ҵ������ļ������ݿ��ڵ������Ϣɾ��
	//				//fix me
	//				quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();
	//				QString sOldestFile;
	//				QString sOldestFileTemp;
	//				for (int i=0;i<tFilePathList.size();i++)
	//				{
	//					QString sFilePathItem=tFilePathList.at(i);
	//					if (QFile::exists(sFilePathItem))
	//					{
	//						QFile tFile;
	//						tFile.setFileName(sFilePathItem);
	//						if (tFile.open(QIODevice::ReadOnly|QIODevice::Text))
	//						{
	//							m_tFileHead.clear();
	//							m_tFileHead=tFile.read(sizeof(tagFileHead));
	//							if (m_tFileHead.size()>=sizeof(tagFileHead))
	//							{
	//								tagFileHead *pFileHead=(tagFileHead*)m_tFileHead.data();
	//								if (NULL!=pFileHead)
	//								{
	//									if (m_tFileHead.contains("JUAN"))
	//									{
	//										if (pFileHead->uiStart<uiStartTime)
	//										{
	//											uiStartTime=pFileHead->uiStart;
	//											sOldestFile=sFilePathItem;
	//										}else{
	//											//do nothing
	//											sOldestFileTemp=sFilePathItem;
	//										}
	//									}else{
	//										//do nothing
	//										sOldestFileTemp=sFilePathItem;
	//									}
	//								}else{
	//									//do nothing
	//									qDebug()<<__FUNCTION__<<__LINE__<<"pFileHead should not been null";
	//									abort();
	//								}
	//								tFile.close();
	//							}else{
	//								sOldestFile=sFilePathItem;
	//								tFile.close();
	//								break;
	//							}
	//						}else{
	//							//do nothing
	//							QVariantMap tInfo;
	//							tInfo.insert("nDamage",1);
	//							m_tOperationDatabase.setRecordFileStatus(sFilePathItem,tInfo);
	//						}
	//					}else{
	//						//do nothing
	//						sOldestFileTemp=sFilePathItem;
	//					}	
	//				}
	//				if (sOldestFile.isEmpty())
	//				{
	//					sOldestFile=sOldestFileTemp;
	//				}else{
	//					//do nothing
	//				}
	//				if (sOldestFile.isEmpty())
	//				{
	//					qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no disk space";
	//					nStep=obtainFilePath_fail;
	//				}else{
	//					sFilePath=sOldestFile;
	//					qDebug()<<__FUNCTION__<<__LINE__<<"clearInfoInDatabase in";
	//					m_tOperationDatabase.clearInfoInDatabase(sFilePath);
	//					bFlag=0;
	//					nStep=obtainFilePath_createFile;
	//					qDebug()<<__FUNCTION__<<__LINE__<<"clearInfoInDatabase out";
	//				}
	//			}else{
	//				qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no disk space";
	//				nStep=obtainFilePath_fail;
	//			}
	//		}else{
	//			qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as there is no disk space and bIsRecover is false";
	//			nStep=obtainFilePath_fail;
	//		}
	//								 }
	//								 break;
	//	case obtainFilePath_createFile:{
	//		//����ļ������ڣ��򴴽��ļ�
	//		if (createNewFile(sFilePath))
	//		{
	//			nStep=obtainFilePath_success;
	//		}else{
	//			qDebug()<<__FUNCTION__<<__LINE__<<"terminate record as createNewFile fail";
	//			QVariantMap tInfo;
	//			tInfo.insert("nLock",1);
	//			m_tOperationDatabase.setRecordFileStatus(sFilePath,tInfo);
	//			nStep=obtainFilePath_fail;
	//		}
	//								   }
	//								   break;
	//	case obtainFilePath_success:{
	//		//��ȡ¼���ļ�·���ɹ�
	//		QVariantMap tInfo;
	//		tInfo.insert("nLock",1);
	//		m_tOperationDatabase.setRecordFileStatus(sFilePath,tInfo);
	//		nStep=obtainFilePath_end;
	//								}
	//								break;
	//	case obtainFilePath_fail:{
	//		//��ȡ¼���ļ�·��ʧ��
	//		bFlag=2;
	//		nStep=obtainFilePath_end;
	//							 }
	//							 break;
	//	case obtainFilePath_end:{
	//		//����
	//		bStop=true;
	//							}
	//	}
	//}
	//sWriteFilePath=sFilePath;
	//return bFlag;
}

//QString recordDatCore::getUsableDisk( QString &sDiskLisk )
//{
//	//����ֵ����ʣ��ռ���õ��̷�������������¼���̷��б�
//	return m_tOperationDatabase.getUsableDisk(sDiskLisk);
//}

//QString recordDatCore::getLatestItem(QString sDisk)
//{
//	//��ȡ���µ��ļ�·��
//	//step1:�������ݿ������µ��ļ�·��,û�еĻ���ֱ�Ӵ���
//	//step2:�������ݿ����µ��ļ�·�����ж��ļ��Ƿ�д����Ϊ������д�����Ļ�������һ���ļ�
//	QString sFilePath=m_tOperationDatabase.getLatestItem(sDisk);
//	if (sFilePath.isEmpty())
//	{
//		sFilePath=m_tOperationDatabase.createLatestItem(sDisk);
//	}else{
//		if (checkFileIsFull(sFilePath))
//		{
//			sFilePath=m_tOperationDatabase.createLatestItem(sDisk);
//		}else{
//			//do nothing
//		}
//	}
//	return sFilePath;
//}

//bool recordDatCore::checkFileIsFull( QString sFilePath )
//{
//	QFile tFile;
//	tFile.setFileName(sFilePath);
//	if (tFile.exists())
//	{
//		if (tFile.open(QIODevice::ReadWrite))
//		{
//			QByteArray tFilehead=tFile.read(sizeof(tagFileHead));
//			if (tFilehead.size()<sizeof(tagFileHead))
//			{
//				qDebug()<<__FUNCTION__<<__LINE__<<"undefined file,it can not bee use";
//				QVariantMap tInfo;
//				tInfo.insert("nDamage",1);
//				m_tOperationDatabase.setRecordFileStatus(sFilePath,tInfo);
//				return true;
//			}else{
//				tagFileHead *pFileHead=(tagFileHead*)tFilehead.data();
//				char *pChar="JUAN";
//				if (memcmp(pChar,pFileHead->ucMagic,4)==0)
//				{
//					if (pFileHead->uiIndex<BUFFERSIZE*1024*1024-1024*1024)
//					{
//						return false;
//					}else{
//						return true;
//					}
//				}else{
//					qDebug()<<__FUNCTION__<<__LINE__<<"undefined file,it can not bee use";
//					QVariantMap tInfo;
//					tInfo.insert("nDamage",1);
//					m_tOperationDatabase.setRecordFileStatus(sFilePath,tInfo);
//					return true;
//				}
//			}
//		}else{
//			qDebug()<<__FUNCTION__<<__LINE__<<"open file fail,it can not been use, create new file"<<sFilePath;
//			QVariantMap tInfo;
//			tInfo.insert("nDamage",1);
//			m_tOperationDatabase.setRecordFileStatus(sFilePath,tInfo);
//			return true;
//		}
//	}else{
//		return false;
//	}
//}

//bool recordDatCore::createNewFile( QString sFilePath )
//{
//	QFile tFile;
//	tFile.setFileName(sFilePath);
//	if (!tFile.exists())
//	{
//		QFileInfo tFileInfo(tFile);
//		QString sDirPath=tFileInfo.absolutePath();
//		QDir tDir;
//		if (!tDir.exists(sDirPath))
//		{
//			if (tDir.mkpath(sDirPath))
//			{
//				//keep going
//			}else{
//				qDebug()<<__FUNCTION__<<__LINE__<<"mkpath fail"<<sDirPath;
//				abort();
//			}
//		}else{
//			//keep going
//		}
//		if (tFile.open(QIODevice::ReadWrite))
//		{
//			tFile.close();
//			return true;
//		}else{
//			qDebug()<<__FUNCTION__<<__LINE__<<"new file fail"<<sFilePath;
//			abort();
//			return false;
//		}
//	}else{
//		return true;
//	}
//}

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
	return m_tOperationDatabase.getIsRecover();
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
		quint64 nSearchItemId=0;
		quint64 nRecordItemId=0;
		int nStep=WriteToBuffer_Init;
		bool bStop=false;
		while(bStop==false){
			switch(nStep){
			case WriteToBuffer_Init:{
				if (nHistoryType==nCurrentType&&nCurrentType==0)
				{
					//historyType==currentType==0,���κβ���
					nStep=WriteToBuffer_00;
				}else if (nHistoryType==0&&nCurrentType!=0)
				{
					//historyType==0,currentType!=0,��ʼ¼����Ҫ�ȴ�I֡
					nStep=WriteToBuffer_01;
				}else if (nHistoryType!=0&&nCurrentType==0)
				{
					//historyType!=0,currentType==0,ֹͣ¼��
					nStep=WriteToBuffer_10;
				}else if (nHistoryType==nCurrentType&&nCurrentType!=0)
				{
					//historyType==currentType!=0,����û��ת�䣬����¼��
					nStep=WriteToBuffer_011;
				}else if (nHistoryType!=0&&nCurrentType!=0&&nHistoryType!=nCurrentType)
				{
					//historyType!=currentType!=0,����ת��������¼��
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
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
							abort();
						}
						pRecBufferNodeTemp->release();
						pFrameHead=NULL;
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"there must exist a error,i will terminate the thread";
						abort();
					}
				}
				//step 2:�������ݿ���Ŀ
				if (bFindIFrame)
				{
					quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();

					RecBufferNode *pRecBufferNodeTemp=NULL;
					if (!pBufferQueue->isEmpty())
					{
						pRecBufferNodeTemp=pBufferQueue->front();
						if (pRecBufferNodeTemp!=NULL)
						{
							tagFrameHead *pFrameHead=NULL;
							pRecBufferNodeTemp->getDataPointer(&pFrameHead);
							if (pFrameHead!=NULL)
							{
								uiStartTime=pFrameHead->uiGentime;
							}else{
								//do nothing
							}
							pRecBufferNodeTemp->release();
							pFrameHead=NULL;
						}else{
							//do nothing
						}
					}else{
						//do nothing
					}

					quint64 uiEndTime=uiStartTime;
					//step1:����¼�����ݿ���Ŀ
					//step2:�����������ݿ���Ŀ
					if (nCurrentType&MOTIONRECORD)
					{
						nRecType=MOTIONRECORD;
					}else if (nCurrentType&MANUALRECORD)
					{
						nRecType=MANUALRECORD;
					}else if (nCurrentType&TIMERECORD)
					{
						nRecType=TIMERECORD;
					}else{
						//do nothing
						qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nCurrentType==0";
						abort();
					}
					QString sDatabaseDisk=sFilePath.left(1);
					if (m_tRecordDatabaseMaxId.contains(sDatabaseDisk))
					{
						tagRecordDatabaseMaxID tMaxIdInfo=m_tRecordDatabaseMaxId.value(sDatabaseDisk);
						nSearchItemId=tMaxIdInfo.uiMaxSearchId+1;
						nRecordItemId=tMaxIdInfo.uiMaxRecordId+1;
						tMaxIdInfo.uiMaxRecordId=tMaxIdInfo.uiMaxRecordId+1;
						tMaxIdInfo.uiMaxSearchId=tMaxIdInfo.uiMaxSearchId+1;
						m_tRecordDatabaseMaxId.replace(sDatabaseDisk,tMaxIdInfo);
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"m_tRecordDatabaseMaxId do not contains:"<<sDatabaseDisk<<"please check";
						abort();
					}
					if (createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,nRecType,sFilePath,nSearchItemId))
					{
						if (createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,nRecType,sFilePath,nRecordItemId))
						{
							m_tDatabaseInfo.tChannelInRecordDatabaseId.insert(nChannel,nRecordItemId);
							m_tDatabaseInfo.tChannelInSearchDatabaseId.insert(nChannel,nSearchItemId);
							m_tFileInfo.tWndInfo[nChannel].uiHistoryRecordType=nCurrentType;
							nHistoryType=nCurrentType;
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
				//step 1:�������ݿ�-�������ݿ��¼�����ݿ�
				QVariantMap tInfo;
				quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
				tInfo.insert("nEndTime",uiEndTime);
				if (m_tDatabaseInfo.tChannelInRecordDatabaseId.contains(nChannel))
				{
					QList<int > tIdList;
					tIdList.append(m_tDatabaseInfo.tChannelInRecordDatabaseId.value(nChannel));
					if (m_tOperationDatabase.updateRecordDatabase(tIdList,tInfo,sFilePath))
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"updateRecordDatabase fail ,please check";
					}
					m_tDatabaseInfo.tChannelInRecordDatabaseId.remove(nChannel);
				}else{
					//do nothing
				}
				tInfo.clear();
				tInfo.insert("nEndTime",uiEndTime);
				if (m_tDatabaseInfo.tChannelInSearchDatabaseId.contains(nChannel))
				{
					QList<int > tIdList;
					tIdList.append(m_tDatabaseInfo.tChannelInSearchDatabaseId.value(nChannel));
					if (m_tOperationDatabase.updateSearchDatabase(tIdList,tInfo,sFilePath))
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"updateSearchDatabase fail,please check";
					}
					m_tDatabaseInfo.tChannelInSearchDatabaseId.remove(nChannel);
				}else{
					//do nothing
				}
				//���浱ǰ��¼��״̬
				m_tFileInfo.tWndInfo[nChannel].uiHistoryRecordType=nCurrentType;
				nHistoryType=nCurrentType;
				nStep=WriteToBuffer_end;
				nFlags=0;
								  }
								  break;
			case WriteToBuffer_011:{
				//historyType==currentType!=0,����û��ת�䣬����¼��
				//step 1:д֡
				if (nCurrentType&MOTIONRECORD)
				{
					nRecType=MOTIONRECORD;
				}else if (nCurrentType&MANUALRECORD)
				{
					nRecType=MANUALRECORD;
				}else if (nCurrentType&TIMERECORD)
				{
					nRecType=TIMERECORD;
				}else{
					//do nothing
					qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nCurrentType==0";
					abort();
				}
				quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();
				RecBufferNode *pRecBufferNodeTemp=NULL;
				if (!pBufferQueue->isEmpty())
				{
					pRecBufferNodeTemp=pBufferQueue->front();
					if (pRecBufferNodeTemp!=NULL)
					{
						tagFrameHead *pFrameHead=NULL;
						pRecBufferNodeTemp->getDataPointer(&pFrameHead);
						if (pFrameHead!=NULL)
						{
							uiStartTime=pFrameHead->uiGentime;
						}else{
							//do nothing
						}
						pRecBufferNodeTemp->release();
						pFrameHead=NULL;
					}else{
						//do nothing
					}
				}else{
					//do nothing
				}
				quint64 uiEndTime=uiStartTime;
				if (m_tDatabaseInfo.tChannelInRecordDatabaseId.contains(nChannel))
				{
					nRecordItemId=m_tDatabaseInfo.tChannelInRecordDatabaseId.value(nChannel);
				}else{
					//create new record item
					QString sRecordDisk=sFilePath.left(1);
					if (m_tRecordDatabaseMaxId.contains(sRecordDisk))
					{
						tagRecordDatabaseMaxID tMaxIdInfo=m_tRecordDatabaseMaxId.value(sRecordDisk);
						nRecordItemId=tMaxIdInfo.uiMaxRecordId+1;
						tMaxIdInfo.uiMaxRecordId=nRecordItemId;
						m_tRecordDatabaseMaxId.replace(sRecordDisk,tMaxIdInfo);
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"m_tRecordDatabaseMaxId do not contain:"<<sRecordDisk;
						abort();
					}
					if (createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,nRecType,sFilePath,nRecordItemId))
					{
						m_tDatabaseInfo.tChannelInRecordDatabaseId.insert(nChannel,nRecordItemId);
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"createRecordDatabaseItem ,i will abort the thread";
						abort();
					}
				}
				//����������Ŀ��������ת��¼��ʱ��
				if (!m_tDatabaseInfo.tChannelInSearchDatabaseId.contains(nChannel))
				{
					QString sSearchDatabaseDisk;
					sSearchDatabaseDisk=sFilePath.left(1);
					if (m_tRecordDatabaseMaxId.contains(sSearchDatabaseDisk))
					{
						tagRecordDatabaseMaxID tMaxIdInfo=m_tRecordDatabaseMaxId.value(sSearchDatabaseDisk);
						nSearchItemId=tMaxIdInfo.uiMaxSearchId+1;
						tMaxIdInfo.uiMaxSearchId=tMaxIdInfo.uiMaxSearchId+1;
						m_tRecordDatabaseMaxId.replace(sSearchDatabaseDisk,tMaxIdInfo);
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"m_tRecordDatabaseMaxId do not contain :"<<sSearchDatabaseDisk<<"please check";
						abort();
					}
					if (m_tOperationDatabase.createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,nRecType,sFilePath,nSearchItemId))
					{
						m_tDatabaseInfo.tChannelInSearchDatabaseId.insert(nChannel,nSearchItemId);
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"createSearchDatabaseItem fail,i will terminate the thread";
						abort();
					}
				}else{
					//do nothing
				}
				nStep=WriteToBuffer_Write;
								   }
								   break;
			case WriteToBuffer_111:{
				//historyType!=currentType!=0,����ת��������¼��
				
				//���浱ǰ��¼��״̬
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
				//step1:�����������ݿ�
				if (m_tDatabaseInfo.tChannelInSearchDatabaseId.contains(nChannel))
				{
					//����������Ŀ��¼������
					quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
					QVariantMap tInfo;
					tInfo.insert("nEndTime",uiEndTime);
					QList<int > tIdList;
					tIdList.append(m_tDatabaseInfo.tChannelInSearchDatabaseId.value(nChannel));
					if (m_tOperationDatabase.updateSearchDatabase(tIdList,tInfo,sFilePath))
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"updateSearchDatabase fail,please checkout";
					}
				}else{
					//do nothing
				}
				//����������Ŀ
				quint64 uiStartTime=QDateTime::currentDateTime().toTime_t();

				RecBufferNode *pRecBufferNodeTemp=NULL;
				if (!pBufferQueue->isEmpty())
				{
					pRecBufferNodeTemp=pBufferQueue->front();
					if (pRecBufferNodeTemp!=NULL)
					{
						tagFrameHead *pFrameHead=NULL;
						pRecBufferNodeTemp->getDataPointer(&pFrameHead);
						if (pFrameHead!=NULL)
						{
							uiStartTime=pFrameHead->uiGentime;
						}else{
							//do nothing
						}
						pRecBufferNodeTemp->release();
						pFrameHead=NULL;
					}else{
						//do nothing
					}
				}else{
					//do nothing
				}

				quint64 uiEndTime=uiStartTime;
				QString sSearchDatabaseDisk;
				sSearchDatabaseDisk=sFilePath.left(1);
				if (m_tRecordDatabaseMaxId.contains(sSearchDatabaseDisk))
				{
					tagRecordDatabaseMaxID tMaxIdInfo=m_tRecordDatabaseMaxId.value(sSearchDatabaseDisk);
					nSearchItemId=tMaxIdInfo.uiMaxSearchId+1;
					tMaxIdInfo.uiMaxSearchId=tMaxIdInfo.uiMaxSearchId+1;
					m_tRecordDatabaseMaxId.replace(sSearchDatabaseDisk,tMaxIdInfo);
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"m_tRecordDatabaseMaxId do not contain :"<<sSearchDatabaseDisk<<"please check";
					abort();
				}
				if (m_tOperationDatabase.createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,nRecType,sFilePath,nSearchItemId))
				{
					m_tDatabaseInfo.tChannelInSearchDatabaseId.insert(nChannel,nSearchItemId);
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"createSearchDatabaseItem fail,i will terminate the thread";
					abort();
				}
				//step2:����¼�����ݿ�
				if (m_tDatabaseInfo.tChannelInRecordDatabaseId.contains(nChannel))
				{
					quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
					QVariantMap tInfo;
					tInfo.insert("nEndTime",uiEndTime);
					QList<int > tIdList;
					tIdList.append(m_tDatabaseInfo.tChannelInRecordDatabaseId.value(nChannel));
					if (m_tOperationDatabase.updateRecordDatabase(tIdList,tInfo,sFilePath))
					{
						//keep going
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"updateRecordDatabase fail ,please checkout";
					}
				}else{
					//do nothing
				}
				if (m_tRecordDatabaseMaxId.contains(sSearchDatabaseDisk))
				{
					tagRecordDatabaseMaxID tMaxIdInfo=m_tRecordDatabaseMaxId.value(sSearchDatabaseDisk);
					nRecordItemId=tMaxIdInfo.uiMaxRecordId+1;
					tMaxIdInfo.uiMaxRecordId=tMaxIdInfo.uiMaxRecordId+1;
					m_tRecordDatabaseMaxId.replace(sSearchDatabaseDisk,tMaxIdInfo);
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"m_tRecordDatabaseMaxId do not contain :"<<sSearchDatabaseDisk<<"please check";
					abort();
				}
				if (m_tOperationDatabase.createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,nRecType,sFilePath,nRecordItemId))
				{
					m_tDatabaseInfo.tChannelInRecordDatabaseId.insert(nChannel,nRecordItemId);
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"createRecordDatabaseItem fail,please checkout";
					abort();
				}

				nStep=WriteToBuffer_Write;
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
							if (uiUnusedLength>uiFrameSize)
							{
								//step1:д������֡			
								if (pFrameHead->uiType==FT_IFrame)
								{
									tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileHead->uiIndex);
									tagFrameHead *pFrameHeadBuffer=(tagFrameHead*)((char*)pFileFrameHead+sizeof(tagPerFrameIndex));
									pFrameHeadBuffer->uiChannel=pFrameHead->uiChannel;
									pFrameHeadBuffer->uiExtension=pFrameHead->uiExtension;
									pFrameHeadBuffer->uiGentime=pFrameHead->uiGentime;
									pFrameHeadBuffer->uiLength=sizeof(tagVideoConfigFrame);
									pFrameHeadBuffer->uiPts=pFrameHead->uiPts;
									if (nRecType==1||nRecType==2||nRecType==4)
									{
										pFrameHeadBuffer->uiRecType=nRecType;
									}else{
										qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nRecType is error";
										abort();
									}
									if (nRecordItemId!=0)
									{
										pFrameHeadBuffer->uiSessionId=nRecordItemId;
									}else{
										qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nRecordItemId==0";
										abort();
									}
									
									pFrameHeadBuffer->uiType=FT_VideoConfig;

									tagVideoConfigFrame *pVideoConfigFrame=(tagVideoConfigFrame*)((char*)pFileFrameHead+sizeof(tagFileFrameHead)-sizeof(pFrameHeadBuffer->pBuffer));
									tagVideoConfigFrame *pVideoConfigFrameTemp=(tagVideoConfigFrame*)((char*)pFrameHead+sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(char*));
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
										tagFileFrameHead *pLastFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileFrameHead->tPerFrameIndex.uiPreIFrame);
										pLastFrameHead->tPerFrameIndex.uiNextIFrame=pFileHead->uiIndex;
									}
									pFileFrameHead->tPerFrameIndex.uiNextIFrame=0;
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
										tagFileFrameHead *pLastFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileFrameHead->tPerFrameIndex.uiPreFrame);
										pLastFrameHead->tPerFrameIndex.uiNextFrame=pFileHead->uiIndex;
									}
									pFileFrameHead->tPerFrameIndex.uiNextFrame=0;
									pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagFileFrameHead)+sizeof(tagVideoConfigFrame)-sizeof(pFileFrameHead->tFrameHead.pBuffer);
								}else if (pFrameHead->uiType==FT_Audio)
								{
									tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileHead->uiIndex);
									tagFrameHead *pFrameHeadBuffer=(tagFrameHead*)((char*)pFileFrameHead+sizeof(tagPerFrameIndex));
									pFrameHeadBuffer->uiChannel=pFrameHead->uiChannel;
									pFrameHeadBuffer->uiExtension=pFrameHead->uiExtension;
									pFrameHeadBuffer->uiGentime=pFrameHead->uiGentime;
									pFrameHeadBuffer->uiLength=sizeof(tagAudioConfigFrame);
									pFrameHeadBuffer->uiPts=pFrameHead->uiPts;
									if (nRecType==1||nRecType==2||nRecType==4)
									{
										pFrameHeadBuffer->uiRecType=nRecType;
									}else{
										qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nRecType is error";
										abort();
									}
									if (nRecordItemId!=0)
									{
										pFrameHeadBuffer->uiSessionId=nRecordItemId;
									}else{
										qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nRecordItemId==0";
										abort();
									}
									pFrameHeadBuffer->uiType=FT_AudioConfig;

									tagAudioConfigFrame *pAudioConfigFrame=(tagAudioConfigFrame*)((char*)pFileFrameHead+sizeof(tagFileFrameHead)-sizeof(pFrameHeadBuffer->pBuffer));
									tagAudioConfigFrame *pAudioConfigFrameTemp=(tagAudioConfigFrame*)((char*)pFrameHead+sizeof(tagFrameHead)+pFrameHead->uiLength-sizeof(char*));
									pAudioConfigFrame->uiChannels=pAudioConfigFrameTemp->uiChannels;
									pAudioConfigFrame->uiSamplebit=pAudioConfigFrameTemp->uiSamplebit;
									pAudioConfigFrame->uiSamplerate=pAudioConfigFrameTemp->uiSamplerate;
									memcpy(pAudioConfigFrame->ucAudioDec,pAudioConfigFrameTemp->ucAudioDec,4);

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
										tagFileFrameHead *pLastFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileFrameHead->tPerFrameIndex.uiPreFrame);
										pLastFileFrameHead->tPerFrameIndex.uiNextFrame=pFileHead->uiIndex;
									}else{
										pFileFrameHead->tPerFrameIndex.uiPreFrame=0;
									}
									pFileFrameHead->tPerFrameIndex.uiNextFrame=0;
									m_tFileInfo.tHistoryFrameIndex.insert(nChannel,pFileHead->uiIndex);

									pFileHead->uiIndex=pFileHead->uiIndex+sizeof(tagFileFrameHead)+sizeof(tagAudioConfigFrame)-sizeof(pFileFrameHead->tFrameHead.pBuffer);
								}else if (pFrameHead->uiType==FT_PFrame)
								{
									//do nothing
								}else{
									qDebug()<<__FUNCTION__<<__LINE__<<"there is a undefined frame type,i will terminate the thread";
									abort();
								}
								//step2:д��֡

								//step 2:д��֡����
								tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileHead->uiIndex);
								tagFrameHead *pFrameHeadBuffer=(tagFrameHead*)((char*)pFileFrameHead+sizeof(tagPerFrameIndex));
								pFrameHeadBuffer->uiChannel=pFrameHead->uiChannel;
								pFrameHeadBuffer->uiExtension=pFrameHead->uiExtension;
								pFrameHeadBuffer->uiGentime=pFrameHead->uiGentime;
								pFrameHeadBuffer->uiLength=pFrameHead->uiLength;
								pFrameHeadBuffer->uiPts=pFrameHead->uiPts;
								if (nRecType==1||nRecType==2||nRecType==4)
								{
									pFrameHeadBuffer->uiRecType=nRecType;
								}else{
									qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nRecType is error";
									abort();
								}
								if (nRecordItemId!=0)
								{
									pFrameHeadBuffer->uiSessionId=nRecordItemId;
								}else{
									qDebug()<<__FUNCTION__<<__LINE__<<"terminate the thread as nRecordItemId==0";
									abort();
								}
								pFrameHeadBuffer->uiType=pFrameHead->uiType;
								pFileHead->uiEnd=pFrameHead->uiGentime;
								memcpy(&pFrameHeadBuffer->pBuffer,&pFrameHead->pBuffer,pFrameHead->uiLength);

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
									tagFileFrameHead *pLastFileFrameHead=(tagFileFrameHead*)((char*)pFileHead+pFileFrameHead->tPerFrameIndex.uiPreFrame);
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
				nStep=WriteToBuffer_end;
									 }
									 break;
			case WriteToBuffer_end:{
				bStop=true;
								   }
			}
		}
		pBufferQueue->enqueueDataUnLock();
		return nFlags;
	}else{
		return nFlags;
	}
}

void recordDatCore::slsetWriteDiskFlag()
{
	m_bWriteDiskTimeFlags=true;
}

bool recordDatCore::updateSearchDatabase()
{
	QMap<int,int>::const_iterator tItem=m_tDatabaseInfo.tChannelInSearchDatabaseId.constBegin();
	quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
	QVariantMap tInfo;
	tInfo.insert("nEndTime",uiEndTime);
	QList<int > tIdList;
	while(tItem!=m_tDatabaseInfo.tChannelInSearchDatabaseId.constEnd()){
		tIdList.append(tItem.value());
		++tItem;
	}
	m_tOperationDatabase.updateSearchDatabase(tIdList,tInfo,m_tFileInfo.sFilePath);
	return true;
}

bool recordDatCore::updateRecordDatabase(int nUpdateType)
{
	QMap<int,int>::const_iterator tItem=m_tDatabaseInfo.tChannelInRecordDatabaseId.constBegin();
	quint64 uiEndTime=QDateTime::currentDateTime().toTime_t();
	QVariantMap tInfo;
	tInfo.insert("nEndTime",uiEndTime);
	QList<int > tIdList;
	while(tItem!=m_tDatabaseInfo.tChannelInRecordDatabaseId.constEnd()){
		tIdList.append(tItem.value());
		++tItem;
	}
	m_tOperationDatabase.updateRecordDatabase(tIdList,tInfo,m_tFileInfo.sFilePath);
	if (nUpdateType==recordDatToDiskType_bufferFull)
	{
		m_tDatabaseInfo.tChannelInRecordDatabaseId.clear();
	}else{
		//do nothing
	}
	return true;
}

bool recordDatCore::writeTodisk()
{
	m_tWriteToDisk.startWriteToDisk(m_pDataBuffer,m_tFileInfo.sFilePath,BUFFERSIZE*1024*1024);
	return true;
}

bool recordDatCore::createSearchDatabaseItem( int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFilePath,quint64 &uiItemId )
{
	return m_tOperationDatabase.createSearchDatabaseItem(nChannel,uiStartTime,uiEndTime,uiType,sFilePath, uiItemId);
}

bool recordDatCore::createRecordDatabaseItem( int nChannel,quint64 uiStartTime,quint64 uiEndTime,uint uiType,QString sFileName,quint64 &uiItemId )
{
	return m_tOperationDatabase.createRecordDatabaseItem(nChannel,uiStartTime,uiEndTime,uiType,sFileName, uiItemId);
}

void recordDatCore::setChannelNumInFileHead()
{
	tagFileHead *pFileHead=(tagFileHead*)(m_pDataBuffer);
	uint uiChannel1=pFileHead->uiChannels[0];
	uint uiChannel2=pFileHead->uiChannels[1];
	QMap<int,int >::const_iterator tItem=m_tFileInfo.tHistoryFrameIndex.constBegin();
	while(tItem!=m_tFileInfo.tHistoryFrameIndex.constEnd()){
		tItem.key();
		if (tItem.key()<=31&&tItem.key()>=0)
		{
			uiChannel1=uiChannel1|(1<<tItem.key());
		}else if (tItem.key()<=63&&tItem.key()>=32)
		{
			uiChannel2=uiChannel2|(1<<(tItem.key()-32));
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"channel num overflow";
			abort();
		}
		++tItem;
	}
	pFileHead->uiChannels[0]=uiChannel1;
	pFileHead->uiChannels[1]=uiChannel2;
}

void recordDatCore::reloadSystemDatabase()
{
	m_tBufferQueueMapLock.lock();
	m_bReloadSystemDatabase=true;
	m_tBufferQueueMapLock.unlock();
}

void recordDatCore::setFileStartTime()
{
	tagFileHead *pFileHead=(tagFileHead*)(m_pDataBuffer);
	quint64 uiCurrent=sizeof(tagFileHead);
	if (uiCurrent<pFileHead->uiIndex)
	{
		tagFileFrameHead *pFileFrameHead=(tagFileFrameHead*)((char*)m_pDataBuffer+uiCurrent);
		pFileHead->uiStart=pFileFrameHead->tFrameHead.uiGentime;
		if (pFileHead->uiEnd==0)
		{
			pFileHead->uiEnd=pFileHead->uiStart;
		}else{
			//do nothing
		}
	}else{
		//do nothing
	}
}







