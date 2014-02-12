#include "Device_Test.h"
#include <guid.h>
#include <IDeviceManager.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>



#define START_DEVICEMANAGER_UNIT_TEST(ii) 	IDeviceManager * ii = NULL; \
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IDeviceManager,(void **)&ii); \
	QVERIFY2(NULL != ii,"Create device manager instance");

#define END_DEVICEMANAGER_UNIT_TEST(ii) ii->Release(); 

Device_Test::Device_Test()
{
}


Device_Test::~Device_Test()
{
}

//����������
//0�� ǰ�ã��������ݿ⣬����dev�����һ����¼��15��"hubei"|80|192168216|"1234"|"yang"|"123"|"tianxia"|8|2|"ONVIF" ����area�����һ����¼��25��guangzhou��0|1|3|4|5|
//1�� ����ظ����豸��.|����-1��GetDeviceCount����2,GetDeviceList���������豸ID
//2������豸��ɾ���豸����|��һ��ɾ������OK���ڶ���ɾ������E_DEVICE_NOT_FOUND��GetUserList��û��֮ǰ��ӵ��豸��¼
//3��ɾ�������ڵ��豸������E_DEVICE_NOT_FOUND
//4���޸��Ѵ����豸�����ƣ�ɾ���豸���޸Ĳ����ڵ��豸���ƣ���һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
//5���޸��Ѵ��ڵ��豸IP��Ϣ���޸Ĳ����ڵ��豸IP��Ϣ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
//6���޸��Ѵ��ڵ��豸��������Ϣ���޸Ĳ����ڵ��豸��������Ϣ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
//7���޸��Ѵ��ڵ��豸��¼�û���Ϣ���޸Ĳ����ڵ��豸��¼�û���Ϣ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
//8���޸��Ѵ��ڵ��豸���ͨ�������޸Ĳ����ڵ��豸���ͨ��������һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
//9���޸��Ѵ��ڵ��豸���ӷ�ʽ���޸Ĳ����ڵ��豸���ӷ�ʽ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
//10����ȡָ�������µ��豸������ɾ�������豸���ٴλ�ȡ��ǰ�����µ��豸��������һ�η���1���ڶ��η���0
//11����ȡָ�������µ��豸�б�ɾ�������豸���ٴλ�ȡָ�������µ��豸�б����һ�η���һ���豸�ļ�¼���ڶ��η��ؿռ�¼
//12����ȡָ��ID���豸���ƣ���ȡ��ЧID���豸���ƣ���һ�η���OK,����Ϊhong���ڶ��η���E_DEVICE_NOT_FOUND������Ϊ��
//13����ȡָ��ID��IP��Ϣ����ȡ��ЧID���豸IP��Ϣ����һ�η���OK��IP��Ϣ��hubei��80��192168216���ڶ��η���E_DEVICE_NOT_FOUND��IP��ַΪ�գ�port��httpΪ0
//14����ȡָ��ID���豸��������Ϣ����ȡ��ЧID���豸��������Ϣ����һ�η���OK����������Ϣ��1234���ڶ��η���E_DEVICE_NOT_FOUND����������ϢΪ��
//15����ȡָ��ID���豸��¼��Ϣ����ȡ��ЧID���豸��¼��Ϣ����һ�η���OK����¼��Ϣ��yang��123���ڶ��η���E_DEVICE_NOT_FOUND����¼��ϢΪ��
//16����ȡָ��ID���豸���ӷ�ʽ����ȡ��ЧID���豸���ӷ�ʽ����һ�η���OK,���ӷ�ʽΪ2���ڶ��η���E_DEVICE_NOT_FOUND�����ӷ�ʽΪ0
//17����ȡָ��ID���豸������Ϣ����ȡ��ЧID���豸������Ϣ����һ�η���OK������ΪONVIF���ڶ��η���E_DEVICE_NOT_FOUND��������ϢΪ��
//18����ȡָ��ID���豸������Ϣ����ȡ��ЧID���豸������Ϣ����һ�η���OK,��ʾ������Ϣ15��"hubei"|80|192168216|"1234"|"yang"|"123"|"tianxia"|8|2|"ONVIF"���ڶ��η���E_DEVICE_NOT_FOUND����ϢΪ��

