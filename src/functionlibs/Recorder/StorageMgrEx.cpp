#include "StorageMgrEx.h"
#include <guid.h>
#include "netlib.h"
#pragma comment(lib,"netlib.lib")
typedef struct __tagMgrDataBaseInfo{
	QString sDatabaseName;
	QSqlDatabase *pDatabase;
	int nCount;
	QList<int *> tThis;
}tagMgrDataBaseInfo;
QMultiMap<QString ,tagMgrDataBaseInfo> g_tMgrDataBase;
QString g_sMgrSearchRecord="C:/CMS_RECORD/search_record.db";
QSqlDatabase *initMgrDataBase(QString sDatabaseName,int *nThis){
	if (g_tMgrDataBase.contains(sDatabaseName))
	{
		if (g_tMgrDataBase.find(sDatabaseName).value().tThis.contains(nThis))
		{
			//do nothing
		}else{
			g_tMgrDataBase.find(sDatabaseName).value().nCount++;
			g_tMgrDataBase.find(sDatabaseName).value().tThis.append(nThis);
		}
		return g_tMgrDataBase.find(sDatabaseName).value().pDatabase;
	}else{
		tagMgrDataBaseInfo tDataBaseInfo;
		tDataBaseInfo.sDatabaseName=sDatabaseName;
		tDataBaseInfo.nCount=1;
		tDataBaseInfo.tThis.append(nThis);

		QSqlDatabase db=QSqlDatabase::addDatabase("QSQLITE",sDatabaseName);
		tDataBaseInfo.pDatabase=new QSqlDatabase(db);
		tDataBaseInfo.pDatabase->setDatabaseName(sDatabaseName);
		if (tDataBaseInfo.pDatabase->open())
		{
			//do nothing
		}else{
			printf("open database fail,in initDataBase function/n");
			return NULL;
		}
		g_tMgrDataBase.insert(sDatabaseName,tDataBaseInfo);
		return tDataBaseInfo.pDatabase;
	}
}
void deInitMgrDataBase(int *nThis){
	QMultiMap<QString,tagMgrDataBaseInfo>::iterator it;
	QStringList sDeleteList;
	for (it=g_tMgrDataBase.begin();it!=g_tMgrDataBase.end();it++)
	{
		if (it.value().tThis.contains(nThis))
		{
			it.value().nCount--;
			if (it.value().nCount==0)
			{
				it.value().pDatabase->close();
				delete it.value().pDatabase;
				it.value().pDatabase=NULL;
				sDeleteList.append(it.value().sDatabaseName);
				QSqlDatabase::removeDatabase(it.value().sDatabaseName);
			}else{
				it.value().tThis.removeOne(nThis);
			}
		}else{
			//keep going
		}
	}
	for (int i=0;i<sDeleteList.size();i++)
	{
		g_tMgrDataBase.remove(sDeleteList.at(i));
	}
}
StorageMgrEx::StorageMgrEx(void):m_bStop(false),
	m_bIsExecute(false),
	m_bExecuteFlag(false),
	m_bIsBlock(false),
	m_iSleepSwitch(0),
	m_pDisksSetting(NULL)
{
	connect(&m_tCheckBlockTimer,SIGNAL(timeout()),this,SLOT(slCheckBlock()));
	m_tCheckBlockTimer.start(1000);
}


StorageMgrEx::~StorageMgrEx(void)
{
}

