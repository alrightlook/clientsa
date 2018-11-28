/************************/
/*	ime.cpp				*/
/************************/
#include "system.h"
#include "winnls32.h"
#include "font.h"
#include "ime_sa.h"
#include "menu.h"

// ＩＭＥのハンドル
HIMC hImc;
HIMC hImcBegin;

// ＩＭＥ入力モード取得用
DWORD conversion;
DWORD sentence;

// ＩＭＥ入力モード文字列  IME字符串输入模式
/*
static char *ImeModeStr[] = { 
								"    ??/abc","","",
								"    ??/???"","","","","", // Nuke.1
								" ??/ａｂｃ",
								" ??/あいう","",
								" ??/アイウ","","","","",
								"   ???/abc","","",
								"   ???/???"","","","","", // Nuke.2
								"???/ａｂｃ",
								"???/あいう","",
								"???/アイウ",
								};
// ＩＭＥ入力モード文字列（Ｃａｐｓ用） IME输入方式文字列（Caps用）
static char *ImeModeStrCaps[] = { 
								"    ??/ABC","","",
								"    ??/???"","","","","", // Nuke.3
								" ??/ＡＢＣ",
								" ??/あいう","",
								" ??/アイウ","","","","",
								"   ???/ABC","","",
								"   ???/???"","","","","", // Nuke.4
								"???/ＡＢＣ",
								"???/あいう","",
								"???/アイウ",
								};*/

//IME字符串输入模式小写
static char *ImeModeStr[] = { 
	"    杜/abc","","",
								"    杜/辈?","","","","", // Nuke.1
								" 杜/倎倐們",
								" 杜/偁偄偆","",
								" 杜/傾僀僂","","","","",
								"   郯?abc","","",
								"   郯?辈?","","","","", // Nuke.2
								"郯?倎倐們",
								"郯?偁偄偆","",
								"郯?傾僀僂",
};
// IME输入方式文字列（Caps用）大写
static char *ImeModeStrCaps[] = { 
	"    杜/ABC","","",
								"    杜/辈?","","","","", // Nuke.3
								" 杜/俙俛俠",
								" 杜/偁偄偆","",
								" 杜/傾僀僂","","","","",
								"   郯?ABC","","",
								"   郯?辈?","","","","", // Nuke.4
								"郯?俙俛俠",
								"郯?偁偄偆","",
								"郯?傾僀僂",
								};
#if 0
// 半角カナから全角カナ変換用文字列
char *ZenkakuKana = "?。「」、?ヲァィゥェォャュョッ"
					"ーアイウエオカキクケコサシスセソ"
					"タチツテトナニヌネノハヒフヘホマ"
					"ミムメモヤユヨラリルレロワン゛゜";
#endif

// ＩＭＥバッファー IME缓冲器 我修改 _asm
char ImeBuffer[ 256 ];
//char *ImeBuffer;
// ＩＭＥ文節バッファー  短语缓冲器
char ImeBlock[ 256 ];
// ＩＭＥバッファーバックアップ  缓冲备份 我修改 _asm
char ImeBufferBak2; 
//char *ImeBufferBak2;

// ＩＭＥ関連の初期化 *********************************************************/
void InitIme( void )
{
	// バッファー初期化  我修改 _asm

	ImeBuffer[ 0 ] = NULL;
	ImeBlock[ 0 ] = NULL;
	ImeBufferBak2 = NULL;
	
	// 以前にＩＭＥがあったら消す。   以前IME的话消失。
	if( hImc != NULL ){
		// ＩＭＥコンテキスト開放
		EndIme();
	}
	//hImc = ImmGetContext( hWnd );
	
	// ＩＭＥコンテキストの取得  上下文的取得
	hImc = ImmCreateContext();
	
#ifdef _DEBUG
	// 失敗した時  
	if( ( hImc = ImmCreateContext() ) == NULL )
	{
		MessageBox( hWnd, "IME初始化失败，太田为止，请联系", "確認", MB_OK | MB_ICONSTOP );
	}
#endif
	
	hImcBegin = ImmAssociateContext( hWnd , hImc );
	/*
	创建一个输入上下文。要把输入上下文分配给一个窗口，需要调用ImmAssociateContext函数。
	这个函数将返回一个句柄，这个句柄是先前与窗口关联的输入上下文的句柄。
	如果先前没有输入上下文与些窗口相关联，函数将返回的句柄将是默认输入上下文（的句柄）。
	通常，你需要保存这个句柄（默认句柄），当你不想再使用自己的输入上下文时，再把它与窗口
	重新关联上。
   */
	// ＩＭＥ  窗口消失
    //WINNLSEnableIME( hWnd, FALSE );
   	//WINNLSEnableIME( NULL, TRUE );

}
	
