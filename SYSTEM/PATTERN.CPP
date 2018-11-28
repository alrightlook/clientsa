/************************/
/*	pattern.cpp		图案	*/
/************************/

/*	ENT	:   ACTION *a0 , 
		:	アニメーション速度（０はｓｐｒ．ｂｉｎの速度），
		:	ループフラグ（ＡＮＭ＿ＬＯＯＰかＡＮＭ＿ＮＯ＿ＬＯＯＰ）

	RET	:   ０：ループしなかった
		:   １：ループした
		ENT /:行动* a0。:动画速度（0spr. bin的速度），
		:循环转折点（ANM＿＿＿LOOP LOOP吗ANM NO）退赛:0 :循环了:1：循环的
*/
#include 	"system.h"
#include	"loadrealbin.h"
#include	"loadsprbin.h"
#include	"anim_tbl.h"
#include	"t_music.h"

void StockDispBuffer2( int x, int y, UCHAR dispPrio, int bmpNo );

/*----------　アニメーション処理　-动画处理---------*/
int pattern( ACTION *a0, int anim_spd, int loop_flg )
{
	ANIMLIST	*ptAnimlist;
	FRAMELIST	*ptFramelist;
	int i;
	short dx,dy;
	int BmpNo;
	U4 chrNo;

	/*新しいアニメーションか動作か向きなら*/
	if(ATR_CHR_ACT_OLD(a0) != ATR_CHR_ACT(a0) ||
	ATR_CHR_ANG(a0) != ATR_CHR_ANG_OLD(a0) ||
	ATR_CHR_NO(a0) != ATR_CHR_NO_OLD(a0)){
		ATR_CHR_NO_OLD(a0) = ATR_CHR_NO(a0);		/*新しいアニメーションセット*/
		ATR_CHR_ACT_OLD(a0) = ATR_CHR_ACT(a0);		/*新しい動作セット*/
		ATR_CHR_ANG_OLD(a0) = ATR_CHR_ANG(a0);		/*新しい向きセット*/
		ATR_CHR_TIM(a0) = 0;		/*アニメーションの時間クリア*/
		ATR_CHR_CNT(a0) = 0;		/*パターンのクリア*/
	}
	if(ATR_CHR_TIM(a0)){		/*パターン変更の時間でなければ*/
		ATR_CHR_TIM(a0)--;
		return 0;		/*アニメーションは続く*/
	}
//	ATR_CHR_TIM(a0)=2;		//１枚２割り込み

	if(ATR_CHR_NO(a0) <= CG_INVISIBLE)
	{
		ATR_PAT_NO(a0) = 0;	//画像のない番号を指定して見えなくする
		return 0;			//予約番号なら抜ける
	}

	if(ATR_CHR_NO(a0) < SPRSTART){		//無いなら
	 	realGetNo( ATR_CHR_NO(a0) , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
		ATR_PAT_NO(a0) = BmpNo;			//ＢＭＰ番号セット
		ATR_CHR_H_POS(a0) = dx;		//Ｘ座標オフセットセット
		ATR_CHR_V_POS(a0) = dy;		//Ｙ座標オフセットセット
		ATR_CHR_TIM(a0)=0x7fffffff;
		return 0;
	}

	if(ATR_CHR_NO(a0) > SPRSTART + mxSPRITE) return 0;	// 規定値外なら抜ける

	chrNo = ATR_CHR_NO(a0) - SPRSTART;
	ptAnimlist = SpriteData[chrNo].ptAnimlist;
	if( SpriteData[chrNo].animSize == 0 ) return 0;		// アニメーションデータが無いなら抜ける

	//アニメーションの種類の設定
	for(i=0 ; i < SpriteData[chrNo].animSize; i++){
		if( ATR_CHR_ANG(a0) == ptAnimlist[i].dir && ptAnimlist[i].no == ATR_CHR_ACT(a0) ){
			break;
		}
	}
	if( i >= SpriteData[chrNo].animSize )
		i = 0;
	ptFramelist = ptAnimlist[i].ptFramelist;

	if(anim_spd){		//アニメーション速度指定なら
		ATR_CHR_TIM(a0) = anim_spd;		//１枚の表示時間セット
	} else {
		ATR_CHR_TIM(a0) = ptAnimlist[i].dtAnim;		//１枚の表示時間セット
	}

//	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
//	if(ATR_CHR_CNT(a0) == i)		//アニメーションループなら
//		ATR_CHR_CNT(a0) = 0;		//先頭に戻す

	if((U4)ATR_CHR_CNT(a0) >= ptAnimlist[i].frameCnt){	//アニメーションループなら
		if(loop_flg == ANM_NO_LOOP){		//ループ無しか
			ATR_CHR_TIM(a0) = 255;
			return 1;		//アニメーション終了
		} else {
			ATR_CHR_CNT(a0) = 0;		//先頭に戻す
		}
	}

	//効果音再生なら
	if( ptFramelist[ATR_CHR_CNT(a0)].SoundNo != 0 ){
		if(ptFramelist[ATR_CHR_CNT(a0)].SoundNo < 10000){
			play_se( ptFramelist[ATR_CHR_CNT(a0)].SoundNo, ATR_H_POS(a0), ATR_V_POS(a0) );
		} else {
			ATR_HIT(a0) = ptFramelist[ATR_CHR_CNT(a0)].SoundNo;
		}
	}

	ATR_PAT_NO(a0) = ptFramelist[ATR_CHR_CNT(a0)].BmpNo;			//ＢＭＰ番号セット
	realGetPos(ptFramelist[ATR_CHR_CNT(a0)].BmpNo, &dx, &dy);		//アダーンビンのＸＹ座標取り出し
	ATR_CHR_H_POS(a0) = ptFramelist[ATR_CHR_CNT(a0)].PosX + dx;		//Ｘ座標オフセットセット
	ATR_CHR_V_POS(a0) = ptFramelist[ATR_CHR_CNT(a0)].PosY + dy;		//Ｙ座標オフセットセット
	ATR_CHR_CNT(a0)++;		//カウンター更新
	ATR_CHR_TIM(a0)--;		//
	return 0;
}






#if 0
int pattern( ACTION *a0, int anim_spd, int loop_flg )
{
	ANIMLIST	*ptAnimlist;
	FRAMELIST	*ptFramelist;
	BMPLIST		*ptBmplist;
	int i;
	short dx,dy;
	int BmpNo;

	/*新しいアニメーションか動作か向きなら*/
	if(ATR_CHR_ACT_OLD(a0) != ATR_CHR_ACT(a0) ||
	ATR_CHR_ANG(a0) != ATR_CHR_ANG_OLD(a0) ||
	ATR_CHR_NO(a0) != ATR_CHR_NO_OLD(a0)){
		ATR_CHR_NO_OLD(a0) = ATR_CHR_NO(a0);		/*新しいアニメーションセット*/
		ATR_CHR_ACT_OLD(a0) = ATR_CHR_ACT(a0);		/*新しい動作セット*/
		ATR_CHR_ANG_OLD(a0) = ATR_CHR_ANG(a0);		/*新しい向きセット*/
		ATR_CHR_TIM(a0) = 0;		/*アニメーションの時間クリア*/
		ATR_CHR_CNT(a0) = 0;		/*パターンのクリア*/
	}
	if(ATR_CHR_TIM(a0)){		/*パターン変更の時間でなければ*/
		ATR_CHR_TIM(a0)--;
		return 0;		/*アニメーションは続く*/
	}
//	ATR_CHR_TIM(a0)=2;		//１枚２割り込み
	if (ATR_CHR_NO(a0) >= mxSPRITE || ATR_CHR_NO(a0) <= CG_INVISIBLE) return 0;		//無いなら抜ける
	ptAnimlist = SpriteData[ATR_CHR_NO(a0)].ptAnimlist;		//？？？アドレスセット
	if (ptAnimlist == NULL){		//無いなら
	 	realGetNo( ATR_CHR_NO(a0) , (U4 *)&BmpNo );
		realGetPos( BmpNo  , &dx, &dy);
		ATR_PAT_NO(a0) = BmpNo;			//ＢＭＰ番号セット
		ATR_CHR_H_POS(a0) = dx;		//Ｘ座標オフセットセット
		ATR_CHR_V_POS(a0) = dy;		//Ｙ座標オフセットセット
		ATR_CHR_TIM(a0)=0x7fffffff;
		return 0;
	}

	//アニメーションの種類の設定
	for (i=0 ;  ; i++){
		if ( ptAnimlist[i].ptFramelist == NULL ){
			if (i==0) return 0;
			i = 0;
			break;
		}
		if ( ATR_CHR_ANG(a0) == ptAnimlist[i].dir && ptAnimlist[i].id == ATR_CHR_ACT(a0) ){
			break;
		}
	}
	ptFramelist = ptAnimlist[i].ptFramelist;

	//効果音再生なら
	if( ptFramelist[ATR_CHR_CNT(a0)].SoundNo != 0 ){
		play_se( ptFramelist[ATR_CHR_CNT(a0)].SoundNo, ATR_H_POS(a0), ATR_V_POS(a0) );
	}

	if(anim_spd){		//アニメーション速度指定なら
		ATR_CHR_TIM(a0) = anim_spd;		//１枚の表示時間セット
	} else {
		ATR_CHR_TIM(a0) = ptAnimlist[i].frameCnt;		//１枚の表示時間セット
	}

//	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
//	if(ATR_CHR_CNT(a0) == i)		//アニメーションループなら
//		ATR_CHR_CNT(a0) = 0;		//先頭に戻す

	if(ptFramelist[ATR_CHR_CNT(a0)].ptBmplist == NULL){		//アニメーションループなら
		if(loop_flg == ANM_NO_LOOP){		//ループ無しか
			ATR_CHR_TIM(a0) = 255;
			return 1;		//アニメーション終了
		} else {
			ATR_CHR_CNT(a0) = 0;		//先頭に戻す
		}
	}
	ptBmplist = ptFramelist[ATR_CHR_CNT(a0)].ptBmplist;		//今回の
#if 0
	for (i=0 ; ptBmplist[i].ptBmppos != NULL ; i++ ){		//枚数分表示
		if(!i){
			ATR_PAT_NO(a0) = ptBmplist[i].ptBmppos->BmpNo;			//ＢＭＰ番号セット
			realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);		//アダーンビンのＸＹ座標取り出し
			ATR_CHR_H_POS(a0) = ptBmplist[i].PosX + dx;		//Ｘ座標オフセットセット
			ATR_CHR_V_POS(a0) = ptBmplist[i].PosY + dy;		//Ｙ座標オフセットセット
		} else {
			realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);		//アダーンビンのＸＹ座標取り出し
			StockDispBuffer2( ATR_H_POS(a0) + ptBmplist[i].PosX + dx,
			ATR_V_POS(a0) + ptBmplist[i].PosY + dy,
			a0->dispPrio,
			ptBmplist[i].ptBmppos->BmpNo );
		}
	}

