#ifndef __ILOCALPLAYER_HEAD_FILE_SADVN98ASD7YASGV__
#define __ILOCALPLAYER_HEAD_FILE_SADVN98ASD7YASGV__
#include <libpcom.h>
#include <QtGui/QWidget>
#include <QtCore/QDateTime>
#include <QStringList>
interface ILocalPlayer:IPComBase
{
	//���������޶�ʱ���ڿɲ��ŵ��ļ�
	//���������
	//start���޶��Ŀ�ʼʱ��(ʱ���ʽ��"yyyy-MM-dd hh:mm:ss")
	//end���޶��Ľ���ʱ��(ʱ���ʽ��"yyyy-MM-dd hh:mm:ss")
	//filelist:�������������ļ�list
	//����ֵ��
	//0���ɹ�
	//1��ʧ��
	virtual int SearchLocalPlayFileList(const QDateTime &start,const QDateTime &end,QStringList &filelist)=0;
	//��Ӳ����ļ���ͬ�����У���ָ���ļ����ŵĴ���ID
	//���������
	//filename:�ļ�������������������·�����磺F:\project\date\devname\chl2\filename.cvi
	//wnd:���Ŵ��ڵ�id
	//����ֵ��
	//0����ӳɹ�
	//1�����ʧ�ܣ�ͨ�����Ѿ���
	//2�����ʧ�ܣ������Ѿ���ռ��
	//3�����ʧ�ܣ���ʧ��ԭ��δ���壩
	virtual int AddFileIntoPlayGroup(QString const filename,QWidget *wnd)=0;
	//ͨ���ļ��ļ����Ӳ�������ȥ���ļ�
	//���������
	//filename���ļ����ƣ���������������·�����磺F:\project\date\devname\chl2\filename.cvi
	//����ֵ��
	//0��ȥ���ɹ�
	//1��ȥ��ʧ��
	virtual int RemoveFileFromPlayGroupByFilename(QString const filename)=0;
	//ͨ������id�Ӳ�������ȥ���ļ�
	//���������
	//Wnd�����ڵ�id
	//����ֵ��
	//0��ȥ���ɹ�
	//1��ȥ��ʧ��
	virtual int RemoveFileFromPlayGroupByWnd(QWidget *Wnd)=0;
	//����ͬ��������
	//���������
	//num:����ͬ���������ֵ
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int SetSynGroupNum(int num)=0;
	//�鲥��
	//�������:
	//start:��ʼ����ʱ��(ʱ���ʽ��"yyyy-MM-dd hh:mm:ss")
	//end����������ʱ��(ʱ���ʽ��"yyyy-MM-dd hh:mm:ss")
	//����ֵ��
	//0�����óɹ�
	//1������ʧ��
	virtual int GroupPlay(const QDateTime & start,const QDateTime &end)=0;
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