void StorageMgrEx::run()
{
	int iRunStep=MGR_UPDATASYSTEMDATABASE;
	bool bRunStop=false;
	int iUpdateConunt=0;
	pcomCreateInstance(CLSID_CommonlibEx,NULL,IID_IDiskSetting,(void**)&m_pDisksSetting);
	if (m_pDisksSetting==NULL)
	{
		qDebug()<<__FUNCTION__<<__LINE__<<"run fail as m_pDisksSetting is null,please checkout";
		return;
	}else{
		//keep going
	}
	while(bRunStop==false){
		switch(iRunStep){
		case MGR_APPLYDISKSPACE:{
			if (priApplyDiskSpace())
			{
				m_bExecuteFlag=true;
			}else{
				m_bExecuteFlag=false;
			}
			m_bIsExecute=true;
			iRunStep=MGR_DEFAULT;
								}
								break;
		case MGR_CREATERECORDITEM:{
			if (priCreateRecordItem())
			{
				m_bExecuteFlag=true;
			}else{
				m_bExecuteFlag=false;
			}
			m_bIsExecute=true;
			iRunStep=MGR_DEFAULT;
								  }
								  break;
		case MGR_CREATESEARCHITEM:{
			if (priCreateSearchItem())
			{
				m_bExecuteFlag=true;
			}else{
				m_bExecuteFlag=false;
			}
			m_bIsExecute=true;
			iRunStep=MGR_DEFAULT;
								  }
								  break;
		case MGR_UPDATASYSTEMDATABASE:{
			if (priUpdateSystemDataBaseData())
			{
				//do nothing
			}else{
				//do nothing
				qDebug()<<__FUNCTION__<<__LINE__<<"priUpdateSystemDataBaseData fail ,please checkout!!!";
			}
			iRunStep=MGR_DEFAULT;
									  }
									  break;
		case MGR_DEFAULT:{
			iUpdateConunt++;
			//�����Ӹ���һ��system���ݿ��е�ֵ
			if (iUpdateConunt<120000)
			{
				if (m_tStepCode.size()>0)
				{
					iRunStep=m_tStepCode.dequeue();
				}else{
					msleep(1);
					iRunStep=MGR_DEFAULT;
				}
			}else{
				iUpdateConunt=0;
				iRunStep=MGR_UPDATASYSTEMDATABASE;
			}

			if (m_bStop)
			{
				iRunStep=MGR_END;
			}else{
				//keep going
			}
						 }
						 break;
		case MGR_END:{
			bRunStop=true;
					 }
					 break;
		}
	}
}

void StorageMgrEx::startMgr()
{
	m_tFuncLock.lock();
	if (!QThread::isRunning())
	{
		m_tStepCode.clear();
		QThread::start();
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"the thread had been running,there is no need to call this func again";
	}
	m_tFuncLock.unlock();
}

void StorageMgrEx::stopMgr()
{
	m_tFuncLock.lock();
	if (QThread::isRunning())
	{
		m_bStop=true;
		int iCount=0;
		while(QThread::isRunning()){
			iCount++;
			sleepEx(10);
			if (iCount>500&&iCount%100==0)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"the thread should been stop,it block at ::"<<m_iPosition;
			}
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"the thread had been stop ,there is  no need to call this func again";
	}
	m_tFuncLock.unlock();
}

bool StorageMgrEx::applyDiskSpace()
{
	//����ռ䣬�����ļ�·���������ļ���
	m_tFuncLock.lock();
	if (QThread::isRunning())
	{
		m_bIsExecute=false;
		m_bExecuteFlag=false;
		m_tStepCode.enqueue(MGR_APPLYDISKSPACE);
		int iCount=0;
		while(m_bIsExecute==false&&QThread::isRunning()){
			sleepEx(10);
			if (iCount>500&&iCount%100==0)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"the thread may be block at::"<<m_iPosition<<"or may be out of control";
			}
		}
		if (m_bExecuteFlag)
		{
			m_tFuncLock.unlock();
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"applyDiskSpace fail as m_bExecuteFlag is false";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"applyDiskSpace fail as the thread is not running";
	}
	m_tFuncLock.unlock();
	return false;
}

bool StorageMgrEx::createRecordItem( unsigned int &uiItem,QString &sFilePath )
{
	m_tFuncLock.lock();
	if (QThread::isRunning())
	{
		m_bIsExecute=false;
		m_bExecuteFlag=false;
		m_tStepCode.enqueue(MGR_CREATERECORDITEM);
		int iCount=0;
		while(m_bIsExecute==false&&QThread::isRunning()){
			sleepEx(10);
			if (iCount>500&&iCount%100==0)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"the thread may be block at::"<<m_iPosition<<"or may be out of control";
			}
		}
		uiItem=m_tStorageMgrExInfo.uiRecordDataBaseId;
		sFilePath=m_tStorageMgrExInfo.sRecordFilePath;
		if (m_bExecuteFlag)
		{
			m_tFuncLock.unlock();
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"applyDiskSpace fail as m_bExecuteFlag is false";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"createRecordItem fail as the thread is not running";
	}
	m_tFuncLock.unlock();
	return false;
}