//0�� ǰ�ã��������ݿ⣬����dev�����һ����¼��15��"hubei"|80|192168216|"1234"|"yang"|"123"|"tianxia"|8|2|"ONVIF" ����area�����һ����¼��25��guangzhou��0|1|3|4|5|
int Device_Test::beforeTest()
{
	QSqlDatabase db;
	if (QSqlDatabase::contains("DeviceManagerTest"))
	{
		db = QSqlDatabase::database("DeviceManagerTest"); 
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE","DeviceManagerTest");
	}
	QString sAppPath = QCoreApplication::applicationDirPath();
	QString sDatabasePath = sAppPath + "/system.db";
	db.setDatabaseName(sDatabasePath);
	db.open();

	QSqlQuery query1(db);
	query1.prepare("delete from dev");
	query1.exec();
	//��dev���������
	QSqlQuery query2(db);
	query2.prepare("insert into dev(area_id,address,port,http,eseeid,username,password,name,channel_count,connect_method,vendor) values(:area_id,:address,:port,:http,:eseeid,:username,:password,:name,:channel_count,:connect_method,:vendor)");
	query2.bindValue(":area_id",15);
	query2.bindValue(":address","hubei");
	query2.bindValue(":port",80);
	query2.bindValue(":http",192168216);
	query2.bindValue(":eseeid","1234");
	query2.bindValue(":username","yang");
	query2.bindValue(":password","123");
	query2.bindValue(":name","tianxia");
	query2.bindValue(":channel_count",8);
	query2.bindValue(":connect_method",2);
	query2.bindValue(":vendor","ONVIF");
	query2.exec();

	int dev_id = -1;
	QSqlQuery query3(db);
	query3.prepare("select id from dev where area_id = 15 and name = \"tianxia\" ");
	query3.exec();

	if (query3.next())
	{
		dev_id = query3.value(0).toInt();
	}

	static bool input = false;
	if (!input)
	{
		QSqlQuery query5(db);
		query1.prepare("delete from area");
		query1.exec();
		query5.prepare("insert into area(id,pid,name,path) values(:id,:pid,:name,:path)");
		query5.bindValue(":id",15);
		query5.bindValue(":pid",25);
		query5.bindValue(":name","guangzhou");
		query5.bindValue(":path","0|1|3|4|5|");
		query5.exec();

		input = true;
	}

	db.close();
	return dev_id;
}