// ＩＭＥの終了 ***************************************************************/
void EndIme( void )
{
	// ＩＭＥコンテキスト開放
	ImmReleaseContext( hWnd, hImc );
	ImmAssociateContext( hWnd , hImcBegin );
	// ＩＭＥウィンドウ消す
    //WINNLSEnableIME( NULL, FALSE );
}

//*****************************************************************************/
// ＩＭＥのＯＮ、ＯＦＦ状態の確認 
// 戻り値　	ＯＮ　：TRUE
// 			ＯＦＦ：FALSE
//*****************************************************************************/
BOOL ImeOpenState( void )
{
    return ImmGetOpenStatus( hImc );
}
//*****************************************************************************/
// ＷＩＮＤＯＷＳのＩＭＥバッファーをクリアする   缓冲通关
//*****************************************************************************/
void ClrImeCompositionString( void )
{
	// ＩＭＥがＯＮの時
    if( ImeOpenState() == TRUE ){
		// ＩＭＥＯＦＦにする
    	ImmSetOpenStatus( hImc, FALSE );
		// ＩＭＥＯＮにする
    	ImmSetOpenStatus( hImc, TRUE );
		// ＯＦＦにしてＯＮにするだけ
	}
}
#if 0
//*****************************************************************************/
// 半角カナがある時、全角カナに変換する  半角片假名的时候，全角假名转换
//*****************************************************************************/
void HanKanaToZenKana( char *str )
{
	char work[ 512 ];
	int i = 0, j = 0;
	
	// 文字数分ループ
	while( str[ i ] != NULL ){
		// 全角のとき
		// 文字が半角か全角かを調べる
		if( GetCharByte( str[ i ] ) == 2 ){
			// 文字バッファに溜める
			work[ j++ ] = str[ i++ ];
			work[ j++ ] = str[ i++ ];
		}else{
			// 半角カナの時、全角かなに変換する
			if( ( char )0xa0 <= str[ i ] && str[ i ] <= ( char )0xdf )
			{ 
				// 文字バッファに溜める
				
				work[ j++ ] = ZenkakuKana[ ( str[ i ] - ( char )0xa0 ) * 2 ];
				work[ j++ ] = ZenkakuKana[ ( str[ i++ ] - ( char )0xa0 ) * 2 + 1 ];
				
				
			}
			else
			{
				// 文字バッファに溜める
				work[ j++ ] = str[ i++ ];
			}
		}
	}
	// 終端記号を入れる
	if( j >= 256 ) work[ 255 ] = NULL;
	else work[ j ] = NULL;
	
	// 変換した文字列をコピーする  转换后的文字复制
	strcpy( str, work );
}
#endif

