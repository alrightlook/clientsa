// unpack.cpp �إå��ե�����

#ifndef _UNPACK_H_
#define _UNPACK_H_
/*
typedef struct
{
	unsigned char id[2];
	unsigned char compressFlag;  //ѹ����־1��ѹ��
	unsigned int width;
	unsigned int height;
	unsigned int size;
} RD_HEADER;*/

 typedef struct
{
 unsigned char id[2];  //�̶�Ϊ'RD'
 unsigned char compressFlag;  //ѹ����־1��ѹ�� //ż����ʾδѹ������λͼ��ţ��������ʾѹ����
 unsigned int width;  //����;
 unsigned int height;
 unsigned int size; //���ݿ�ĳ��ȣ���������ͷ�����ĳ���(16BYTE)
 //������͵�ַ�ļ��е�һ����ͼ�����ݽ���������ͷ���档
} RD_HEADER;

unsigned char *encoder( unsigned char *, unsigned char **,
	unsigned int, unsigned int, unsigned int *, int );
unsigned char *decoder( unsigned char *, unsigned char **,
	unsigned int *, unsigned int *, unsigned int * );

#endif