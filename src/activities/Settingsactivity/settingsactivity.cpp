#include "settingsactivity.h"
#include <guid.h>
#include <QDebug>
#include <QString>
#include <qtconcurrentrun.h>

settingsActivity::settingsActivity():
	m_nRef(0),
	m_bMouseTrace(false),
	m_SettingThread(NULL),
	m_bIsRemoving(false),
	m_bIsAdding(false)
{

}

long __stdcall settingsActivity::QueryInterface( const IID & iid,void **ppv )
{
	if (IID_IPcomBase == iid)
	{
		*ppv = static_cast<IPcomBase *>(this);
	}
	else if (IID_IActivities == iid)
	{
		*ppv = static_cast<IActivities *>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}
	static_cast<IPcomBase *>(this)->AddRef();

	return S_OK;
}

unsigned long __stdcall settingsActivity::AddRef()
{
	m_csRef.lock();
	m_nRef ++;
	m_csRef.unlock();
	return m_nRef;
}

unsigned long __stdcall settingsActivity::Release()
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

void settingsActivity::Active( QWebFrame * frame)
{
	m_MainView = (frame->page())->view();
	m_MainView->setMouseTracking(true);
	QWFW_MSGMAP_BEGIN(frame);
// 	QWFW_MSGMAP("app_top","dblclick","OnTopActDbClick()");
// 	QWFW_MSGMAP("app_move","mousedown","OnMouseDown()");
// 	QWFW_MSGMAP("app_move","mouseup","OnMouseUp()");
// 	QWFW_MSGMAP("app_move","mousemove","OnMouseMove()");


// 	QWFW_MSGMAP("app_maxsize","click","OnMaxClick()");
	QWFW_MSGMAP("app_minsize","click","OnMinClick()");
	QWFW_MSGMAP("app_close_window","click","OnCloseClick()");

// 	QWFW_MSGMAP("AddUser_ok","click","OnAddUserOk()");
// 	QWFW_MSGMAP("ModifyUser_ok","click","OnModifyUserOk()");
// 	QWFW_MSGMAP("DeleteUser_ok","click","OnDeleteUserOk()");

	QWFW_MSGMAP("AddUserEx_ok","click","OnAddUserExOk()");
	QWFW_MSGMAP("ModifyUserEx_ok","click","OnModifyUserExOk()");
	QWFW_MSGMAP("DeleteUserEx_ok","click","OnDeleteUserExOk()");
	QWFW_MSGMAP("ModifyCurrentUserEx_ok","click","OnModifyCurrentUserExOk()");

	QWFW_MSGMAP("AddDevice_ok","click","OnAddDevice()");
	QWFW_MSGMAP("AddDeviceDouble_ok","click","OnAddDeviceDouble()");
	QWFW_MSGMAP("AddDeviceALL_ok","click","OnAddDeviceALLThread()");
	QWFW_MSGMAP("RemoveDevice_ok","click","OnRemoveDevice()");
	QWFW_MSGMAP("RemoveDeviceEx_ok","click","OnRemoveDevice()");
	QWFW_MSGMAP("RemoveDeviceALL_ok","click","OnRemoveDeviceALLThread()");
	QWFW_MSGMAP("ModifyDevice_ok","click","OnModifyDevice()");
	QWFW_MSGMAP("ModifyDeviceEx_ok","click","OnModifyDeviceEx()");

	QWFW_MSGMAP("AddGroup_ok","click","OnAddGroup()");
	QWFW_MSGMAP("RemoveGroup_ok","click","OnRemoveGroup()");
	QWFW_MSGMAP("ModifyGroup_ok","click","OnModifyGroup()");

	QWFW_MSGMAP("AddArea_ok","click","OnAddArea()");
	QWFW_MSGMAP("RemoveArea_ok","click","OnRemoveArea()");
	QWFW_MSGMAP("ModifyArea_ok","click","OnModifyArea()");

// 	QWFW_MSGMAP("AddChannel_ok","click","OnAddChannel()");
// 	QWFW_MSGMAP("RemoveChannel_ok","click","OnRemoveChannel()");
	QWFW_MSGMAP("ModifyChannel_ok","click","OnModifyChannel()");

	QWFW_MSGMAP("AddChannelInGroupDouble_ok","click","OnAddChannelInGroupDouble()");
	QWFW_MSGMAP("AddChannelInGroup_ok","click","OnAddChannelInGroup()");
	QWFW_MSGMAP("RemoveChannelFromGroup_ok","click","OnRemoveChannelFromGroup()");
	QWFW_MSGMAP("ModifyGroupChannelName_ok","click","OnModifyGroupChannelName()");

	QWFW_MSGMAP("SettingStorageParm_ok","click","OnSettingStorageParm()");

	QWFW_MSGMAP("SettingCommonParm_ok","click","OnSettingCommonParm()");
	
	QWFW_MSGMAP("SettingRecordTimeParm_ok","click","OnSettingRecordTimeParm()");
	QWFW_MSGMAP("SettingRecordDoubleTimeParm_ok","click","OnSettingRecordTimeParmDouble()");
	QWFW_MSGMAP("SettingRecordCopyTimeParm_ok","click","OnSettingRecordTimeParmCopy()");
	QWFW_MSGMAP_END;
}

void settingsActivity::OnTopActDbClick()
{
	if (m_MainView->isMaximized()||m_MainView->size().height()==QApplication::desktop()->screenGeometry().height()||m_MainView->size().width()==QApplication::desktop()->screenGeometry().width())
	{
		m_MainView->showNormal();
		QRect rcScreen = QApplication::desktop()->screenGeometry();
		
		QSize currentSize=m_MainView->size();
		if (rcScreen.height()*2/3>600)
		{
			currentSize.setHeight(rcScreen.height()*2/3);
		}else{
			currentSize.setHeight(600);
		}
		if (rcScreen.width()*2/3>1000)
		{
			currentSize.setWidth(rcScreen.width()*2/3);
		}
		else{
			currentSize.setWidth(1000);
		}
		m_MainView->resize(currentSize);
		int nX=rcScreen.width()-currentSize.width();
		int nY=rcScreen.height()-currentSize.height();
		m_MainView->move(nX/2,nY/2);
	}
	else
	{
		/*m_MainView->showMaximized();*/
		m_MainView->showFullScreen();
	}
}

void settingsActivity::OnMinClick()
{
	if (verify(1000000000, 0)){
		return;
	}
	m_MainView->showMinimized();
}

void settingsActivity::OnMaxClick()
{
	if (m_MainView->isMaximized()||m_MainView->size().height()==QApplication::desktop()->screenGeometry().height()||m_MainView->size().width()==QApplication::desktop()->screenGeometry().width())
	{
		m_MainView->showNormal();
		QRect rcScreen = QApplication::desktop()->screenGeometry();

		QSize currentSize=m_MainView->size();
		currentSize.setHeight(rcScreen.height()*2/3);
		currentSize.setWidth(rcScreen.width()*2/3);
		m_MainView->resize(currentSize);
		int nX=rcScreen.width()-currentSize.width();
		int nY=rcScreen.height()-currentSize.height();
		m_MainView->move(nX/2,nY/2);
	}else
	{
		m_MainView->showMaximized();
	}
}

void settingsActivity::OnCloseClick()
{
	if (verify(1000000000, 0)){
		return;
	}
	m_MainView->close();
	qDebug()<<__FUNCTION__<<__LINE__<<"CLOSE CMS FROM QUIT BUTTON";
}

void settingsActivity::OnMouseDown()
{
	m_pos = QCursor::pos();
	m_bMouseTrace = true;
}

void settingsActivity::OnMouseUp()
{
	m_bMouseTrace = false;
}

void settingsActivity::OnMouseMove()
{
	int nMainFormXcoord = m_MainView->frameGeometry().topLeft().x();
	int nMainFormYcoord = m_MainView->frameGeometry().topLeft().y();
	if (m_bMouseTrace)
	{
		if (! m_MainView->isMaximized())
		{
			QPoint curPos = QCursor::pos();
			int dx = curPos.x() - m_pos.x();
			int dy = curPos.y() - m_pos.y();
			nMainFormXcoord += dx;
			nMainFormYcoord += dy;
			m_MainView->move(nMainFormXcoord,nMainFormYcoord);
			m_pos = curPos;
		}
	}
}


