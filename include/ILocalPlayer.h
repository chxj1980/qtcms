#ifndef __ILOCALPLAYER_HEAD_FILE_SADVN98ASD7YASGV__
#define __ILOCALPLAYER_HEAD_FILE_SADVN98ASD7YASGV__
#include <libpcom.h>
#include <QtGui/QWidget>
#include <QtCore/QDateTime>
#include <QStringList>
interface ILocalPlayer:IPComBase
{
	//��Ӳ����ļ���ͬ�����У���ָ���ļ����ŵĴ���ID
	//���������
	//filelist:�ļ����б��б��е��ļ�������ͬһ��ͨ�����µ��ļ�����������������·�����磺F:\project\date1\devname\chl2\filename.avi��F:\project\date2\devname\chl2\filename.avi
	//wnd:���Ŵ��ڵ�id
	//start:��ʼ����ʱ��(ʱ���ʽ��"yyyy-MM-dd hh:mm:ss")
	//end����������ʱ��(ʱ���ʽ��"yyyy-MM-dd hh:mm:ss")
	//����ֵ��
	//0����ӳɹ�
	//1�����ʧ�ܣ�ͨ�����Ѿ���
	//2�����ʧ�ܣ������Ѿ���ռ��
	//3�����ʧ�ܣ���ʧ��ԭ��δ���壩
	virtual int AddFileIntoPlayGroup(QStringList const filelist,QWidget *wnd,const QDateTime &start,const QDateTime &end)=0;

	//����ͬ��������
	//���������
	//num:����ͬ���������ֵ
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int SetSynGroupNum(int num)=0;
	//�鲥��
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupPlay()=0;
	//��ͣ����
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupPause()=0;
	//��������
	//����ֵ��
	//0�����óɹ�
	//����ʧ��
	virtual int GroupContinue()=0;
	//ֹͣ����
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupStop()=0;
	//���
	//���������
	//speed�����������ٶȵı�������ѡֵ��2��4��8��
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupSpeedFast(int speed)=0;
	//����
	//���������
	//speed�����������ٶȵ�1/N������ѡֵ��2��4��8��
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupSpeedSlow(int speed)=0;
	//�ָ������Ĳ����ٶ�
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupSpeedNormal() = 0;
};
#endif