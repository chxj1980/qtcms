#ifndef __H264WH_H__
#define __H264WH_H__
/*return: 0 ������sps���õ������Ϣ, -1ʧ��
 *param:  stream : ��������
 *		  stream_len : ��������(byte)���������
 *        width/height:��ſ����Ϣ
 *
 */
int GetWidthHeight(char *stream,int stream_len,int *width,int *height);

#endif