void settingsActivity::OnAddUserOk()
{
	IUserManager *Iuser = NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IUserManager,(void **)&Iuser);
	if (NULL == Iuser)
	{
		return;
	}
	QVariant sUserName = QueryValue("username_add_ID");
	if (Iuser->IsUserExists(sUserName.toString()))
	{
		DEF_EVENT_PARAM(arg);
		EP_ADD_PARAM(arg,"fail","AddUserFail");
		EventProcCall("AddUserFail",arg);
		return;
	}

	QVariant sPassWd = QueryValue("password_add_ID");
	QVariant sPassWd2 = QueryValue("password_add_again_ID");
	QVariant sLevel = QueryValue("level_add_ID");

	QVariant nMask1 = QueryValue("mask1_ID");
	QVariant nMask2 = QueryValue("mask2_ID");

	int nLevel = sLevel.toInt();
	//int nMask1 = 0xFFFFFFFF;
	//int nMask2 = 0xFFFFFFFF;
	bool bRes = Iuser->AddUser(sUserName.toString(),sPassWd.toString(),nLevel,nMask1.toInt(),nMask2.toInt());

	if (!bRes)
	{
		DEF_EVENT_PARAM(arg);
		EP_ADD_PARAM(arg,"success","AddUserSuccess");
		EventProcCall("AddUserSuccess",arg);
	}
	else
	{
		DEF_EVENT_PARAM(arg);
		EP_ADD_PARAM(arg,"fail","AddUserFail");
		EventProcCall("AddUserFail",arg);
	}

	Iuser->Release();
}

void settingsActivity::OnModifyUserOk()
{
	IUserManager *Iuser = NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IUserManager,(void **)&Iuser);
	if (NULL == Iuser)
	{
		return;
	}
	QVariant sUserName = QueryValue("username_modify_ID");
	QVariant sOldPassWd = QueryValue("password_modify_old_user_ID");
	//检查输入密码是否正确
	if (Iuser->CheckUser(sUserName.toString(),sOldPassWd.toString()))
	{
		QVariant sNewPassWd = QueryValue("password_modify_new_user_ID");
		QVariant sNewPassWd2 = QueryValue("password_modify_new_again_user_ID");
		//获取combox里的权限值，修改权限
		QVariant sLevel = QueryValue("level_modify_ID");
		int nLevel = sLevel.toInt();
		if (!(sNewPassWd.toString().isEmpty() || sNewPassWd2.toString().isEmpty()))
		{
			bool bRes2 = Iuser->ModifyUserPassword(sUserName.toString(),sNewPassWd.toString());	
			if (!bRes2)
			{
				DEF_EVENT_PARAM(arg);
				EP_ADD_PARAM(arg,"success","Modify User Password Success");
				EventProcCall("ModifyUserSuccess",arg);
			}else
			{
				DEF_EVENT_PARAM(arg);
				EP_ADD_PARAM(arg,"fail","Modify User Password Fail");
				EventProcCall("ModifyUserFail",arg);
			}
		}
		bool bRes = Iuser->ModifyUserLevel(sUserName.toString(),nLevel);
		if (!bRes)
		{
			DEF_EVENT_PARAM(arg);
			EP_ADD_PARAM(arg,"success","Modify User Level Success");
			EventProcCall("ModifyUserSuccess",arg);
		}else
		{
			DEF_EVENT_PARAM(arg);
			EP_ADD_PARAM(arg,"fail","Modify User Level Fail");
			EventProcCall("ModifyUserLevelFaild",arg);
		}	
	}
	else
	{
		//event
		DEF_EVENT_PARAM(arg);
		EP_ADD_PARAM(arg,"fail","Modify User Fail");
		EventProcCall("ModifyUserFail",arg);
	}
	Iuser->Release();
}

void settingsActivity::OnDeleteUserOk()
{
	IUserManager *Iuser = NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IUserManager,(void **)&Iuser);
	//获取用户列表
	QVariant sUser = QueryValue("username_list_ID");
	QStringList user_list = sUser.toString().split(",");
	for (int i = 0;i<user_list.size();i++)
	{
		sUser = user_list.at(i).toLatin1().data();
		bool bRes = Iuser->RemoveUser(sUser.toString());
		if (!bRes)
		{
			DEF_EVENT_PARAM(arg);
			EP_ADD_PARAM(arg,"username",sUser.toString());
			EP_ADD_PARAM(arg,"success","success");
			EventProcCall("DeleteUserSuccess",arg);
		}else
		{
			DEF_EVENT_PARAM(arg);
			EP_ADD_PARAM(arg,"username",sUser.toString());
			EP_ADD_PARAM(arg,"fail","fail");
			EventProcCall("DeleteUserFail",arg);
		}
	}
	Iuser->Release();
}
/*device module*/
void settingsActivity::OnAddDeviceDouble()
{
	if (verify(100000, 0)){
		return;
	}
	int nRet_id;
	IDeviceManager *Idevice=NULL;
	IAreaManager *Iarea=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void**)&Iarea);
	//==================
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;

	//==================

	DEF_EVENT_PARAM(arg);
	QString Content="system fail";
	QString State_num="0";
	QString sDevice_Name="0";

	if(NULL==Idevice||NULL==Iarea){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		return;
	}

	QVariant DevListFile=QueryValue("adddevicedouble_ID");
	QDomDocument ConfFile;
	ConfFile.setContent(DevListFile.toString());

	QDomNode DevListInfoNode=ConfFile.elementsByTagName("devListInfo").at(0);
	QDomNodeList itemList=DevListInfoNode.childNodes();
	if(0==itemList.count()){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		return;
	}
	int n;
	//判断添加的区域是否存在
	int Area_ID=ConfFile.firstChild().toElement().attribute("area_id").toInt();
	bool nRet_bool=false;
	nRet_bool=Iarea->IsAreaIdExist(Area_ID);
	if(false==nRet_bool){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		return;
	}
	total=itemList.count();
	for (n=0;n<itemList.count();n++)
	{
		QDomNode item;
		item= itemList.at(n);
		//搜索返回的参数
		QString SearchVendor_ID=item.toElement().attribute("SearchVendor_ID");
		QString SearchDeviceName_ID=item.toElement().attribute("SearchDeviceName_ID");
		QString SearchDeviceId_ID=item.toElement().attribute("SearchDeviceId_ID");
		QString SearchDeviceModelId_ID=item.toElement().attribute("SearchDeviceModelId_ID");
		QString SearchSeeId_ID=item.toElement().attribute("SearchSeeId_ID");
		QString SearchChannelCount_ID=item.toElement().attribute("SearchChannelCount_ID");
		QString SearchIP_ID=item.toElement().attribute("SearchIP_ID");
		QString SearchMask_ID=item.toElement().attribute("SearchMask_ID");
		QString SearchMac_ID=item.toElement().attribute("SearchMac_ID");
		QString SearchGateway_ID=item.toElement().attribute("SearchGateway_ID");
		QString SearchHttpport_ID=item.toElement().attribute("SearchHttpport_ID");
		QString SearchMediaPort_ID=item.toElement().attribute("SearchMediaPort_ID");

		//添加的默认参数

		QString	UserName_ID=item.toElement().attribute("username");
		QString	PassWord_ID=item.toElement().attribute("password");
		//设置默认参数
		QString ConnectMethod="0";
		SearchDeviceName_ID.clear();
		if (0!=SearchSeeId_ID.size()&&SearchSeeId_ID.toInt()>2&&false==SearchSeeId_ID.isNull())
		{
			SearchDeviceName_ID.append(SearchSeeId_ID);
		}
		else 
		{
			SearchDeviceName_ID.append(SearchIP_ID);
		}



		if (0==UserName_ID.size()||UserName_ID.isNull())
		{
			UserName_ID.append("admin");
		}


		//默认IP模式连接，判断ip模式连接的参数是否齐全

		if(SearchIP_ID.isNull()||SearchHttpport_ID.isNull()||SearchMediaPort_ID.isNull()){
			continue;
		}

		nRet_id=Idevice->AddDevice(Area_ID,SearchDeviceName_ID,SearchIP_ID,SearchMediaPort_ID.toInt(),SearchHttpport_ID.toInt(),SearchSeeId_ID,UserName_ID,PassWord_ID,SearchChannelCount_ID.toInt(),ConnectMethod.toInt(),SearchVendor_ID);
		if(-1==nRet_id){

			continue;
		}

		devId.append(QString("%1").arg(nRet_id));
		devId.append(";");
		continue;
	}

	if(NULL!=Idevice){Idevice->Release();}
	if(NULL!=Iarea){Iarea->Release();}
	EP_ADD_PARAM(bcitem,"total",total);
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("AddDeviceFeedBackSuccess",bcitem);
}

