//
// データ圧縮展開
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"unpack.h"

#define BIT_CMP			(unsigned char)0x80
#define BIT_ZERO		(unsigned char)0x40
#define BIT_REP_LARG	(unsigned char)0x10
#define BIT_REP_LARG2	(unsigned char)0x20

// 圧縮
// 引数
// 		buf    ... 圧縮するデータへのポインタ  压缩数据的指针
//		disBuf ... 圧縮後のデータを入れるポインタ（NULLを入れとけばメモリを自動で確保）
//压缩后的数据放入指针（NULL放入就能让内存自动确保）

//		width  ... 画像の幅  图像宽度
//		height ... 画像の高さ 图像的高度
//		len    ... 圧縮後のサイズへのポインタ  压缩后的尺寸的指针
//		cmpFlag ... 1 で圧縮。0 で非圧縮。 1压缩。0非压缩。
// 戻り値 返回值
// 		圧縮データへのポインタ。失敗は0  压缩数据的指针。失败是0
unsigned char *encoder( unsigned char *buf, unsigned char **disBuf,
	unsigned int width, unsigned int height, unsigned int *len, int cmpFlag )
{
	unsigned char *wBuf, *ewBuf, *eBuf;
	unsigned char *wBuf1, *buf1;
	unsigned char *wBuf2, *buf2;
	unsigned char idx;
	int flag = 1;
	RD_HEADER *header;
	unsigned int cnt, cnt2;
	unsigned char repData;
	int mode;
	unsigned int l, el;
	int addWBuf;

	// メモリをヘッダ＋画像サイズ分確保
	//内存+图像大小分确保头
	if( *disBuf == NULL )
	{
		if( (wBuf = (unsigned char *)malloc( width * height + sizeof( RD_HEADER ) )) == NULL )
		{
			return NULL;
		}
		*disBuf = wBuf;
	}
	else
	{
		wBuf = *disBuf;
	}

	if( cmpFlag )
	{
		flag = 1;
	}
	else
	{
		flag = 0;
	}

	// バッファのエンドアドレス 缓冲的结束地址
	eBuf = buf + width * height;
	ewBuf = wBuf + width * height + sizeof( RD_HEADER );

	wBuf1 = wBuf+sizeof( RD_HEADER );
	buf1 = buf;
	while( cmpFlag )
	{
		// 終了チェック
		if( buf1 >= eBuf )
		{
			// 読み込みデータが無くなった
			flag = 1;
			break;
		}
		if( wBuf1 >= ewBuf )
		{
			// 圧縮データサイズが元データサイズを超えたので終了
			flag = 0;
			break;
		}

		// ゼロの圧縮（２個以上なら圧縮）
		if( *buf1 == 0 && *(buf1+1) == 0 )
		{
			// まずは圧縮個数を数える
			idx = BIT_CMP | BIT_ZERO;
			cnt = 2;
			buf1 += 2;
			while( buf1 < eBuf && cnt < 0xfffff )
			{
				if( *buf1 != 0 )
				{
					break;
				}
				buf1++;
				cnt++;
			}
			// ０の連続が16個未満の時
			if( cnt <= 0xf )
			{
				addWBuf = 0;
			}
			else
			// 16個以上4096未満の時
			if( cnt <= 0xfff )
			{
				addWBuf = 1;
			}
			// 4096以上の時
			else
			{
				addWBuf = 2;
			}

			// 圧縮データサイズが元データサイズを超えたので終了
			if( wBuf1+addWBuf >= ewBuf )
			{
				flag = 0;
				break;
			}

			// 圧縮データを書き込む
			if( addWBuf == 0 )
			{
				idx |= (cnt & 0x0f);
				*wBuf1++ = idx;
				continue;
			}
			else
			if( addWBuf == 1 )
			{
				idx |= (BIT_REP_LARG | ((cnt >> 8) & 0xf));
				*wBuf1++ = idx;
				*wBuf1++ = (unsigned char)(cnt & 0xff);
				continue;
			}
			else
			{
				idx |= (BIT_REP_LARG2 | ((cnt >> 16) & 0xf));
				*wBuf1++ = idx;
				*wBuf1++ = (unsigned char)((cnt >> 8) & 0xff);
				*wBuf1++ = (unsigned char)(cnt & 0xff);
				continue;
			}
		}
#if 1
		// ゼロ以外で圧縮（３個以上なら圧縮）
		if( *buf1 == *(buf1+1) && *buf1 == *(buf1+2) )
		{
			repData = *buf1;
			idx = BIT_CMP;
			cnt = 3;
			buf1 += 3;
			while( buf1 < eBuf && cnt < 0xfffff )
			{
				if( *buf1 != repData )
				{
					break;
				}
				buf1++;
				cnt++;
			}

			// 連続データが16個未満の時
			if( cnt <= 0xf )
			{
				addWBuf = 1;
			}
			else
			// 16個以上4096未満の時
			if( cnt <= 0xfff )
			{
				addWBuf = 2;
			}
			// 4096以上の時
			else
			{
				addWBuf = 3;
			}

			// 圧縮データサイズが元データサイズを超えたので終了
			if( wBuf1+addWBuf >= ewBuf )
			{
				flag = 0;
				break;
			}

			// 圧縮データを書き込む
			if( addWBuf == 1 )
			{
				idx |= (cnt & 0x0f);
				*wBuf1++ = idx;
				*wBuf1++ = repData;
				continue;
			}
			else
			if( addWBuf == 2 )
			{
				idx |= (BIT_REP_LARG | ((cnt >> 8) & 0xf));
				*wBuf1++ = idx;
				*wBuf1++ = repData;
				*wBuf1++ = (unsigned char)(cnt & 0xff);
				continue;
			}
			else
			{
				idx |= (BIT_REP_LARG2 | ((cnt >> 16) & 0xf));
				*wBuf1++ = idx;
				*wBuf1++ = repData;
				*wBuf1++ = (unsigned char)((cnt >> 8) & 0xff);
				*wBuf1++ = (unsigned char)(cnt & 0xff);
				continue;
			}
		}
#endif

		// 圧縮無し
		// 最初はどこまで圧縮しないかを見るだけ
		idx = 0;
		cnt2 = 0;
		buf2 = buf1;
		wBuf2 = wBuf1;
		while( 1 )
		{
			// 終了チェック
			if( buf2 >= eBuf		// 読み込むデータが無いので終了
			 || cnt2 >= 0xfff )		// 一回に扱えるデータ長を超えたので次へ
			{
				mode = 0;
				break;
			}
			if( wBuf2 >= ewBuf )
			{
				// 圧縮データサイズが元データサイズを超えたので終了
				mode = 1;
				break;
			}
			// データの最後から４番目以前なら圧縮データがあるか確認する
			// それ以外は無圧縮として扱う
			if( buf2+2 < eBuf )
			{
				if( *buf2 == 0 && *(buf2+1) == 0 )
				{
					mode = 0;
					break;
				}
				if( *buf2 != 0 && *buf2 == *(buf2+1) && *(buf2+1) == *(buf2+2) )
				{
					mode = 0;
					break;
				}

			}
			buf2++;
			wBuf2++;
			cnt2++;
		}

		if( cnt2 <= 0xf )
		{
			addWBuf = 0;
		}
		else
		if( cnt2 <= 0xfff )
		{
			addWBuf = 1;
		}
		else
		{
			addWBuf = 2;
		}

		if( mode == 1
		 || wBuf2+addWBuf >= ewBuf )
		{
			// 圧縮データサイズが元データサイズを超えたので終了
			flag = 0;
			break;
		}

		// 非圧縮データの数がわかったので転送
		if( addWBuf == 0 )
		{
			idx = (cnt2 & 0xf);
			*wBuf1++ = idx;
		}
		else
		if( addWBuf == 1 )
		{
			idx = BIT_REP_LARG | ((cnt2 >> 8) & 0xf);
			*wBuf1++ = idx;
			*wBuf1++ = (unsigned char)(cnt2 & 0xff);
		}
		else
		{
			idx = BIT_REP_LARG2 | ((cnt2 >> 16) & 0xf);
			*wBuf1++ = idx;
			*wBuf1++ = (unsigned char)((cnt2 >> 8) & 0xff);
			*wBuf1++ = (unsigned char)(cnt2 & 0xff);
		}
		for( cnt = 0; cnt < cnt2; cnt++ )
		{
			*wBuf1++ = *buf1++;
		}
	}


	header = (RD_HEADER *)wBuf;
	header->id[0] = 'R';
	header->id[1] = 'D';
	header->width = width;
	header->height = height;

	if( flag == 1 )
	{
		header->compressFlag = 1;
		header->size = wBuf1 - wBuf;
		l = header->size;
	}
	else
	// 圧縮せずにコピー
	{
		header->compressFlag = 0;
		header->size = (int)wBuf + width * height + sizeof( RD_HEADER );
		wBuf1 = wBuf + sizeof( RD_HEADER );
		buf1 = buf;
		el = width * height;
		for( l = 0; l < el; l++ )
			*wBuf1++ = *buf1++;
		l += sizeof( RD_HEADER );
	}

	*len = l;
	return wBuf;
}


