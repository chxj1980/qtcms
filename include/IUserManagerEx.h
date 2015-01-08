#ifndef __IUSERMANAGEREX_HEAD_FILE__
#define __IUSERMANAGEREX_HEAD_FILE__
#include "libpcom.h"
#include <QtCore/QString>
#include <QtCore/QStringList>

interface IUserManagerEx : public IPComBase
{
	virtual int addUser(const QString &sUserName,const QString &sPassword,quint64 uiLimit,quint64 uiLogOutInterval,QVariantMap tSubCode)=0;//0:����û��ɹ���1������û�ʧ��
	virtual int deleteUser(const QString &sUserName)=0;//0:ɾ���û��ɹ�;1:ɾ��ʧ��
	virtual int checkUserLimit(quint64 uiMainCode,quint64 uiSubCode)=0;//0���û�����Ȩ�ޣ�1���û�δ��¼��2���û���¼����û��Ȩ��
	virtual int login(const QString &sUserName,const QString &sPassword ,int nCode)=0;//nCode:0 ��ʾ�û������¼��nCode:1 ��ʾ�û�����ע��;
	virtual int loginEx()=0;
	virtual int setLoginOutInterval(int nTime)=0;//0:���óɹ���1������ʧ�ܣ��������󣩣�nTime ��λΪS�����ֵ�޶�Ϊ1һ��Сʱ��3600��
	virtual int getUserList(QStringList &sUserList)=0;//0:��ȡ�ɹ� ��1����ȡʧ��
	virtual int getUserLimit(QString sUserName,quint64 &uiLimit,QVariantMap &tSubCode)=0;//0:��ȡ�ɹ���1����ȡʧ��
	virtual int getUserDatabaseId(QString sUserName,int &nId)=0;//0:��ȡ�ɹ���1����ȡʧ��
	virtual int getLoginOutInterval(QString sUserName)=0;//����ֵΪʱ����
	virtual int modifyUserInfo(const QString &sOldUserName,const QString &sNewUserName,const QString &sNewPassword,quint64 uiLimit, quint64 uiLogOutInterval, QVariantMap tSubCode)=0;//0:���óɹ���1������ʧ��
	virtual QString getCurrentUser()=0;
	virtual void setIsKeepCurrentUserPassWord(bool bFlags)=0;
	virtual bool getIsKeepCurrentUserPassWord(QString &sUserName,QString &sUserPassword)=0;
	virtual bool setCurrentUserInfo(QString sUserName,QString sUserPassword)=0;
	virtual int modifyCurrentUserInfo(const QString &sOldUserName,const QString &sNewUserName,const QString &sOldPassword,const QString &sNewPassword,int iLogOutInterval)=0;
};

#endif