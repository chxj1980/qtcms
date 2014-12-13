#pragma once
#include <QThread>
#include <QStringList>
#include <QDebug>
class checkoutIpOnlive:public QThread
{
	Q_OBJECT
public:
	checkoutIpOnlive(void);
	~checkoutIpOnlive(void);
public:
	bool ipIsOnlive(int nTimeout,QString sIp);//true:ip���ߣ�false��������
	bool checkIsRuning();
protected:
	void run();
private:
	QString m_sIp;
	volatile bool m_bIsOnLive;
};

