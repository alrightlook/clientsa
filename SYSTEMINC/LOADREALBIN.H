#ifndef _LOADREALBIN_H_
#define _LOADREALBIN_H_

#include <stdio.h>
//#include "common.h"

typedef unsigned char MOJI;
typedef unsigned char U1;
typedef          char S1;
typedef unsigned short U2;
typedef          short S2;
typedef unsigned long U4;
typedef          long S4;
typedef float  F4;
typedef double F8;

int realGetImage(int graphicNo, unsigned char **bmpdata, int *width, int *height);
int Linear(void);


int initRealbinFileOpen(char *realbinfilename, char *addrbinfilename);
void cleanupRealbin(void);

void initAutoMapColor( char * );
void makeAutoMapColor( void );
int writeAutoMapColor( char *, char * );
int readAutoMapColor( char *, char * );

extern unsigned char autoMapColorTbl[];

//extern	PBITMAPINFO         PBiInfo;
//extern	PBYTE               PBits;
//extern	BITMAPFILEHEADER    header;

//����ե��å��ʥ�Щ`�������λ�ä��餦
//	���� -1:ʧ�� or ����ɹ�
//ͼ�κ���ȵõ�վ����λ��/ /��ֵ- 1:ʧ�ܻ�����ɹ�
int realGetPos(U4 GraphicNo , S2 *x , S2 *y);

//����ե��å��ʥ�Щ`���饰��ե��å������ߤ��򷵤�
//	���� -1:ʧ�� or ����ɹ�
//ͼ�κ�ƽ������߻ر�
int realGetWH(U4 GraphicNo , S2 *w , S2 *h);

//����ե��å��ʥ�Щ`��굱���ꥵ���������롣
//	���� -1:ʧ�� or ����ɹ� ͼ�α�Ҫ��ÿ�ųߴ硣
int realGetHitPoints(U4 GraphicNo , S2 *HitX , S2 *HitY);

//����ե��å��ʥ�Щ`��굱�����ж������롣
//	���� -1:ʧ�� or ����ɹ� ͼ�κ����Ҫ��ÿ�ж� ����
int realGetHitFlag(U4 GraphicNo , S2 *Hit);

//����ե��å��ʥ�Щ`����軭�����λ�Q������ȡ�������
//	���� -1:ʧ�� or ����ɹ� ͼ�κ���Ȼ�ͼ����˳�������ȡ����
BOOL realGetPrioType(U4 GraphicNo , S2 *prioType);

//����ե��å��ʥ�Щ`���ߤ����Пo�����롣
//	���� -1:ʧ�� or ����ɹ� 
int realGetHeightFlag(U4 GraphicNo , S2 *Height);

//�����˥ӥåȥޥåץե����뷬��(�ե������������֤Τ�)���뤨�ƥ���ե��å��ʥ�Щ`��ͨ�����ţ��򷵤�
//	���� -1:ʧ�� or ����ɹ�
int realGetNo( U4 CharAction , U4 *GraphicNo );

//����ե��å��ʥ�Щ`���h������
//	���ꂎ��:��0 �O���ʤ�
int realGetSoundEffect(U4 GraphicNo);

//����ե��å��ʥ�Щ`���i������
//	���ꂎ��:��0 �O���ʤ�
int realGetWalkSoundEffect(U4 GraphicNo);

//�����˥���ե��å��ʥ�Щ`��ͨ�����ţ����뤨�ƥӥåȥޥåץե����ַ��Ť򷵤�
int realGetBitmapNo( int num );


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//��ͼ·���ṹ ħ���� MAP_ATTRֻ��5λ ��֪����ʲô�ģ�����
typedef struct {
	unsigned char atari_x,atari_y;	//�󤭤�  �ߴ�
	unsigned short hit;				//�i���뤫 0 ������ 1�ϰ���???
	short height;				//�ߤ����뤫 �߶�
	short broken;				//����뤫  ��
	short indamage;				//��HP����`��  ��HP����
	short outdamage;			//������`�� �˺�
	short inpoison;				//�붾 ??
	short innumb;				//�뤷�Ӥ� ��ľ
	short inquiet;				//�����a  �����Ĭ  
	short instone;				//��ʯ��  
	short indark;				//�밵� �ڰ�
	short inconfuse;			//�����  
	short outpoison;			//����
	short outnumb;				//���w��
	short outquiet;				//�����a
	short outstone;				//��ʯ��
	short outdark;				//�����
	short outconfuse;			//������
	//Ч��1�����濴�����r��������Ч���ȣ�?
	short effect1;				//���ե�����1�������Ҋ����r�������愿���ʤɣ�  ????1?????????????�g��????َ??ӕ???
	//Ч��2�������ʱ������������Ч���ȣ�
	short effect2;				//���ե�����2����ä��Ȥ��������愿���ʤɣ�  ????2?????���a?�g��????َ??ӕ???
	unsigned short damy_a;
	unsigned short damy_b;
	unsigned short damy_c;
	unsigned int bmpnumber;		//���񷬺� ͼƬ��
} MAP_ATTR;

struct ADRNBIN{
	unsigned long	bitmapno;
	unsigned long	adder;
	unsigned long	size;
	int	xoffset;
	int	yoffset;
	unsigned int width;
	unsigned int height;

	MAP_ATTR attr;

};

#endif /*_LOADREALBIN_H_*/

/*
typedef struct {
unsigned char atari_x,atari_y; //�󤭤�  �ߴ�  ռ������������ռ�Ĵ�С��1�ͱ�ʾռ1��
unsigned short hit;    //�i���뤫 0 ������ 1�ϰ���??? 0��ʾ�ϰ��1��ʾ��������ȥ
short height;    //�ߤ����뤫 �߶�
short broken;    //����뤫  ��
short indamage;    //��HP����`��  ��HP����
short outdamage;   //������`�� �˺�
short inpoison;    //�붾 ??
short innumb;    //�뤷�Ӥ� ��ľ
short inquiet;    //�����a  �����Ĭ  
short instone;    //��ʯ��  
short indark;    //�밵� �ڰ�
short inconfuse;   //�����  
short outpoison;   //����
short outnumb;    //���w��
short outquiet;    //�����a
short outstone;    //��ʯ��
short outdark;    //�����
short outconfuse;   //������
//Ч��1�����濴�����r��������Ч���ȣ�?
short effect1;    //���ե�����1�������Ҋ����r�������愿���ʤɣ�  ????1?????????????�g��????َ??ӕ???
//Ч��2�������ʱ������������Ч���ȣ�
short effect2;    //���ե�����2����ä��Ȥ��������愿���ʤɣ�  ????2?????���a?�g��????َ??ӕ???
unsigned short damy_a;
unsigned short damy_b;
unsigned short damy_c;
unsigned int bmpnumber;  //���񷬺� ͼƬ��ID   �ڵ�ͼ�ļ���ı��
} MAP_ATTR;
struct ADRNBIN{
unsigned long bitmapno;  // ͼƬ���к�  ͼƬ�ı��
unsigned long adder;   // ָ��ͼƬ�������ļ��е���ʼλ��
unsigned long size; // ͼƬ���ݿ�Ĵ�С
int xoffset;   //��ʾͼƬʱ��������ƫ��X
int yoffset;
unsigned int width;    //ͼƬ����;
unsigned int height;
MAP_ATTR attr;
};
*/