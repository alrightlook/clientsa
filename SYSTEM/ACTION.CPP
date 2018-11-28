/************************/
/*	action.c			*/
/************************/
#include "system.h"



/* はじめと後のリストポインタ */
ACTION *pActTop;
ACTION *pActBtm;

#ifdef _DEBUG		
/* アクション数カウント */
int ActCnt;
#endif

/* アクションリスト初期化 *****动作名单初期化************************************************/
void InitAction( void )
{
	/* 最初と最後のポインタ取得 */
	pActTop = ( ACTION * )calloc( 1, sizeof( ACTION ) );
	pActBtm = ( ACTION * )calloc( 1, sizeof( ACTION ) );

	pActTop->pPrev = NULL;	 	/* 前のリストは無し */
	pActTop->pNext = pActBtm;	/* 次のリストは最後尾 */
	pActTop->func  = NULL;		/* 実行関数は無し */
	pActTop->prio  = PRIO_TOP;	/* 優先順位最大 */
	pActTop->bmpNo = -1;		/* ＢＭＰ番号初期化 */
	
	pActBtm->pPrev = pActTop;	/* 前のリストは先頭 */
	pActBtm->pNext = NULL;		/* 次のリストは無し */
	pActBtm->func  = NULL;		/* 実行関数は無し */
	pActBtm->prio  = PRIO_BTM;	/* 優先順位最低 */
	pActBtm->bmpNo = -1;		/* ＢＭＰ番号初期化 */
	
	//MessageBox( hWnd, "アクション初期化完了。", "確認", MB_OK );
}

/* アクションリストに登録 *****************************************************/
ACTION *GetAction( UCHAR prio, UINT yobiSize )
{
	ACTION *pAct;
	ACTION *pActLoop;
	
	/* アクション構造体メモリ確保 */
	pAct = ( ACTION * )calloc( 1, sizeof( ACTION ) );
	if( pAct == NULL ){
//#ifdef _DEBUG		
		//MessageBox( hWnd, "GetAction Erorr!!", "確認", MB_OK );
		MessageBox( hWnd, "ハードディスクの空き容量が足りません！", "GetAction Erorr", MB_OK );
//#endif
		return NULL;
	}
	/* 予備構造体使う時 */
	if( yobiSize > 0 ){
		/* 予備構造体メモリ確保 */
		pAct->pYobi = calloc( 1, yobiSize );
		// 失敗したら
		if( pAct->pYobi == NULL ){
			/* アクション構造体のメモリも解放 */
			free( pAct );
//#ifdef _DEBUG		
			//MessageBox( hWnd, "GetYobi Erorr!!", "確認", MB_OK );
			MessageBox( hWnd, "ハードディスクの空き容量が足りません！", "GetYobi Erorr", MB_OK );
//#endif
			return NULL;
		}
	}
	
	/* 初期化 */
	pAct->func  = NULL;			/* 実行関数は無し */
	pAct->prio  = prio;			/* 優先順位 */
	pAct->bmpNo = -1;			/* ＢＭＰ番号初期化 */
	pAct->hitDispNo = -2;		/* マウスカーソル当たり判定番号初期化 */
	
	/* 優先順に並べる */
	for( pActLoop = pActTop->pNext ; pActLoop != pActBtm->pNext ; 
		pActLoop = pActLoop->pNext ){
		
		/* 優先度が低い時 */
		if( pActLoop->prio > prio ){
			/* この上に追加 */
			/* 前後から教えてもらう */
			pAct->pPrev = pActLoop->pPrev;	/* 前のリスト */
			pAct->pNext = pActLoop;			/* 次のリスト */
			/* 前後に教える */
			pActLoop->pPrev->pNext = pAct;	/* 前のリスト */
			pActLoop->pPrev = pAct;			/* 次のリスト */
#ifdef _DEBUG		
			/* アクション数カウント */
   			ActCnt++;
#endif			
			break;
		}
	}
	
	return pAct;
}

/* アクション走らせる ******动作运行***************************************************/
void RunAction( void )
{
	ACTION *pActLoop = pActTop->pNext; 	/* 先頭のリストポインタ取得  最前头的名单指针取得*/
	ACTION *pActLoopBak;  /* 死亡時バックアップ用 */
	
	// 処理ループ	
	while(1){
	
		/* 最後尾が来るまでループ  最后来之前循环*/	
		if( pActLoop == pActBtm ) break;

		/* 生きていたら 如果活着的话*/
		if( pActLoop->deathFlag == FALSE )
		{
		
			/* アドレスがあったら実行   地址的话，执行*/
			if( pActLoop->func != NULL )
			{
				pActLoop->func( pActLoop );
			}
			
			/* 次のポインタをセット */
			pActLoop = pActLoop->pNext;
			
		}
		else
		{	/* 死んでいたら */
		
			/* 前後に教える */
			pActLoop->pPrev->pNext = pActLoop->pNext;
			pActLoop->pNext->pPrev = pActLoop->pPrev;
			/* バックアップ */
			pActLoopBak = pActLoop->pNext;
			/* リストから抹殺 */
			ClearAction( pActLoop );
			/* バックアップ戻す */
			pActLoop = pActLoopBak;
		}
	}
}

/* アクションリストから抹殺準備 ***********************************************/
void DeathAction( ACTION *pAct )
{
	if( pAct == NULL ) return;
	pAct->deathFlag = TRUE;
}

/* アクションリストから完全抹殺 ***********************************************/
void ClearAction( ACTION *pAct )
{
	// 予備構造体があるとき
	if( pAct->pYobi != NULL ){
		// 予備構造体を開放
		free( pAct->pYobi );
	}
	// アクション構造体を開放
	free( pAct );
	
#ifdef _DEBUG		
	/* アクション数マイナス */
	ActCnt--;
#endif		

}

/* 全アクション抹殺 ***********************************************************/
void DeathAllAction( void )
{
	ACTION *pActLoop = pActTop->pNext;
	
	/* 最後まできたら終了 */
	while( pActLoop != pActBtm ){
		/* 死亡フラグ立てる */
		pActLoop->deathFlag = TRUE;
		/* 次のポインタへ */
		pActLoop = pActLoop->pNext;
	}
	//MessageBox( hWnd, "DeathAllAction OK!!", "確認", MB_OK );
}

/* アクション終了処理 *********************************************************/
void EndAction( void )
{
	// 全アクション抹殺
	DeathAllAction();
	//アクション走らせる（完全抹殺）
	RunAction();
	
	/* 最初と最後のポインタ開放 */
	free( pActTop );
	free( pActBtm );
	
	//MessageBox( NULL, "アクション開放完了。", "確認", MB_OK );
}
