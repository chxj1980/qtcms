#include "StorageMgr.h"
#include "guid.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include "netlib.h"
#pragma comment(lib,"netlib.lib")
QMutex StorageMgr::m_sLock;
QMutex StorageMgr::m_dblock;
QList<int > StorageMgr::m_insertIdList;
StorageMgr::StorageMgr(void):
	m_insertId(-1),
	m_pDisksSetting(NULL),
	m_db(NULL)
/*	m_currdisk('0')*/
{
	pcomCreateInstance(CLSID_CommonlibEx,NULL,IID_IDiskSetting,(void **)&m_pDisksSetting);
	if (!m_pDisksSetting)
	{
		qDebug("can not create diskssetting instance");
	}
	QDateTime curTime = QDateTime::currentDateTime();
	m_connectId = QString::number(curTime.toTime_t()) + QString::number((int)this);
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectId);
	m_dblock.lock();
	m_db = new QSqlDatabase(db);
	if (NULL == m_db)
	{
		qDebug()<<"init database failed!";
	}
	m_dblock.unlock();
}


StorageMgr::~StorageMgr(void)
{
	if (m_pDisksSetting)
	{
		m_pDisksSetting->Release();
	}

	m_dblock.lock();
	m_db->close();
	delete m_db;
	m_db = NULL;
	QSqlDatabase::removeDatabase(m_connectId);
	m_dblock.unlock();
}