//1������ظ����豸��.|����-1��GetDeviceCount����2,GetDeviceList���������豸ID
void Device_Test::DeviceTestCase1()
{
	beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	// step 1
	int nRet = Itest->AddDevice(10,"hehong","heilongjiang",80,192168211,"2345","cheng","888",12,4,"ONVIF");
	QVERIFY2(nRet > 0, "ADD_TEST step 1:add device");

	int nCount = Itest->GetDeviceCount(10);
	QVERIFY2(1 == nCount, "ADD_TEST step 1:get device count");

	QStringList DeviceList = Itest->GetDeviceList(10);
	for (int i = 0; i < DeviceList.count(); i++)
	{
		QVERIFY2(DeviceList.contains("hehong"),"ADD_TEST step 1:GetDeviceList");
	}

	//step 2
	nRet = Itest->AddDevice(10,"hehong","heilongjiang",80,192168211,"2345","cheng","888",12,4,"ONVIF");
	QVERIFY2(-1, "step 2:add device");

	nCount = Itest->GetDeviceCount(10);
	QVERIFY2(1 == nCount, "ADD_TEST step 2:get device count");

	DeviceList = Itest->GetDeviceList(10);
	for (int i = 0; i < DeviceList.count(); i++)
	{
		QVERIFY2(DeviceList.contains("hehong"),"ADD_TEST step 2:GetDeviceList");
	}

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//2������豸��ɾ���豸����|��һ��ɾ������OK���ڶ���ɾ������E_DEVICE_NOT_FOUND��GetUserList��û��֮ǰ��ӵ��豸��¼
void Device_Test::DeviceTestCase2()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "ROMOVE_TEST delete device 1");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "delete device 2");

	QStringList DeviceList = Itest->GetDeviceList(15);
	for (int i = 0; i < DeviceList.count(); i++)
	{
		QVERIFY2(10 == DeviceList[1].toInt(),"ROMOVE_TEST GetDeviceList");
	}

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//3��ɾ�������ڵ��豸������E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase3()
{
	beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->RemoveDevice(10000);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "ROMOVE_TEST delete device no id");


	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//4���޸��Ѵ����豸�����ƣ�ɾ���豸���޸Ĳ����ڵ��豸���ƣ���һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase4()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->ModifyDeviceName(dev_id,"xiaofeixia");
	QVERIFY2(0 == nRet, "MODIFY_NAME_TEST modify device name");

	QString strName;
	nRet = Itest->GetDeviceName(dev_id, strName);
	QVERIFY2("xiaofeixia" == strName, "MODIFY_NAME_TEST modify device name OK");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "MODIFY_NAME_TEST remove device");

	nRet = Itest->ModifyDeviceName(dev_id,"sunxia");
	QVERIFY2( IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "MODIFY_NAME_TEST modify device name");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//5���޸��Ѵ��ڵ��豸IP��Ϣ���޸Ĳ����ڵ��豸IP��Ϣ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase5()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->ModifyDeviceHost(dev_id,"bianzhou",81,1921682110);
	QVERIFY2(0 == nRet, "MODIFY_IP_TEST modify ip");

	QString sAddress;
	int port = 0;
	int http = 0;
	nRet = Itest->GetDeviceHost(dev_id, sAddress, port, http);
	QVERIFY2("bianzhou" == sAddress, "MODIFY_IP_TEST modify address OK");
	QVERIFY2(81 == port, "MODIFY_IP_TEST modify port OK");
	QVERIFY2(1921682110 == http, "MODIFY_IP_TEST modify http OK");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "MODIFY_IP_TEST remove device");

	nRet = Itest->ModifyDeviceHost(dev_id,"xiyang",80,192168234);
	QVERIFY2( IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "MODIFY_IP_TEST modify device IP");


	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//6���޸��Ѵ��ڵ��豸��������Ϣ���޸Ĳ����ڵ��豸��������Ϣ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase6()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->ModifyDeviceEseeId(dev_id,"7189");
	QVERIFY2(0 == nRet, "MODIFY_ESEEID_TEST modify eseeid");

	QString eseeid;
	nRet = Itest->GetDeviceEseeId(dev_id,eseeid);
	QVERIFY2("7189" == eseeid, "MODIFY_ESEEID_TEST modify eseeid OK");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "MODIFY_ESEEID_TEST remove device");

	nRet = Itest->ModifyDeviceEseeId(dev_id,"1928");
	QVERIFY2( IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "MODIFY_ESEEID_TEST modify device eseeid");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//7���޸��Ѵ��ڵ��豸��¼�û���Ϣ���޸Ĳ����ڵ��豸��¼�û���Ϣ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase7()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->ModifyDeviceAuthority(dev_id,"lengong","7189");
	QVERIFY2(0 == nRet, "MODIFY_LOGIN_TEST modify login");

	QString sName;
	QString spasswd;
	nRet = Itest->GetDeviceLoginInfo(dev_id, sName, spasswd);
	QVERIFY2("lengong" == sName, "MODIFY_LOGIN_TEST modify name OK");
	QVERIFY2("7189" == spasswd, "MODIFY_LOGIN_TEST modify password OK");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "MODIFY_LOGIN_TEST remove device");

	nRet = Itest->ModifyDeviceAuthority(dev_id,"xiang","1048");
	QVERIFY2( IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "MODIFY_LOGIN_TEST modify device login");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//8���޸��Ѵ��ڵ��豸���ͨ�������޸Ĳ����ڵ��豸���ͨ��������һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase8()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->ModifyDeviceChannelCount(dev_id,16);
	QVERIFY2(0 == nRet, "MODIFY_CHANNELCOUNT_TEST modify channel count");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "MODIFY_CHANNELCOUNT_TEST remove device");

	nRet = Itest->ModifyDeviceChannelCount(dev_id, 20);
	QVERIFY2( IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "MODIFY_CHANNELCOUNT_TEST modify device channel count");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//9���޸��Ѵ��ڵ��豸���ӷ�ʽ���޸Ĳ����ڵ��豸���ӷ�ʽ����һ�η���OK���ڶ��η���E_DEVICE_NOT_FOUND
void Device_Test::DeviceTestCase9()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->ModifyDeviceConnectMethod(dev_id,8);
	QVERIFY2(0 == nRet, "MODIFY_CHANNELCOUNT_TEST modify connect method");

	int ConnectMethod = 0;
	nRet = Itest->GetDeviceConnectMethod(dev_id,ConnectMethod);
	QVERIFY2(8 == ConnectMethod, "MODIFY_CHANNELCOUNT_TEST modify connect method OK");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "MODIFY_CHANNELCOUNT_TEST remove device");

	nRet = Itest->ModifyDeviceChannelCount(dev_id,16);
	QVERIFY2( IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "MODIFY_CHANNELCOUNT_TEST modify device connect method");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//10����ȡָ�������µ��豸������ɾ�������豸���ٴλ�ȡ��ǰ�����µ��豸��������һ�η���1���ڶ��η���0