bool StorageMgrEx::createSearchItem( unsigned int &uiItem )
{
	m_tFuncLock.lock();
	if (QThread::isRunning())
	{
		m_bIsExecute=false;
		m_bExecuteFlag=false;
		m_tStepCode.enqueue(MGR_CREATESEARCHITEM);
		int iCount=0;
		while(m_bIsExecute==false&&QThread::isRunning()){
			sleepEx(10);
			if (iCount>500&&iCount%100==0)
			{
				qDebug()<<__FUNCTION__<<__LINE__<<"the thread may be block at::"<<m_iPosition<<"or may be out of control";
			}
		}
		uiItem=m_tStorageMgrExInfo.uiSearchDataBaseId;
		if (m_bExecuteFlag)
		{
			m_tFuncLock.unlock();
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"applyDiskSpace fail as m_bExecuteFlag is false";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"createSearchItem fail as the thread is not running";
	}
	m_tFuncLock.unlock();
	return false;
}

void StorageMgrEx::sleepEx( int iTime )
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

bool StorageMgrEx::priApplyDiskSpace()
{
	//����ռ䣬�����ļ�·���������ļ���
	QString sDisk;
	if (freeDisk(sDisk))
	{

	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"priApplyDiskSpace fail freeDisk fail";
	}
	return false;
}

bool StorageMgrEx::priCreateRecordItem()
{
	return false;
}

bool StorageMgrEx::priCreateSearchItem()
{
	return false;
}

void StorageMgrEx::slCheckBlock()
{

}

bool StorageMgrEx::priUpdateSystemDataBaseData()
{
	//��ȡ¼����̣���ȡ���̱����ռ䣬��ȡ�ļ���������С,�Ƿ�ѭ��¼��
	tagStorageMgrExInfo tMgrExInfo;
	if (NULL!=m_pDisksSetting)
	{
		//��ȡ¼�����
		tMgrExInfo.sAllRecordDisks.clear();
		if (0==m_pDisksSetting->getUseDisks(tMgrExInfo.sAllRecordDisks))
		{
			//��ȡ�Ƿ�ѭ��¼��
			tMgrExInfo.bRecoverRecorder=m_pDisksSetting->getLoopRecording();
			//��ȡ�ļ���������С
			m_pDisksSetting->getFilePackageSize(tMgrExInfo.iFileMaxSize);
			//��ȡ���̽��鱣���ռ�
			m_pDisksSetting->getDiskSpaceReservedSize(tMgrExInfo.iDiskReservedSize);
			m_tGetMgrInfoLock.lock();
			m_tStorageMgrExInfo.sAllRecordDisks.clear();
			m_tStorageMgrExInfo.sAllRecordDisks=tMgrExInfo.sAllRecordDisks;
			m_tStorageMgrExInfo.bRecoverRecorder=tMgrExInfo.bRecoverRecorder;
			m_tStorageMgrExInfo.iFileMaxSize=tMgrExInfo.iFileMaxSize;
			m_tStorageMgrExInfo.iDiskReservedSize=tMgrExInfo.iDiskReservedSize;
			m_tGetMgrInfoLock.unlock();
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"priUpdateSystemDataBaseData fail as getUseDisks fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"priUpdateSystemDataBaseData fail as m_pDisksSetting is null";
	}
	return false;
}

tagStorageMgrExInfo StorageMgrEx::getStorageMgrExInfo()
{
	m_tGetMgrInfoLock.lock();
	tagStorageMgrExInfo tMgrExInfo;
	tMgrExInfo.bRecoverRecorder=m_tStorageMgrExInfo.bRecoverRecorder;
	tMgrExInfo.iDiskReservedSize=m_tStorageMgrExInfo.iDiskReservedSize;
	tMgrExInfo.iFileMaxSize=m_tStorageMgrExInfo.iFileMaxSize;
	tMgrExInfo.sAllRecordDisks=m_tStorageMgrExInfo.sAllRecordDisks;
	tMgrExInfo.sRecordFilePath=m_tStorageMgrExInfo.sRecordFilePath;
	tMgrExInfo.uiRecordDataBaseId=m_tStorageMgrExInfo.uiRecordDataBaseId;
	tMgrExInfo.uiSearchDataBaseId=m_tStorageMgrExInfo.uiSearchDataBaseId;
	m_tGetMgrInfoLock.unlock();
	return tMgrExInfo;
}

bool StorageMgrEx::freeDisk(QString &sDisk)
{
	QStringList sDiskList=m_tStorageMgrExInfo.sAllRecordDisks.split(":");
	if (sDiskList.size()!=0)
	{
		int iFreeDiskStep=0;
		bool bFreeDiskStop=false;
		bool bFreeSucceed=false;
		QMap<QDate,tagMgrRecInfo> tPreDeleteItem;
		while (bFreeDiskStop==false)
		{
			switch (iFreeDiskStep)
			{
			case 0:{
				//���ռ�
				bool bFound=false;
				foreach(QString sDiskItem,sDiskList){
					quint64 uiFreeByteAvailable=0;
					quint64 uiTotalNumberOfBytes=0;
					quint64 uiTotalNumberOfFreeByte=0;
					QString sDiskEx=sDiskItem+":";
					if (GetDiskFreeSpaceExQ(sDiskEx.toAscii().data(),&uiFreeByteAvailable,&uiTotalNumberOfBytes,&uiTotalNumberOfFreeByte))
					{
						if (uiTotalNumberOfFreeByte/1024/1024>(quint64)m_tStorageMgrExInfo.iDiskReservedSize)
						{
							bFound=true;
							sDisk=sDiskItem;
							break;
						}else{
							//keep going 
							qDebug()<<__FUNCTION__<<__LINE__<<sDiskEx<<"do not have enough space for recorder,find next disk";
						}
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<sDiskEx<<"can not been gotten message,it may be a system disk";
						}
					}
				if (bFound)
				{
					iFreeDiskStep=1;
				}else{
					iFreeDiskStep=2;
				}
			}
				break;
			case 1:{
				//�����㹻�Ŀռ�
				iFreeDiskStep=3;
				bFreeSucceed=true;
				   }
				   break;
			case 2:{
				//�ռ䲻�� ɾ���ռ�
				//step1:���ҳ�����ļ�¼������ɾ�����죬ͬʱ�������3��Сʱ�����뱣֤Ӳ�̿ռ��㹻¼��һ��
				tPreDeleteItem.empty();
				QDate tEarlestDate;
				foreach (QString sDiskEx,sDiskList)
				{
					QString sDataBasePath=sDiskEx+":/REC/record.db";
					QDate tDate;
					QMap<int ,QString> tMaxEndTimeMap;
					if (QFile::exists(sDataBasePath))
					{
						QStringList tList=findEarliestRecord(sDataBasePath,tDate,tMaxEndTimeMap);
						if (!tList.isEmpty())
						{
							tagMgrRecInfo tRecItem;
							tRecItem.sDbPath=sDataBasePath;
							tRecItem.tFileList=tList;
							tRecItem.tMaxEndTimeMap=tMaxEndTimeMap;
							tPreDeleteItem.insertMulti(tDate,tRecItem);
						}else{
							//keep going
							qDebug()<<__FUNCTION__<<__LINE__<<sDataBasePath<<"do not find any item for delete";
						}
					}else{
						//keep going
						qDebug()<<__FUNCTION__<<__LINE__<<"warn::"<<sDataBasePath<<"do not exists";
					}
				}
				//step2:ɾ���ļ�
				if (!tPreDeleteItem.isEmpty())
				{
					//���Ҹ�������֮�������һ���¼��
					tEarlestDate=minDate(tPreDeleteItem.keys());
					QList<tagMgrRecInfo> tRecInfo=tPreDeleteItem.values(tEarlestDate);
					QStringList tRemoteItemList;
					for(int i=0;i<tRecInfo.size();i++){
						tagMgrRecInfo tEach=tRecInfo.at(i);
						QStringList tItemList=removeFile(tEach.tFileList);
						tRemoteItemList<<tItemList;
					}
					if (!tRemoteItemList.isEmpty())
					{
						//step3:ɾ��¼������Ŀ
						if (removeRecordDataBaseItem(tRemoteItemList,tRecInfo))
						{
							//keep going
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"removeRecordDataBaseItem fail ,please check";
						}
						//step4:ɾ���������е���Ŀ
						if (removeSearchDataBaseItem(tRemoteItemList,tRecInfo))
						{
							//keep going
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<"removeSearchDataBaseItem fail ,please check";
						}
						iFreeDiskStep=0;
					}else{
						qDebug()<<__FUNCTION__<<__LINE__<<"delete file fail,as the tRemoteItemList is empty";
						iFreeDiskStep=3;
					}
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"there are not any item for delete";
					iFreeDiskStep=3;
				}
				   }
				   break;
			case 3:{
				//����ѭ��
				bFreeDiskStop=true;
				   }
			}
		}
		if (bFreeSucceed)
		{
			return true;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"freeDisk fail as it can not free enough space for recorder";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"freeDisk fail as there are no disk for recorder";
	}
	return false;
}

QStringList StorageMgrEx::findEarliestRecord( QString tDbPath,QDate &tEarlestDate,QMap<int ,QString>&tMaxEndTimeMap )
{
	QStringList sPathItemList;
	QSqlDatabase *pDataBase=NULL;
	pDataBase=initMgrDataBase(tDbPath,(int *)this);
	if (NULL!=pDataBase)
	{
		QString sEarlyDate;
		QSqlQuery _query(*pDataBase);
		QString sCommand=QString("select distinct date from local_record order by date limit 1");
		if (_query.exec(sCommand))
		{
			if (_query.next())
			{
				sEarlyDate=_query.value(0).toString();
				sCommand.clear();
				sCommand=QString("select date, path,id, win_id, end_time from local_record where date='%1' order by start_time").arg(sEarlyDate);
				if (_query.exec(sCommand))
				{
					while(_query.next()){
						if (!m_tCurrentUseRecordId.contains(_query.value(2).toInt()))
						{
							//ȷ����Ҫɾ������Ŀ �뵱ǰʱ����� ����Сʱ
							//fix me
							int iWidId=_query.value(3).toInt();
							QString sEndTime=_query.value(4).toString();
							sPathItemList<<_query.value(1).toString();
							if (!tMaxEndTimeMap.contains(iWidId))
							{
								tMaxEndTimeMap.insert(iWidId,sEndTime);
							}else{
								QString sTemp=tMaxEndTimeMap.value(iWidId);
								if (sTemp<sEndTime)
								{
									tMaxEndTimeMap[iWidId]=sEndTime;
								}else{
									//do nothing
								}
							}
						}else{
							//keep going
							qDebug()<<__FUNCTION__<<__LINE__<<"this item can not been delete,as it is be using"<<_query.value(1).toString();
						}
					}
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"findEarliestRecord fail as exec the cmd::"<<sCommand<<"fail";
				}
			}else{
				//keep going 
			}
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"findEarliestRecord fail as exec the cmd::"<<sCommand<<"fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"findEarliestRecord fail as pDataBase is null";
	}
	return sPathItemList;
}

QDate StorageMgrEx::minDate( QList<QDate> tDateList )
{
	QDate tMinDate=tDateList.at(0);
	for(int i=1;i<tDateList.size();++i){
		tMinDate=qMin(tDateList.at(i),tMinDate);
	}
	return tMinDate;
}

QStringList StorageMgrEx::removeFile( QStringList tItemList )
{
	QStringList tRemoteItemList;
	if (!tItemList.isEmpty())
	{
		foreach(QString sFilePath,tItemList){
			if (QFile::remove(sFilePath))
			{
				tRemoteItemList<<sFilePath;
				QString sDirPath=sFilePath.left(sFilePath.lastIndexOf("/"));
				QDir tDir(sDirPath);
				tDir.setFilter(QDir::Files);
				if (!tDir.count())
				{
					tDir.rmpath(sDirPath);
				}else{
					//do nothing
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"delete record file::"<<sFilePath<<"fail,please check";
			}
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"removeFile do not remove any file as tItemList is empty";
	}
	return tRemoteItemList;
}

bool StorageMgrEx::removeRecordDataBaseItem( QStringList tRemoveFileItem,QList<tagMgrRecInfo> tRecInfo )
{
	return false;
}

bool StorageMgrEx::removeSearchDataBaseItem( QStringList tRemoveFileItem,QList<tagMgrRecInfo> tRecInfo )
{
	return false;
}