int StorageMgr::getFilePackageSize()
{
	int filesize = 128;
	if (m_pDisksSetting)
	{
		m_pDisksSetting->getFilePackageSize(filesize);
	}

	return filesize;
}
QString StorageMgr::getUseDisks()
{
	QString sDisks;
	if (m_pDisksSetting)
	{
		m_pDisksSetting->getUseDisks(sDisks);
	}
	return sDisks;
}
bool StorageMgr::getLoopRecording()
{
	bool loop = false;
	if (m_pDisksSetting)
	{
		loop = getLoopRecording();
	}
	return loop;
}
int StorageMgr::getFreeSizeForDisk()
{
	int spacereservedsize=1024;
	if (m_pDisksSetting)
	{
		m_pDisksSetting->getDiskSpaceReservedSize(spacereservedsize);
	}
	return spacereservedsize;
}
QString StorageMgr::getFileSavePath( QString devname,int nChannelNum,int winId, int type, QTime &start )
{
	QString sFileSavePath="none";
	m_insertId=-1;
	m_sLock.lock();
	QString sDisk=getUsableDisk();
	m_sLock.unlock();
	if ("0"!=sDisk)
	{
		sFileSavePath=sDisk+":/REC";
		m_dblock.lock();
		if (!m_db->isOpen())
		{
			m_db->setDatabaseName(sFileSavePath+"/record.db");
			if (m_db->open())
			{
				//keep going
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"getFileSavePath fail as open db fail";
				m_dblock.unlock();
				return "none";
			}
		}

		//find the newest record in database and create path base on it
		QSqlQuery _query(*m_db);
		QString command = QString("select path from local_record order by id desc limit 1");
		_query.exec(command);

		if (_query.next())//has find it
		{
			QString path = _query.value(0).toString();
			int pos = path.lastIndexOf("/");
			int fileNum = path.mid(pos + 1, 3).toInt();
			int dirNum = path.mid(pos - 3, 3).toInt();
			fileNum++;
			if (fileNum > 127)//Ensure that each directory only 128 files
			{
				dirNum++;
				fileNum = 0;
			}
			QString temp;
			temp = temp.sprintf("/%04d/%03d.avi", dirNum, fileNum);
			sFileSavePath += temp;
		}
		else
		{
			//use default path
			sFileSavePath += "/0000/000.avi"; 
		}
		//insert new data into database
		_query.prepare("insert into local_record(dev_name,dev_chl,win_id,date,start_time,record_type,path) values(:dev_name,:dev_chl,:win_id,:date,:start_time,:record_type,:path)");
		_query.bindValue(":dev_name",devname);
		_query.bindValue(":dev_chl",nChannelNum + 1);//ensure chl num start with 1
		_query.bindValue(":win_id",winId);
		_query.bindValue(":date",QDate::currentDate().toString("yyyy-MM-dd"));
		start = QTime::currentTime();
		_query.bindValue(":start_time",start.toString("hh:mm:ss"));
		_query.bindValue(":record_type", type);
		_query.bindValue(":path", sFileSavePath);
		_query.exec();
		
		//save the id inserted just now
		command = QString("select max(id) from local_record");
		_query.exec(command);
		if (_query.next())
		{
			m_insertId = _query.value(0).toInt();
			m_insertIdList.append(m_insertId);
		}

		_query.finish();
		m_dblock.unlock();
		return sFileSavePath;
	}else{
		return sFileSavePath;
	}
}
QString StorageMgr::getUsableDisk()
{
	QString sDisks;
	QString sGottenDisk="0";
	int nTryCount=3;
	int nFreeSize;
	bool bRecover;
	if (m_pDisksSetting!=NULL)
	{
		bRecover=m_pDisksSetting->getLoopRecording();
		if (0!=m_pDisksSetting->getDiskSpaceReservedSize(nFreeSize))
		{
			nFreeSize=124;
		}else{
			//do nothing
		}
		if (0==m_pDisksSetting->getUseDisks(sDisks))
		{
			QStringList sDiskList = sDisks.split(":");
			if (sDiskList.size()!=0)
			{
				while(nTryCount>0){
					foreach(QString stritem,sDiskList){
						quint64 FreeByteAvailable = 0;
						quint64 TotalNumberOfBytes = 0;
						quint64 TotalNumberOfFreeBytes = 0;
						QString sdisk = stritem+":";
						if (GetDiskFreeSpaceExQ(sdisk.toAscii().data(),&FreeByteAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes))
						{
							if (TotalNumberOfFreeBytes/1024/1024 > (quint64)nFreeSize){
								sGottenDisk = stritem;
								if (!m_curDisk.isEmpty() && m_curDisk != sGottenDisk && m_db->isOpen())
								{
									m_db->close();
								}
								QString dbpath = sdisk + "/REC";
								QDir dDir;
								m_dblock.lock();
								if (dDir.exists(dbpath))
								{
									//do nothing
								}else{
									//create dir ::rec
									dDir.mkpath(dbpath);
								}
								dbpath+="/record.db";
								if (!QFile::exists(dbpath) && !m_db->isOpen())
								{
									m_db->setDatabaseName(dbpath);
									if (!m_db->open())
									{
										qDebug()<<__FUNCTION__<<__LINE__<<"open data base fail,please check";
									}else{

									}
									createTable();
								}else{
									//do nothing
								}
								m_dblock.unlock();
								m_curDisk = sGottenDisk;
								break;
							}else{
								// there is not enough space in this disk,please find anther or free disk
							}
						}else{
							qDebug()<<__FUNCTION__<<__LINE__<<sdisk<<"can not been gotten message,it may be a system disk ";
						}
					}
					if (bRecover && "0" == sGottenDisk)
					{
						if (deleteOldDir(sDiskList))
						{
							//keep going
						}else{
							//there is no need to continue
							nTryCount=0;
						}
					}else{
						//keep going
					}
					if (bRecover&&sGottenDisk=="0")
					{
						nTryCount--;
					}else{
						//there is no need to continue
						nTryCount=0;
					}
					
				}
			}else{
				qDebug()<<__FUNCTION__<<__LINE__<<"can not get usableDisk ,there may be exist a error in database";
			}
		}else{
			//
			qDebug()<<__FUNCTION__<<__LINE__<<"can not get usableDisk,please select at least one disk as record disk in device setting page";
		}
	}else{
		//
		qDebug()<<__FUNCTION__<<__LINE__<<"can not get usableDisk as m_pDisksSetting is null";
	}
	return sGottenDisk;
}