// ＩＭＥ関連処理 ********相关处理*****************************************************/
void ImeProc( void )
{

	//int strWidth;	// 文字列の横幅（ドット数）
	int strWidth2;	// 文字列の横幅（ドット数） 文字列的宽（点数）
	int blockXcnt = 0; 		// 文節ボックスの座標のカウンター  短语框的坐标的柜台
	int blockX1 = 0,blockX2 = 0; 	// 文節ボックスの座標  短语框的坐标
	int blockFlag = FALSE; 	// 文節フラグ  短语转折点
	char *ImeBufferBak = ImeBuffer;	// 作業用
	char *ImeBlockBak = ImeBlock;	// 作業用
	BOOL imeOpenFlag;				// ＩＭＥのＯＮ、ＯＦＦフラグ
	BOOL capsLockFlag;				// capsLock のＯＮ、ＯＦＦフラグ
	char keyBoardState[ 256 ];		// キーボードの状態  键盘的状态
	char imePrio;					// ＩＭＥボックスの表示優先度  箱的显示优先度
	
	
	// バックアップ  备份 我修改 _asm

	ImeBufferBak2 = ImeBuffer[ 0 ];
	//ImeBufferBak2 = ImeBuffer;
	// ＩＭＥの状態を記憶（　ＯＮ、ＯＦＦ　）  记忆的状态
	imeOpenFlag = ImeOpenState();
	
	// capsLock の判定
	// キーボードの状態をとってくる（全てのキー）
	//判定/ /键盘的状态抓起（所有密钥
	GetKeyboardState( ( BYTE *) keyBoardState );
	// capsLock 状態（　ＯＮ、ＯＦＦ　）  得到  capsLock 状態
	capsLockFlag = keyBoardState[ VK_CAPITAL ];
	
	// ＩＭＥ入力モード文字列の表示
	// タスクバーが非表示の時
	/// / IME输入文字列显示模式/ /任务栏不显示的时候
	if( TaskBarFlag == FALSE )
	{
		// ＩＭＥがＯＮの時   ON的时候
		if( imeOpenFlag )
		{
			// CapsLock の時   负责大小写的处理
			if( capsLockFlag )
			{
				if( conversion >= 28 )
				{
					// 文字表示バッファに溜める 文字表示缓冲蓄
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       ABC", 0 );
				}
				else
				{
					// 文字表示バッファに溜める
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, ImeModeStrCaps[ conversion ], 0 );
				}
			}
			else
			{
				if( conversion >= 28 )
				{
					// 文字表示バッファに溜める
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       abc", 0 );
				}
				else
				{
					// 文字表示バッファに溜める
					StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, ImeModeStr[ conversion ], 0 );
				}
			}
		}
		// ＩＭＥがＯＦＦの時
		else
		{	
			// CapsLock の時
			if( capsLockFlag )
			{
				// 文字表示バッファに溜める
				StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       ABC", 0 );
			}
			else
			{
				// 文字表示バッファに溜める
				StockFontBuffer( 545, 460, FONT_PRIO_FRONT, 0, "       abc", 0 );
			}
		}
	}
	
	// ＩＭＥ文字列の表示
	// ＩＭＥがＯＮの時かつＩＭＥバッファに文字があるとき
	/// / IME字符串的表示/ / IME ON时且IME缓冲文字的时候
	if( imeOpenFlag && *ImeBuffer != NULL )
	{
		
		// 入力場所がない時戻る   输入，没有地方点回来
		if( pNowStrBuffer == NULL )
			return;
		
		// 入力文字の優先度からＩＭＥボックスの優先度を決める  
		//输入文字的优先度IME框的优先度决定
		if( pNowStrBuffer->fontPrio == FONT_PRIO_BACK )
		{
			imePrio = DISP_PRIO_IME1;
		}
		else 
			imePrio = DISP_PRIO_IME3;
		
		// 文字列の横幅を求める
		//strWidth = GetStrWidth( pNowStrBuffer->buffer );
		// 文字表示バッファに溜める
		StockFontBuffer( pNowStrBuffer->imeX, pNowStrBuffer->imeY, pNowStrBuffer->fontPrio, 0, ImeBuffer, 0 );
		
		// 文字の背景を描画
		// ＩＭＥバッファーの文字列の横幅を求める
		strWidth2 = GetStrWidth( ImeBuffer );
		//３２０＊２４０ドットモードの時
		if(ResoMode == 1)
		{
			// ボックス表示データをバッファに溜める
			StockBoxDispBuffer( pNowStrBuffer->imeX - 1, 
				pNowStrBuffer->imeY   , 
				pNowStrBuffer->imeX + strWidth2, 
				pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
				imePrio, 0, 1 );
		}
		else 
		{
			// ボックス表示データをバッファに溜める
			StockBoxDispBuffer( pNowStrBuffer->imeX - 1, 
				pNowStrBuffer->imeY -1, 
				pNowStrBuffer->imeX + strWidth2, 
				pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
				imePrio, 0, 1 );
		}
		// 文節の座標の取得   文节坐标的取得
		while( 1 )
		{
			// 文節の始まりだったら  文节开始的话
			if( ( *ImeBlockBak == 1 || *ImeBlockBak == 3 ) && blockFlag == FALSE )
			{
				// 文節の始まりの座標
				blockX1 = blockXcnt;
				// 文節フラグＯＮ
				blockFlag = TRUE;
				//blockFlag = *ImeBlockBak;
			}
			
			// 文節終わりだったら 文节结束的话
			if( ( *ImeBlockBak == 2 || *ImeBlockBak == NULL ) && blockFlag >= TRUE )
			{
				// 文節の終わりの座標
				blockX2 = blockXcnt;
				break;
			}
			
			// 終了条件
			if( *ImeBlockBak == NULL ) break;
			
			// アドレスと座標の更新
			// 文字が全角の時 / /地址和坐标的更新全角/ /文字的时候
			
			if( GetCharByte( *ImeBufferBak ) == 2 )
			{
				
				ImeBufferBak += 2;	// ２バイト進める
				ImeBlockBak += 2;	// ２バイト進める
				blockXcnt += FontZenkauWidth; // 全角ドット分足す全角圆点分加
				
			}

			else
			{ // 半角のとき
				
				ImeBufferBak += 1;	// １バイト進める
				ImeBlockBak += 1;	// １バイト進める
				blockXcnt += FontHankakuWidth; // 半角ドット分足す
			}
		}
		
		//文節があるとき
		if( blockFlag >= TRUE ){
			//３２０＊２４０ドットモードの時
			if(ResoMode == 1){
				// ボックス表示データをバッファに溜める
				StockBoxDispBuffer( pNowStrBuffer->imeX + blockX1 - 1, 
									pNowStrBuffer->imeY    , 
									pNowStrBuffer->imeX + blockX2, 
									pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
									imePrio + 1, 252, 1 );
			} else {
				// ボックス表示データをバッファに溜める
				StockBoxDispBuffer( pNowStrBuffer->imeX + blockX1 - 1, 
									pNowStrBuffer->imeY - 1, 
									pNowStrBuffer->imeX + blockX2, 
									pNowStrBuffer->imeY - 1 + FONT_ZENKAU_WIDTH, 
									imePrio + 1, 252, 1 );
			}
		}
	}
}
	
	
// WM_IME_COMPOSITION メッセージの処理 **信息处理**************************************/
void ImeComposition( LPARAM lParam )
{
	
	int strLen;
	char str[ 256 ];
	int i;
	BOOL zenkakuFlag = FALSE; // 全角フラグ
	
	// 文字確定したとき 文字确定的时候
	if ( lParam & GCS_RESULTSTR ){
		// 文字の長さの取得  文字的长度的取得
		strLen = ImmGetCompositionString( hImc, GCS_RESULTSTR, NULL, 0 );
		// リミットチェック 极限检查
		if( strLen >= 256 ) strLen = 255;
		// 文字列を取ってくる  文字列拿来
		ImmGetCompositionString( hImc, GCS_RESULTSTR, str, strLen );
		// 終端記号
		str[ strLen ] = NULL;
		// 半角カナがある時、全角カナに変換する
		//HanKanaToZenKana( str );
		// 変換後の文字の長さの取得  转换后的文字的长度的取得
		strLen = strlen( str );
		// リミットチェック
		if( strLen >= 256 ) strLen = 255;
		// 文字の数だけループ
		//pNowStrBuffer->buffer[256] =str[256];//我加  加上后服务端的返回NPC消息消失
		//我去 中文输入问题在这里
		
		for( i = 0 ; i < strLen ; i++ )
		{
			// 文字バッファに溜める  文字缓冲蓄 我去
			StockStrBufferChar( str[ i ] );
		}
	}
	
	// 表示されている文字（確定前） 显示的文字（确定前）
	if( lParam & GCS_COMPSTR )
	{
		
		// 文字の長さの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPSTR, NULL, 0 );
		// リミットチェック
		if( strLen >= 256 ) strLen = 255;
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPSTR, ImeBuffer, strLen );
		// 終端記号 我去
		ImeBuffer[ strLen ] = NULL;
			//pNowStrBuffer->buffer[256] =str[256];//我加
		// 半角カナがある時、全角カナに変換する
		//HanKanaToZenKana( ImeBuffer );
		
	}
