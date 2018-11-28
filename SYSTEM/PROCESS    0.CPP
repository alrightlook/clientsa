/************************/
/*	process.c		进程重要处理事务	*/
/************************/
#include "system.h"
#include "../ohta/ohta.h"
#include "../dwaf/dwafTestProc.h"
#include "ime_sa.h"
#include "menu.h"
#include "pc.h"
#include "character.h"
#include "login.h"
#include "netproc.h"
#include "savedata.h"
#include "testView.h"
#include "battleProc.h"
#include "produce.h"
#include "lssproto_cli.h"
#include "netmain.h"
#include "battleMenu.h"
#include "t_music.h"
#include "field.h"
#include "handletime.h"

/* 命令处理隐藏 *******************************************************************/
extern void kakushi_command(void);
/* 进程号 */
UINT ProcNo;
/* 子进程 */
UINT SubProcNo;

int palNo;//兄弟
int oldPalNo;

/* 僾儘僙僗張棟 ********************************************************************/
void Process( void )
{
	/* 僾儘僙僗暘婒 */
	switch( ProcNo ){
	
		case PROC_OPENNING:
		
			break;
			
		case PROC_INIT: 	/* 弶婜壔僾儘僙僗 */
		
			InitProc();
			
			break;
			

		// 身份证，密码处理
		case PROC_ID_PASSWORD:
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			idPasswordProc();
			RunAction();			// 执行动作
			StockTaskDispBuffer();	// 显示数据缓冲区的任务
			FlashKeyboardCursor();	// 处理光标闪烁键盘/ /
			ImeProc();				// 输入法有关的处理
			break;

		// 标题和菜单选择
		case PROC_TITLE_MENU:
			// /改变了退缓冲区
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			titleProc();
			TitleProduce();			// 职衔/ 
			RunAction();			// /执行动作
			StockTaskDispBuffer();	// 显示数据缓冲区的任务
			kakushi_command();		//隐藏的命令处理
			break;


		// 人物选择
		case PROC_CHAR_SELECT:		// 人物选择
			// 更改后的渲染处理
			BackBufferDrawType = DRAW_BACK_NORMAL;// 标准
			selectCharacterProc();
			break;

		// 创建人物
		case PROC_CHAR_MAKE:
			// 渲染处理
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			makeCharacterProc();
			break;

		// 开始登陆选定的人物
		case PROC_CHAR_LOGIN_START:
			// 转换人物
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			// charLoginStart();因此如何从网络上的数据备份缓冲区
			//初始化以保持其
			initMap();		// 地图
			initPcAll();	// PC的初始化信息（清除
			initCharObj();	// 初始化人物管理表
			initOpenServerWindow();	// 初始化的窗口中显示服务器的线路
			initFieldProc();		// 窗口初始化
			initMapEffect();		// 地图初始化的影响
			EncountFlag = FALSE;
			logOutFlag = FALSE;
			InitOhtaParam();	// 其他参数
			
			ChangeProc( PROC_CHAR_LOGIN );
			// 俛俧俵僼僃乕僪傾僂僩奐巒
			fade_out_bgm();//退出GM？
			break;

		case PROC_CHAR_LOGIN:
			// 僶僢僋僶僢僼傽乕昤夋曽朄曄峏
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLoginProc();
			break;

		// 儘僌傾僂僩
		case PROC_CHAR_LOGOUT:
			// 日志输出
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			characterLogoutProc();
			break;


		case PROC_GAME:     /*进程的游戏 */
		
			GameProc();
			break;

		case PROC_DISCONNECT_SERVER://断开游戏？
			switch( SubProcNo )
			{
				case 0:
					// 弶婜壔
					// PC儕僙僢僩  / /初始化/ /电脑复位
					resetPc();
					// 人物管理表
					initCharObj();
					// 取消动作
					DeathAllAction();


					//  /初始化生产
					ProduceInitFlag = TRUE;
					// 复制背景创建图像
					CopyBackBuffer();
					// 渲染方法
					BackBufferDrawType = DRAW_BACK_PRODUCE; 
					// 背景音乐
					fade_out_bgm();
					// 记录当前时间
					NowTime = GetTickCount();
					SubProcNo++;

					break;

				case 1:
					// 生成
					if( DrawProduce( PRODUCE_DOWN_ACCELE ) == TRUE )
					{
						BackBufferDrawType = DRAW_BACK_NORMAL; 
						SubProcNo++;
					}
					break;

				case 2:
					// 结束
					break;
			}
			// 断开警告Windows 窗口
			if( disconnectServer() )
			{
				// 初始化网络
				cleanupNetwork();
				// 恢复默认调色板
				PaletteChange( DEF_PAL, 0 );
				// 到标题画面
				ChangeProc( PROC_TITLE_MENU );
				// /消除行动
				DeathAllAction();
				disconnectServerFlag = FALSE;
				oldDisconnectServerFlag = FALSE;
				break;
			}
			RunAction();			// 傾僋僔儑儞憱傜偣傞
			StockTaskDispBuffer();	// 僞僗僋昞帵僨乕僞傪僶僢僼傽偵棴傔傞
			break;


		case PROC_BATTLE:     /* 僶僩儖僾儘僙僗 */
		
			BattleProc();
			break;
			
#ifdef _DEBUG		
		//case PROC_OHTA_TEST:     /* 懢揷僥僗僩僾儘僙僗 */
		
		//	OhtaTestProc();
		//	break;
			
		//case PROC_TAKE_TEST:     /* 抾撪偝傫僥僗僩僾儘僙僗 */
		
		//	TakeTestProc();
		//	break;

		//case PROC_DWAF_TEST:	// DWAF僥僗僩
		//	dwafTestProc();
		//	break;

		case PROC_SPR_VIEW:		// 初始验证
			SprViewProc();
			break;
			
		case PROC_ANIM_VIEW:	// 动画验证
			AnimViewProc();
			break;
			
		case PROC_SE_TEST:	//  SE的审查过程
			SeTestProc();
			break;
#endif
		case PROC_ENDING:
			
			break;
			
	}
}