// void StorageMgr::deleteOldDir(const QStringList& diskslist)
// {
// 
// 	int deletetotalsize;
// 	int freesizem;
// 	if(0 != m_pDisksSetting->getDiskSpaceReservedSize(freesizem))
// 		freesizem = 128;
// 
// 	freesizem=(freesizem+200)*1024;
// 	bool flag=false;
// 	QList<unsigned int> datetimelist;
// 	QDateTime earliestTime;
// 	earliestTime = QDateTime::fromString("2030-12-31","yyyy-MM-dd");
// 	foreach(QString sdisk,diskslist)
// 	{
// 		QString spath = sdisk+":/REC/";
// 		QDir dir(spath);
// 		dir.setFilter(QDir::AllDirs);
// 		QFileInfoList fileList = dir.entryInfoList();
// 		foreach(QFileInfo fi,fileList)
// 		{
// 			QDateTime dtime = QDateTime::fromString(fi.fileName(),"yyyy-MM-dd");
// 			if (!datetimelist.contains(dtime.toTime_t()))
// 			datetimelist.append(dtime.toTime_t());
// 		}
// 	}
// 
// 	foreach(QString sdisk,diskslist)
// 	{
// 		unsigned int oldestdate=1900000000;
// 		/*unsigned int oldestdate=QDateTime::currentDateTimeUtc().toTime_t();*/
// 		foreach(unsigned int datelist,datetimelist){
// 			oldestdate=qMin(oldestdate,datelist);
// 			QDateTime datetime=QDateTime::fromTime_t(datelist);
// 		}
// 		datetimelist.removeAt(datetimelist.indexOf(oldestdate));
// 		QDateTime earliestTime=QDateTime::fromTime_t(oldestdate);
// 		QString spath = sdisk+":/REC/" + earliestTime.toString("yyyy-MM-dd");
// 		QDir dir(spath);
// 		dir.setFilter(QDir::AllDirs);
// 		QFileInfoList fileList = dir.entryInfoList();
// 
// 		foreach(QFileInfo fi,fileList)
// 		{
// 			QString spath=sdisk+":/REC/" + earliestTime.toString("yyyy-MM-dd")+"/"+fi.fileName();
// 			if (fi.fileName()=="."||fi.fileName()=="..")
// 			{
// 				continue;
// 			}
// 			QDir dir(spath);
// 			dir.setFilter(QDir::AllDirs);
// 			QFileInfoList filechllist=dir.entryInfoList();
// 			foreach(QFileInfo fil,filechllist){
// 				QString spath=sdisk+":/REC/" + earliestTime.toString("yyyy-MM-dd")+"/"+fi.fileName()+"/"+fil.fileName();
// 				if (fil.fileName()!="."&&fil.fileName()!="..")
// 				{
// 					QDir dir(spath);
// 					dir.setFilter(QDir::AllEntries|QDir::NoDotAndDotDot);
// 					QFileInfoList fileList = dir.entryInfoList();
// 					foreach(QFileInfo finame,fileList){
// 						QString spath=sdisk+":/REC/" + earliestTime.toString("yyyy-MM-dd")+"/"+fi.fileName()+"/"+fil.fileName()+"/"+finame.fileName();
// 						QFile file(spath);
// 						if (file.exists())
// 						{
// 							freesizem=freesizem-file.size();
// 						}else{
// 							freesizem=freesizem-fil.size();
// 						}				
// 						deleteDir(spath);
// 						if (freesizem<0)
// 						{
// 							flag=true;
// 							break;
// 						}
// 					}
// 				}		
// 			}
// 			if (flag==true)
// 				break;
// 		}
// 		if (flag==true)
// 			break;
// 	}
// 
// }
bool StorageMgr::deleteOldDir( const QStringList& diskslist )
{
	QDate earlestDate;
	QMap<QDate, RecInfo> result;
	quint64 FreeByteAvailable = 0;
	quint64 TotalNumberOfBytes = 0;
	quint64 TotalNumberOfFreeBytes = 0;
	int freesizem = 0;
	if(0 != m_pDisksSetting->getDiskSpaceReservedSize(freesizem))
		freesizem = 1024;
	int nStep=0;
	bool nIsStop=false;
	bool bRet=false;
	m_dblock.lock();
	while(!nIsStop){
		switch(nStep){
		case 0:{
			//����Ƿ���Ҫɾ��
			foreach(QString disk,diskslist){
				QString sdisk = disk + ":";
				GetDiskFreeSpaceExQ(sdisk.toAscii().data(),&FreeByteAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes);
				if (TotalNumberOfFreeBytes/1024/1024 > (quint64)freesizem)
				{
					//���㹻�Ŀռ�
					bRet=true;
					nIsStop=true;
					break;
				}else{
					//keep going
					nStep=1;
				}
			}
			   }
			   break;
		case 1:{
			//��������ļ�¼
			result.empty();
			foreach(QString disk, diskslist)
			{
				QString path = disk + ":/REC/record.db";
				QDate date;
				QStringList list = findEarlestRecord(path, date);
				if (list.isEmpty())
				{
					continue;//open database failed or no record in database
				}
				RecInfo re;
				re.dbPath = path;
				re.fileLsit = list;
				result.insertMulti(date, re);
			}
			if (result.isEmpty())
			{
				//û��¼�����ɾ����
				qDebug()<<__FUNCTION__<<__LINE__<<"there is no record for free,please remove anther file from disk";
				bRet=false;
				nIsStop=true;
			}else{
				earlestDate = minDate(result.keys());
				nStep=2;
			}
			   }
			   break;
		case 2:{
			//ɾ��
			QList<RecInfo> recInfo = result.values(earlestDate);
			foreach(RecInfo each, recInfo)
			{
				//delete file from directory
				QStringList hasDelete = deleteFile(each.fileLsit);
				//delete record from database
				deleteRecord(each.dbPath, earlestDate.toString("yyyy-MM-dd"), hasDelete);
			}
			nStep=0;
			   }
			   break;
		case 3:{
			//����
			   }
			   break;
		}
	}
	m_dblock.unlock();
	return bRet;
}
//bool StorageMgr::deleteOldDirEx(const QStringList& diskslist)
//{
//	QDate earlestDate;
//	QMap<QDate, RecInfo> result;
//	quint64 FreeByteAvailable = 0;
//	quint64 TotalNumberOfBytes = 0;
//	quint64 TotalNumberOfFreeBytes = 0;
//	int freesizem = 0;
//	if(0 != m_pDisksSetting->getDiskSpaceReservedSize(freesizem))
//		freesizem = 1024;
//
//	while (true)
//	{
//		m_dblock.lock();
//		//check free space in disk
//		foreach(QString disk, diskslist)
//		{
//			QString sdisk = disk + ":";
//			GetDiskFreeSpaceExQ(sdisk.toAscii().data(),&FreeByteAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes);
//			if (TotalNumberOfFreeBytes/1024/1024 > (quint64)freesizem)
//			{
//				m_dblock.unlock();
//				return true;//has enough space
//			}
//		}
//		//collect the earliest record date in each database
//		result.empty();
//		foreach(QString disk, diskslist)
//		{
//			QString path = disk + ":/REC/record.db";
//			QDate date;
//			QStringList list = findEarlestRecord(path, date);
//			if (list.isEmpty())
//			{
//				continue;//open database failed or no record in database
//			}
//			RecInfo re;
//			re.dbPath = path;
//			re.fileLsit = list;
//			result.insertMulti(date, re);
//		}
//		if (result.isEmpty())
//		{
//			qDebug()<<__FUNCTION__<<__LINE__<<"there is no record for free ,please remove anther file to release space";
//			return false;
//		}else{
//			//keep going
//		}
//		//find the earliest date
//		earlestDate = minDate(result.keys());
//		QList<RecInfo> recInfo = result.values(earlestDate);
//		foreach(RecInfo each, recInfo)
//		{
//			//delete file from directory
//			QStringList hasDelete = deleteFile(each.fileLsit);
//			//delete record from database
//			deleteRecord(each.dbPath, earlestDate.toString("yyyy-MM-dd"), hasDelete);
//		}
//		m_dblock.unlock();
//	}
//}