void settingsActivity::OnAddDevice()
{	
	if (verify(100000, 0)){
		return;
	}
	int nRet_id;
	IDeviceManager *Idevice=NULL;
	IAreaManager *Iarea=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void**)&Iarea);
	//==================
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;
	//==================
	if(NULL==Idevice||NULL==Iarea){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		return;
	}
	QVariant Area_Id=QueryValue("area_id_ID");
	QVariant sDeviceName=QueryValue("device_name_ID");
	QVariant sAddress=QueryValue("address_ID");
	QVariant port=QueryValue("port_ID");
	QVariant http=QueryValue("http_ID");
	QVariant sEseeId=QueryValue("eseeid_ID");
	QVariant sUserName=QueryValue("username_ID");
	QVariant sPassWord=QueryValue("password_ID");
	QVariant chlCount=QueryValue("channel_count_ID");
	QVariant ConnectMethod=QueryValue("method_ID");
	QVariant sVendor=QueryValue("vendor_ID");

	DEF_EVENT_PARAM(arg);
	QString Content;

	bool nRet_bool=false;
	nRet_bool=Iarea->IsAreaIdExist(Area_Id.toInt());
	if(false==nRet_bool){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccessSuccess",bcitem);
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		return;
	}
	//设备名为空时，使用易视网id填充，易视网为空时，使用ip地址填充
	if (sDeviceName.Size==0||sDeviceName=="")
	{
		if (sEseeId.isNull()==false&&sEseeId!="")
		{
			sDeviceName=sEseeId;
		}
		else{
			sDeviceName=sAddress;
		}
	}
	if(sDeviceName.toString().isEmpty()||sUserName.toString().isEmpty()||chlCount.toString().isEmpty()||ConnectMethod.toString().isEmpty()||sVendor.toString().isEmpty()){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		return;
	}
	if("0"==ConnectMethod.toString()){
		if(sAddress.isNull()||port.isNull()||http.isNull()){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("AddDeviceFeedBackSuccess",bcitem);
			if(NULL!=Idevice){Idevice->Release();}
			if(NULL!=Iarea){Iarea->Release();}
			return;
		}
		nRet_id=Idevice->AddDevice(Area_Id.toInt(),sDeviceName.toString(),sAddress.toString(),port.toInt(),http.toInt(),sEseeId.toString(),sUserName.toString(),sPassWord.toString(),chlCount.toInt(),ConnectMethod.toInt(),sVendor.toString());
		if(-1==nRet_id){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("AddDeviceFeedBackSuccess",bcitem);
			if(NULL!=Idevice){Idevice->Release();}
			if(NULL!=Iarea){Iarea->Release();}
			return;
		}
		goto end1;
	}
	else if("1"==ConnectMethod.toString()){
		if(sEseeId.isNull()){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("AddDeviceFeedBackSuccess",bcitem);
			if(NULL!=Idevice){Idevice->Release();}
			if(NULL!=Iarea){Iarea->Release();}
			return;
		}
		nRet_id=Idevice->AddDevice(Area_Id.toInt(),sDeviceName.toString(),sAddress.toString(),port.toInt(),http.toInt(),sEseeId.toString(),sUserName.toString(),sPassWord.toString(),chlCount.toInt(),ConnectMethod.toInt(),sVendor.toString());
		if(nRet_id>0){
			goto end1;
		}else{
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("AddDeviceFeedBackSuccess",bcitem);
			if(NULL!=Idevice){Idevice->Release();}
			if(NULL!=Iarea){Iarea->Release();}
			return;
		}
	}

	EP_ADD_PARAM(bcitem,"total",total);
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("AddDeviceFeedBackSuccess",bcitem);
	if(NULL!=Idevice){Idevice->Release();}
	if(NULL!=Iarea){Iarea->Release();}
	return;
end1:
	devId.append(QString("%1").arg(nRet_id));
	EP_ADD_PARAM(bcitem,"total",1);
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("AddDeviceFeedBackSuccess",bcitem);
	if(NULL!=Idevice){Idevice->Release();}
	if(NULL!=Iarea){Iarea->Release();}
	return;
}
void settingsActivity::OnRemoveDevice()
{
	if (verify(100000, 0)){
		return;
	}

	IDeviceManager *Idevice=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	//
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;
	//

	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==Idevice){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("RemoveDeviceFeedBackSuccess",bcitem);
		return;
	}


	QVariant Dev_Id=QueryValue("dev_id_ID");
	bool nRet_bool=false;
	nRet_bool=Idevice->IsDeviceExist(Dev_Id.toInt());
	if(false==nRet_bool){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("RemoveDeviceFeedBackSuccess",bcitem);
		Idevice->Release();
		return;
	}
	int nRet_int=-1;
	nRet_int=Idevice->RemoveDevice(Dev_Id.toInt());
	if(0!=nRet_int){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("RemoveDeviceFeedBackSuccess",bcitem);
		Idevice->Release();
		return;
	}

	devId.append(QString("%1").arg(Dev_Id.toInt()));
	EP_ADD_PARAM(bcitem,"total",1);
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("RemoveDeviceFeedBackSuccess",bcitem);
	Idevice->Release();
	return;
}
void settingsActivity::OnModifyDeviceEx()
{
	if (verify(100000, 0)){
		return;
	}

	IDeviceManager *Idevice=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;
	//==================

	if(NULL==Idevice){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
		return;
	}
	QVariant Dev_Id=QueryValue("dev_id_ID_Ex");
	bool nRet_bool=Idevice->IsDeviceExist(Dev_Id.toInt());
	if(false==nRet_bool){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
		Idevice->Release();
		return;
	}
	QVariant sAddress=QueryValue("address_ID_Ex");
	QVariant port=QueryValue("port_ID_Ex");
	QVariant http=QueryValue("http_ID_Ex");
	if(false==sAddress.isNull()&&false==port.isNull()&&false==http.isNull()){
		int nRet_int=Idevice->ModifyDeviceHost(Dev_Id.toInt(),sAddress.toString(),port.toInt(),http.toInt());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}
	EP_ADD_PARAM(bcitem,"total",1);
	devId.append(QString("%1").arg(Dev_Id.toInt()));
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
	Idevice->Release();
	return;
}
void settingsActivity::OnModifyDevice()
{
	if (verify(100000, 0)){
		return;
	}

	IDeviceManager *Idevice=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	//==================
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;
	//==================

	if(NULL==Idevice){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
		return;
	}

	QVariant Dev_Id=QueryValue("dev_id_ID");
	bool nRet_bool=Idevice->IsDeviceExist(Dev_Id.toInt());
	if(false==nRet_bool){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
		Idevice->Release();
		return;
	}

	QVariant sDeviceName=QueryValue("device_name_ID");
	if(false==Dev_Id.isNull()&&false==sDeviceName.isNull()){
		int nRet_int=Idevice->ModifyDeviceName(Dev_Id.toInt(),sDeviceName.toString());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}
	QVariant sUsername=QueryValue("username_ID");
	QVariant sPassword=QueryValue("password_ID");
	if(false==sUsername.isNull()){
		int nRet_int=Idevice->ModifyDeviceAuthority(Dev_Id.toInt(),sUsername.toString(),sPassword.toString());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}

	QVariant chlCount=QueryValue("channel_count_ID");
	if(false==chlCount.isNull()){
		int nRet_int=Idevice->ModifyDeviceChannelCount(Dev_Id.toInt(),chlCount.toInt());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}

	QVariant sVentor=QueryValue("vendor_ID");
	if(false==sVentor.isNull()){
		int nRet_int=Idevice->ModifyDeviceVendor(Dev_Id.toInt(),sVentor.toString());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}

	QVariant sEseeId=QueryValue("eseeid_ID");
	if(false==sEseeId.isNull()){
		int nRet_int=Idevice->ModifyDeviceEseeId(Dev_Id.toInt(),sEseeId.toString());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}

	QVariant sAddress=QueryValue("address_ID");
	QVariant port=QueryValue("port_ID");
	QVariant http=QueryValue("http_ID");
	if(false==sAddress.isNull()&&false==port.isNull()&&false==http.isNull()){
		int nRet_int=Idevice->ModifyDeviceHost(Dev_Id.toInt(),sAddress.toString(),port.toInt(),http.toInt());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}

	QVariant connect_method=QueryValue("method_ID");
	if (0==connect_method.toInt()||1==connect_method.toInt())
	{
		int nRet_int=Idevice->ModifyDeviceConnectMethod(Dev_Id.toInt(),connect_method.toInt());
		if(0!=nRet_int){
			EP_ADD_PARAM(bcitem,"total",total);
			EP_ADD_PARAM(bcitem,"succeedId",devId);
			EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
			Idevice->Release();
			return;
		}
	}

	EP_ADD_PARAM(bcitem,"total",1);
	devId.append(QString("%1").arg(Dev_Id.toInt()));
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("ModifyDeviceFeedBackSuccess",bcitem);
	Idevice->Release();
	return;
}