void Device_Test::DeviceTestCase10()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int nRet = Itest->GetDeviceCount(15);
	QVERIFY2(1 == nRet, "GET_COUNT get count");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "GET_COUNT remove device");

	nRet = Itest->GetDeviceCount(15);
	QVERIFY2( 0 == nRet, "GET_COUNT get device count");


	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//11����ȡָ�������µ��豸�б�ɾ�������豸���ٴλ�ȡָ�������µ��豸�б����һ�η���һ���豸�ļ�¼���ڶ��η��ؿռ�¼
void Device_Test::DeviceTestCase11()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QStringList DeviceList = Itest->GetDeviceList(15);
	QStringList::const_iterator it;
	// ��Ҫ�޸ģ���֮ǰ�Ƿ����豸���ƣ����޸ĳɼ�ⷵ�ص��豸id�ţ�
	for (it = DeviceList.begin(); it != DeviceList.end(); it++)
	{
		QVERIFY2(*it == "tianxia","step 1:GetUserList");
	}

	QCOMPARE(1,DeviceList.count());

	int nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "GET_DEVICELIST remove device");

	DeviceList = Itest->GetDeviceList(15);
	QVERIFY2(0 == DeviceList.count(),"GET_DEVICELIST GetDeviceList after");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//12����ȡָ��ID���豸���ƣ���ȡ��ЧID���豸���ƣ���һ�η���OK,����Ϊhong���ڶ��η���E_DEVICE_NOT_FOUND������Ϊ��
void Device_Test::DeviceTestCase12()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QString device_name;
	int nRet = Itest->GetDeviceName(dev_id,device_name);
	QVERIFY2(0 == nRet, "GET_NAME get device name before");
	QVERIFY2("tianxia" == device_name,"GET_NAME before");


	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(IDeviceManager::OK == nRet, "GET_NAME remove device");

	device_name = "";
	nRet = Itest->GetDeviceName(dev_id,device_name);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_NAME get device name after");
	QVERIFY2("" == device_name,"GET_NAME after");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//13����ȡָ��ID��IP��Ϣ����ȡ��ЧID���豸IP��Ϣ����һ�η���OK��IP��Ϣ��hubei��80��192168216���ڶ��η���E_DEVICE_NOT_FOUND��IP��ַΪ�գ�port��httpΪ0
void Device_Test::DeviceTestCase13()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QString address;
	int nPort = 0;
	int nHttp = 0;
	int nRet = Itest->GetDeviceHost(dev_id,address,nPort,nHttp);
	QVERIFY2(0 == nRet, "GET_IP get device IP");
	QVERIFY2("hubei" == address, "GET_IP get device address before");
	QVERIFY2(80 == nPort, "GET_IP get device port before");
	QVERIFY2(192168216 == nHttp, "GET_IP get device http before");

	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(0 == nRet, "GET_IP remove device");

	address = "";
	nPort = 0;
	nHttp = 0;
	nRet = Itest->GetDeviceHost(dev_id,address,nPort,nHttp);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_IP get device name after");
	QVERIFY2("" == address, "GET_IP get device address after");
	QVERIFY2(0 == nPort, "GET_IP get device port after");
	QVERIFY2(0 == nHttp, "GET_IP get device http after");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//14����ȡָ��ID���豸��������Ϣ����ȡ��ЧID���豸��������Ϣ����һ�η���OK����������Ϣ��1234���ڶ��η���E_DEVICE_NOT_FOUND����������ϢΪ��
void Device_Test::DeviceTestCase14()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QString eseeid;
	int nRet = Itest->GetDeviceEseeId(dev_id,eseeid);
	QVERIFY2(0 == nRet, "GET_ESEEID get device eseeid before");
	QVERIFY2("1234" == eseeid,"GET_ESEEID before");


	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(0 == nRet, "GET_ESEEID remove device");

	eseeid = "";
	nRet = Itest->GetDeviceEseeId(dev_id,eseeid);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_ESEEID get device eseeid after");
	QVERIFY2("" == eseeid,"GET_ESEEID after");


	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//15����ȡָ��ID���豸��¼��Ϣ����ȡ��ЧID���豸��¼��Ϣ����һ�η���OK����¼��Ϣ��yang��123���ڶ��η���E_DEVICE_NOT_FOUND����¼��ϢΪ��
void Device_Test::DeviceTestCase15()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QString sUsername, sPassword;
	int nRet = Itest->GetDeviceLoginInfo(dev_id,sUsername, sPassword);
	QVERIFY2(0 == nRet, "GET_LOGIN get device login before");
	QVERIFY2("yang" == sUsername && "123" == sPassword, "GET_LOGIN before");


	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(0 == nRet, "GET_LOGIN remove device");

	sUsername = "";
	sPassword = "";
	nRet = Itest->GetDeviceLoginInfo(dev_id,sUsername, sPassword);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_LOGIN get device login after");
	QVERIFY2("" == sUsername && "" == sPassword,"GET_LOGIN after");


	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//16����ȡָ��ID���豸���ӷ�ʽ����ȡ��ЧID���豸���ӷ�ʽ����һ�η���OK,���ӷ�ʽΪ2���ڶ��η���E_DEVICE_NOT_FOUND�����ӷ�ʽΪ0
void Device_Test::DeviceTestCase16()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	int ConnectMethod = 0;
	int nRet = Itest->GetDeviceConnectMethod(dev_id,ConnectMethod);
	QVERIFY2(0 == nRet, "GET_CONNECTMETHOD get device connect method before");
	QVERIFY2(2 == ConnectMethod, "GET_CONNECTMETHOD before");


	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(0 == nRet, "GET_CONNECTMETHOD remove device");

	ConnectMethod = 0;
	nRet = Itest->GetDeviceConnectMethod(dev_id,ConnectMethod);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_CONNECTMETHOD get device connect method after");
	QVERIFY2(0 == ConnectMethod,"GET_CONNECTMETHOD after");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//17����ȡָ��ID���豸������Ϣ����ȡ��ЧID���豸������Ϣ����һ�η���OK������ΪONVIF���ڶ��η���E_DEVICE_NOT_FOUND��������ϢΪ��
void Device_Test::DeviceTestCase17()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QString Vendor = "";
	int nRet = Itest->GetDevicdVendor(dev_id,Vendor);
	QVERIFY2(0 == nRet, "GET_VENDOR get device vendor before");
	QVERIFY2("ONVIF" == Vendor, "GET_VENDOR before");


	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(0 == nRet, "GET_VENDOR remove device");

	Vendor = "";
	nRet = Itest->GetDevicdVendor(dev_id,Vendor);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_VENDOR get device vendor after");
	QVERIFY2("" == Vendor,"GET_VENDOR after");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}