QDate StorageMgr::minDate(QList<QDate> dateList)
{
	QDate minDate = dateList.at(0);
	for (int i = 1; i < dateList.size(); ++i)
	{
		minDate = qMin(dateList.at(i), minDate);
	}
	return minDate;
}

QStringList StorageMgr::deleteFile(const QStringList& fileList)
{
	QStringList hasDelete;
	if (!fileList.isEmpty())
	{
		quint64 FreeByteAvailable = 0;
		quint64 TotalNumberOfBytes = 0;
		quint64 TotalNumberOfFreeBytes = 0;
		int filesize = 0;
		if(0 != m_pDisksSetting->getFilePackageSize(filesize))
			filesize = 128;
		int freesizem = 0;
		if(0 != m_pDisksSetting->getDiskSpaceReservedSize(freesizem))
			freesizem = 1024;

		foreach(QString file, fileList)
		{
			QFile::remove(file);
			hasDelete<<file;
			QString dirpath = file.left(file.lastIndexOf("/"));
			QDir dir(dirpath);
			dir.setFilter(QDir::Files);
			if (!dir.count())
			{
				dir.rmpath(dirpath);
			}
			QString sdisk = file.left(2);
			GetDiskFreeSpaceExQ(sdisk.toAscii().data(),&FreeByteAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes);
			qint64 freeSize = (qint64)TotalNumberOfFreeBytes - (qint64)freesizem*1024*1024;
			qint64 fiveFile = filesize * 1024 * 1024 * 5;
			if (freeSize > fiveFile)//Ensure that the free space can store 5 files
			{
				break;
			}
		}
	}
	return hasDelete;
}