//	else{	// 最後の一文字消したとき
		// 文字の長さの取得
//		strLen = ImmGetCompositionString( hImc, GCS_COMPSTR, NULL, 0 );
//		if( strLen == 0 ){
//			// 文字列を空にする
//			*ImeBuffer = NULL;
//		}
//	}
	
	// 文節を取ってくる  短语拿来
	if( lParam & GCS_COMPATTR )
	{
		// 文字の長さの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPATTR, NULL, 0 );
		// リミットチェック
		if( strLen >= 256 ) strLen = 255;
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPATTR, ImeBlock, strLen );
		// 終端記号
		ImeBlock[ strLen ] = NULL;
	}
#if 0	
	// 振り仮名を取得
	if( lParam & GCS_COMPREADSTR ){
		// 文字の長さの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPREADSTR, NULL, 0 );
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPREADSTR, str, strLen );
	}
	
	if( lParam & GCS_COMPCLAUSE ){
		// 終端記号
		str[ 0 ] = NULL;
		// 文字の長さの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPCLAUSE, NULL, 0 );
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPCLAUSE, str, strLen );
	}
	
	if( lParam & GCS_COMPREADCLAUSE ){
		// 文字の長さの取得
		strLen = ImmGetCompositionString( hImc, GCS_COMPREADCLAUSE, NULL, 0 );
		// 文字列を取ってくる
		ImmGetCompositionString( hImc, GCS_COMPREADCLAUSE, str, strLen );
	}