/* 僾儘僙僗曄峏張棟 ***********************************************************/
void ChangeProc( int procNo )
{
	// 僾儘僙僗斣崋傪婰壇
	ProcNo = procNo;
	// 僒僽僾儘僙僗斣崋偺弶婜壔
	SubProcNo = 0;
}
/* 僾儘僙僗曄峏張棟 ***********************************************************/
void ChangeProc( int procNo, int subProcNo )
{
	// 僾儘僙僗斣崋傪婰壇
	ProcNo = procNo;
	// 僒僽僾儘僙僗斣崋偺弶婜壔
	SubProcNo = subProcNo;
}

/* 僎乕儉僾儘僙僗張棟 ********************************************************************/
void GameProc( void )
{
	static int now_bgm;
	static BOOL produceFlag;	// 墘弌僼儔僌

	/* 僒僽僾儘僙僗斣崋偱暘婒 */
	switch( SubProcNo ){
	
		case 0:
#ifdef _DEBUG
			// 僆僼儔僀儞儌乕僪偺帪偺儅僢僾弶婜壔
			if( offlineFlag )
			{
				initMap();		// 儅僢僾弶婜壔
			}
#endif

			//InitIme();		// 俬俵俤娭楢偺弶婜壔
			ClrImeCompositionString(); // 僂傿儞僪僂僘偺俬俵俤僶僢僼傽乕傪僋儕傾偡傞
			InitChat();		/* 僠儍僢僩娭楢弶婜壔 */
			// 墘弌斣崋儔儞僟儉乮嬼悢偺傒乯
			//ProduceNo = Rnd( 0, PRODUCE_END - 1 ) & 0xfffffffe;
			produceFlag = FALSE;	// 墘弌僼儔僌俷俥俥
			EncountFlag = FALSE;	// 僄儞僇僂儞僩僼儔僌俷俥俥
			//僼傿乕儖僪俛俧俵嵞惗奐巒
//			play_bgm(0);
			// 傾儖僶儉偺撉傒崬傒
			LoadAlbum();
			
			SubProcNo++;

		// 儘僌僀儞墘弌
		case 150:
			SubProcNo = 100;

		case 100:
			initPc();				// 僾儗僀儎乕僉儍儔嶌惉
			resetFieldProc();		// 僼傿乕儖僪張棟偺儕僙僢僩
			restorePtActCharObjAll();	// 僉儍儔娗棟僥乕僽儖偺傾僋僔儑儞暅妶
			initOpenServerWindow();	// 僒乕僶偺巜帵偱昞帵偡傞僂傿儞僪僂偺弶婜壔
			InitMenu();		// 儊僯儏乕娭楢偺弶婜壔
//			BattleStatusBak[ 0 ] = NULL;// 僶僩儖忬懺弶婜壔
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// 擖椡僼僅乕僇僗庢摼
			GetKeyInputFocus( &MyChatBuffer );
			SubProcNo++;

		case 101:
			if( loginFlag )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				break;
			}
			SubProcNo++;

		case 102:
			// 墘弌夋憸偺嶌惉
			// 僶僢僼傽弶婜壔
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// 僉儍儔丒僷乕僣偺桪愭弴埵寛掕張棟偺弶婜壔
			RunAction();			// 傾僋僔儑儞憱傜偣傞
			StockTaskDispBuffer();	// 僞僗僋昞帵僨乕僞傪僶僢僼傽偵棴傔傞
			stop_bgm();				//俛俧俵掆巭
			drawMap();				//	儅僢僾昞帵
			MenuProc();				// 儊僯儏乕張棟
			// 堦峴僀儞僼僅棑揾傝偮傇偟乮崟怓乯
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// 僠儍僢僩張棟
			ChatBufferToFontBuffer(); // 僠儍僢僩僶僢僼傽乕傪僼僅儞僩僶僢僼傽偵棴傔傞
			ImeProc();				// 俬俵俤娭楢張棟
			SortDispBuffer(); 	// 昞帵僶僢僼傽僜乕僩
			// 僶僢僋僒乕僼僃僗傪崟偱僋儕傾乕
			ClearBackSurface();	
			// 俛俵俹傪僶僢僋僒乕僼僃僗偵僙僢僩
			PutBmp();	
			// 僶僢僋僒乕僼僃僗偐傜僶僩儖僒乕僼僃僗傊僐僺乕
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// 僶僢僼傽弶婜壔
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

			play_bgm( map_bgm_no );

			// 尰嵼偺帪娫傪婰壇
			NowTime = GetTickCount();
			// 僶僢僋僶僢僼傽乕昤夋曽朄曄峏
//			BackBufferDrawType = DRAW_BACK_NORMAL; 
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			// 墘弌拞
			ProduceInitFlag = TRUE;
			DrawProduce( PRODUCE_BRAN_BIG );
			SubProcNo++;
			break;

		case 103:
			// 墘弌拞
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_BRAN_BIG ) == TRUE )
			{
				SubProcNo = 3;
				produceFlag = TRUE;
				fieldInfoTime = GetTickCount();
			}
			break;


		// 儚乕僾墘弌
		case 200:
			// 俛俧俵僼僃乕僪傾僂僩奐巒
