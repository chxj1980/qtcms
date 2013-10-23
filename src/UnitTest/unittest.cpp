#include "unittest.h"
#include <guid.h>
#include <IUserManager.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#define START_USER_UNIT_TEST(ii) 	IUserManager * ii = NULL; \
	pcomCreateInstance(CLSID_CommonLibPlugin,NULL,IID_IUserManager,(void **)&ii); \
	QVERIFY2(NULL != ii,"Create user manager instance");

#define END_USER_UNIT_TEST(ii) ii->Release(); 

UnitTest::UnitTest()
{

}

UnitTest::~UnitTest()
{

}

//��������:
// ǰ�ã��������ݿ⣬���ݿ��д�����user_infomation�����ݿ��н���һ����¼��admin|md5('admin')|0|-1|-1
// 1 #008 ����ظ��û�|����E_USER_EXISTS��GetUserCount����2(admin����һ����ӵ��û�),GetUserList���������û���(admin����һ����ӵ��û�)
// 2 ����û���ɾ���û�����|��һ��ɾ������OK���ڶ���ɾ������E_USER_NOT_FOUND��GetUserList����֮ǰ��ӵ��û�
// 3 ɾ���������û�|����E_USER_NOT_FOUND
// 4 �޸Ĳ������û�������|����E_USER_NOT_FOUND
// 5 �޸�admin����|����OK��ʹ���޸�ǰ�������CheckUser����false,ʹ���޸ĺ����뷵��true
// 6 �޸������û�Ȩ�޵ȼ�[0-3]|����OK,GetUserLevel����OK��nLevelΪ�޸ĺ��Ȩ�޵ȼ�
// 7 �޸Ĳ������û���Ȩ�޵ȼ�[0-3]|����E_USER_NOT_FOUND,GetUserLevel��ȡ�����û�Ȩ�޵ȼ��������ǰ���ֲ���
// 8 �޸������û�Ȩ�޵ȼ�[^0-3]|����E_SYSTEM_FAILED,GetUserLevel����OK��nLevelΪ�޸�ǰ��Ȩ�޵ȼ�
// 9 �޸������û�Ȩ������|����OK,GetUserAuthorityMask����OK,nAuthorityMask1��nAuthorityMask2Ϊ�޸ĺ��ֵ
//10 �޸Ĳ������û���Ȩ������|����E_USER_NOT_FOUND,GetUserAuthorityMask��ȡ�����û�Ȩ�����룬�����ǰ���ֲ���
//11 ����admin�Ƿ����|����true
//12 ���Բ������û��Ƿ����|����false
//13 ʹ������adminУ���û�admin|����true
//14 ʹ������1234У���û�admin|����false
//15 ��ȡ�û�admin��Ȩ�޵ȼ�|����OK��nLevelΪ0
//16 ��ȡ�������û���Ȩ�޵ȼ�|����E_USER_NOT_FOUND
//17 ��ȡ�û�admin��Ȩ������|����OK,nAuthorityMask1��nAuthorityMask1Ϊ-1
//18 ��ȡ�������û���Ȩ������|����E_USER_NOT_FOUND
//19 ��ȡϵͳ���û���|����1
//20 ��ȡϵͳ���û��б�|�б���ֻ����admin�û�

// 1 ����ظ��û�|����E_USER_EXISTS��GetUserCount����2(admin����һ����ӵ��û�),GetUserList���������û���(admin����һ����ӵ��û�)
void UnitTest::UserCase1()
{
	beforeUserTest();

	START_USER_UNIT_TEST(Iuser);
	// step 1
	int nRet = Iuser->AddUser("test","test",1,-1,-1);

	QVERIFY2(IUserManager::OK == nRet,"step 1:return");

	int nCount = Iuser->GetUserCount();
	QVERIFY2(2 == nCount,"step 1:GetUserCount");

	QStringList users = Iuser->GetUserList();
	QStringList::const_iterator it;
	for (it = users.begin(); it != users.end(); it ++)
	{
		QVERIFY2(*it == "admin" || *it == "test","step 1:GetUserList");
	}

	// step 2
	nRet = Iuser->AddUser("test","test",1,-1,-1);

	QVERIFY2(IUserManager::E_USER_EXISTS == nRet,"step 2:return");

	nCount = Iuser->GetUserCount();
	QVERIFY2(2 == nCount,"step 2:GetUserCount");

	users = Iuser->GetUserList();
	for (it = users.begin(); it != users.end(); it ++)
	{
		QVERIFY2(*it == "admin" || *it == "test","step 2:GetUserList");
	}

	END_USER_UNIT_TEST(Iuser);
}