// 展開
// 引数
// 		buf    ... 展開するデータへのポインタ 展开对数据的指针
//		disBuf ... 展開後のデータを入れるポインタ 展开后的数据放入指针
//		width  ... 画像の幅へのポインタ 图像宽度的指针
//		height ... 画像の高さへのポインタ 图像的高度的指针
//		len    ... 展開するデータサイズへのポインタ  展开数据大小的指针
// 戻り値   返回值/ /展开对数据的指针。失败是0
// 		展開データへのポインタ。失敗は0
unsigned char *decoder( unsigned char *buf, unsigned char **disBuf,
	unsigned int *width1, unsigned int *height1, unsigned int *len )
{
	RD_HEADER *header;
	unsigned char *wBuf, *ewBuf, *eBuf;
	unsigned char *wBuf1, *buf1;
	unsigned int width, height;
	unsigned int cnt;
	unsigned int l, el;
	unsigned char repData;
	unsigned char idx;

	if( *disBuf == NULL )
		return NULL;

	wBuf = *disBuf;

	header = (RD_HEADER *)buf;

	// フォーマットのチェック 格式的检查
	if( header->id[0] != 'R' || header->id[1] != 'D' )
	{
		return NULL;
	}

	width  = header->width;
	height = header->height;

#if 0
	// メモリを画像サイズ分確保
	if( (wBuf = (unsigned char *)malloc( width * height )) == NULL )
	{
		return NULL;
	}
#endif

	// 圧縮されてなければコピー  无压缩的话复制
	if( header->compressFlag == 0 )
	{
		wBuf1 = wBuf;
		buf1 = buf+sizeof( RD_HEADER );
		el = width * height;
		for( l = 0; l < el; l++ )
			*wBuf1++ = *buf1++;

		*len = l;
		*width1 = width;
		*height1 = height;
		return wBuf;
	}

	// バッファのエンドアドレス 缓冲的结束地址
	eBuf = buf + header->size;
	ewBuf = wBuf + width * height;

	wBuf1 = wBuf;
	buf1 = buf+sizeof( RD_HEADER );
	while( buf1 < eBuf )
	{
		idx = *buf1++;
		// 圧縮されいる  被压缩的
		if( (idx & BIT_CMP) != 0 )
		{
			// ゼロ圧縮の場合  无压缩
			if( (idx & BIT_ZERO) != 0 )
			{
				repData = 0;
			}
			else
			// その他の場合  其他情况
			{
				repData = *buf1++;
			}
			// 4096個以上連続の時  4096个以上连续的时候
			if( (idx & BIT_REP_LARG2) != 0 )
			{
				cnt = ((idx & 0x0f)<<16);
				cnt |= ((*buf1) << 8);
				buf1++;
				cnt |= *buf1++;
			}
			else
			// 16個以上連続の時
			if( (idx & BIT_REP_LARG) != 0 )
			{
				cnt = ((idx & 0x0f)<<8);
				cnt |= *buf1++;
			}
			else
			{
				cnt = (idx & 0x0f);
			}
			memset( wBuf1, repData, cnt );
			wBuf1 += cnt;
		}
		else
		// 圧縮されてない  不被压缩
		{
			// １６個以上連続の時
			if( (idx & BIT_REP_LARG) != 0 )
			{
				cnt = ((idx & 0x0f)<<8);
				cnt |= *buf1++;
			}
			else
			{
				cnt = (idx & 0x0f);
			}
			if( cnt >= 0xfffff )
			{
				// ここにきたらどっか変  来这里的话哪里奇怪
				return NULL;
			}
			for( l = 0; l < cnt; l++ )
				*wBuf1++ = *buf1++;
		}
	}

	*len = width * height;
	*width1  = width;
	*height1 = height;
	return wBuf;
}