//			fade_out_bgm();

			// S僾儘僩僐儖偺C偱 warpEffectProc();偑屇偽傟偰
			// 夋柺傪嶌傞偺偱偙偙偱偼偟側偔偰椙偄
			SubProcNo++;

		case 201:
			// 墘弌拞
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSIN ) == TRUE )
			{
				SubProcNo++;
			}
			drawField();	// 僼傿乕儖僪夋柺偵偄傠偄傠昞帵
			MenuProc();	// 儊僯儏乕張棟
			ImeProc();	// 俬俵俤娭楢張棟
			break;

		case 202:
			if( !warpEffectStart || !warpEffectOk )
			{
				BackBufferDrawType = DRAW_BACK_NORMAL; 
				drawField();	// 僼傿乕儖僪夋柺偵偄傠偄傠昞帵
				MenuProc();	// 儊僯儏乕張棟
				ImeProc();	// 俬俵俤娭楢張棟
				break;
			}
			warpEffectStart = FALSE;
			warpEffectOk = FALSE;
			SubProcNo++;

		case 203:
			// 墘弌夋憸偺嶌惉
			// 僶僢僼傽弶婜壔
			paletteProc();

			DispBuffer.DispCnt = 0;
			FontCnt = 0;
			initCharPartsPrio();	// 僉儍儔丒僷乕僣偺桪愭弴埵寛掕張棟偺弶婜壔
			RunAction();			// 傾僋僔儑儞憱傜偣傞
			StockTaskDispBuffer();	// 僞僗僋昞帵僨乕僞傪僶僢僼傽偵棴傔傞
			redrawMap();
			drawMap();				//	儅僢僾昞帵