/*Group Module*/
void settingsActivity::OnAddGroup()
{
	if (verify(100000, 0)){
		return;
	}

	int nRet_id;
	IGroupManager *Igroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void**)&Igroup);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==Igroup){
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddGroupFail",arg);
		return;
	}
	QVariant sName_Id=QueryValue("group_name_ID");

	if(sName_Id.isNull()){
		Content.clear();
		arg.clear();
		Content.append("the params are not complete");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddGroupFail",arg);
		Igroup->Release();
		return;
	}

	nRet_id=Igroup->AddGroup(sName_Id.toString());
	if(-1==nRet_id){
		Content.clear();
		arg.clear();
		Content.append("AddDeviceFail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddDeviceFail",arg);
		Igroup->Release();
		return;
	}
	Content.clear();
	arg.clear();
	//Content.append("add group success");
	//EP_ADD_PARAM(arg,"success",Content);
	//EventProcCall("AddGroupSuccess",arg);
	QString nSret=QString("%1").arg(nRet_id);
	Content.append(nSret);
	EP_ADD_PARAM(arg,"groupid",Content);
	EventProcCall("AddGroupSuccess",arg);
	Igroup->Release();
	return;
}

void settingsActivity::OnModifyGroup()
{
	if (verify(100000, 0)){
		return;
	}

	IGroupManager *Igroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void**)&Igroup);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if (NULL==Igroup)
	{
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyGroupFail",arg);
		return;
	}
	QVariant Group_id=QueryValue("group_id_ID");
	QVariant Group_name=QueryValue("group_name_ID");
	bool nRet_bool=false;
	nRet_bool=Igroup->IsGroupExists(Group_id.toInt());

	if (false==nRet_bool)
	{
		arg.clear();
		Content.clear();
		Content.append("Group_Id is not exist");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyGroupFail",arg);
		Igroup->Release();
		return;
	}
	int nret_0=Igroup->ModifyGroupName(Group_id.toInt(),Group_name.toString());
	if (0!=nret_0)
	{
		arg.clear();
		Content.clear();
		Content.append("ModifyGroupFail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyGroupFail",arg);
		Igroup->Release();
		return;
	}
	arg.clear();
	Content.clear();
	Content.append("ModifyGroupSuccess");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("ModifyGroupSuccess",arg);
	Igroup->Release();

	return;
}

void settingsActivity::OnRemoveGroup()
{	
	if (verify(100000, 0)){
		return;
	}

	IGroupManager *Igroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void**)&Igroup);
	DEF_EVENT_PARAM(arg);
	QString Content="";
	if(NULL==Igroup){
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("RemoveGroupFail",arg);
		return;
	}

	QVariant Group_Id=QueryValue("group_id_ID");
	bool nRet_bool=false;
	nRet_bool=Igroup->IsGroupExists(Group_Id.toInt());
	if(false==nRet_bool){
		arg.clear();
		Content.clear();
		Content.append("Group_Id is not exist");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("RemoveGroupFail",arg);
		Igroup->Release();
		return;
	}

	int nRet_int = -1;
	nRet_int = Igroup->RemoveGroup(Group_Id.toInt());
	if(0!=nRet_int){
		arg.clear();
		Content.clear();
		Content.append("remove fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("RemoveGroupFail",arg);
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("Remove Success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("RemoveGroupSuccess",arg);
	Igroup->Release();
	return;
}

/*Area Module*/
void settingsActivity::OnAddArea()
{
	if (verify(100000, 0)){
		return;
	}

	int nRet_id;
	IAreaManager *Iarea=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void**)&Iarea);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==Iarea){
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddAreaFail",arg);
		return;
	}

	QVariant nPid_Id=QueryValue("pid_ID");
	QVariant sName_Id=QueryValue("area_name_ID");
	 nRet_id=Iarea->AddArea(nPid_Id.toInt(),sName_Id.toString());
	if(-1==nRet_id){
		Content.clear();
		arg.clear();
		Content.append("AddArea Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddAreaFail",arg);
		Iarea->Release();
		return;
	}
	Content.clear();
	arg.clear();
	//Content.append("AddArea success");
	//EP_ADD_PARAM(arg,"success",Content);
	//EventProcCall("AddAreaSuccess",arg);
	QString nSret=QString("%1").arg(nRet_id);
	Content.append(nSret);
	EP_ADD_PARAM(arg,"areaid",Content);
	EventProcCall("AddAreaSuccess",arg);
	Iarea->Release();
	return;
}

void settingsActivity::OnRemoveArea()
{
	if (verify(100000, 0)){
		return;
	}

	IAreaManager *Iarea=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void **)&Iarea);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==Iarea){
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("RemoveAreaFail",arg);
		return;
	}
	QVariant Area_id_ID=QueryValue("area_id_ID");
	QVariant Area_Name_ID=QueryValue("area_name_ID");
	int nRet1=Iarea->RemoveAreaById(Area_id_ID.toInt());
	int nRet2=Iarea->RemoveAreaByName(Area_Name_ID.toString());
	if(0==nRet1||0==nRet2){
		arg.clear();
		Content.clear();
		Content.append("remove success");
		EP_ADD_PARAM(arg,"success",Content);
		EventProcCall("RemoveAreaSuccess",arg);
		Iarea->Release();
		return;
	}
	arg.clear();
	Content.clear();
	Content.append("remove fail");
	EP_ADD_PARAM(arg,"fail",Content);
	EventProcCall("RemoveAreaFail",arg);
	Iarea->Release();
	return;
}

void settingsActivity::OnModifyArea()
{
	if (verify(100000, 0)){
		return;
	}

	IAreaManager *IArea=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void **)&IArea);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==IArea){
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyAreaFail",arg);
		return;
	}

	QVariant Area_id_ID=QueryValue("area_id_ID");
	QVariant Area_Name_ID=QueryValue("area_name_ID");

	int nRet=-1;
	nRet=IArea->SetAreaName(Area_id_ID.toInt(),Area_Name_ID.toString());
	if(0==nRet){
		arg.clear();
		Content.clear();
		Content.append("modify area success");
		EP_ADD_PARAM(arg,"success",Content);
		EventProcCall("ModifyAreaSuccess",arg);
		IArea->Release();
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("modify area fail");
	EP_ADD_PARAM(arg,"fail",Content);
	EventProcCall("ModifyAreaFail",arg);
	IArea->Release();
	return;
}

/*Channel Module*/
void settingsActivity::OnAddChannel()
{
	//useless
	return;
}

