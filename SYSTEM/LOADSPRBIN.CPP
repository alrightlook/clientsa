#define sprVERSION	3				//SPR の　対応バージョン
#include 	"system.h"
#include 	"loadsprbin.h"
#include 	"anim_tbl.h"

#include	"../oft/vg410.h"
#include	"../oft/work.h"

#define ERRPRINT(a);

#define maxBUFFER	(0x2fffff)
U2 TBbuffer[maxBUFFER];
U2 *ptAnim;

SPRITEDATA SpriteData[mxSPRITE];



//
// アニメーションファイル読み込み
//
BOOL InitSprBinFileOpen( char *SPR_BIN, char *SPRADRN_BIN )
{
	char useFlag[mxSPRITE];
	SPRADRN spradrn[mxSPRITE];
	SPRADRN _spradrn;
	U2 buf[30000];
	U2 *ptBuf;
	FILE *fp1, *fp2;
	int i, j;
	unsigned int k;
	int size;


	if( (fp1 = fopen( SPR_BIN, "rb" )) == NULL )
	{
		return FALSE;
	}

	if( (fp2 = fopen( SPRADRN_BIN, "rb" )) == NULL )
	{
		return FALSE;
	}

	memset( TBbuffer, 0, sizeof( TBbuffer ) );
	memset( SpriteData, 0, sizeof( SpriteData ) );
	memset( spradrn, 0, sizeof( spradrn ) );
	memset( useFlag, 0, sizeof( useFlag ) );

	for( i = 0; i < mxSPRITE; i++ )
	{
		fread( &_spradrn, 1, sizeof( _spradrn ), fp2 );
		if( feof( fp2 ) != 0 )
			break;
		spradrn[_spradrn.sprNo - SPRSTART].sprNo = _spradrn.sprNo - SPRSTART;
		spradrn[_spradrn.sprNo - SPRSTART].offset = _spradrn.offset;
		spradrn[_spradrn.sprNo - SPRSTART].animSize = _spradrn.animSize;
		useFlag[_spradrn.sprNo - SPRSTART] = 1;
	}
	if( i >= mxSPRITE )
	{
		fclose( fp1 );
		fclose( fp2 );
		return FALSE;
	}

	ptAnim = TBbuffer;
	for( i = 0; i < mxSPRITE; i++ )
	{
		if( useFlag[i] != 0 )
		{
			SpriteData[i].animSize = spradrn[i].animSize;
			SpriteData[i].ptAnimlist = (ANIMLIST *)ptAnim;
			ptAnim += sizeof( ANIMLIST ) / sizeof( U2 ) * SpriteData[i].animSize;
			fseek( fp1, spradrn[i].offset, SEEK_SET );
			for( j = 0; j < SpriteData[i].animSize; j++ )
			{
				fread( buf, 1, 12, fp1 );
				ptBuf = buf;

				SpriteData[i].ptAnimlist[j].dir = *ptBuf++;	// 向き
				SpriteData[i].ptAnimlist[j].no = *ptBuf++;	// どんなアニメーションかをあらわす番号
				SpriteData[i].ptAnimlist[j].dtAnim = *ptBuf++;
				SpriteData[i].ptAnimlist[j].dtAnim |= ((*ptBuf)<<16);
				ptBuf++;
				SpriteData[i].ptAnimlist[j].frameCnt = *ptBuf++;
				SpriteData[i].ptAnimlist[j].frameCnt |= ((*ptBuf)<<16);
				ptBuf++;

				//１枚の表示割り込み数計算
				if( SpriteData[i].ptAnimlist[j].frameCnt )
				{
					SpriteData[i].ptAnimlist[j].dtAnim =
						SpriteData[i].ptAnimlist[j].dtAnim /
							(SpriteData[i].ptAnimlist[j].frameCnt << 4);
					if( SpriteData[i].ptAnimlist[j].dtAnim < 1 )
						SpriteData[i].ptAnimlist[j].dtAnim = 1;
				}

				SpriteData[i].ptAnimlist[j].ptFramelist = (FRAMELIST *)ptAnim;
				ptAnim += sizeof( FRAMELIST ) / sizeof( U2 )
					* SpriteData[i].ptAnimlist[j].frameCnt;

				size = 5 * SpriteData[i].ptAnimlist[j].frameCnt;
				fread( buf, 1, size*sizeof( U2 ), fp1 );
				ptBuf = buf;

				for( k = 0; k < SpriteData[i].ptAnimlist[j].frameCnt; k++ )
				{
					SpriteData[i].ptAnimlist[j].ptFramelist[k].BmpNo   = *ptBuf++;
					SpriteData[i].ptAnimlist[j].ptFramelist[k].BmpNo   |= ((*ptBuf)<<16);
					ptBuf++;
					SpriteData[i].ptAnimlist[j].ptFramelist[k].PosX    = *((S2 *)ptBuf);
					ptBuf++;
					SpriteData[i].ptAnimlist[j].ptFramelist[k].PosY    = *((S2 *)ptBuf);
					ptBuf++;
					SpriteData[i].ptAnimlist[j].ptFramelist[k].SoundNo = *ptBuf++;
				}
			}
		}
	}

	fclose( fp1 );
	fclose( fp2 );

	if( ptAnim >= &TBbuffer[maxBUFFER] )
		return FALSE;

	return TRUE;
}