#if 1
			// 僄僼僃僋僩偺僋儕傾
			if( (mapEffectRainLevel == 0 && oldMapEffectRainLevel != 0 )
			 || (mapEffectSnowLevel == 0 && oldMapEffectSnowLevel != 0 ) )
			{
				initMapEffect();
			}
#endif

			// 堦峴僀儞僼僅棑揾傝偮傇偟乮崟怓乯
			//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
			ChatProc();				// 僠儍僢僩張棟
			ChatBufferToFontBuffer(); // 僠儍僢僩僶僢僼傽乕傪僼僅儞僩僶僢僼傽偵棴傔傞
			SortDispBuffer(); 	// 昞帵僶僢僼傽僜乕僩
			// 僶僢僋僒乕僼僃僗傪崟偱僋儕傾乕
			ClearBackSurface();	
#if 1
			if( (mapEffectRainLevel != 0 && oldMapEffectRainLevel == 0 )
			 || (mapEffectSnowLevel != 0 && oldMapEffectSnowLevel == 0 ) )
			{
				mapEffectProc2( 80 );		// 儅僢僾僄僼僃僋僩乮塉丒愥摍乯
			}
#endif
			// 俛俵俹傪僶僢僋僒乕僼僃僗偵僙僢僩
			PutBmp();	
			// 僶僢僋僒乕僼僃僗偐傜僶僩儖僒乕僼僃僗傊僐僺乕
			lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
			// 僶僢僼傽弶婜壔
			DispBuffer.DispCnt = 0;
			FontCnt = 0;

//			if( map_bgm_no != now_bgm )
//			{
//				stop_bgm();				//俛俧俵掆巭
//				play_bgm( map_bgm_no );
//			}

			// 尰嵼偺帪娫傪婰壇
			NowTime = GetTickCount();

			ProduceInitFlag = TRUE;

			SubProcNo++;

		case 204:
			// 墘弌拞
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			if( DrawProduce( PRODUCE_CENTER_PRESSOUT ) == TRUE )
			{
				SubProcNo = 3;
				fieldInfoTime = GetTickCount();
			}
			drawField();	// 僼傿乕儖僪夋柺偵偄傠偄傠昞帵
			MenuProc();	// 儊僯儏乕張棟
			ImeProc();	// 俬俵俤娭楢張棟
			break;


		case 1:
			initPc();				// 僾儗僀儎乕僉儍儔嶌惉
			resetFieldProc();		// 僼傿乕儖僪張棟偺儕僙僢僩
			restorePtActCharObjAll();	// 僉儍儔娗棟僥乕僽儖偺傾僋僔儑儞暅妶
			initOpenServerWindow();	// 僒乕僶偺巜帵偱昞帵偡傞僂傿儞僪僂偺弶婜壔
			InitMenu();		// 儊僯儏乕娭楢偺弶婜壔