void settingsActivity::OnModifyChannel()
{
	if (verify(100000, 0)){
		return;
	}

	IChannelManager *IChannel=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IChannelManager,(void **)&IChannel);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==IChannel){
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyChannelFail",arg);
		return;
	}

	QVariant Channel_id_ID=QueryValue("channel_id_ID");
	QVariant Stream_id_ID=QueryValue("stream_id_ID");
	QVariant Channel_Name_ID=QueryValue("channel_name_ID");

	int nRet1=-1;
	int nRet2=-1;

	nRet1=IChannel->ModifyChannelName(Channel_id_ID.toInt(),Channel_Name_ID.toString());
	nRet2=IChannel->ModifyChannelStream(Channel_id_ID.toInt(),Stream_id_ID.toInt());

	if(0!=nRet1&&0!=nRet2){
		arg.clear();
		Content.clear();
		Content.append("modify channel fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyChannelFail",arg);
		IChannel->Release();
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("modify channel success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("ModifyChannelSuccess",arg);
	IChannel->Release();
	return;
}

void settingsActivity::OnRemoveChannel()
{
	//useless
	return;
}
void settingsActivity::OnAddChannelInGroupDouble()
{
	if (verify(100000, 0)){
		return;
	}

	int nRet_id=-1;
    Q_UNUSED(nRet_id);
	IGroupManager *IGroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void**)&IGroup);
	DEF_EVENT_PARAM(arg);
	QString Content;
	QString ChannelName;
	QString chlid;
	//申请接口判定
	if(NULL==IGroup){
		arg.clear();
		Content.clear();
		ChannelName.clear();
		chlid.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EP_ADD_PARAM(arg,"chlid",chlid);
		EP_ADD_PARAM(arg,"channelname",ChannelName);
		EventProcCall("AddChannelDoubleInGroupFail",arg);
		return;
	}

	QVariant ChlintoGroupFile=QueryValue("addchannelingroupdouble_ID");
	QDomDocument ConfFile;
	ConfFile.setContent(ChlintoGroupFile.toString());

	QDomNode ChlintoGroupInfoNode=ConfFile.elementsByTagName("chlintogroup").at(0);
	QDomNodeList itemList=ChlintoGroupInfoNode.childNodes();

	//若选择的通道号 为零，返回错误提示
	if(0==itemList.count()){
		arg.clear();
		Content.clear();
		ChannelName.clear();
		Content.append("please choose the channel");
		EP_ADD_PARAM(arg,"fail",Content);
		EP_ADD_PARAM(arg,"chlid",chlid);
		EP_ADD_PARAM(arg,"channelname",ChannelName);
		EventProcCall("AddChannelDoubleInGroupFail",arg);
		if(NULL!=IGroup){IGroup->Release();}
		return;
	}
	//读取组id号
	int Group_ID=ConfFile.firstChild().toElement().attribute("group_id").toInt(); 
	bool nRet_bool=false;
	nRet_bool=IGroup->IsGroupExists(Group_ID);
	if (false==nRet_bool)
	{
		Content.clear();
		Content.append("Group is not exist");
		arg.clear();
		EP_ADD_PARAM(arg,"fail",Content);
		EP_ADD_PARAM(arg,"chlid",chlid);
		EP_ADD_PARAM(arg,"channelname",ChannelName);
		EventProcCall("AddChannelDoubleInGroupFail",arg);
		if(NULL!=IGroup){IGroup->Release();}
		return;
	}
	//读取xml的节点，添加通道到组
	int n=0;
	int nRet_chl_group_id=-1;
	for(n=0;n<itemList.count();n++){
		QDomNode item;
		item=itemList.at(n);
		QString Channel_id_ID=item.toElement().attribute("channel_id_ID");
		QString R_Chl_Group_Name_ID=item.toElement().attribute("channel_name_ID");

		nRet_chl_group_id=IGroup->AddChannelInGroup(Group_ID,Channel_id_ID.toInt(),R_Chl_Group_Name_ID);
		if (-1==nRet_chl_group_id)
		{
			arg.clear();
			Content.clear();
			ChannelName.clear();
			chlid.clear();
			chlid.append(Channel_id_ID);
			Content.append("AddChannelDoubleInGroupFail");
			ChannelName.append(R_Chl_Group_Name_ID);
			EP_ADD_PARAM(arg,"fail",Content);
			EP_ADD_PARAM(arg,"chlid",chlid);
			EP_ADD_PARAM(arg,"channelname",ChannelName);
			EventProcCall("AddChannelDoubleInGroupFail",arg);
		}

		arg.clear();
		Content.clear();
		chlid.clear();
		ChannelName.clear();
		chlid.append(Channel_id_ID);
		QString nSret=QString("%1").arg(nRet_chl_group_id);
		Content.append(nSret);
		ChannelName.append(R_Chl_Group_Name_ID);

		EP_ADD_PARAM(arg,"chlgroupid",Content);
		EP_ADD_PARAM(arg,"chlid",chlid);
		EP_ADD_PARAM(arg,"channelname",ChannelName);
		EventProcCall("AddChannelDoubleInGroupSuccess",arg);
	}
	if(NULL!=IGroup){IGroup->Release();}
	return ;
}
void settingsActivity::OnAddChannelInGroup()
{
	if (verify(100000, 0)){
		return;
	}

	int nRet_id=-1;
	IGroupManager *IGroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void**)&IGroup);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==IGroup){
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddChannelInGroupFail",arg);
		return;
	}

	QVariant Group_id_ID=QueryValue("group_id_ID");
	QVariant Channel_id_ID=QueryValue("channel_id_ID");
	QVariant R_Chl_Group_Name_ID=QueryValue("channel_name_ID");


	nRet_id=IGroup->AddChannelInGroup(Group_id_ID.toInt(),Channel_id_ID.toInt(),R_Chl_Group_Name_ID.toString());
	if(-1==nRet_id){
		arg.clear();
		Content.clear();
		Content.append("AddChannelInGroup fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("AddChannelInGroupFail",arg);
		IGroup->Release();
		return;
	}
	arg.clear();
	Content.clear();
	//Content.append("AddChannelInGroup success");
	//EP_ADD_PARAM(arg,"success",Content);
	//EventProcCall("AddChannelInGroupSuccess",arg);
	QString nSret=QString("%1").arg(nRet_id);
	Content.append(nSret);
	EP_ADD_PARAM(arg,"chlgroupid",Content);
	EventProcCall("AddChannelInGroupSuccess",arg);
	IGroup->Release();
	return;
}