//18����ȡָ��ID���豸������Ϣ����ȡ��ЧID���豸������Ϣ����һ�η���OK,��ʾ������Ϣ15��"hubei"|80|192168216|"1234"|"yang"|"123"|"tianxia"|8|2|"ONVIF"���ڶ��η���E_DEVICE_NOT_FOUND����ϢΪ��
void Device_Test::DeviceTestCase18()
{
	int dev_id = beforeTest();
	START_DEVICEMANAGER_UNIT_TEST(Itest);

	QString sDeviceName = "";
	QString sAddress = "";
	int port = 0;
	int http = 0;
	QString sEseeid = ""; 
	QString sUsername = "";
	QString sPassword = "";
	int connectMethod = 0;
	QString sVendor = "";
	QVariantMap DeviceInfo;

	//method 1 before
	int nRet = Itest->GetDeviceInfo(dev_id,sDeviceName,sAddress,port,http,sEseeid,sUsername,sPassword,connectMethod,sVendor);
	QVERIFY2(0 == nRet, "GET_DEVICE_INFO_METHOD_1 get device info before");
	QVERIFY2("tianxia" == sDeviceName, "GET_DEVICE_INFO_METHOD_1  device name before");
	QVERIFY2("hubei" == sAddress, "GET_DEVICE_INFO_METHOD_1 device address before");
	QVERIFY2(80 == port, "GET_DEVICE_INFO_METHOD_1 device port before");
	QVERIFY2(192168216 == http, "GET_DEVICE_INFO_METHOD_1 device http before");
	QVERIFY2("1234" == sEseeid, "GET_DEVICE_INFO_METHOD_1 device sessid before");
	QVERIFY2("yang" == sUsername, "GET_DEVICE_INFO_METHOD_1 username before");
	QVERIFY2("123" == sPassword, "GET_DEVICE_INFO_METHOD_1 password before");
	QVERIFY2(2 == connectMethod, "GET_DEVICE_INFO_METHOD_1 connect method before");
	QVERIFY2("ONVIF" == sVendor, "GET_DEVICE_INFO_METHOD_1 device vendor before");

	//method 2 before
	DeviceInfo = Itest->GetDeviceInfo(dev_id);
	QVERIFY2(0 == nRet, "GET_DEVICE_INFO_METHOD_2 get device info before");
	QVERIFY2("tianxia" == DeviceInfo["name"].toString(), "GET_DEVICE_INFO_METHOD_2  device name before");
	QVERIFY2("hubei" == DeviceInfo["address"].toString(), "GET_DEVICE_INFO_METHOD_2 device address before");
	QVERIFY2(80 == DeviceInfo["port"].toInt(), "GET_DEVICE_INFO_METHOD_2 device port before");
	QVERIFY2(192168216 == DeviceInfo["http"].toInt(), "GET_DEVICE_INFO_METHOD_2 device http before");
	QVERIFY2("1234" == DeviceInfo["eseeid"].toString(), "GET_DEVICE_INFO_METHOD_2 device sessid before");
	QVERIFY2("yang" == DeviceInfo["username"].toString(), "GET_DEVICE_INFO_METHOD_2 username before");
	QVERIFY2("123" == DeviceInfo["password"].toString(), "GET_DEVICE_INFO_METHOD_2 password before");
	QVERIFY2(2 == DeviceInfo["method"].toInt(), "GET_DEVICE_INFO_METHOD_2 connect method before");
	QVERIFY2("ONVIF" == DeviceInfo["vendor"].toString(), "GET_DEVICE_INFO_METHOD_2 device vendor before");


	nRet = Itest->RemoveDevice(dev_id);
	QVERIFY2(0 == nRet, "GET_DEVICE_INFO remove device");

	sDeviceName = "";
	sAddress = "";
	port = 0;
	http = 0;
	sEseeid = ""; 
	sUsername = "";
	sPassword = "";
	connectMethod = 0;
	sVendor = "";
	DeviceInfo.empty();

	//method 1 after
	nRet = Itest->GetDeviceInfo(dev_id,sDeviceName,sAddress,port,http,sEseeid,sUsername,sPassword,connectMethod,sVendor);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_VENDOR get device info after");
	QVERIFY2("" == sDeviceName, "GET_DEVICE_INFO  device name after");
	QVERIFY2("" == sAddress, "GET_DEVICE_INFO device address after");
	QVERIFY2(0 == port, "GET_DEVICE_INFO device port after");
	QVERIFY2(0 == http, "GET_DEVICE_INFO device http after");
	QVERIFY2("" == sEseeid, "GET_DEVICE_INFO device sessid after");
	QVERIFY2("" == sUsername, "GET_DEVICE_INFO username after");
	QVERIFY2("" == sPassword, "GET_DEVICE_INFO password after");
	QVERIFY2(0 == connectMethod, "GET_DEVICE_INFO connect method after");
	QVERIFY2("" == sVendor, "GET_DEVICE_INFO device vendor after");

	//method 2 after
	DeviceInfo = Itest->GetDeviceInfo(dev_id);
	QVERIFY2(IDeviceManager::E_DEVICE_NOT_FOUND == nRet, "GET_DEVICE_INFO_METHOD_2 get device info after");
	QVERIFY2("" == DeviceInfo["name"].toString(), "GET_DEVICE_INFO_METHOD_2  device name after");
	QVERIFY2("" == DeviceInfo["address"].toString(), "GET_DEVICE_INFO_METHOD_2 device address after");
	QVERIFY2(0 == DeviceInfo["port"].toInt(), "GET_DEVICE_INFO_METHOD_2 device port after");
	QVERIFY2(0 == DeviceInfo["http"].toInt(), "GET_DEVICE_INFO_METHOD_2 device http after");
	QVERIFY2("" == DeviceInfo["eseeid"].toString(), "GET_DEVICE_INFO_METHOD_2 device sessid after");
	QVERIFY2("" == DeviceInfo["username"].toString(), "GET_DEVICE_INFO_METHOD_2 username after");
	QVERIFY2("" == DeviceInfo["password"].toString(), "GET_DEVICE_INFO_METHOD_2 password after");
	QVERIFY2(0 == DeviceInfo["method"].toInt(), "GET_DEVICE_INFO_METHOD_2 connect method after");
	QVERIFY2("" == DeviceInfo["vendor"].toString(), "GET_DEVICE_INFO_METHOD_2 device vendor after");

	END_DEVICEMANAGER_UNIT_TEST(Itest);
}