#else
	ATR_PAT_NO(a0) = ptBmplist[0].ptBmppos->BmpNo;			//ＢＭＰ番号セット
	realGetPos(ptBmplist[0].ptBmppos->BmpNo, &dx, &dy);		//アダーンビンのＸＹ座標取り出し
	ATR_CHR_H_POS(a0) = ptBmplist[0].PosX + dx;		//Ｘ座標オフセットセット
	ATR_CHR_V_POS(a0) = ptBmplist[0].PosY + dy;		//Ｙ座標オフセットセット
#endif
	ATR_CHR_CNT(a0)++;		//カウンター更新
	ATR_CHR_TIM(a0)--;		//
	return 0;

#if 0	//未使用//////////////////////////////////////////////////////
	// １フレームアニメーション枚数を計算
	for( i = 0; ptFramelist[i].ptBmplist != NULL; i++ );
	animSize = i;

	//現在のフレーム位置を取得
	if (ptsprite->ctAnim < 1){		//１未満モード
		ptBmplist = ptFramelist[(int)(ptsprite->ctAnim * animSize)].ptBmplist;
	}else{
		ptBmplist = ptFramelist[(int)(ptsprite->ctAnim)                        ].ptBmplist;
	}

	//1フレーム内の
	for (i=0 ; ptBmplist[i].ptBmppos != NULL ; i++ ){
		realGetPos(ptBmplist[i].ptBmppos->BmpNo, &dx, &dy);
		drawImage( ptBmplist[i].ptBmppos->BmpNo , 
				  x + ptBmplist[i].ptBmppos->PosX + ptBmplist[i].PosX + dx ,
				  y + ptBmplist[i].ptBmppos->PosY + ptBmplist[i].PosY + dy);
	}
#endif	/////////////////////////////////////////////////////////////

}
#endif