void settingsActivity::OnRemoveChannelFromGroup()
{
	if (verify(100000, 0)){
		return;
	}

	IGroupManager * IGroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void **)&IGroup);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==IGroup){
		arg.clear();
		Content.clear();
		Content.append("OnRemoveChannelFromGroup fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("RemoveChannelFromGroupFail",arg);
		return;
	}

	QVariant R_Chl_Group_id_ID=QueryValue("r_chl_group_id_ID");
	
	QStringList R_Chl_Group_id_ID_list=R_Chl_Group_id_ID.toString().split(",");

	QStringList::const_iterator it;
	bool n_input=false;
	for(it=R_Chl_Group_id_ID_list.begin();it!=R_Chl_Group_id_ID_list.end();++it){
		n_input=true;
		int r_ch_id=it->toInt();
		int nRet=-1;
		nRet=IGroup->RemoveChannelFromGroup(r_ch_id);
		if (-1==nRet)
		{
			arg.clear();
			Content.clear();
			Content.append("RemoveChannelFromGroup fail");
			EP_ADD_PARAM(arg,"fail",Content);
			EP_ADD_PARAM(arg,"r_chl_group_id_ID",r_ch_id);
			EventProcCall("RemoveChannelFromGroupFail",arg);
		}
		arg.clear();
		Content.clear();
		Content.append("RemoveChannelFromGroup success");
		EP_ADD_PARAM(arg,"success",Content);
		EP_ADD_PARAM(arg,"r_chl_group_id_ID",r_ch_id);
		EventProcCall("RemoveChannelFromGroupSuccess",arg);
	}
	if (true==n_input)
	{
		IGroup->Release();
		return;
	}
	arg.clear();
	Content.clear();
	Content.append("RemoveChannelFromGroup success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("RemoveChannelFromGroupSuccess",arg);
	IGroup->Release();
	return;
}

void settingsActivity::OnModifyGroupChannelName()
{
	if (verify(100000, 0)){
		return;
	}

	IGroupManager *IGroup=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IGroupManager,(void **)&IGroup);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if(NULL==IGroup){
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyGroupChannelNameFail",arg);
		return;
	}

	QVariant R_Chl_Group_id_ID=QueryValue("r_chl_group_id_ID");
	QVariant R_Chl_Group_Name_ID=QueryValue("channel_name_ID");

	int nRet=-1;

	nRet=IGroup->ModifyGroupChannelName(R_Chl_Group_id_ID.toInt(),R_Chl_Group_Name_ID.toString());
	if(0!=nRet){
		arg.clear();
		Content.clear();
		Content.append("ModifyGroupChannelName fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("ModifyGroupChannelNameFail",arg);
		IGroup->Release();
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("ModifyGroupChannelName success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("ModifyGroupChannelNameSuccess",arg);
	IGroup->Release();
	return;
}

void settingsActivity::OnSettingStorageParm()
{
	if (verify(10000000, 0)){
		return;
	}
	IDisksSetting *IDisk=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDiskSetting,(void**)&IDisk);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if (NULL==IDisk)
	{
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingStorageParmFail",arg);
		return;
	}
	QVariant sDisk_ID=QueryValue("UseDisks_ID");
	QVariant filesize_ID=QueryValue("FilePackageSize_ID");
	QVariant bcover_ID=QueryValue("LoopRecording_ID");
	QVariant spacereservedsize_ID=QueryValue("DiskSpaceReservedSize_ID");

	int nRet=-1;
	nRet=IDisk->setUseDisks(sDisk_ID.toString());
	if (IDisksSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setUseDisks Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingStorageParmFail",arg);
		IDisk->Release();
		return;
	}

	nRet=-1;
	nRet=IDisk->setFilePackageSize(/*filesize_ID.toInt()*/64);
	if (IDisksSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setFilePackageSize Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingStorageParmFail",arg);
		IDisk->Release();
		return;
	}

	nRet=-1;
	nRet=IDisk->setLoopRecording(bcover_ID.toBool());
	if (IDisksSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setLoopRecording Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingStorageParmFail",arg);
		IDisk->Release();
		return;
	}
	nRet=-1;
	nRet=IDisk->setDiskSpaceReservedSize(spacereservedsize_ID.toInt());
	if (IDisksSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setDiskSpaceReservedSize Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingStorageParmFail",arg);
		IDisk->Release();
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("SettingStorageParm success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("SettingStorageParmSuccess",arg);
	IDisk->Release();
	return;
}

void settingsActivity::OnSettingCommonParm()
{
	if (verify(10000000, 0)){
		return;
	}

	ILocalSetting *Ilocal=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_ILocalSetting,(void**)&Ilocal);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if (NULL==Ilocal)
	{
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		return;
	}
	QVariant sLanguage_ID=QueryValue("Language_ID");
	QVariant aptime_ID=QueryValue("AutoPollingTime_ID");
	QVariant smode_ID=QueryValue("SplitScreenMode_ID");
	QVariant alogin_ID=QueryValue("AutoLogin_ID");
	QVariant synctime_ID=QueryValue("AutoSyncTime_ID");
	QVariant aconnent_ID=QueryValue("AutoConnect_ID");
	QVariant afullscreen_ID=QueryValue("AutoFullscreen_ID");
	QVariant bootstart_ID=QueryValue("BootFromStart_ID");
	QVariant bootPersian_ID=QueryValue("BootPersian_ID");

	int nRet=-1;
	nRet=Ilocal->setLanguage(sLanguage_ID.toString());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setLanguage Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setAutoPollingTime(aptime_ID.toInt());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setAutoPollingTime Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setSplitScreenMode(smode_ID.toString());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setSplitScreenMode Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setAutoLogin(alogin_ID.toBool());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setAutoLogin Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setAutoSyncTime(synctime_ID.toBool());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setAutoSyncTime Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setAutoConnect(aconnent_ID.toBool());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setAutoConnect Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setAutoFullscreen(afullscreen_ID.toBool());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setAutoFullscreen Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setBootFromStart(bootstart_ID.toBool());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setAutoFullscreen Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	nRet=-1;
	nRet=Ilocal->setIsPersian(bootPersian_ID.toBool());
	if (ILocalSetting::OK!=nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("setPersian Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingCommonParmFail",arg);
		Ilocal->Release();
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("SettingCommonParm success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("SettingCommonParmSuccess",arg);
	Ilocal->Release();
	return;
}

void settingsActivity::OnSettingRecordTimeParm()
{
	if (verify(10000000, 0)){
		return;
	}

	ISetRecordTime *ISetRecord=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_ISetRecordTime,(void**)&ISetRecord);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if (NULL==ISetRecord)
	{
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingRecordTimeParmFail",arg);
		return;
	}
	QVariant recordtime_ID=QueryValue("recordtime_ID");
	QVariant starttime_ID=QueryValue("starttime_ID");
	QVariant endtime_ID=QueryValue("endtime_ID");
	QVariant enable_ID=QueryValue("enable_ID");

	int nRet=-1;
	nRet= ISetRecord->ModifyRecordTime(recordtime_ID.toInt(),starttime_ID.toString(),endtime_ID.toString(),enable_ID.toInt());
	if (1==nRet)
	{
		arg.clear();
		Content.clear();
		Content.append("ModifyRecordTime Fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingRecordTimeParmFail",arg);
		ISetRecord->Release();
		return;
	}

	arg.clear();
	Content.clear();
	Content.append("ModifyRecordTime success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("SettingRecordTimeParmSuccess",arg);
	ISetRecord->Release();
	return;
}

void settingsActivity::OnSettingRecordTimeParmDouble()
{
	if (verify(10000000, 0)){
		return;
	}

	ISetRecordTime *ISetRecord=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_ISetRecordTime,(void**)&ISetRecord);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if (NULL==ISetRecord)
	{
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingRecordDoubleTimeParmFail",arg);
		return;
	}

	QVariant RecordtimeFile=QueryValue("recordtimedouble_ID");
	QDomDocument ConfFile;
	ConfFile.setContent(RecordtimeFile.toString());
	QDomNode RecordtimeFileNode=ConfFile.elementsByTagName("recordtime").at(0);
	QDomNodeList itemList=RecordtimeFileNode.childNodes();
	if (0==itemList.count())
	{
		arg.clear();
		Content.clear();
		Content.append("parm is null");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingRecordDoubleTimeParmFail",arg);
		ISetRecord->Release();
		return;
	}
	for (int n=0;n<itemList.count();n++)
	{
		QDomNode item;
		item=itemList.at(n);
		QString recordtime_ID=item.toElement().attribute("recordtime_ID");
		QString starttime_ID=item.toElement().attribute("starttime_ID");
		QString endtime_ID=item.toElement().attribute("endtime_ID");
		QString enable_ID=item.toElement().attribute("enable_ID");
		int nRet=-1;
		nRet= ISetRecord->ModifyRecordTime(recordtime_ID.toInt(),starttime_ID,endtime_ID,enable_ID.toInt());
		if (1==nRet)
		{
			arg.clear();
			Content.clear();
			Content.append("ModifyRecordTime Fail");
			EP_ADD_PARAM(arg,"fail",Content);
			EventProcCall("SettingRecordDoubleTimeParmFail",arg);
			ISetRecord->Release();
			return;
		}
	}
	arg.clear();
	Content.clear();
	Content.append("ModifyRecordTime success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("SettingRecordDoubleTimeParmSuccess",arg);
	ISetRecord->Release();
	return;
}
void settingsActivity::OnAddDeviceALL()
{
	int nRet_id;
	IDeviceManager *Idevice=NULL;
	IAreaManager *Iarea=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void**)&Iarea);
	//==================
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;

	//==================

	if(NULL==Idevice||NULL==Iarea){
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		m_bIsAdding=false;
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		return;
	}

	QVariant DevListFile=QueryValue("adddeviceall_ID");
	QDomDocument ConfFile;
	ConfFile.setContent(DevListFile.toString());

	QDomNode DevListInfoNode=ConfFile.elementsByTagName("devListInfo").at(0);
	QDomNodeList itemList=DevListInfoNode.childNodes();
	if(0==itemList.count()){
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		m_bIsAdding=false;
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		return;
	}
	int n;
	//判断添加的区域是否存在
	int Area_ID=ConfFile.firstChild().toElement().attribute("area_id").toInt();
	bool nRet_bool=false;
	nRet_bool=Iarea->IsAreaIdExist(Area_ID);
	if(false==nRet_bool){
		if(NULL!=Idevice){Idevice->Release();}
		if(NULL!=Iarea){Iarea->Release();}
		m_bIsAdding=false;
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("AddDeviceFeedBackSuccess",bcitem);
		return;
	}
	total=itemList.count();
	for (n=0;n<itemList.count();n++)
	{
		QDomNode item;
		item= itemList.at(n);
		//搜索返回的参数
		QString SearchVendor_ID=item.toElement().attribute("SearchVendor_ID");
		QString SearchDeviceName_ID=item.toElement().attribute("SearchDeviceName_ID");
		QString SearchDeviceId_ID=item.toElement().attribute("SearchDeviceId_ID");
		QString SearchDeviceModelId_ID=item.toElement().attribute("SearchDeviceModelId_ID");
		QString SearchSeeId_ID=item.toElement().attribute("SearchSeeId_ID");
		QString SearchChannelCount_ID=item.toElement().attribute("SearchChannelCount_ID");
		QString SearchIP_ID=item.toElement().attribute("SearchIP_ID");
		QString SearchMask_ID=item.toElement().attribute("SearchMask_ID");
		QString SearchMac_ID=item.toElement().attribute("SearchMac_ID");
		QString SearchGateway_ID=item.toElement().attribute("SearchGateway_ID");
		QString SearchHttpport_ID=item.toElement().attribute("SearchHttpport_ID");
		QString SearchMediaPort_ID=item.toElement().attribute("SearchMediaPort_ID");
		//添加的默认参数

		QString	UserName_ID=item.toElement().attribute("username");
		QString	PassWord_ID=item.toElement().attribute("password");
		//设置默认参数
		QString ConnectMethod="0";
		SearchDeviceName_ID.clear();

		if (0!=SearchSeeId_ID.size()&&-1!=SearchSeeId_ID.toInt()&&false==SearchSeeId_ID.isNull())
		{
			SearchDeviceName_ID.append(SearchSeeId_ID);
		}
		else 
		{
			SearchDeviceName_ID.append(SearchIP_ID);
		}

		if (0==UserName_ID.size()||UserName_ID.isNull())
		{
			UserName_ID.append("admin");
		}


		//默认IP模式连接，判断ip模式连接的参数是否齐全

		if(SearchIP_ID.isNull()||SearchHttpport_ID.isNull()||SearchMediaPort_ID.isNull()){
			continue;
		}

		//添加设备
		nRet_id=Idevice->AddDevice(Area_ID,SearchDeviceName_ID,SearchIP_ID,SearchMediaPort_ID.toInt(),SearchHttpport_ID.toInt(),SearchSeeId_ID,UserName_ID,PassWord_ID,SearchChannelCount_ID.toInt(),ConnectMethod.toInt(),SearchVendor_ID);
		//		nRet_id=Idevice->AddDevice(Area_Id.toInt(),sDeviceName.toString(),sAddress.toString(),port.toInt(),http.toInt(),sEseeId.toString(),sUserName.toString(),sPassWord.toString(),chlCount.toInt(),ConnectMethod.toInt(),sVendor.toString());
		if(-1==nRet_id){
			continue;
		}
		devId.append(QString("%1").arg(nRet_id));
		devId.append(";");
		continue;
	}

	if(NULL!=Idevice){Idevice->Release();}
	if(NULL!=Iarea){Iarea->Release();}
	m_bIsAdding=false;
	EP_ADD_PARAM(bcitem,"total",total);
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("AddDeviceFeedBackSuccess",bcitem);
}

void settingsActivity::OnAddDeviceALLThread()
{
	if (verify(100000, 0)){
		return;
	}

	//if (m_bIsAdding!=true)
	//{
	//	IDeviceManager *Idevice=NULL;
	//	IAreaManager *Iarea=NULL;
	//	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);
	//	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IAreaManager,(void**)&Iarea);
	//	m_bIsAdding=true;
	//	QFuture<void>ret=QtConcurrent::run(this,&settingsActivity::OnAddDeviceALL,Idevice,Iarea);
	//}
	OnAddDeviceALL();
}
void settingsActivity::OnRemoveDeviceALLThread()
{
	if (verify(100000, 0)){
		return;
	}
	OnRemoveDeviceALL();
	/*QFuture<void>ret=QtConcurrent::run(this,&settingsActivity::OnRemoveDeviceALL);*/
}
void settingsActivity::OnRemoveDeviceALL()
{
	IDeviceManager *Idevice=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void**)&Idevice);

	//==================
	DEF_EVENT_PARAM(bcitem);
	int total=0;
	QString devId;
	//==================
	if(NULL==Idevice){
		EP_ADD_PARAM(bcitem,"total",total);
		EP_ADD_PARAM(bcitem,"succeedId",devId);
		EventProcCall("RemoveDeviceFeedBackSuccess",bcitem);
		return;
	}
	QVariant DevListFile=QueryValue("removedeviceall_ID");
	QString DevString=DevListFile.toString();
	int DevNum=DevString.count(",")+1;
	for (int n=0;n<DevNum;n++)
	{
		int liDevId=DevString.section(",",n,n).toInt();

		bool nRet_bool=false;
		nRet_bool=Idevice->IsDeviceExist(liDevId);
		if(false==nRet_bool){
			continue;
		}
		int nRet_int=-1;
		nRet_int=Idevice->RemoveDevice(liDevId);
		if(0!=nRet_int){
			continue;
		}
		devId.append(QString("%1").arg(liDevId));
		devId.append(";");
		total++;
	}
	Idevice->Release();

	EP_ADD_PARAM(bcitem,"total",total);
	EP_ADD_PARAM(bcitem,"succeedId",devId);
	EventProcCall("RemoveDeviceFeedBackSuccess",bcitem);
	return;
}

void settingsActivity::OnSettingRecordTimeParmCopy()
{

	ISetRecordTime *ISetRecord=NULL;
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_ISetRecordTime,(void**)&ISetRecord);
	DEF_EVENT_PARAM(arg);
	QString Content;
	if (NULL==ISetRecord)
	{
		arg.clear();
		Content.clear();
		Content.append("system fail");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingRecordCopyTimeParmFail",arg);
		return;
	}

	QVariant RecordtimeFile=QueryValue("recordtimeCopy_ID");
	QDomDocument ConfFile;
	ConfFile.setContent(RecordtimeFile.toString());
	QDomNode RecordtimeFileNode=ConfFile.elementsByTagName("recordtime").at(0);
	QDomNodeList itemList=RecordtimeFileNode.childNodes();
	if (0==itemList.count())
	{
		arg.clear();
		Content.clear();
		Content.append("parm is null");
		EP_ADD_PARAM(arg,"fail",Content);
		EventProcCall("SettingRecordCopyTimeParmFail",arg);
		ISetRecord->Release();
		return;
	}
	for (int n=0;n<itemList.count();n++)
	{
		QDomNode item;
		item=itemList.at(n);
		QString recordtime_ID=item.toElement().attribute("recordtime_ID");
		QString starttime_ID=item.toElement().attribute("starttime_ID");
		QString endtime_ID=item.toElement().attribute("endtime_ID");
		QString enable_ID=item.toElement().attribute("enable_ID");
		int nRet=-1;
		nRet= ISetRecord->ModifyRecordTime(recordtime_ID.toInt(),starttime_ID,endtime_ID,enable_ID.toInt());
		if (1==nRet)
		{
			arg.clear();
			Content.clear();
			Content.append("ModifyRecordTime Fail");
			EP_ADD_PARAM(arg,"fail",Content);
			EventProcCall("SettingRecordCopyTimeParmFail",arg);
			ISetRecord->Release();
			return;
		}
	}
	arg.clear();
	Content.clear();
	Content.append("ModifyRecordTime success");
	EP_ADD_PARAM(arg,"success",Content);
	EventProcCall("SettingRecordCopyTimeParmSuccess",arg);
	ISetRecord->Release();
	return;
}

