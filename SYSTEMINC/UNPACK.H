// unpack.cpp ヘッダファイル

#ifndef _UNPACK_H_
#define _UNPACK_H_
/*
typedef struct
{
	unsigned char id[2];
	unsigned char compressFlag;  //压缩标志1是压缩
	unsigned int width;
	unsigned int height;
	unsigned int size;
} RD_HEADER;*/

 typedef struct
{
 unsigned char id[2];  //固定为'RD'
 unsigned char compressFlag;  //压缩标志1是压缩 //偶数表示未压缩，按位图存放；奇数则表示压缩过
 unsigned int width;  //宽度;
 unsigned int height;
 unsigned int size; //数据块的长度，包括数据头本身的长度(16BYTE)
 //后三项和地址文件中的一样，图像数据紧跟在数据头后面。
} RD_HEADER;

unsigned char *encoder( unsigned char *, unsigned char **,
	unsigned int, unsigned int, unsigned int *, int );
unsigned char *decoder( unsigned char *, unsigned char **,
	unsigned int *, unsigned int *, unsigned int * );

#endif