// 2 ����û���ɾ���û�����|��һ��ɾ������OK���ڶ���ɾ������E_USER_NOT_FOUND��GetUserList����֮ǰ��ӵ��û�
void UnitTest::UserCase2()
{
	int nRet;
	int nCount;
	QStringList users;
	QStringList::const_iterator it;
	beforeUserTest();

	START_USER_UNIT_TEST(Iuser);

	// step1
	nRet = Iuser->AddUser("test","test",1,-1,-1);
	QVERIFY2(IUserManager::OK == nRet,"step 1");

	nCount = Iuser->GetUserCount();
	QVERIFY2(2 == nCount,"step 1:GetUserCount");

	users = Iuser->GetUserList();
	for (it = users.begin();it != users.end();it++)
	{
		QVERIFY2(*it == "admin" || *it == "test","step 1:GetUserList");
	}

	// step2
	nRet = Iuser->RemoveUser("test");
	QVERIFY2(IUserManager::OK == nRet,"step 2");

	nCount = Iuser->GetUserCount();
	QVERIFY2(1 == nCount,"step 2:GetUserCount");

	users = Iuser->GetUserList();
	for (it = users.begin();it != users.end();it++)
	{
		QVERIFY2(*it == "admin","step 2:GetUserList");
	}

	// step3
	nRet = Iuser->RemoveUser("test");
	QVERIFY2(IUserManager::E_USER_NOT_FOUND == nRet,"step 3");

	nCount = Iuser->GetUserCount();
	QVERIFY2(1 == nCount,"step 3:GetUserCount");

	users = Iuser->GetUserList();
	for (it = users.begin();it != users.end();it++)
	{
		QVERIFY2(*it == "admin","step 3:GetUserList");
	}

	END_USER_UNIT_TEST(Iuser);

}

// 3 ɾ���������û�|����E_USER_NOT_FOUND
void UnitTest::UserCase3()
{
	int nRet;
	int nCount;
	QStringList users;
	QStringList::const_iterator it;
	beforeUserTest();

	START_USER_UNIT_TEST(Iuser);

	// step 1
	nRet = Iuser->RemoveUser("test");
	QVERIFY2(IUserManager::E_USER_NOT_FOUND,"step1");

	nCount = Iuser->GetUserCount();
	QVERIFY2(1 == nCount,"step 1:GetUserCount");

	users = Iuser->GetUserList();
	for (it = users.begin(); it != users.end(); it ++)
	{
		QVERIFY2("admin" == *it,"step 1:GetUserList");
	}

	END_USER_UNIT_TEST(Iuser);
}

// 4 �޸Ĳ������û�������|����E_USER_NOT_FOUND
void UnitTest::UserCase4()
{
	int nRet;
	beforeUserTest();

	START_USER_UNIT_TEST(Itest);

	// step 1
	nRet = Itest->ModifyUserPassword("test","1234");
	QVERIFY2(IUserManager::E_USER_NOT_FOUND == nRet,"step1");

	END_USER_UNIT_TEST(Itest);
}

// 5 �޸�admin����|����OK��ʹ���޸�ǰ�������CheckUser����false,ʹ���޸ĺ����뷵��true
void UnitTest::UserCase5()
{
	int nRet;
	bool bRet;
	beforeUserTest();

	START_USER_UNIT_TEST(Itest);

	// step 1
	nRet = Itest->ModifyUserPassword("admin","check");
	QVERIFY2(IUserManager::OK == nRet,"step 1");

	// step 2
	bRet = Itest->CheckUser("admin","admin");
	QVERIFY2(false == bRet,"step 2");

	// step 3
	bRet = Itest->CheckUser("admin","check");
	QVERIFY2(true == bRet,"step 3");	

	END_USER_UNIT_TEST(Itest);
}