void settingsActivity::OnAddUserExOk()
{
	if (verify(100000000, 0)){
		return;
	}
	IUserManagerEx *pUserMangerEx=NULL;
	pcomCreateInstance(CLSID_CommonlibEx,NULL,IID_IUserMangerEx,(void**)&pUserMangerEx);
	QString sRet;
	int nRet=-1;
	DEF_EVENT_PARAM(arg);
	if (NULL!=pUserMangerEx)
	{
		QVariant tAddUserFile=QueryValue("addUserEx_ID");
		QDomDocument tConfFile;
		tConfFile.setContent(tAddUserFile.toString());
		QDomNode tMainNode=tConfFile.elementsByTagName("main").at(0);
		QString sMainLimit=tMainNode.toElement().attribute("limitCode");
		QString sUserName=tMainNode.toElement().attribute("sUserName");
		QString sPassword=tMainNode.toElement().attribute("sPassword");
		QString sLogOutInterval=tMainNode.toElement().attribute("sLogOutInterval");
		QDomNodeList tItemList=tMainNode.childNodes();
		QVariantMap tVariantMap;
		for (int n=0;n<tItemList.count();n++)
		{
			QDomNode tItem;
			tItem=tItemList.at(n);
			QString sMainSubCode=tItem.toElement().attribute("mainCode");
			QString sSubCode=tItem.toElement().attribute("subCode");
			tVariantMap.insertMulti(sMainSubCode,sSubCode);
		}
		nRet =pUserMangerEx->addUser(sUserName,sPassword,sMainLimit.toLongLong(),sLogOutInterval.toUInt(),tVariantMap);
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"call abort as pUserMangerEx is null";
		abort();
	}
	if (NULL!=pUserMangerEx)
	{
		pUserMangerEx->Release();
		pUserMangerEx=NULL;
	}
	if (0==nRet)
	{
		sRet.append("AddUserSuccess");
		EP_ADD_PARAM(arg,"success",sRet);
		EventProcCall("AddUserSuccess",arg);
	}else{
		sRet.append("AddUserFail");
		EP_ADD_PARAM(arg,"fail",sRet);
		EventProcCall("AddUserFail",arg);
	}
}