#endif
}
	
/* WM_IME_NOTIFY メッセージの処理 *****处理IME消息函数****************************************/
void ImeNotify( WPARAM wParam ,LPARAM lParam )
{
	switch ( wParam )
	{
	
	case IMN_OPENCANDIDATE:
		//这条留言，变换候补进入的小窗口打开的时机来
		// このメッセージは、変換候補が入っているちっちゃいウインドウが開くタイミングで来る。
	case IMN_CHANGECANDIDATE:// このメッセージは、変換候補を変えるたびに来る。
		//这条留言，每次来改变变换候补。
		break;
		
	case IMN_CLOSECANDIDATE:
		// このメッセージは、変換候補が入っているちっちゃいウインドウが消えるタイミングでくる。
		//这条留言，变换候补进入的小窗口消失的时机。
		break;
		
#if 0	
	case IMN_SETOPENSTATUS:// IMEを有効にしたり無効にしたりしたとき来る。
		break;
#endif
    }
}
#if 0
/** ＩＭＥ関連メッセージ処理 **************************************************/
void ImeMsgProc( UINT Message, WPARAM wParam, LPARAM lParam )
{
	// メッセージで分岐
	switch( Message ){
	
		//IME的小窗口变化的时候
		case WM_IME_COMPOSITION: // ＩＭＥのちっちゃいウィンドウに変化がある時
	
			// WM_IME_COMPOSITION メッセージの処理 信息处理
			ImeComposition( lParam );
			break;
		case WM_IME_ENDCOMPOSITION: // 文字を確定した直後に来る。変換中の文字を全部BSでけしたときもくる。
			// ＩＭＥバッファーを空にする
			*ImeBuffer = NULL;
			break;
		
		case WM_IME_NOTIFY:
			/* WM_IME_NOTIFY メッセージの処理 */
			//ImeNotify( wParam ,lParam );
			// ここにDEFWINDOWPROCがあると、変換候補ウインドウを出すようだ。
			break;
		case WM_IME_SETCONTEXT:
			//初期化したとき来てるのかな？フォーカスが移ったときも来るようだ。
			break;
#if 0
		case WM_IME_STARTCOMPOSITION:// IMEがONの状態で、文字を入力した直後に来る。
			break;
		case WM_IME_CONTROL:
			break;
		case WM_IME_SELECT: // IMEをえらんだとき来るらしい。バッファを初期化とかすればいいのかな？
			break;
		case WM_IME_KEYDOWN:   // IMEのキーコード
			// ここにDefWindowProcがあると、WM_KEYDOWNを生成する
			break;
		case WM_IME_CHAR:
			// ここにDefWindowProcがあると、WM_CHARを生成する
			break;
#endif
	}
}
#endif