//			BattleStatusBak[ 0 ] = NULL;// 僶僩儖忬懺弶婜壔
//			BattleStatusReadPointer = BattleStatusWritePointer =0;
			// 擖椡僼僅乕僇僗庢摼
			GetKeyInputFocus( &MyChatBuffer );
			
			// 墘弌偡傞帪
			if( produceFlag == TRUE ){
			// 愴摤寢壥僂傿儞僪僂僼儔僌俷俶
			//if( BattleResultWndFlag == TRUE ){

				// 墘弌夋憸偺嶌惉
				// 僶僢僼傽弶婜壔
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				initCharPartsPrio();	// 僉儍儔丒僷乕僣偺桪愭弴埵寛掕張棟偺弶婜壔
				RunAction();			// 傾僋僔儑儞憱傜偣傞
				StockTaskDispBuffer();	// 僞僗僋昞帵僨乕僞傪僶僢僼傽偵棴傔傞
				stop_bgm();				//俛俧俵掆巭
				drawMap();				//	儅僢僾昞帵
				//俛俧俵偑側偗傟偽
				if(!draw_map_bgm_flg){
					//偝偭偒偺俛俧俵嵞惗
					play_bgm(map_bgm_no = now_bgm);
					draw_map_bgm_flg = 1;
				}
				MenuProc();				// 儊僯儏乕張棟
				// 堦峴僀儞僼僅棑揾傝偮傇偟乮崟怓乯
				//StockBoxDispBuffer( 0, 456, 640, 480, DISP_PRIO_MENU, 0, 1 );
				ChatProc();				// 僠儍僢僩張棟
				ChatBufferToFontBuffer(); // 僠儍僢僩僶僢僼傽乕傪僼僅儞僩僶僢僼傽偵棴傔傞
				ImeProc();				// 俬俵俤娭楢張棟
				SortDispBuffer(); 	// 昞帵僶僢僼傽僜乕僩
				// 僶僢僋僒乕僼僃僗傪崟偱僋儕傾乕
				ClearBackSurface();	
				// 俛俵俹傪僶僢僋僒乕僼僃僗偵僙僢僩
				PutBmp();	
				// 僶僢僋僒乕僼僃僗偐傜僶僩儖僒乕僼僃僗傊僐僺乕
				lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );
				// 僶僢僼傽弶婜壔
				DispBuffer.DispCnt = 0;
				FontCnt = 0;
				// 尰嵼偺帪娫傪婰壇
				NowTime = GetTickCount();
				// 僶僢僋僶僢僼傽乕昤夋曽朄曄峏
				BackBufferDrawType = DRAW_BACK_PRODUCE; 
				// 墘弌拞
				DrawProduce( PRODUCE_4WAY_IN );
			}else{
				// 墘弌僼儔僌俷俶
				produceFlag = TRUE;
				SubProcNo++;
			}
			SubProcNo++;
			
			break;
		
		case 2:	// 僼傿乕儖僪僀儞墘弌
			
			// 墘弌拞
			if( DrawProduce( PRODUCE_4WAY_IN ) == TRUE ){
				SubProcNo++;
				warpEffectStart = FALSE;
				warpEffectOk = FALSE;
			}
			break;
			
		case 3:
			// 僶僢僋僶僢僼傽乕昤夋曽朄曄峏
			BackBufferDrawType = DRAW_BACK_NORMAL; 
			
			paletteProc();
			
			initItemOverlapCheck();	// 傾僀僥儉廳側傝僠僃僢僋張棟偺弶婜壔
			drawGrid();				// 僌儕僢僪僇乕僜儖昞帵
			fieldProc();			// 僼傿乕儖僪娭楢張棟
			moveProc();				// 堏摦張棟
			charObjProc();			// 僉儍儔娗棟張棟
			initCharPartsPrio();	// 僉儍儔丒僷乕僣偺桪愭弴埵寛掕張棟偺弶婜壔
			addressBookProc();		// 傾僪儗僗僽僢僋張棟
			openServerWindowProc();	// 僒乕僶嫮惂僂傿儞僪僂
			drawFieldInfoWin();		// 儚乕僾屻偺応強忣曬
			
			/* 傾僋僔儑儞憱傜偣傞 */
			RunAction();
			// 僞僗僋昞帵僨乕僞傪僶僢僼傽偵棴傔傞
			// 巜掕斖埻偼僗僩僢僋偟側偄
			StockTaskDispBuffer();

			mapEffectProc();		// 儅僢僾僄僼僃僋僩乮塉丒愥摍乯

			drawMap();		// 儅僢僾昞帵
			drawField();	// 僼傿乕儖僪夋柺偵偄傠偄傠昞帵

			/* 僠儍僢僩張棟 */
			ChatProc();
			// 僠儍僢僩僶僢僼傽乕傪僼僅儞僩僶僢僼傽偵棴傔傞
			ChatBufferToFontBuffer(); 
			// 僉乕儃乕僪僇乕僜儖揰柵張棟
			FlashKeyboardCursor();
			// 儊僯儏乕張棟
			MenuProc();
			// 俬俵俤娭楢張棟
			ImeProc();		
			// 帪娫懷張棟偲僷儗僢僩僠僃儞僕張棟
			TimeZoneProc();
			
