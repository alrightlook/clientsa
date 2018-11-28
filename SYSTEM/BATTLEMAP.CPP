/************************/
/*	battleMap.c			*/
/************************/
#include "system.h"
#include "ime_sa.h"
#include "battleMap.h"

// �Хȥ�ޥåפΥ�����
#define BATTLE_MAP_SIZE 400

// �F�ڤΥХȥ�ޥå׷���
int BattleMapNo = 0;

// �Хȥ�ޥå� �ե�������
char *BattleMapFile[] = {	
	#include "battleMapName.h"
};
						
// �Хȥ�ޥå��i���z�ߤȥХȥ륵�`�ե����λ������� ***************************/
BOOL ReadBattleMap( int no )
{
	char msg[ 256 ];
	FILE *fp;
	char header[ 5 ];
	unsigned short tile[ BATTLE_MAP_SIZE ];
	int cnt = 0;
	int i, j, x = 0, y = 0, posX, posY;
	unsigned short c1, c2;
	
	// ��ߥåȥ����å�
	if( no >= BATTLE_MAP_FILES ) no = 0;
	
	// �Хȥ�ޥå׷���ӛ��
	BattleMapNo = no;
	
	// �Хȥ�ޥåץե����륪�`�ץ�
	if( ( fp = fopen( BattleMapFile[ no ], "rb" ) ) == NULL )
	{
		sprintf( msg, "%s ���i���z�ߤ�ʧ�����ޤ�����", BattleMapFile[ no ] );
		MessageBox( hWnd, msg, "�Хȥ�ޥåץ���`", MB_OK | MB_ICONSTOP );
		return FALSE;
	}
	
	// �إå��`���i���z��
	fread( header, sizeof( char ), 4, fp );
	header[ 4 ] = '\0';
	
	// �إå��`�����å�
	if( !( strstr( header, "SAB" )) )
	{
		MessageBox( hWnd, "SAB �ե�����ǤϤ���ޤ���", "�Хȥ�ޥåץ���`", MB_OK | MB_ICONSTOP );
	}
	
	// �ݥ����M�ޤ��루 �إå��w�Ф� ��
	//fseek( fp, 4, SEEK_SET );
	
	// ������ǩ`�����i���z��
	//fread( tile, sizeof( short ), BATTLE_MAP_SIZE, fp );
	
	//fread( tile, 1, 800, fp );
	
	// �i���z�ߥ�`��
	for( i = 0 ; i < BATTLE_MAP_SIZE ; i++ )
	{
		// ���Х��Ȥ����i���z�ࣨ �ǩ`�����줬��Τ��� ��
		c1 = fgetc( fp );
		c2 = fgetc( fp );
		
		// ���Х��Ȥˤ���
		tile[ i ] = ( c1 << 8 ) | c2;
	}
	// �ե������]����
	fclose( fp );
	
	// λ���O��
	posX = 32 * ( -9 );
	posY = 24 * 10;
	
	// ������֥�`��
	for( i = 0 ; i < 20 ; i++ ){
		x = 0;
		y = 0;
		for( j = 0 ; j < 20 ; j++ ){
			// �£ͣб�ʾ�Хåե��ˤ����
			StockDispBuffer( posX + x, posY + y, DISP_PRIO_TILE, tile[ cnt++ ], 0 );
			
			x += 32;
			y -= 24;
		}
		posX += 32;
		posY += 24;
	}
	return TRUE;
}	
	
// �Хȥ�ޥå��軭 ***********************************************************/
#define	RASTER_CLEARANCE	8
int piyo_tbl[]={
	0,1,2,3,4,5,6,7,8,9,10,10,11,11,11,12,
	12,12,11,11,11,10,9,9,8,7,6,5,4,3,2,1,
	0,-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-10,-11,-11,-11,-12,
	-12,-12,-11,-11,-11,-10,-9,-9,-8,-7,-6,-5,-4,-3,-2,-1,
};
int piyo_point = 0;

void DrawBattleMap( void )
{
	RECT rect = { 0, 0, DEF_APPSIZEX, DEF_APPSIZEY };
	int d0,d1,d6,d7;
	short bx , by;

	if(BattleMapNo >= 148 && BattleMapNo <= 150){		//�饹���`�������`��ʤ�
		d1 = 1;
	} else {
		d1 = 0;
	}

//	switch( BattleMapNo & 1 ){
	switch( d1 ){
	case 0:
		//bx = Rnd( 0,10);
		//by = Rnd( 0,10);
		bx = 0;
		by = 0;
		short x0, y0;
		long w, h;

		// ܞ���I��Υ��å�
		x0 = bx;
		y0 = by;
		w = rect.right - rect.left;
		h = rect.bottom - rect.top;

		// ����åԥ󥰄I��
		//   �����ʤߤ� RECT ���������ˤΥɥåȤϱ�ʾ����ʤ���

		// ȫ����ʾ���벿�֤��o����Б���
		if( bx >= lpDraw->xSize || bx + w <= 0 || by >= lpDraw->ySize || by + h <= 0 ){
			return;
		}
		
		// ��ˤΥ����å�
		if( bx < 0 ){
			rect.left -= bx;
			x0 = 0;
		}
		// �ҶˤΥ����å�
		if( bx + w > lpDraw->xSize ){
			rect.right -= bx + w - lpDraw->xSize;
		}
		// �϶ˤΥ����å�
		if( by < 0 ){
			rect.top -= by;
			y0 = 0;
		}
		// �¶ˤΥ����å�
		if( by + h > lpDraw->ySize ){
			rect.bottom -= by + h - lpDraw->ySize;
		}
		
#ifdef _DEBUG		
		// �F�ڱ�ʾ���Ƥ��륵�`�ե��������������
		SurfaceDispCnt++;
#endif
		
		// �Хå����`�ե����ظ���ܞ��
		lpDraw->lpBACKBUFFER->BltFast( x0, y0, lpBattleSurface, &rect, DDBLTFAST_WAIT );
		break;

	case 1:
		d6 = 0;
		rect.left = 0;		//���
		rect.right = 640;		//�Ҷ�
		for(d7=0; d7<480; d7+=RASTER_CLEARANCE){
			rect.top = d7 + piyo_tbl[(d6+piyo_point) & 63] + 12;		//�϶�
			rect.bottom = rect.top + RASTER_CLEARANCE;		//�¶�
			// �Хå����`�ե����ظ���ܞ��
			lpDraw->lpBACKBUFFER->BltFast( 0, d7, lpBattleSurface, &rect, DDBLTFAST_WAIT );
			d6++;
		}
		break;

	case 2:
		d6 = 0;
		for(d7=0; d7<480; d7+=RASTER_CLEARANCE/2){
			rect.top = d7;		//�϶�
			rect.bottom = rect.top + RASTER_CLEARANCE;		//�¶�

			d0 = piyo_tbl[(d6+piyo_point) & 63];		//���
			if(d0 < 0){		//�ޥ��ʥ��ʤ�
				rect.left = 0 - d0;
				rect.right = 640 + d0;		//�Ҷ�
				d0 = 0;
			} else {
				rect.left = 0;
				rect.right = 640 - d0;		//�Ҷ�
			}
			// �Хå����`�ե����ظ���ܞ��
			lpDraw->lpBACKBUFFER->BltFast( d0, d7, lpBattleSurface, &rect, DDBLTFAST_WAIT );
			d6++;
		}
		break;
	}
}