void settingsActivity::OnModifyUserExOk()
{
	if (verify(100000000, 0)){
		return;
	}
	IUserManagerEx *pUserMangerEx=NULL;
	pcomCreateInstance(CLSID_CommonlibEx,NULL,IID_IUserMangerEx,(void**)&pUserMangerEx);
	QString sRet;
	DEF_EVENT_PARAM(arg);
	int nRet=-1;
	if (NULL!=pUserMangerEx)
	{
		QVariant tModifyUserFile=QueryValue("modifyUserEx_ID");
		QDomDocument tConfFile;
		tConfFile.setContent(tModifyUserFile.toString());
		QDomNode tModifyUserNode=tConfFile.elementsByTagName("modify").at(0);
		QDomNodeList tModifyUserNodeList=tModifyUserNode.childNodes();
		for(int n=0;n<tModifyUserNodeList.count();n++){
			QDomNode tModifyOneUserNode=tModifyUserNodeList.at(n);
			QString sOldUserName=tModifyOneUserNode.toElement().attribute("sOlduserName");
			QString sNewUserName=tModifyOneUserNode.toElement().attribute("sNewUserName");
			QString sNewPassword=tModifyOneUserNode.toElement().attribute("sNewPassword");
			QString sNewLimit=tModifyOneUserNode.toElement().attribute("uiNewLimit");
			QString sLogOutInterval=tModifyOneUserNode.toElement().attribute("sLogOutInterval");
			QDomNodeList tModifyOneUserNodeList=tModifyOneUserNode.childNodes();
			QVariantMap tVariantMap;
			for (int i=0;i<tModifyOneUserNodeList.count();i++)
			{
				QDomNode tSubLimtNode=tModifyOneUserNodeList.at(i);
				QString tMainCode=tSubLimtNode.toElement().attribute("mainCode");
				QString tSubLimit=tSubLimtNode.toElement().attribute("subLimit");
				tVariantMap.insertMulti(tMainCode,tSubLimit);
			}
			int nUserId=-1;
			pUserMangerEx->getUserDatabaseId(sOldUserName,nUserId);
			if (nUserId!=-1)
			{
				if (verify(100000000, nUserId)){
					if(NULL!=pUserMangerEx){
						pUserMangerEx->Release();
						pUserMangerEx=NULL;
					}
					return;
				}
				nRet=pUserMangerEx->modifyUserInfo(sOldUserName,sNewUserName,sNewPassword,sNewLimit.toULongLong(),sLogOutInterval.toUInt(),tVariantMap);
				if (nRet==0)
				{
					sRet.clear();
					sRet.append("ModifyUserSuccess");
					EP_ADD_PARAM(arg,"success",sRet);
					EventProcCall("ModifyUserSuccess",arg);
				}else{
					sRet.clear();
					sRet.append("ModifyUserFail");
					EP_ADD_PARAM(arg,"fail",sRet);
					EventProcCall("ModifyUserFail",arg);
				}
			}else{
				//keep going
				qDebug()<<__FUNCTION__<<__LINE__<<"there is no such user";
				sRet.clear();
				sRet.append("ModifyUserFail");
				EP_ADD_PARAM(arg,"fail",sRet);
				EventProcCall("ModifyUserFail",arg);
			}
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"call abort as pUserMangerEx is null";
		abort();
	}
	if(NULL!=pUserMangerEx){
		pUserMangerEx->Release();
		pUserMangerEx=NULL;
	}
}
void settingsActivity::OnModifyCurrentUserExOk()
{
	IUserManagerEx *pUserMangerEx=NULL;
	pcomCreateInstance(CLSID_CommonlibEx,NULL,IID_IUserMangerEx,(void**)&pUserMangerEx);
	QString sRet;
	DEF_EVENT_PARAM(arg);
	if (NULL!=pUserMangerEx)
	{
		QVariant tModiyCurrentUserInfoFile=QueryValue("modifyCurrentUserEx_ID");
		QDomDocument tConfFile;
		tConfFile.setContent(tModiyCurrentUserInfoFile.toString());
		QDomNode tModifyNode=tConfFile.elementsByTagName("modifyCurrentUserInfo").at(0);
		QString sOldPassword=tModifyNode.toElement().attribute("sOldPassword");
		QString sNewPassword=tModifyNode.toElement().attribute("sNewPassword");
		QString sLogOutInterval=tModifyNode.toElement().attribute("sLogOutInterval");
		QString sNewUserName=tModifyNode.toElement().attribute("sNewUserName");
		QString sOldeUserName=pUserMangerEx->getCurrentUser();
		if (!sOldeUserName.isEmpty())
		{
			int nRet=1;
			nRet=pUserMangerEx->modifyCurrentUserInfo(sOldeUserName,sNewUserName,sOldPassword,sNewPassword,sLogOutInterval.toInt());
			if (nRet==1)
			{
				//设置失败
				sRet.append("ModifyCurrentLoginUserFail");
				EP_ADD_PARAM(arg,"fail",sRet);
				EventProcCall("ModifyCurrentLoginUserFail",arg);
			}else{
				//设置成功
				sRet.append("ModifyCurrentLoginUserSuccess");
				EP_ADD_PARAM(arg,"success",sRet);
				EventProcCall("ModifyCurrentLoginUserSuccess",arg);
			}
		}else{
			//没有用户登录,设置失败
			sRet.append("ModifyCurrentLoginUserFail");
			EP_ADD_PARAM(arg,"fail",sRet);
			EventProcCall("ModifyCurrentLoginUserFail",arg);
		}
	}else{
		qDebug()<<__FUNCTION__<<__LINE__<<"system call abort as pUserMangerEx is null";
		abort();
	}
}
void settingsActivity::OnDeleteUserExOk()
{
	if (verify(100000000, 0)){
		return;
	}
	IUserManagerEx *pUserMangerEx=NULL;
	pcomCreateInstance(CLSID_CommonlibEx,NULL,IID_IUserMangerEx,(void**)&pUserMangerEx);
	QString sRet;
	DEF_EVENT_PARAM(arg);
	if (NULL!=pUserMangerEx)
	{
		QVariant tdeleteUserFile=QueryValue("deleteUserEx_ID");
		QDomDocument tConfFile;
		tConfFile.setContent(tdeleteUserFile.toString());
		QDomNode tDeleteFileNode=tConfFile.elementsByTagName("del").at(0);
		QString sSize=tDeleteFileNode.toElement().attribute("size");
		QDomNodeList tItemList=tDeleteFileNode.childNodes();
		int nCount=tItemList.count();
		if (tItemList.count()!=0)
		{
			for (int n=0;n<tItemList.count();n++)
			{
				QDomNode tItem;
				tItem=tItemList.at(n);
				QString sDelteName=tItem.toElement().attribute("username");
				int nRet=-1;
				int nUserID=-1;
				pUserMangerEx->getUserDatabaseId(sDelteName,nUserID);
				QString sCurrentUserName=pUserMangerEx->getCurrentUser();
				if (nUserID!=-1)
				{
					if (verify(100000000, nUserID)){
						if (NULL!=pUserMangerEx)
						{
							pUserMangerEx->Release();
							pUserMangerEx=NULL;
						}
						return;
					}else{
						if (sCurrentUserName!=sDelteName)
						{
							nRet=pUserMangerEx->deleteUser(sDelteName);
							if (0!=nRet)
							{
								sRet.clear();
								sRet.append("DeleteUserFail");
								EP_ADD_PARAM(arg,"fail",sRet);
								EventProcCall("DeleteUserFail",arg);
							}else{
								//keep going
							}
						}else{
							//do nothing
						}
					}
				}else{
					//keep going
					qDebug()<<__FUNCTION__<<__LINE__<<"there is no that userName";
				}
			}
		}else{
			qDebug()<<__FUNCTION__<<__LINE__<<"DeleteUserEx fail as the item is empty";
			sRet.append("DeleteUserFail");
			EP_ADD_PARAM(arg,"fail",sRet);
			EventProcCall("DeleteUserFail",arg);
		}
	}else{
		sRet.append("DeleteUserFail");
		EP_ADD_PARAM(arg,"fail",sRet);
		EventProcCall("DeleteUserFail",arg);
	}
	if (NULL!=pUserMangerEx)
	{
		pUserMangerEx->Release();
		pUserMangerEx=NULL;
	}
	if (sRet.isEmpty())
	{
		sRet.append("DeleteUserSuccess");
		EP_ADD_PARAM(arg,"success",sRet);
		EventProcCall("DeleteUserSuccess",arg);
	}else{
		//do nothing
	}
}

int settingsActivity::verify( qint64 mainCode, qint64 subCode )
{
	int ret = 0;
	IUserManagerEx *pUserMgrEx = NULL;
	pcomCreateInstance(CLSID_CommonlibEx, NULL, IID_IUserMangerEx, (void**)&pUserMgrEx);
	if (pUserMgrEx){
		ret = pUserMgrEx->checkUserLimit(mainCode, subCode);
		if (ret){
			QVariantMap vmap;
			vmap.insert("MainPermissionCode", qint64(mainCode));
			vmap.insert("SubPermissionCode", qint64(subCode));
			vmap.insert("ErrorCode", ret);
			EventProcCall(QString("Validation"), vmap);;
		}
		pUserMgrEx->Release();
	}
	return ret;
}