// bool StorageMgr::deleteDir(const QString& dirpath)
// {
// 	if (dirpath.isEmpty())
// 		return false;
// 	QFile file(dirpath);
// 	if (file.exists())
// 	{
// 		QFile::remove(dirpath);
// 		return true;
// 	}
// 	QDir dir(dirpath);
// 	if (!dir.exists())
// 		return true;
// 
// 	dir.setFilter(QDir::AllEntries|QDir::NoDotAndDotDot);
// 	QFileInfoList fileList = dir.entryInfoList();
// 	foreach(QFileInfo fi,fileList)
// 	{
// 		if (fi.isFile())
// 			fi.dir().remove(fi.fileName());
// 		else
// 			deleteDir(fi.absoluteFilePath());
// 	}
// 	return dir.rmpath(dir.absolutePath());
// }

bool StorageMgr::GetDiskFreeSpaceEx(char* lpDirectoryName, quint64* lpFreeBytesAvailableToCaller, quint64* lpTotalNumberOfBytes, quint64* lpTotalNumberOfFreeBytes)
{
	return GetDiskFreeSpaceExQ(lpDirectoryName,lpFreeBytesAvailableToCaller,lpTotalNumberOfBytes,lpTotalNumberOfFreeBytes);
}

bool StorageMgr::freeDisk()
{
	QString sDisks;
	if (m_pDisksSetting!=NULL)
	{
		if (0==m_pDisksSetting->getUseDisks(sDisks))
		{
			QStringList sDiskList=sDisks.split(":");
			if (sDiskList.size()>0)
			{
				if (deleteOldDir(sDiskList))
				{
					return true;
				}else{
					qDebug()<<__FUNCTION__<<__LINE__<<"freeDisk fail as deleteOldDir fail";
				}
			}else{
				//
				qDebug()<<__FUNCTION__<<__LINE__<<"freeDisk fail ,please check database ,there may be some error in database";
			}
		}else{
			//
			qDebug()<<__FUNCTION__<<__LINE__<<"freeDisk fail as getUseDisks fail";
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"freeDisk fail as m_pDisksSetting is null";
	}
	return false;
}

// int StorageMgr::addRecord( QString sDevName, int chl, int winId, QString sDate, QString sStart, int type, QString sPath )
// {
// 	if (sDevName.isEmpty() || chl < 0 || sDate.isEmpty() || sStart.isEmpty() || type < 0 || type > 4 || sPath.isEmpty())
// 	{
// 		return -1;
// 	}
// 
// 	int insertId = -1;
// 	m_dblock.lock();
// 	QSqlQuery _query(*m_db);
// 	_query.prepare("insert into local_record(dev_name,dev_chl,win_id,date,start_time,record_type,path) values(:dev_name,:dev_chl,:win_id,:date,:start_time,:record_type,:path)");
// 	_query.bindValue(":dev_name",sDevName);
// 	_query.bindValue(":dev_chl",chl);
// 	_query.bindValue(":win_id",winId);
// 	_query.bindValue(":date",sDate);
// 	_query.bindValue(":start_time",sStart);
// 	_query.bindValue(":record_type", type);
// 	_query.bindValue(":path", sPath);
// 	_query.exec();
// 
// 	QString command = QString("select max(id) from local_record");
// 	_query.exec(command);
// 	if (_query.next())
// 	{
// 		insertId = _query.value(0).toInt();
// 	}
// 	m_dblock.unlock();
// 
// 	return insertId;
// }
bool StorageMgr::deleteRecord()
{
	bool bRet=false;
	m_dblock.lock();
	if (m_insertId!=-1)
	{
		QSqlQuery _query(*m_db);
		QString command=QString("delete from local_record where id=%1").arg(m_insertId);
		if (_query.exec(command))
		{
			bRet =true;
			if (m_insertIdList.contains(m_insertId))
			{
				int npos=m_insertIdList.indexOf(m_insertId);
				m_insertIdList.removeAt(npos);
			}else{

			}
			m_insertId=-1;
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"deleteRecord fail as exec the command fail,please check the command or the id";
			bRet=false;
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"deleteRecord fail as the id ==-1";
		bRet=false;
	}
	m_dblock.unlock();
	return bRet;
}
bool StorageMgr::updateRecord( QString sEnd, int size )
{
	//update file end time and file size
	m_dblock.lock();
	QSqlQuery _query(*m_db);
	QString command = QString("update local_record set end_time = '%1', file_size = %2 where id = %3").arg(sEnd).arg(QString::number(size)).arg(m_insertId);
	if (_query.exec(command))
	{
		m_dblock.unlock();
		if (m_insertIdList.contains(m_insertId))
		{
			int npos=m_insertIdList.indexOf(m_insertId);
			m_insertIdList.removeAt(npos);
		}else{

		}
		m_insertId = -1;
		return true;
	}else{
		m_dblock.unlock();
		qDebug()<<__FUNCTION__<<__LINE__<<"m_insertId::"<<m_insertId<<"sEnd::"<<sEnd<<"size::"<<size<<"updateRecord fail ,please check the database or the command";
		return false;
	}
}

QStringList StorageMgr::findEarlestRecord( QString dbPath, QDate &earlestDate )
{
	QStringList pathList;
	pathList.empty();
	m_db->setDatabaseName(dbPath);
	if (!m_db->open())
	{
		qDebug()<<"open " + dbPath + "failed!";
		return pathList;
	}
	QSqlQuery _query(*m_db);
	QString command = QString("select date, path,id from local_record where date=(select date from local_record order by date limit 1) order by start_time");
	_query.exec(command);

	while (_query.next())
	{
		earlestDate = _query.value(0).toDate();
		if (m_insertIdList.contains(_query.value(2).toInt()))
		{
		}else{
			pathList<<_query.value(1).toString();
		}
	}
	_query.finish();
	m_db->close();

	return pathList;
}

void StorageMgr::deleteRecord( QString dbPath, QString date, QStringList filelist )
{
	m_db->setDatabaseName(dbPath);
	if (!m_db->open())
	{
		qDebug()<<"open " + dbPath + "failed!";
	}
	QSqlQuery _query(*m_db);
	foreach(QString path, filelist)
	{
		QString command = QString("delete from local_record where date = '%1' and path = '%2'").arg(date).arg(path);
		_query.exec(command);
	}

	_query.finish();
	m_db->close();
}



void StorageMgr::createTable()
{
	QSqlQuery _query(*m_db);
	QString command = "create table local_record(id integer primary key autoincrement,";
	command += "dev_name chr(32),";
	command += "dev_chl integer,";
	command += "win_id integer,";
	command += "date char(32),";
	command += "start_time char(32),";
	command += "end_time char(32),";
	command += "record_type integer,";
	command += "file_size integer,";
	command += "path char(64))";
	_query.exec(command);
}

int StorageMgr::getInsertId()
{
	return m_insertId;
}