#ifdef _DEBUG
			// 僄儞僇僂儞僩儃僞儞
			if( joy_trg[ 0 ] & JOY_P_DOWN ){
				if( pc.ptAct != NULL ){
					resetMap();					// 儅僢僾張棟儕僙僢僩
					// 僄儞僇僂儞僩憲怣
					lssproto_EN_send( sockfd, pc.ptAct->gx, pc.ptAct->gy );
					eventEnemyFlag = 0;
				}
			}
#endif
			// 僄儞僇僂儞僩偟偨帪
			if( EncountFlag == TRUE ){
				resetPc();				// PC儕僙僢僩
				resetCharObj();			// 僉儍儔娗棟僥乕僽儖儕僙僢僩
				resetMap();				// 儅僢僾張棟儕僙僢僩
				clearPtActPartyParam();	// 拠娫忣曬偺傾僋僔儑儞億僀儞僞偩偗傪NULL偵偡傞
				fieldInfoTime = 0;		// 応強忣曬偑弌偰偨傜徚偡
				drawFieldInfoWin();		// 儚乕僾屻偺応強忣曬
				nowEncountPercentage = minEncountPercentage;// 僄儞僇僂儞僩棪傪嵟彫偵栠偡
				sendEnFlag = 0;
				encountNowFlag = 1;
				eventEnemySendFlag = 0;
				duelSendFlag = 0;
				jbSendFlag = 0;
				// 儅僢僾僂傿儞僪僂僼儔僌僶僢僋傾僢僾
				if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
				// 徚偊傞傑偱偺帪娫僞僀儅乕
				ResultWndTimer = RESULT_WND_CLOSE_TIME;
				InitMenu();			// 儊僯儏乕娭楢偺弶婜壔
				BattleCmd[ 0 ] = NULL;		// 僶僩儖僐儅儞僪弶婜壔
//				BattleCmdBak[ 0 ] = NULL;	// 僶僩儖僐儅儞僪弶婜壔
//				BattleCmdReadPointer = BattleCmdWritePointer =0;
				BattleStatus[ 0 ] = NULL;	// 僶僩儖忬懺弶婜壔
				//BattleStatusBak[ 0 ] = NULL;// 僶僩儖忬懺弶婜壔
//				BattleStatusReadPointer = BattleStatusWritePointer =0;
				BattleTurnReceiveFlag = TRUE;	// 僞乕儞庴怣僼儔僌俷俶
				// 僄儞僇僂儞僩壒
				play_se( 215, 320, 240 );
				// 尰嵼偺俛俧俵曐懚
				now_bgm = t_music_bgm_no;
				// 俛俧俵掆巭
				stop_bgm();
				SubProcNo++;
			}
			break;
			
		case 4:	// 僶僢僋僶僢僼傽乕傪墘弌梡僒乕僼僃僗偵僐僺乕
			// 僶僩儖僒乕僼僃僗偺夋憸嶌惉 
			CopyBackBuffer();
			// 僶僢僋僶僢僼傽乕昤夋曽朄曄峏
			BackBufferDrawType = DRAW_BACK_PRODUCE; 
			SubProcNo++;
			
			break;
			
		case 5:	// 僄儞僇僂儞僩墘弌
		
			// 墘弌拞
			if( DrawProduce( PRODUCE_HAGARE_OCHI_OUT ) == TRUE ){
			//if( GameState == GAME_ENCOUNT_TO_BATTLE ){ 
				// 僾儘僙僗僠僃儞僕
				ChangeProc( PROC_BATTLE );
			}
			MenuProc();	// 儊僯儏乕張棟
			ImeProc();	// 俬俵俤娭楢張棟
			
			break;
	}
}