// 6 �޸������û�Ȩ�޵ȼ�[0-3]|����OK,GetUserLevel����OK��nLevelΪ�޸ĺ��Ȩ�޵ȼ�
void UnitTest::UserCase6()
{
	int nRet;
	beforeUserTest();

	START_USER_UNIT_TEST(Itest);

	int i;
	for (i = 0;i < 4;i++)
	{
		int nLevel;
		QVERIFY2(IUserManager::OK == Itest->ModifyUserLevel("admin",i),"ModifyUserLevel");
		QVERIFY2(IUserManager::OK == Itest->GetUserLevel("admin",nLevel),"GetUserLevel");
		QCOMPARE(i,nLevel);
	}

	END_USER_UNIT_TEST(Itest);
}

// 7 �޸Ĳ������û���Ȩ�޵ȼ�[0-3]|����E_USER_NOT_FOUND,GetUserLevel��ȡ�����û�Ȩ�޵ȼ��������ǰ���ֲ���
void UnitTest::UserCase7()
{
	beforeUserTest();

	START_USER_UNIT_TEST(Itest);

	int i;
	for (i = 0; i < 4 ;i ++)
	{
		QVERIFY2(IUserManager::E_USER_NOT_FOUND == Itest->ModifyUserLevel("test",i),"");
	}

	END_USER_UNIT_TEST(Itest);
}

// 8 �޸������û�Ȩ�޵ȼ�[^0-3]|����E_SYSTEM_FAILED,GetUserLevel����OK��nLevelΪ�޸�ǰ��Ȩ�޵ȼ�
void UnitTest::UserCase8()
{
	beforeUserTest();

	START_USER_UNIT_TEST(Itest);
	int nLevelBefor;
	QVERIFY2(IUserManager::OK == Itest->GetUserLevel("admin",nLevelBefor),"");

	int n;
	qsrand(QTime::currentTime().msec());
	n = qrand();
	int i;
	for ( i = 0; i < n; i ++)
	{
		int nLevel = 3 + qrand();
		QVERIFY2(IUserManager::E_SYSTEM_FAILED == Itest->ModifyUserLevel("admin",nLevel),"");
		QVERIFY2(IUserManager::OK == Itest->GetUserLevel("admin",nLevel),"");
		QCOMPARE(nLevel,nLevelBefor);
	}


	END_USER_UNIT_TEST(Itest);
}

// 9 �޸������û�Ȩ������|����OK,GetUserAuthorityMask����OK,nAuthorityMask1��nAuthorityMask2Ϊ�޸ĺ��ֵ
void UnitTest::UserCase9()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	qsrand(QTime::currentTime().msec());
	int n = qrand();
	int i;
	for (i = 0; i < n; i++)
	{
		int mask1 = qrand();
		int mask2 = qrand();
		QVERIFY2(IUserManager::OK == Itest->ModifyUserAuthorityMask("admin",mask1,mask2),"");
		int currentMask1,currentMask2;
		QVERIFY2(IUserManager::OK == Itest->GetUserAuthorityMask("admin",currentMask1,currentMask2),"");
		QCOMPARE(mask1,currentMask1);
		QCOMPARE(mask2,currentMask2);
	}

	END_USER_UNIT_TEST(Itest);
}

//10 �޸Ĳ������û���Ȩ������|����E_USER_NOT_FOUND,GetUserAuthorityMask��ȡ�����û�Ȩ�����룬�����ǰ���ֲ���
void UnitTest::UserCase10()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	qsrand(QTime::currentTime().msec());
	int mask1 = qrand();
	int mask2 = qrand();
	QVERIFY2(IUserManager::E_USER_NOT_FOUND == Itest->ModifyUserAuthorityMask("test",mask1,mask2),"");

	END_USER_UNIT_TEST(Itest);
}

//11 ����admin�Ƿ����|����true
void UnitTest::UserCase11()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	QVERIFY2(true == Itest->IsUserExists("admin"),"");

	END_USER_UNIT_TEST(Itest);
}

//12 ���Բ������û��Ƿ����|����false
void UnitTest::UserCase12()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	QVERIFY2(false == Itest->IsUserExists("test"),"");

	END_USER_UNIT_TEST(Itest);
}