// 儚乕僾墘弌梡
void warpEffectProc( void )
{
	oldMapEffectRainLevel = mapEffectRainLevel;
	oldMapEffectSnowLevel = mapEffectSnowLevel;

	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// 儅僢僾僂傿儞僪僂僼儔僌僶僢僋傾僢僾
	if( MenuToggleFlag & JOY_CTRL_M ) MapWmdFlagBak = TRUE;
	InitMenu2();

	fieldProc();			// 僼傿乕儖僪娭楢張棟
	charObjProc();			// 僉儍儔娗棟張棟
	initCharPartsPrio();	// 僉儍儔丒僷乕僣偺桪愭弴埵寛掕張棟偺弶婜壔
	openServerWindowProc();	// 僒乕僶嫮惂僂傿儞僪僂

	fieldInfoTime = 0;		// 応強忣曬偑弌偰偨傜徚偡
	drawFieldInfoWin();		// 儚乕僾屻偺応強忣曬


	/* 傾僋僔儑儞憱傜偣傞 */
	RunAction();
	// 僞僗僋昞帵僨乕僞傪僶僢僼傽偵棴傔傞
	StockTaskDispBuffer();

	mapEffectProc();		// 儅僢僾僄僼僃僋僩乮塉丒愥摍乯

	redrawMap();
	drawMap();		// 儅僢僾昞帵

	/* 僠儍僢僩張棟 */
	ChatProc();
	// 僠儍僢僩僶僢僼傽乕傪僼僅儞僩僶僢僼傽偵棴傔傞
	ChatBufferToFontBuffer(); 
	// 帪娫懷張棟偲僷儗僢僩僠僃儞僕張棟
//	TimeZoneProc();

	SortDispBuffer(); 	// 昞帵僶僢僼傽僜乕僩

	// 僶僢僋僒乕僼僃僗傪崟偱僋儕傾乕
	ClearBackSurface();	
	// 俛俵俹傪僶僢僋僒乕僼僃僗偵僙僢僩
	PutBmp();	


	// 僶僢僋僒乕僼僃僗偐傜僶僩儖僒乕僼僃僗傊僐僺乕
	lpBattleSurface->BltFast( 0, 0, lpDraw->lpBACKBUFFER, NULL, DDBLTFAST_WAIT );

	// 僶僢僼傽弶婜壔
	DispBuffer.DispCnt = 0;
	FontCnt = 0;

	// 尰嵼偺帪娫傪婰壇
	NowTime = GetTickCount();

	ProduceInitFlag = TRUE;
}


void paletteProc( void )
{
	if( palNo == -1 )
	{
		// 僷儗僢僩僠僃儞僕
		PaletteChange( SaTimeZoneNo, 0 );
		// 帪娫懷偱僷儗僢僩僠僃儞僕偡傞帪
		TimeZonePalChangeFlag = TRUE;
		palNo = -2;
	}
	else
	if( palNo >= 0 )
	{
		// 屌掕僷儗僢僩愝掕
		PaletteChange( palNo, 0 );// 僷儗僢僩僠僃儞僕
		// 帪娫懷偱僷儗僢僩僠僃儞僕偟側偄帪
		TimeZonePalChangeFlag = FALSE;
		palNo = -2;
	}
}