//13 ʹ������adminУ���û�admin|����true
void UnitTest::UserCase13()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	QVERIFY2(true == Itest->CheckUser("admin","admin"),"");

	END_USER_UNIT_TEST(Itest);
}

//14 ʹ������1234У���û�admin|����false
void UnitTest::UserCase14()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	QVERIFY2(false == Itest->CheckUser("admin","1234"),"");

	END_USER_UNIT_TEST(Itest);
}

//15 ��ȡ�û�admin��Ȩ�޵ȼ�|����OK��nLevelΪ0
void UnitTest::UserCase15()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	int nLevel;
	QVERIFY2(IUserManager::OK == Itest->GetUserLevel("admin",nLevel),"");
	QCOMPARE(0,nLevel);

	END_USER_UNIT_TEST(Itest);
}

//16 ��ȡ�������û���Ȩ�޵ȼ�|����E_USER_NOT_FOUND
void UnitTest::UserCase16()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	int nLevel;
	QVERIFY2(IUserManager::E_USER_NOT_FOUND == Itest->GetUserLevel("test",nLevel),"");

	END_USER_UNIT_TEST(Itest);
}

//17 ��ȡ�û�admin��Ȩ������|����OK,nAuthorityMask1��nAuthorityMask1Ϊ-1
void UnitTest::UserCase17()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	int nAuthorityMask1,nAuthorityMask2;
	QVERIFY2(IUserManager::OK == Itest->GetUserAuthorityMask("admin",nAuthorityMask1,nAuthorityMask2),"");
	QCOMPARE(-1,nAuthorityMask1);
	QCOMPARE(-1,nAuthorityMask2);

	END_USER_UNIT_TEST(Itest);
}

//18 ��ȡ�������û���Ȩ������|����E_USER_NOT_FOUND
void UnitTest::UserCase18()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	int nAuthorityMask1,nAuthorityMask2;
	QVERIFY2(IUserManager::E_USER_NOT_FOUND == Itest->GetUserAuthorityMask("test",nAuthorityMask1,nAuthorityMask2),"");

	END_USER_UNIT_TEST(Itest);
}

//19 ��ȡϵͳ���û���|����1
void UnitTest::UserCase19()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	QCOMPARE(1,Itest->GetUserCount());
	QVERIFY2(IUserManager::OK == Itest->AddUser("test","1234",1,-1,-1),"");
	QCOMPARE(2,Itest->GetUserCount());

	END_USER_UNIT_TEST(Itest);
}

//20 ��ȡϵͳ���û��б�|�б���ֻ����admin�û�
void UnitTest::UserCase20()
{
	beforeUserTest();
	START_USER_UNIT_TEST(Itest);

	QStringList users = Itest->GetUserList();
	QVERIFY2(users.contains("admin"),"");
	QCOMPARE(1,users.count());
	QVERIFY2(IUserManager::OK == Itest->AddUser("test","1234",1,-1,-1),"");
	users = Itest->GetUserList();
	QVERIFY2(users.contains("admin") && users.contains("test"),"");
	QCOMPARE(2,users.count());

	END_USER_UNIT_TEST(Itest);
}

void UnitTest::beforeUserTest()
{
	QSqlDatabase db;
	if (QSqlDatabase::contains("usertest"))
	{
		db = QSqlDatabase::database("usertest"); 
	}
	else
	{
		db = QSqlDatabase::addDatabase("QSQLITE","usertest");
	}
	QString sAppPath = QCoreApplication::applicationDirPath();
	QString sDatabasePath = sAppPath + "/system.db";
	db.setDatabaseName(sDatabasePath);
	db.open();

	QSqlQuery query1(db);
	query1.prepare("delete from user_infomation");
	query1.exec();

	QSqlQuery query2(db);
	query2.prepare("insert into user_infomation(username,password,level,mask1,mask2) values(:username,:password,:level,:mask1,:mask2)");
	query2.bindValue(":username","admin");
	query2.bindValue(":password",QString(QCryptographicHash::hash(QString("admin").toAscii(),QCryptographicHash::Md5).toHex()));
	query2.bindValue(":level",0);
	query2.bindValue(":mask1",-1);
	query2.bindValue(":maks2",-1);
	query2.exec();

	db.close();
}
