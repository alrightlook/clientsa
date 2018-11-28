/* output by ./lsgen.perl 0.41 ( 1998 May)
 * made Mon Nov  8 16:59:21 1999
 * user kawata
 * host indy
 * file /opt/kawata/sa/bin/output/lssproto_cli.c
 * util output/lssproto_util.c , output/lssproto_util.h
 * src  /opt/kawata/sa/bin/../doc/lssproto.html
 */
#define _LSSPROTOCLI_C_
#include "lssproto_cli.h"

/*
163 :   <LI><a name="CS_W"><font color=red>clienttoserver W( int x, int y, string direction);<br></font></a>
164 :       (Walk)歩く。向きをかえる。クライアントはユーザーが指定した経路を計算し，
165 :       その歩きの経路を送信する。クライアントは既に受けたマップデータなどにより
166 :       当たり判定などを行うが，これはサーバーと既に完全一致していることが前提と
167 :       なる。サーバーはこのWによる応答はしない。サーバーはクライアントから受け
168 :       取った座標を元に歩く処理を行うが，それは他ユーザーに見えるための処理など
169 :       を行う為だけに行う。
170 :       また，サーバーは１歩キャラが歩く毎にMCを送信する。また，クライアントはMC
171 :       にてチェックサムが違えばマップをサーバーに要求しないといけないが，その際
172 :       歩いている状態などでは歩きの再計算が必要になるかもしれない。
173 :       
174 : 
175 : 
176 : 
177 :       <dl>
178 : 	<dt>int x,y
179 : 	<dd>歩き始めのプレイヤーのフロア，ｘ，ｙ座標。
180 : 	???????????????衙???????x??y?遙?
181 : 	<dt>string direction
182 : 	<dd>あるく方向 a,b,c,d,e,f,g,hが場所が変化する移動で、
183 : 	    aが真上(dx,dy)=(0,-1)で、みぎまわりに hが左上(-1,-1)
184 : 	    A,B,C,D,E,F,G,H がその場回転。向きはそれぞれ真上か
185 : 	    らみぎまわり。右右上下とあるく場合は "ccae"という
186 : 	    文字列を送信し、その場で左に向くばあいは"G"と送信
187 : 	    することになる。かならず1歩1文字に対応している。こ
188 : 	    の文字列はエスケープしない。
189 : 	   
190 :       </dl>?????鈊??????蠇??????????????訒??郢鉙??蠇?ccae??186????????????????郢鈺羇???????????????G??187?????????羹盞????????????雜????????????188?????????????????
191 :       <br>
192 :       <br>
193 :       <hr>
194 :       <br>
*/

void lssproto_W_send( int fd,int x,int y,char* direction )
{
	lssproto_CreateHeader( lssproto.work , "W" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( direction ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
195 :   <LI><a name="CS_W2"><font color=red>clienttoserver w( int x, int y, string direction);<br></font></a>
196 :       基本的にはＷと全く同じ。クライアントが，その１回のログイン（起動？）の内に，
197 :       既にマップを読んでいたら（もしくは，歩いてMCが来て，チェックがOKだった）場合，
198 :       もうMCは必要ないので，歩く時にMC要らない要求をだしながら歩くのがこのwプロトコルである。
199 :       このｗを送信する条件として，次に出現するマップのイベントオブジェクトが <a href="#CS_EV">CHAR_EVENT_ALTERRATIVE</a>
200 :       だった場合には，このイベントオブジェクトが変化している可能性があるので普通のＷ，又はその部分をマップ要求して歩く事。
201 :       <br>
202 :       <br>
203 :       <hr>
204 :       <br>
205 :       
*/

void lssproto_w_send( int fd,int x,int y,char* direction )
{
	lssproto_CreateHeader( lssproto.work , "w" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( direction ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
206 :   <LI><a name="CS_EV"><font color=red>clienttoserver EV( int event,int seqno,int x, int y, int dir);<br></font></a>
207 : 	クライアントがイベントオブジェクトを踏んだ時に発生する。
208 : 	ワープや固定敵へのエンカウント等に使用される。
209 : 	
210 :       <br><br>
211 :       <dl>
212 : 	<dt>int event
213 : 	<dd>イベントの種類。char_base.hでは以下のように定義されている。<br>
214 : 	<pre>
215 : typedef enum
216 : {
217 : 	CHAR_EVENT_NONE,		イベントは発生しない
218 : 	CHAR_EVENT_NPC,			固定NPCである
219 : 	CHAR_EVENT_ENEMY,		固定敵エンカウントイベント
220 : 	CHAR_EVENT_WARP,		ワープイベント
221 : 	CHAR_EVENT_ALTERRATIVE,		何も無いが，変化するかもしれないもの。（マップ読込時に要求する事）
222 : 	CHAR_EVENTNUM,
223 : }CHAR_EVENT;
224 : 	    
225 : 	</pre>
226 : 	CHAR_EVENT_NPCの場合は，単なるNPCで，しかもぶつかって欲しいものなど。
227 : 	クライアントはEVを送らず，ぶつかったかを判断して手前で止まるようにする事。<br>
228 : 	<dt>int seqno
229 : 	<dd>イベントにシーケンスNoを打っておく。イベントが起こる度にインクリメントしていく事。
230 : 	    これはクライアントが管理する。
231 : 	    サーバーのEVの応答に対して使う。整合性を取るため。
232 : 	<dt>int x,y
233 : 	<dd>自分の位置
234 : 	<dt>int dir
235 : 	<dd>イベントのある向き。自分自身の位置とこの向きからイベントの場所を探す。自分自身と同じ場所の時は-1。
236 :       </dl>
237 :       <br>
238 :       <br>
239 :       <hr>
240 :       <br>
*/

void lssproto_EV_send( int fd,int event,int seqno,int x,int y,int dir )
{
	lssproto_CreateHeader( lssproto.work , "EV" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( event ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( seqno ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
255 :   <LI><a name="CS_EN"><font color=red>clienttoserver EN( int x, int y );<br></font></a>
256 : 	クライアントが（固定じゃない）敵とエンカウントしたい時に送信する。
257 : 	仲間歩きの時のリーダーじゃない人はこれを送信しないようにする事。
258 :       <br><br>
259 :       <dl>
260 : 	<dt>int x,y
261 : 	<dd>エンカウントしたプレイヤーのフロア，ｘ，ｙ座標。
262 :       </dl>
263 :       <br>
264 :       <br>
265 :       <hr>
266 :       <br>
*/

void lssproto_EN_send( int fd,int x,int y )
{
	lssproto_CreateHeader( lssproto.work , "EN" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
267 :   <LI><a name="CS_DU"><font color=red>clienttoserver DU( int x, int y);<br></font></a>
268 : 	クライアントがプレイヤー同士のエンカウントを要求した場合に送信する。
269 : 	仲間歩きの時のリーダーじゃない人はこれを送信しないようにする事。
270 :       <br><br> 客户要求玩家的遇敌时发送。269:伙伴在走路的时候的首领不是人把这不发的事。
271 :       <dl>
272 : 	<dt>int x,y
273 : 	<dd>エンカウントしたプレイヤーのフロア，ｘ，ｙ座標。
274 :       </dl>
275 :       <br>
276 :       <br>
277 :       <hr>
278 :       <br>
*/

void lssproto_DU_send( int fd,int x,int y )
{
	lssproto_CreateHeader( lssproto.work , "DU" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
292 :   <LI><a name="CS_EO"><font color=red>clienttoserver EO( int dummy );<br></font></a>
293 : 	クライアントが最後のムービーを見終わったときに送る。客户最后的电影看完的时候送。
294 :       <dl>
295 : 	<dt>int dummy
296 : 	<dd>ダミーデータ。今は何でも良い。模拟数据。现在什么都好
297 :       </dL>
298 :       <br>
299 :       <br>
300 :       <hr>
301 :       <br>
302 : 
*/

void lssproto_EO_send( int fd,int dummy )
{
	lssproto_CreateHeader( lssproto.work , "EO" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dummy ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
303 :   <LI><a name="CS_BU"><font color=red>clienttoserver BU( int dummy );<br></font></a>
304 : 	クライアントがエンカウントを中断したい場合に送る。 客户遇敌中断
305 :       <dl>
306 : 	<dt>int dummy
307 : 	<dd>ダミーデータ。今は何でも良い。模拟数据
308 :       </dL>
309 :       <br>
310 :       <br>
311 :       <hr>
312 :       <br>
313 : 
*/

void lssproto_BU_send( int fd,int dummy )
{
	lssproto_CreateHeader( lssproto.work , "BU" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dummy ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
314 :   <LI><a name="CS_JB"><font color=red>clienttoserver JB( int x, int y );<br></font></a>
315 : 	他の戦闘に参戦したい場合に送る。失敗、成功は EN で送り返す。
316 :       <dl>
317 : 	<dt>int x, int y;
318 : 	<dd>クライアントの現在の座標。
319 :       </dL>
320 :       <br>
321 :       <br>
322 :       <hr>
323 :       <br>
324 : 
*/

void lssproto_JB_send( int fd,int x,int y )
{
	lssproto_CreateHeader( lssproto.work , "JB" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
325 :   <LI><a name="CS_LB"><font color=red>clienttoserver LB( int x, int y );<br></font></a>
326 : 	他の戦闘を観戦したい場合に送る。失敗、成功は EN で送り返す。
327 :       <dl>
328 : 	<dt>int x, int y;
329 : 	<dd>クライアントの現在の座標。
330 :       </dL>
331 :       <br>
332 :       <br>
333 :       <hr>
334 :       <br>
335 : 
*/

void lssproto_LB_send( int fd,int x,int y )
{
	lssproto_CreateHeader( lssproto.work , "LB" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
348 :   <LI><a name="CS_B"><font color=red>clienttoserver B( string command );<br></font></a>
349 :       クライアントからプレイヤーの戦闘時コマンドを指定する。ここで相手番号とは０～１９で指定すると個人を指し、２０だと右側サイド全体を指定。２１だと左側サイド全体。２２だとフィールド全体を指す。<br><br>
350 : <ul>
351 : <li>H|相手番号(%X)(0～19)<br>通常攻撃。相手の番号を指定<br><br>
352 : <li>E<br>逃げる。<br><br>
353 : <li>G<br>防御。<br><br>
354 : <li>N<br>何もしない。<br><br>
355 : <li>T|相手番号(%X)<br>捕獲。<br><br>
356 : <li>J|呪術の番号(%X)|相手番号(%X)。
357 : 	<br>呪術を使う。ここで呪術の番号とはプレイヤーの呪術枠の番号を指す。
358 : 	よって(0～5)。<br><br>
359 : <li>W|ペット技番号(%X)|相手番号(%X)<br>
360 : 	ペット技を使用する。ここでのペット技番号はペットの技枠の番号。
361 : 	よって(0～7)<br><br>
362 : <li>S|ペットの番号(%d)。<br>ペット出す。負の数ならペット戻す。<br><br>
363 : <li>I|アイテムの番号(%X)|相手番号(%X)。
364 : 	<br>アイテムを使う。ここでアイテムの番号とはプレイヤーのアイテム枠の番号を指す。
365 : 	よって(5～19)。<br><br>
366 : </ul>
367 :       <br>
368 :       <br>
369 :       <br>   11个
370 : 
*/

void lssproto_B_send( int fd,char* command )
{
	lssproto_CreateHeader( lssproto.work , "B" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( command ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
536 :   <LI><a name="SKD"><font color=red>clienttoserver SKD( int dir , int index );<br></font></a>
537 :       (SKillDir)
538 :       指定した方向にスキルをつかう。
539 :       <br><br>
540 :       <dl>
541 : 	<dt>int dir
542 : 	<dd>スキルを使う方向
543 : 	<dt>int index
544 : 	<dd>自分の持っているスキルの表へのindex. 自己所拥有的技能表index。
545 :       </dl>
546 :       スキル表は以下の通りである。これはサーバのソースの一部分である。
547 :       <pre>
548 : 
549 :       ソースファイルではコメント
550 :       rpcgen Cのコメントそのままするから。コンパイル通らんくなる。
551 :       コメントスキル一覧enum
552 : 
553 :       みぎの名前は適当。とりあえず、意味がわかるていど
554 :       typedef enum
555 :       {
556 :       SKILL_FIRE,
557 :       SKILL_MAGICIAN,         魔法使い
558 :       SKILL_PRIEST,           僧侶
559 :       SKILL_ALOTOFTHINGS,     ものもち
560 :                               求め方は、
561 :                              7(装備) + 8 (初期 ) + 4*(スキルのレベル)
562 :       SKILL_AVOIDRATEUP,      みかわしのじゅつ 3% ずつあがる
563 :       SKILL_DETERMINEITEM,    アイテム識別
564 : 
565 :       SKILL_DETERMINOTHERS,   千里眼
566 :                              0 当り判定の白黒(地形)
567 :                              1 地形が色つきで見える。パーツが入れな
568 :                                  かったら黒で、入れたらタイルの色
569 :                              2 プレイヤー
570 :                              3 NPC
571 :                              4 アイテム
572 :                              5 敵
573 : 
574 :       SKILL_FIRECUT,         炎削減  5 %
575 :       SKILL_THUNDERCUT,      稲妻削減  5 %
576 :       SKILL_ICECUT,          氷削減  5 %
577 :       SKILL_MERCHANT,        商人 売り20% 買い100% から 5%ずつあがる
578 :       SKILL_HEALER,          僧侶
579 :                               パラメータ回復を順番に上にいくほど下を含む
580 :                               1 毒
581 :                               2 しびれ
582 :                               3 沈黙
583 :                               4 石化
584 :                               5 暗闇
585 :                               6 混乱
586 :                               7 HP 1 で復活
587 :                               8 HP まんたんで復活
588 :       武器関係全部
589 :       命中率をどれだけいかせるか
590 :       0  0
591 :       1 50
592 :       2 57
593 :       3 64
594 :       4 71
595 :       5 78
596 :       6 85
597 :       7 92
598 :       8 100
599 :       クリティカルアップ( 100 でわる )初期値はほぼ 1 10 までいく
600 :       これは + でいいかな
601 :       0 0
602 :       1 1
603 :       2 2
604 :       3 3
605 :       4 4
606 :       5 5
607 :       6 6
608 :       7 7
609 :       8 8
610 : 
611 :       SKILL_FIST,             素手装備
612 :       SKILL_SWORD,            剣装備
613 :       SKILL_AXE,              斧装備
614 :       SKILL_SPEAR,            槍装備
615 :       SKILL_BOW,              弓装備
616 :       SKILL_CANE,             杖装備
617 : 
618 :       SKILL_LARGEVOICE,       大きな声   Lv 8
619 :                               詳しいことはプロトコル TK を
620 :       SKILL_NUM
621 :       }SKILL_ID;
622 :       </pre>
623 :       <br>
624 :       <hr>
625 :       <br>
626 : 	
*/

void lssproto_SKD_send( int fd,int dir,int index )
{
	lssproto_CreateHeader( lssproto.work , "SKD" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );//使用技能的方向
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
627 :   <li><a name="CS_ID"><font color=red>clienttoserver ID( int x, int y, int haveitemindex,int toindex);<br></font></a>
628 :       (useItemDir)アイテムを使う。ほぼフィールド上からしか呼ばれない。バトル中は別プロトコルにてアイテム使用の
629 :       プロトコルが飛んでくる。
630 : 
631 :       
632 :       <dl>
633 : 	<dt>int x,y
634 : 	<dd> 自分のx,y 座標
635 : 	<dt>int haveitemindex
636 : 	<dd> どのアイテムを使ったか。アイテムの場所を送る。
637 : 	<dt>int toindex
638 : 	<dd> 誰にアイテムを使用したか。これはオブジェクトやキャラのindexではない。以下の様になっている。
639 : 	<pre>
640 : 	  自分    = 0
641 : 	  ペット  = 1 ～5
642 : 	  仲間    = 6 ～10 （S N の0～4に対応。自分自身も含まれている）
643 : 	</pre>
644 : 	対象が全員，とか分からない，とかの場合は-1で送信する。<br>
645 : 	<br>
646 :       </dl>
647 :       <br>
648 :       <br>
649 :       <hr>
650 :       <br>
651 :       
652 : 
653 :       
654 : 
655 :       
*/

void lssproto_ID_send( int fd,int x,int y,int haveitemindex,int toindex )
{
	lssproto_CreateHeader( lssproto.work , "ID" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( haveitemindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( toindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
656 :   <LI><a name="PI"><font color=red>clienttoserver PI( int x, int y,int dir );<br></font></a>
657 :       (PickupItem)
658 :       アイテムを拾う。拾うのは足元をふくむ隣接する9マスである。<br><br>
659 :       <dl>
660 : 	<dt>int x,y
661 : 	<dd> 自分のx,y 座標
662 : 	<dt>int dir
663 : 	<dd>拾う方向。その方向と、足元を見る。 負の時は足元だけ見る。
664 :       </dl>
665 :       <br>
666 :       <hr>
667 :       <br>
668 : 
669 :     拾取物品  
*/

void lssproto_PI_send( int fd,int x,int y,int dir )
{
	lssproto_CreateHeader( lssproto.work , "PI" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
670 :   <LI><a name="DI"><font color=red>clienttoserver DI( int x, int y, int itemindex);<br></font></a>
671 :       (DropItem)
672 :       アイテムを置く。置くのはドロップボタンに置くので、方向や位置の
673 :       指定はない。itemindexのアイテムを置く。いつでも自分の前に置く。
674 :       <br><br>
675 : 
676 :       <dl>
677 : 	<dt>int x,y
678 : 	<dd> 自分のx,y 座標
679 : 	<dt>int itemindex
680 : 	<dd>置きたいアイテムのインデクス。
681 :       </dl>
682 :       <br>
683 :       <hr>
684 :       <br>
685 : 
686 :       
*/

void lssproto_DI_send( int fd,int x,int y,int itemindex )
{
	lssproto_CreateHeader( lssproto.work , "DI" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( itemindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
687 :   <LI><a name="DG"><font color=red>clienttoserver DG( int x, int y, int amount );<br></font></a>
688 :       (DropGold)
689 :       お金を置く。SA用に現在位置の座標を追加。<br><br>
690 :       <dl>
691 : 	<dt>int x,y
692 : 	<dd> 自分のx,y 座標
693 : 	<dt>int amount
694 : 	<dd>置くお金の量。
695 :       </dl>
696 :       <br>
697 :       <hr>
698 :       <br>
699 :   
*/

void lssproto_DG_send( int fd,int x,int y,int amount )
{
	lssproto_CreateHeader( lssproto.work , "DG" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( amount ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
700 :   <LI><a name="DP"><font color=red>clienttoserver DP( int x, int y,int petindex);<br></font></a>
701 :       (DropPet)
702 :       ペットを置く。方向や位置の指定はない。
703 :       petindexのアイテムを置く。いつでも自分の前に置く。
704 :       <br><br>
705 : 
706 :       <dl>
707 : 	<dt>int x,y
708 : 	<dd> 自分のx,y 座標
709 : 	<dt>int petindex
710 : 	<dd>置きたいペットのindex。プレイヤーが何番目に持っているペットか。
711 :       </dl>
712 :       <br>
713 :       <hr>
714 :       <br>
715 : 
*/

void lssproto_DP_send( int fd,int x,int y,int petindex )
{
	lssproto_CreateHeader( lssproto.work , "DP" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( petindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
735 :   <LI><a name="MI"><font color=red>clienttoserver MI( int fromindex , int toindex);<br></font></a>
736 :       (MoveItem)<br>
737 :       アイテムを移動/装備/装備をはずす。装備になるかどうかは
738 :       移動の場所で決まる。<br><br>
739 :       <dl>
740 : 	<dt>int fromindex
741 : 	<dd>移動するアイテムのインデクス。
742 : 	<dt>int toindex
743 : 	<dd>目的地のアイテムインデクス。
744 :       </dl>
745 :       目的地とは以下の通り。
746 :       <ul>
747 : 	<li>0    兜を装備する所( 頭 )
748 : 	<li>1    鎧を装備する所( 体 )
749 : 	<li>2    攻撃アイテムを装備する所(手)
750 : 	<li>3    装飾品を装備する所1
751 : 	<li>4    装飾品を装備する所2
752 : 	<li>それ以上。   アイテム欄
753 :       </ul>
754 :       <br>
755 :       <hr>
756 :       <br>  道具移动/装备/装备摘下。装备是否能成为  摘下装备？移动的场所决定。
*/

void lssproto_MI_send( int fd,int fromindex,int toindex )
{
	lssproto_CreateHeader( lssproto.work , "MI" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( fromindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( toindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
782 :   <LI><a name="CS_MSG"><font color=red>clienttoserver MSG( int index , string  message , int color );<br></font></a>
783 :       (MeSsaGe)
784 :       アドレスブックの相手にメッセージを送信。indexは自分のアドレスブッ
785 :       クのインデクスである。<br><br>
786 :       <dl>
787 : 	<dt>int index
788 : 	<dd>アドレスブックにいる相手のインデクス。
789 : 	<dt>string message
790 : 	<dd>相手に送信するメッセージ。EUCでサーバーに送信する。この文
791 : 	    字列はデリミタをふくまないので、エスケープする必要はない。
792 : 	<dt>int color
793 : 	<dd>メッセージの色。
794 :       </dl>
795 :       <br>
796 :       <br>
797 :       <hr>
798 :       <br>
799 : 
*/

void lssproto_MSG_send( int fd,int index,char* message,int color )
{
	lssproto_CreateHeader( lssproto.work , "MSG" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( message ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( color ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
824 :   <LI><a name="CS_PMSG"><font color=red>clienttoserver PMSG( int index, int petindex, int itemindex, string  message , int color );<br></font></a>
825 :       (PetMeSsaGe)
826 :       アドレスブックの相手にペットでメッセージを送信。indexは自分のアドレスブッ
827 :       クのインデクスである。アイテムも送信することが出来る。<br><br>
828 :       <dl>
829 : 	<dt>int index
830 : 	<dd>アドレスブックにいる相手のインデクス。
831 : 	<dt>int petindex
832 : 	<dd>何番目のペットで送信するか
833 : 	<dt>int itemindex
834 : 	<dd>何番目のアイテムを送信するか。
835 : 	<dt>string message
836 : 	<dd>相手に送信するメッセージ。EUCでサーバーに送信する。この文
837 : 	    字列はデリミタをふくまないので、エスケープする必要はない。
838 : 	<dt>int color
839 : 	<dd>メッセージの色。
840 :       </dl>
841 :       <br>
842 :       <br>
843 :       <hr>
844 :       <br>  21个
*/

void lssproto_PMSG_send( int fd,int index,int petindex,int itemindex,char* message,int color )
{
	lssproto_CreateHeader( lssproto.work , "PMSG" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( index ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( petindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( itemindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( message ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( color ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
871 :   <LI><a name="CS_AB"><font color=red>clienttoserver AB( void );</font></a><br>
872 :       (AddressBook)
873 :       アドレスブックの内容をサーバーに問いあわせる。引数はない。サーバー
874 :       はこのコマンドをうけとるとアドレスブックの全エントリの内容を返す。
875 : 
*/

void lssproto_AB_send( int fd )
{


void lssproto_L_send( int fd,int dir )
{
	lssproto_CreateHeader( lssproto.work , "L" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dir ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
956 :   <LI><a name="C2STK"><font color=red>clienttoserver TK( int x, int y, string message ,int color, int area);<br></font></a>
957 :       (TalK)
958 :       チャット用メッセージを送信する。返答はない。<br><br>
959 :       <dl>
960 :         <dt>int x,y
961 :         <dd>自分のｘ，ｙ座標
962 : 	<dt>string message
963 : 	<dd>string の内容は, "|" で2つに区切られている。ひとつめのトー
964 : 	    クンの文字によって2つめのトークンの内容が変更される。2つめ
965 : 	    のトークンは<a href="#escaping">エスケープ</a>されている。
966 : 	    ひとつめのトークンは以下のパターンがある。
967 : 	    <ul>
968 : 	      <li>Pの時<br>
969 : 		  先頭にC文字列の"P"がつく場合、それはプレイヤーが入
970 : 		  力したチャットメッセージである。クライアントプログラ
971 : 		  ムは、文字列の先頭にかならずこの文字とデリミタをつ
972 : 		  けくわえて送信する。サーバーは文字列の先頭に"P"を
973 : 		  検出したら声の当たり判定をして、聞こえる範囲に送信す
974 : 		  る。そのときはTKコマンドをつかうが、TKコマンドで送信す
975 : 		  る文字列にはこの"P"をふくめてそのまま入れる。
976 : 		  <br>
977 : 		  文字列の内容の例：
978 : 		  <pre>
979 : 		  "P|きょうはいい天気ですな!!"
980 : 		  </pre>
981 : 		  <br>
982 : 		  チャットメッセージを送信したときに聞こえる範囲を指定
983 : 		  するには、語尾に以下の文字列があるかをサーバが判定す
984 : 		  る。文字列の表記はCにおける表現である。'!' は全角文
985 : 		  字で入力される場合がままあるが、サーバは'!'だけに反
986 : 		  応する。クライアントプログラムが全角文字を半角文字に
987 : 		  なおして送信する。”!”一つおきにプレイヤーを中心に
988 : 		  １マス聞こえる範囲が増える。
989 : 		  <table border>
990 : 		    <tr><td>"..."</td><td>話したキャラクタと自分だけ
991 : 		    </td></tr>
992 : 		      
993 : 		  </table>
994 : 	      </ul>
995 : 	  <dt>int color
996 : 	  <dd>文字列の色。
997 : 	  <dt>int area
998 : 	  <dd>話した言葉が聞こえる範囲（１～を指定する。）
999 : 	  
1000 : 	</dl>
1001 :       <br>
1002 :       <hr>
1003 :       <br>
1004 : 
*/


void lssproto_PR_send( int fd,int x,int y,int request )
{
	lssproto_CreateHeader( lssproto.work , "PR" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( request ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1810 :   <li><a name="CS_KS"><font color=red>clienttoserver KS( int petarray);<br></font></a>
1811 :       (Kyoryu Select)戦闘に出すペットを選択した。<br><br>
1812 :       
1813 :       <dl>
1814 : 	<dt>int petarray
1815 : 	<dd>0 ～ 4 までの数字。選んだペットの持っている場所を示す。また，ペットを戦闘にエントリーしたくないようにする時は-1を入れる。
1816 :       </dl>
1817 :       <br>
1818 :       <br>
1819 :       <hr>
1820 :       <br>
*/

//战斗发出的宠物选择。
void lssproto_KS_send( int fd,int petarray )
{
	lssproto_CreateHeader( lssproto.work , "KS" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( petarray ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1838 :   <li><a name="CS_AC"><font color=red>clienttoserver AC( int x, int y, int actionno);<br></font></a>
1839 :       (action)喜怒哀楽などを表現する。<br><br>
1840 :       
1841 :       <dl>
1842 : 	<dt>int x,y
1843 : 	<dd> 自分のx,y 座標
1844 : 	<dt>int actionno
1845 : 	<dd>プレイヤーのとりたいアクションを送信する。サーバーはこれを受けとるとそのまま周りにアクション(CA)を送信する。
1846 : 	    もしかしたら連打制御等をしないといけないかもしれない。番号とアクションの対応は以下の通り。<br>
1847 : 	<br>
1848 : 	  <table border=1>
1849 : 	    <tr><td>action</td><td>no</td></tr>
1850 : 	    <tr><td>攻撃</td><td>0</td></tr>
1851 : 	    <tr><td>ダメージ</td><td>1</td></tr>
1852 : 	    <tr><td>倒れる</td><td>2</td></tr>
1853 : 	    <tr><td>立っている</td><td>3</td></tr>
1854 : 	    <tr><td>歩く</td><td>4</td></tr>
1855 : 	    <tr><td>座る</td><td>5</td></tr>
1856 : 	    <tr><td>手を振る</td><td>6</td></tr>
1857 : 	    <tr><td>喜ぶ</td><td>7</td></tr>
1858 : 	    <tr><td>怒る</td><td>8</td></tr>
1859 : 	    <tr><td>悲しむ</td><td>9</td></tr>
1860 : 	    <tr><td>ガード</td><td>10</td></tr>
1861 : 	    <tr><td>うなずく</td><td>11</td></tr>
1862 : 	    <tr><td>投げる</td><td>12</td></tr>
1863 : 	  </table>
1864 :       </dl>
1865 :       <br>
1866 :       <br>
1867 :       <hr>
1868 :       <br>
*/

void lssproto_AC_send( int fd,int x,int y,int actionno )
{
	lssproto_CreateHeader( lssproto.work , "AC" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( actionno ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1869 :   <li><a name="CS_MU"><font color=red>clienttoserver MU( int x, int y, int array, int toindex);<br></font></a>
1870 :       (magic use)魔法を使う。ほぼフィールド上からしか呼ばれない。バトル中は別プロトコルにて魔法使用の
1871 :       プロトコルが飛んでくる。
1872 : 
1873 :       
1874 :       <dl>
1875 : 	<dt>int x,y
1876 : 	<dd> 自分のx,y 座標
1877 : 	<dt>int array
1878 : 	<dd> どの魔法を使ったか。この数字は P の Jn(n は任意の数字)に対応しなければいけない。
1879 : 	<dt>int toindex
1880 : 	<dd> 誰に魔法を使用したか。これはオブジェクトやキャラのindexではない。以下の様になっている。
1881 : 	<pre>
1882 : 	  自分    = 0
1883 : 	  ペット  = 1 ～5
1884 : 	  仲間    = 6 ～10 （S N の0～4に対応。自分自身も含まれている）
1885 : 	</pre>
1886 : 	対象が全員，とか分からない，とかの場合は-1で送信する。<br>
1887 : 	<br>
1888 :       </dl>
1889 :       <br>
1890 :       <br>
1891 :       <hr>
1892 :       <br>
*/

void lssproto_MU_send( int fd,int x,int y,int array,int toindex )
{
	lssproto_CreateHeader( lssproto.work , "MU" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( array ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( toindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1893 :   <li><a name="CS_PS"><font color=red>clienttoserver PS( int havepetindex, int havepetskill, int toindex, string data);<br></font></a>
1894 :       (PetSkill use )ペットの技を使う。フィールド上からしか呼ばれない。
1895 :       
1896 :       <dl>
1897 :         <dt>int havepetindex
1898 :         <dd>何番目のペットが使用したか。
1899 :         <dt>int havepetskill
1900 :         <dd>何番目の技を使用したか。
1901 : 	<dt>int toindex
1902 : 	<dd> 誰に魔法を使用したか。これはオブジェクトやキャラのindexではない。以下の様になっている。
1903 : 	<pre>
1904 : 	  自分    = 0
1905 : 	  ペット  = 1 ～5
1906 : 	  仲間    = 6 ～10 （S N の0～4に対応。自分自身も含まれている）
1907 : 	</pre>
1908 : 	対象が全員，とか分からない，とかの場合は-1で送信する。<br>
1909 : 	<br>
1910 :       	<dt>string data
1911 :       	<dd>付加情報。アイテム合成の技の場合は，haveitemindex1|haveitemindex2|haveitemindex3|...<br>
1912 :       	    と，合成するアイテムの番号が入っている。
1913 :       	    
1914 :       </dl>
1915 :       
1916 :       <br>
1917 :       <br>
1918 :       <hr>
1919 :       <br>
1920 : 
*/

void lssproto_PS_send( int fd,int havepetindex,int havepetskill,int toindex,char* data )
{
	lssproto_CreateHeader( lssproto.work , "PS" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( havepetindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( havepetskill ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( toindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( data ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1949 :   <LI><a name="ST"><font color=red>clienttoserver ST( int titleindex );<br></font></a>
1950 :       (SelectTitle)
1951 :       称号をえらぶ。titleindex が -1 のときは称号をはずすという事にな
1952 :       る。<br><br>
1953 :       <dl>
1954 : 	<dt>int titleindex
1955 : 	<dd>何番目の称号か。
1956 :       </dl>
1957 :       結果は、Txt関数をつかって、「～のしょうごうをえらんだ！」のよう
1958 :       に通知される。
1959 :       <br>
1960 :       <br>
1961 :       <hr>
1962 :       <br>
1963 : 
1964 :       
*/

void lssproto_ST_send( int fd,int titleindex )
{
	lssproto_CreateHeader( lssproto.work , "ST" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( titleindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1965 :   <LI><a name="DT"><font color=red>clienttoserver DT( int titleindex );<br></font></a>
1966 :       (DeleteTitle)
1967 :       称号を削除する。<br><br>
1968 :       <dl>
1969 : 	<dt>int titleindex
1970 : 	<dd>削除する称号のインデクス。
1971 :       </dl>
1972 :       結果は、Txt関数をつかって、「～のしょうごうをさくじょした！」のように通
1973 :       知される。
1974 :       <br>
1975 :       <br>
1976 :       <hr>
1977 :       <br>
1978 : 
*/

void lssproto_DT_send( int fd,int titleindex )
{
	lssproto_CreateHeader( lssproto.work , "DT" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( titleindex ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
1979 :   <LI><a name="FT"><font color=red>clienttoserver FT( string data );<br></font></a>
1980 :       (inputFreeTitle)
1981 :       自分で入力する称号を入力した。<br><br>
1982 :       <dl>
1983 : 	<dt>string data
1984 : 	<dd>自分で入力した称号。EUCでサーバーに送信される。この称号は、
1985 : 	    マウスカーソルをあわせたときに画面下の1行infoに表示され
1986 : 	    るものである。この文字列はデリミタをふくまないので、
1987 : 	    エスケープする必要はない。
1988 :       </dl>
1989 :       この結果は、Txt関数を使って、文字列で通知される。これにともない、
1990 :       自分のCHARINDEXに対して、Chデータがふたたび送信されてくることに
1991 :       なる。
1992 :       <br>
1993 :       <br>
1994 :       <hr>
1995 :       <br>
1996 : 
*/

//自己输入的称号输入了
void lssproto_FT_send( int fd,char* data )
{
	lssproto_CreateHeader( lssproto.work , "FT" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( data ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2005 :   <LI><a name="C2SSKUP"><font color=red>clienttoserver SKUP( int skillid );<br></font></a>
2006 :       (SKillUP)
2007 :       スキルアップボタンをおした。<br><br>
2008 :       <dl>
2009 : 	<dt>int skillid
2010 : 	<dd>どのスキルをあげるか。0=体力, 1=腕力, 2=丈夫さ, 3=素早さ
2011 :       </dl>
2012 :       <br>
2013 :       <hr>
2014 :       <br> 41个
2015 : 
*/

void lssproto_SKUP_send( int fd,int skillid )
{
	lssproto_CreateHeader( lssproto.work , "SKUP" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( skillid ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2016 :   <LI><a name="CS_KN"><font color=red>clienttoserver KN( int havepetindex, string data );<br></font></a>
2017 :       (inputKyoryuName)
2018 :       ペットの名前を入力した。<br><br>
2019 :       <dl>
2020 : 	<dt>int havepetindex
2021 : 	<dd>ペットの番号。
2022 : 	<dt>string data
2023 : 	<dd>自分のペットにオリジナルの名前を与える。   自己的宠物原创给名字。
2024 : 	
2025 :       </dl>
2026 :       この結果は、Txt関数を使って、文字列で通知される。これにともない、
2027 :       ペットのステータスが再び送られてくる。
2028 :       <br>
2029 :       <br>这样的结果，Txt函数，使用文字列，被告知。这和不，2027:宠物的地位，再次被送的。
2030 :       <hr>
2031 :       <br>
2032 : 
*/
//换名字宠物？
void lssproto_KN_send( int fd,int havepetindex,char* data )
{
	lssproto_CreateHeader( lssproto.work , "KN" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( havepetindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( data ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2141 :   <LI><a name="CS_WN"><font color=red>clienttoserver WN( int x, int y, int seqno, int objindex, int select, string data );<br></font></a>
2142 :       <br>
2143 :       ウィンドウ表示に対する返答<br>
2144 :       <br>
2145 :       <dl>
2146 :         <dt>int x,y
2147 :         <dd>キャラクターのx,y座標
2148 :         <dt>int seqno
2149 :         <dd>サーバーから来たseqnoをそのまま返す  服务器来的seqno直接回报
2150 :         <dt>int objindex
2151 :         <dd>サーバーから来たobjindexをそのまま返す 服务器来的objindex直接回报
2152 :         <dt>int select  选择了哪个按钮，或者选择什么了吗？。服务器的WN的buttontype的define和同样的形式回报。
2153 :         <dd>どのボタンを選択した，または何を選択したか。サーバーのWN のbuttontype のdefineと同じ形式で返す。
2154 :         <dt>string data
2155 :         <dd>１行入力ウィンドウがあれば，その入力したデータ。
2156 :             選択ウィンドウの場合は，その選択した番号。（1から）
2157 :             また，ペット，仲間選択ウィンドウでは，ペットが１～５，仲間が６～１０となる。
2158 :             <br><br>?お店の場合?<br>
2159 :             	買うの時：選択した番号（１～？）と個数。<br>
2160 :             	売るの時：選択したアイテム欄の番号（１～？）|金額<br>
2161 :       		その他	：買う：1　売る：2　出る：3
2162 : 一行输入窗口的话，那个输入的数据。2156:选择窗口的场合，这个选择的号码。（1）2157:同时，宠物，同伴选择窗口中，宠物1～5，同伴6～10。2158< br /:“br / >?店的情况?< br / >2159:买的时候：选择的号码（1～？）和数。< br / >2160:卖的时候：选择的条款栏的号码（1～？）|金额< br / >2161:其他：买：1：2: 3卖出去
2163 :             <br><br><br>?ペットの技の場合?<br>
2164 : 		どの技をえらんだか？(選択番号)｜どのペットか？(選択番号)｜どのスロットか？(選択番号)｜値段<br>
2165 : 
2166 :       </dl>
2167 :       <br>
2168 :       <br>
2169 :       <hr>
2170 :       <br>NPC多重选择 发送
*/

void lssproto_WN_send( int fd,int x,int y,int seqno,int objindex,int select,char* data )
{
	lssproto_CreateHeader( lssproto.work , "WN" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( x ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( y ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( seqno ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( objindex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( select ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( data ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2190 :   <LI><a name="CS_CLIENTLOGIN"><font color=red>clienttoserver ClientLogin(string cdkey,string passwd);<br></font></a>
2191 :       クライアントがサーバーにログインするときに使う。つまりアカウント
2192 :       のログインと考えればよい。ゲーム内容にタッチする種類の関数は、す
2193 :       べてこのログインをしないと実行されないようになっている。この関数
2194 :       はただ単にクライアントの情報をゲームサーバにたくわえるためだけの
2195 :       物である。また、パスワード変更したら、再度この関数を呼ぶ事。
2196 :???嶹????????????????????????????嶹????????????????????????????獎??????????????????????簑????touch?????????????????????????????????????????????????????????????????????????楨筌??郢鈺羣??????????????????       
2197 :       <br><br>
2198 :       <dl>
2199 : 	<dt>string cdkey
2200 : 	<dd>CDKEY
2201 : 	<dt>string passwd
2202 : 	<dd>パスワード。この文字列はエスケープされない。  ??????篋??逸?
2203 :       </dl>
2204 :       <br>
2205 :       <hr>
2206 :       <br>
2207 :       
*/

void lssproto_ClientLogin_send( int fd,char* cdkey,char* passwd )
{
	lssproto_CreateHeader( lssproto.work , "ClientLogin" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( cdkey ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( passwd ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2221 :   <LI><a name="CS_CREATENEWCHAR"><font color=red>clienttoserver CreateNewChar(int dataplacenum, string charname,int imgno , int faceimgno, int vital, int str, int tgh, int dex, int earth, int water, int fire, int wind, int hometown);<br></font></a>
2222 :       新しいキャラクターを作る。
2223 :       <br><br>       
2224 :       <dl>
2225 :         <dt>int dataplacenum
2226 :         <dd>キャラの番号。これによってクライアントはキャラクターリストのどの場所にこのキャラを表示すればいいかを判断する。
2227 : 	<dt>string charname
2228 : 	<dd>キャラ名(空白と改行がない、プレーンな文字列)。
2229 : 	    この文字列はデリミタを使わないので、エスケープする必要はない。
2230 : 	<dt>int imgno
2231 : 	<dd>キャラクターの画像番号。
2232 : 	<dt>int faceimgno
2233 : 	<dd>顔の画像番号。名刺交換等に使用される。
2234 : 	<dt>int vital, str,tgh dex
2235 : 	<dd>各パラメータ。
2236 : 	<dt>int earth, water, fire, wind
2237 : 	<dd>各属性値
2238 : 	<dt>int hometown
2239 : 	<dd>出身地。0～の数値で渡される。
2240 : 	<dt>string option
2241 : 	</table>
2242 : 	さらに、ゲーム内容的に、キャラ作成時に指定できるパラメータの値
2243 : 	には限界がある。これは仕様書を参照すること。この文字列はエスケー
2244 : 	プしない。
2245 :       </dl>
2246 :       <br>
2247 :       <hr>
2248 :       <br>
2249 : 
2250 :       
*/

void lssproto_CreateNewChar_send( int fd,int dataplacenum,char* charname,int imgno,int faceimgno,int vital,int str,int tgh,int dex,int earth,int water,int fire,int wind,int hometown )
{
	lssproto_CreateHeader( lssproto.work , "CreateNewChar" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dataplacenum ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( charname ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( imgno ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( faceimgno ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( vital ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( str ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( tgh ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( dex ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( earth ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( water ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( fire ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( wind ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( hometown ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2275 :   <LI><a name="CS_CHARDELETE"><font color=red>clienttoserver CharDelete(string charname);<br></font></a>
2276 :       キャラを消す。
2277 :       <br><br>
2278 :       <dl>
2279 : 	<dt>string charname
2280 : 	<dd>キャラ名。デリミタをふくまないのでエスケープしない。
2281 :       </dl>
2282 :       <br>
2283 :       <br>
2284 :       <hr>
2285 :       <br>
*/

void lssproto_CharDelete_send( int fd,char* charname )
{
	lssproto_CreateHeader( lssproto.work , "CharDelete" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( charname ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2303 :   <LI><a name="CS_CHARLOGIN"><font color=red>clienttoserver CharLogin(string charname);<br></font></a>
2304 :       キャラクターがサーバーにログインするときに使う。
2305 :       <br><br>
2306 :       <dl>
2307 : 	<dt>string charname
2308 : 	<dd>キャラ名。デリミタをふくまないので、エスケープしない。
2309 :       </dl>
2310 :       <br>
2311 :       <hr>
2312 :       <br>
*/

void lssproto_CharLogin_send( int fd,char* charname )
{
	lssproto_CreateHeader( lssproto.work , "CharLogin" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( charname ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2328 :   <LI><a name="CS_CHARLIST"><font color=red>clienttoserver CharList( void );<br></font></a>
2329 :       キャラリストを得る。
2330 :       <br><br>
2331 :       <hr>
2332 :       <br>
*/

void lssproto_CharList_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "CharList" );
	lssproto_strcatsafe( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2375 :   <LI><a name="CS_CHARLOGOUT"><font color=red>clienttoserver CharLogout(void);<br></font></a>
2376 :       キャラクターがサーバーからログアウトするときに使う。 角色服务器退出时使用
2377 :       引数なし。参数
2378 :       <br>
2379 :       <hr>
2380 :       <br>
2381 :       
*/

void lssproto_CharLogout_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "CharLogout" );
	lssproto_strcatsafe( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2396 :   <LI><a name="CS_PROCGET"><font color=red>clienttoserver ProcGet(void);<br></font></a>
2397 :       proc データを取得する。これによってログイン人数を調べたりする事が出来る。
2398 :       <br>
2399 :       <hr>取得数据。根据这个注册人数可以查。
2400 :       <br> 51个
2401 :       
*/

void lssproto_ProcGet_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "ProcGet" );
	lssproto_strcatsafe( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2414 :   <LI><a name="CS_PLAYERNUMGET"><font color=red>clienttoserver PlayerNumGet(void);<br></font></a>
2415 :       プレイヤーとログイン人数を取得する。
2416 :       <br>
2417 :       <hr>
2418 :       <br>
2419 :       52
*/

void lssproto_PlayerNumGet_send( int fd )
{
	lssproto_CreateHeader( lssproto.work , "PlayerNumGet" );
	lssproto_strcatsafe( lssproto.work , "" ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2432 :   <LI><a name="CS_ECHO"><font color=red>clienttoserver Echo( string test);<br></font></a>
2433 :       サーバーが生きているかをテストしたり、テスト用に使う。
2434 :       <br><br>服务器活着测试，测试或用。 就是握手
2435 :       <dl>
2436 : 	<dt>string test
2437 : 	<dd>エコー用文字列。エスケープしない。
2438 :       </dl>
2439 :       <br>
2440 :       <hr>
2441 :       <br>
2442 :    53  就是握手 
*/

void lssproto_Echo_send( int fd,char* test )
{
	lssproto_CreateHeader( lssproto.work , "Echo" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( test ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}
/*
2454 :   <LI><a name="CS_SHUTDOWN"><font color=red>clienttoserver Shutdown( string passwd, int min);<br></font></a>
2455 :       シャットダウン処理開始送信<br><br>
2456 :       メンテナンスを行いたい時に，自動的に「あとｎ分でメンテナンスを行います」のメッセージを１分毎に
2457 :       流してくれる。最初にdenyaccept()を行うので，ユーザーはログイン出来なくなり，
2458 :       最後にcloseallsockets()を行ってキャラのセーブと強制ログアウトを行う。
2459 :       <br><br>
2460 :       <dl>
2461 : 	<dt>string passwd
2462 : 	<dd>やばいプロトコルの為，一応パスワードを設定しておく。
2463 : 	<dt>int min
2464 : 	<dd>「あとｎ分でメンテナンスを行います」のｎ分を設定する。
2465 :       </dl>
2466 :       <br>
2467 :       <hr>
2468 :       <br> 55
2469 : 
*/

void lssproto_Shutdown_send( int fd,char* passwd,int min )
{
	lssproto_CreateHeader( lssproto.work , "Shutdown" );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_string( passwd ) ,lssproto.workbufsize );
	lssproto_strcatsafe( lssproto.work , lssproto_mkstr_int( min ) ,lssproto.workbufsize );
	lssproto_Send( fd , lssproto.work );
}

int lssproto_ClientDispatchMessage(int fd ,char*line)
{
	int msgid;
	char funcname[1024];
	lssproto_strcpysafe( lssproto.work , line,lssproto.workbufsize );
	lssproto_splitString( lssproto.work);
	lssproto_GetMessageInfo( &msgid , funcname , sizeof(funcname),lssproto.token_list);
/*
241 :   <LI><a name="SC_EV"><font color=blue>servertoclient EV(int seqno,int result);<br></font></a>
242 : 	クライアントのEVに対しての返答を行う。
243 : 	これを受け取るまでクライアントは再び歩いたりすることが
244 : 	出来ない。
245 :       对客户的EV的进行回答。243:这到收到客户再次走路的事244:不能。
246 :       <dl>
247 : 	<dt>int seqno
248 : 	<dd>クライアントEVのseqnoに対応する。  客户EV的seqno对应。
249 : 	<dt>int result
250 : 	<dd>結果。0:イベント実行不可。 1:成功。この後に色んなイベントに対する通信テキストが投げられてくるはず。
251 :       </dL>	
252 :       <br>
253 :       <hr>
254 :       <br>
*/

	if( strcmp( funcname , "EV" ) == 0 ){
		int seqno;
		int result;
		seqno = lssproto_demkstr_int( lssproto.token_list[2] );
		result = lssproto_demkstr_int( lssproto.token_list[3] );
		lssproto_EV_recv( fd,seqno,result);
		return 0;
	}
/*
279 :   <LI><a name="SC_EN"><font color=blue>servertoclient EN(int result,int field);<br></font></a>
280 : 	クライアントのEN又はDUに対しての返答を行う。または，仲間歩き時に親がエンカウントした場合に送られてくる。
281 :   客户的EN或者DU对进行的回答。或，朋友走的时候父母遇敌时被送的。    
282 :       <dl>
283 : 	<dt>int result
284 : 	<dd>結果。0:エンカウント不可orエラー。 1:敵との戦闘ＯＫ。2:対人との戦闘ＯＫ。この後は戦闘に関する通信	テキストが投げられてくるはず。
285 : 	<dt>int field
286 : 	<dd>バトルフィールド番号。 战地号码
287 :      </dL>
288 :       <br>
289 :       <hr>结果。0:遇不可或错误。一:和敌人战斗OK。2:人际战斗OK。这之后是战斗相关的通信文本扔出来。
290 :       <br>
291 : 
*/

	if( strcmp( funcname , "EN" ) == 0 ){
		int result;
		int field;
		result = lssproto_demkstr_int( lssproto.token_list[2] );
		field = lssproto_demkstr_int( lssproto.token_list[3] );
		lssproto_EN_recv( fd,result,field);
		return 0;
	}
/*
336 :   <LI><a name="SC_RS"><font color=blue>servertoclient RS( string data );<br></font></a>
337 : 	戦闘終了後、取得したＥＸＰ及びアイテムを通知。<br>
338 :         RS|番号(10進)|レベルアップフラグ(10進)|EXP(62進),番号|レベルアップフラグ|EXP,???,アイテム名|アイテム名|アイテム名<br><br>
339 : 　　　　番号～経験値は5人分。一人分の区切りは","。最初の一人目には必ずプレイヤーが来る。そのときの番号は-2。<br>
340 : 	2人目以降4人目まではペットの番号（0～4)ペットが経験値をもらわなかった場合は項目が無くなり、次の","が来る。<br>
341 : 	最後にアイテムの名前が"|"で区切られてくる。最大３個。無かった場合はすぐに次の"|"が来る。
342 : 
*/
   //好像战斗结束后的技能什么的返回数据
	if( strcmp( funcname , "RS" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_RS_recv( fd,data);
		return 0;
	}
/*
343 :   <LI><a name="SC_RD"><font color=blue>servertoclient RD( string data );<br></font></a>
344 : 	ＤＵＥＬ終了後、取得又は失ったＤＵＥＬポイントを通知。<br>
345 :         RD|得た(失った)DP(62進)|最終的なDP(62進)|<br><br>
346 : 
347 : 
*/
    //好像战斗结束后的血液什么的返回数据
	if( strcmp( funcname , "RD" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_RD_recv( fd,data);
		return 0;
	}
/*
371 :   <LI><a name="SC_B"><font color=blue>servertoclient B( string command );<br></font></a>
372 : 戦闘時のコマンドを全て扱う。数値には全て 大文字16進を使う。<BR>
373 : ここで[相手番号]とは、バトルの中での管理番号となる。
374 : 手前プレイヤーは０～４、手前ペットは５～９、向こう側は１０～１９である。
375 : 	<br><br>
376 : <h4>
377 : ムービーデータ
378 : </h4>
379 : ムービーデータ部分は全てつながった文字列で処理される。文字のつなげ方は<BR>
380 : マクロ( BATTLESTR_ADD( 文字列 ) )を使ってつなげる。つながった文字はターン終了後にクライアントにまとめて転送する。<BR>
381 : ここではキーワード以外は全て数値データであり、数値以外は無視。また小文字英字も無視(コメントとして使用できる)<br>
382 : 
383 : <BR>
384 : ムービー時のフラグの意味：ビット処理する。<BR>
385 : ---０ビット目：空振り<BR>
386 : ---１ビット目：通常ヒット<BR>
387 : ---２ビット目：会心の一撃<BR>
388 : ---３ビット目：防御<BR>
389 : ---４ビット目：カウンター<BR>
390 : ---５ビット目：よけた<BR>
391 : ---６ビット目：アルティメットＫＯその１<BR>
392 : ---７ビット目：アルティメットＫＯその２<BR>
393 : ---８ビット目：ガードブレイク攻撃した<BR>
394 : ---９ビット目：守護者がいる<BR>
395 : <br>
396 : <ul>
397 : <li>BH|のとき、通常攻撃ムービー<br>
398 : BH|攻撃側番号|守備側番号|フラグ|ダメージ|守備側番号|フラグ|ダメージ|????と繰り返す。守備側番号がFFだったらこのコマンドの終了とみなす。<br>
399 : <br>
400 : 
401 : (通常攻撃例) B|BH|attack_0|defense_A|flag_2|damage_32|defense_B|flag_2|damage_32|FF|<BR>
402 : これは0番が(attack_0)A番(defense_A)に0x32ダメージを与え(damage_32)その後B番(defense_B)にも攻撃を行い、0x32ダメージ(damage_32)を与えたこととする。<Br><Br>
403 : 
404 : (カウンター時例) B|BH|attack_0|defense_A|flag_2|dagame_32|counter_0|flag_10|damage_16|FF|<BR>
405 : これは0番が(attack_0)がA番に攻撃し(defense_A)ダメージ0x32(damage_32)を与えその後、0番(counter_0)がカウンターで(flag_10)ダメージ0x16(damage_16)を食らった。ここでだれがカウンターを発動したかはその直前に攻撃されたものが選ばれる。<Br><br>
406 : 
407 : (守護者がいる例) B|BH|attack_0|defense_A|flag_202|dagame_32|gardian_B|FF|<BR>
408 : これは0番が(attack_0)がA番に攻撃し(defense_A)ダメージ0x32(damage_32)を与えたはずだが、B番が守護者になった為そちらにダメージがいった。<Br><Br>
409 : <Br>
410 : 
411 : <li>BT|のとき、敵捕獲ムービー<br>
412 : BT|攻撃側番号|守備側番号|フラグ|<br>
413 : フラグが１だったら捕獲成功。０だったら捕獲失敗。失敗時にどのあたりまで敵を
414 : 引っ張ってくるかはクライアントがランダムで判定する。<br><br>
415 : 
416 : <li>BE|のとき、逃亡ムービー<br>
417 : BE|逃げる側番号|守備側番号|フラグ|<br>
418 : フラグが１だったら捕獲成功。０だったら捕獲失敗。失敗時にどのあたりまで敵を
419 : 引っ張ってくるかはクライアントがランダムで判定する。プレイヤーキャラが逃げた
420 : 場合はペット用のBEは無しでペットも逃げるようにする。<br>
421 : <br>
422 : 
423 : <li>bg|のとき、ガードムービー<br>
424 : bg|ガードする側番号|<br>
425 : これは小文字で送ること。キャラクタの動きは無い。クライアントも別にこの文字を処理せず、コメント扱い。そのための小文字。
426 : だがわかりにくいため一応送っておく。<br>
427 : <br>
428 : 
429 : <li>BD|のとき、ＨＰ、ＭＰ変更ムービー<br>
430 : BD|変更されるキャラ番号|変更の種類|プラスかマイナスか|増減値|<br>
431 : 変更の種類は０がＨＰ。１がＭＰ。プラスかマイナスかは１がプラス０がマイナス<br>
432 : <br>
433 : 
434 : <li>BF|のとき、ペットが後ろに隠れるムービー<br>
435 : BF|隠れるキャラ番号|<br>
436 : ペットのみが使用できて、クライアントではペットが背後に走っていっていなくなる
437 : 演出がある。地球一週等で使用<br>
438 : <br>
439 : 
440 : <li>BJ|のとき、呪術、アイテム演出ムービー<br>
441 : BJ|呪術使用キャラ番号|呪術使用側エフェクト番号|呪術受け側エフェクト番号|受けるキャラ番号|受けるキャラ番号|???|FF|<br>
442 : アイテム、呪術を使用した場合に使う。影響を受けるキャラ番号は連続して記述できるが最後はFFでしめる。
443 : <br>
444 : <br>
445 : 
446 : <li>BB|のとき、飛び道具攻撃ムービー<br>
447 : BB|攻撃側番号|飛び道具の種類|守備側番号|フラグ|ダメージ|守備側番号|???|FF|<BR>
448 : ２番目の引数に飛び道具の種類が入る以外は内容は通常攻撃と同じ。<br>
449 : 飛び道具の種類は０なら弓矢。１なら命中時に跳ねかえる武器。２なら命中時に破裂する武器。<br>
450 : 
451 : <br>
452 : <li>BM|のとき、ステータス異常変化ムービー<br>
453 : BM|ステータス変化するキャラ番号|どのステータス異常か|<br>
454 : ステータス異常番号は<br>
455 : ---０：ステータス異常無し<br>
456 : ---１：毒<br>
457 : ---２：麻痺<br>
458 : ---３：眠り<br>
459 : ---４：石化<br>
460 : ---５：酔っ払い<br>
461 : ---６：混乱<br>
462 : <br>
463 : 
464 : <li>BO|のとき、ブーメランムービー<br>
465 : BO|攻撃側番号|守備側番号|フラグ|ダメージ|守備側番号|???|FF|<BR>
466 : ２番目の引数に飛び道具の種類が入る以外は内容は通常攻撃と同じ。<br>
467 : <br>
468 : 
469 : <li>BS|のとき、ペット出し入れムービー<br>
470 : BS|ペットの番号[0～19](%X)|フラグ|画像番号|レベル|ＨＰ|名前|<BR>
471 : フラグ０の時はペットが戻るとき。その際はそれ以降後ろの文字は無視される<br>
472 : フラグ１の時はペットが出るとき。ペットの交換をしたい場合は戻してから出すというように２回BSコマンドを送ること。<br>
473 : 
474 : <li>BU|のとき、戦闘から抜けるムービー<br>
475 : 引数無し。このコマンドはクライアントがネット等の状態で異常に遅延し、サーバーが待ちきれなくなった場合にそのクライアントを強引に戦闘から抜けさせる処理を行った後に送信される。<br>
476 : <br>
477 : 
478 : <li>BV|のとき、フィールド属性変更ムービー<br>
479 : BV|変更をかけたキャラ番号|変更する属性番号|<br>
480 : 変更する属性番号は、<br>
481 : ---０：無属性<br>
482 : ---１：地属性<br>
483 : ---２：水属性<br>
484 : ---３：火属性<br>
485 : ---４：風属性<br>
486 : <br>
487 : 
488 : <li>BY|のとき、合体攻撃ムービー<br>
489 : BY|守備側番号|攻撃側番号|フラグ|ダメージ|攻撃側番号|フラグ|ダメージ|????と繰り返す。通常攻撃と攻守が入れ替わった形。<br>
490 : <br>
491 : </ul>
492 : 
493 : <h4>
494 : 毎ターン最初に送られるデータ
495 : </h4>
496 : <ul>
497 : <li>BC|のとき、キャラクターデータ。これは毎ターン全員に同じものが送られる。<br>
498 : BC|番号[0～19](%X)|キャラ名(%s)|画像番号(%X)|レベル(%X)|HP(%X)|フラグ(%X)|???<BR><BR>
499 : フラグの意味：ビット処理する。<BR>
500 : ---０ビット目：新規参加<BR>
501 : ---１ビット目：死亡<BR>
502 : ---２ビット目：プレイヤーかどうか<BR>
503 : ---３ビット目：毒<BR>
504 : ---４ビット目：麻痺<BR>
505 : ---５ビット目：眠り<BR>
506 : ---６ビット目：石化<BR>
507 : ---７ビット目：酔っ払い<BR>
508 : ---８ビット目：混乱<BR>
509 : <br>
510 : <li>BP|のとき、自分の個人データ。これは毎ターン本人にのみ送られる。<br>
511 : BP|自分の番号[0～19](%X)|フラグ(%X)|現在の気力(%X)|???<BR><BR>
512 : フラグの意味：ビット処理する。<BR>
513 : ---０ビット目：自分が新規参加<BR>
514 : ---１ビット目：自分が不意打ちを食らっている<BR>
515 : ---２ビット目：ブーメランを持っているか<BR>
516 : </ul>
517 : <h4>
518 : 毎ターンコマンド決定までのカウントダウン中に送られるデータ
519 : </h4>
520 : <ul>
521 : <li>BA|のとき、コマンド完了＆ターン数データ。これは誰かがコマンドをサーバーに送るたびに、全員に送られる。<br>
522 : BA|コマンド完了ビット(%X)|ターン数(%X)|
523 : <BR><BR>
524 : コマンド完了ビットはビット単位で構成し、０番目のキャラがコマンドを完了したら、
525 : ０ビット目がＯＮになる。クライアントはビットがＯＮになっているキャラクタは
526 : アニメーションさせ、ＯＦＦのキャラはアニメーションを停止させ、コマンドの完了したかどうかを区別する。<BR>
527 : ターン数はムービーが全く開始されていない場合は０ターン目とし、ムービー
528 : が開始された瞬間にインクリメントされる。
529 : </ul>
530 :       <br>
531 :       <br>
532 :       <hr>
533 :       <br>
534 : 
535 : 
*/
    //战斗时的命令全部处理。数值全部使用大写16进制。

	if( strcmp( funcname , "B" ) == 0 ){
		char* command;
		command = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_B_recv( fd,command);
		return 0;
	}
/*
716 :   <LI><a name="SC_I"><font color=blue>servertoclient I( string data );<br></font></a>
717 :       (item)<br>
718 :       <br>
719 :       アイテム情報。S I は全アイテムを送信するのに対してこれは指定されたアイテム番号のアイテムを
720 :       複数個送信することが出来る。
721 :       
722 :       <dl>
723 :         <dt>string data
724 :         <dd>アイテム情報。
725 :             アイテムの内容は以下の方法でパックする。
726 : 	    ひとつのアイテムは、かならずトークンのセットで
727 : 	    送信される。ほとんど，S Iのプロトコルと同じだが，先頭のトークンにアイテムの場所が
728 : 	    付いています。これによって何番目のアイテムかを判断する。<br>
729 :         </dl>
730 :       <br>
731 :       <hr>
732 :       <br>
733 :       <br>
734 : 
*/

	//物品 没发送
	if( strcmp( funcname , "I" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_I_recv( fd,data);
		return 0;
	}
/*
757 :   <LI><a name="SI"><font color=blue>servertoclient SI( int fromindex, int toindex );</font><br>
758 : 	  (SwapItem)
759 : 	  アイテムをいれかえる。みかけ上の意味はMIと同じだが、
760 : 	  サーバーからクライアントへの通知であるということと、
761 : 	  使用する目的が異なる。SIは、サーバが勝手にアイテムの
762 : 	  状況を変化させたとき(典型的には、装備を変更したとき)に、
763 : 	  どのアイテムをどこに移動させたのかをクライアントに通知するために
764 : 	  使用する。この目的は、クライアントでアクションボタンをつかって
765 : 	  装備を変更することを可能にするためである。
766 : 	  装備を変更したらそのアイテムがどこにいったのかがわからないと
767 : 	  どのアイテムをアクションボタンに残せばいいのかがわからないからである。
768 : 	  サーバーは、装備を変更したときに、
769 : 	  何番から何番に変更したのかをクライアントに通知する。
770 : 
771 :       <dl>
772 : 	<dt>int fromindex
773 : 	<dd>移動するアイテムのインデクス。
774 : 	<dt>int toindex
775 : 	<dd>目的地のアイテムインデクス。
776 :       </dl>
777 :       目的地の意味は、MIと同じである。
778 :       <br>
779 :       <hr>
780 :       <br>
781 : 	  
*/

	//物品交易？ 没有发送给服务端 应该是服务器发送的物品？
	if( strcmp( funcname , "SI" ) == 0 ){
		int fromindex;
		int toindex;
		fromindex = lssproto_demkstr_int( lssproto.token_list[2] );
		toindex = lssproto_demkstr_int( lssproto.token_list[3] );
		lssproto_SI_recv( fd,fromindex,toindex);
		return 0;
	}
/*
800 :   <LI><a name="SC_MSG"><font color=blue>servertoclient MSG( int aindex , string text , int color );</font></a><br>
801 :       (MeSsaGe)
802 :       サーバからクライアントにたいして、アドレスブックによるメッセージ
803 :       の送信をおこなう。通常のチャットメッセージとことなり、受信フラグ
804 :       やキューイングなどの処理があるためTとは別のプロトコルとしている。
805 :       <dl>
806 : 	<dt>int aindex
807 : 	<dd>アドレスブックのインデクス。このインデクスで特定される相手
808 : 	    からのメッセージであることをしめす。
809 : 	<dt>string text
810 : 	<dd>メッセージの内容。エスケープする必要はない。メッセージの内容は，mm/dd hh:mm|text...|graphicsno|lv|name|itemgraphicsno
811 : 	    となっていて最初のトークンは日付ヘッダが入っている。次は実際のメッセージの内容。次は画像番号。
812 : 	    通常メールは-1。この場合はこれ以降のトークンは存在しない。（例）mm/dd hh:mm|text|-1
813 : 	    ペットメールの場合は，運んできたペットの画像番号が格納されている。
814 : 	    lvはメールを運んできたペットのレベル。次は名前が格納されている。これはエスケープされている。
815 : 	    itemgraphicsno は運んできたアイテムの画像番号が格納されている。
816 : 	<dt>int color
817 : 	<dd>色。
818 : 	<dt>
819 :       </dl>
820 :       <br>
821 :       <hr>
822 :       <br>
823 :   
*/

	if( strcmp( funcname , "MSG" ) == 0 ){
		int aindex;
		char* text;
		int color;
		aindex = lssproto_demkstr_int( lssproto.token_list[2] );
		text = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		color = lssproto_demkstr_int( lssproto.token_list[4] );
		lssproto_MSG_recv( fd,aindex,text,color);
		return 0;
	}
/*
845 :   <LI><a name="SC_PME"><font color=blue>servertoclient PME( int objindex, int graphicsno, int x, int y, int dir, int flg, int no, string cdata );<br></font></a>
846 :       (PetMeSsaEffect)
847 :       ペットメールでの，ペットの登場を表現する。<br><br>
848 :       <dl>
849 : 	<dt>int objindex
850 : 	<dd>ペットのオブジェクト番号。
851 : 	<dt>int graphicsno
852 : 	<dd>登場するペットの画像番号
853 : 	<dt>int x,y
854 : 	<dd>どの位置に出現するか
855 : 	<dt>int dir;
856 : 	<dd>どの方向に向かっていくか。
857 : 	<dt>int flg
858 : 	<dd>行きしなか(0)，帰りしなか(1)。
859 : 	<dt>int no
860 : 	<dd>演出番号。
861 : 	<dt>string cdata
862 : 	<dd>C のデータが入っている。帰ってくる時のような演出の時には，そのままオブジェクトとなるので
863 : 	    これが必要。これを元にオブジェクトを作成する。このプロトコルが来た場合，
864 : 	    このキャラクタのC はこないものとする。
865 :       </dl>
866 :       <br>
867 :       <br>
868 :       <hr>
869 :       <br>
870 : 用邮件的宠物，宠物登场表现。	  
*/
      //用邮件的宠物，宠物登场表现。服务端发送 无接受	  
	if( strcmp( funcname , "PME" ) == 0 ){
		int objindex;
		int graphicsno;
		int x;
		int y;
		int dir;
		int flg;
		int no;
		char* cdata;
		objindex = lssproto_demkstr_int( lssproto.token_list[2] );
		graphicsno = lssproto_demkstr_int( lssproto.token_list[3] );
		x = lssproto_demkstr_int( lssproto.token_list[4] );
		y = lssproto_demkstr_int( lssproto.token_list[5] );
		dir = lssproto_demkstr_int( lssproto.token_list[6] );
		flg = lssproto_demkstr_int( lssproto.token_list[7] );
		no = lssproto_demkstr_int( lssproto.token_list[8] );
		cdata = lssproto_wrapStringAddr( lssproto_stringwrapper[8] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[9] ));
		lssproto_PME_recv( fd,objindex,graphicsno,x,y,dir,flg,no,cdata);
		return 0;
	}
/*
876 :   <LI><a name="SC_AB"><font color=blue>servertoclient AB( string data );</font></a><br>
877 :       サーバーがクライアントに送信するアドレスブックの全内容。
878 :       <dl>服务器客户端发送地址簿的全部内容。
879 : 	<dt>string data
880 : 	<dd>情報の内容。表示するために必要な情報は、有効か無効かのフラグ，相手のキャラの名前、
881 : 	    レベル、デュエルポイント，オンラインかどうかのフラグ。
882 : 	    それが人数分ならんでいる。
883 : 	    データはその時点での最新である。
884 : 	    <code>一人目|二人目|...</code>人数に制限はない。
885 : 	    
886 : 	    一人分のメッセージは以下の構成である。
887 : 	    <br>
888 : 	    <code>使用フラグ|名前(文字列)|レベル数値|duelpoint|フラグ|画像番号
889 : 	    </code>
890 : 	    <br>
891 : 	    したがって、 N人目の名前にアクセスするには 4(N-1)+1番目の
892 : 	    トークンを、しらべればよいことになる。そのトークンが存在し
893 : 	    ないならば、それ以降のエントリは存在しない。名前文字列は<a
894 : 	    href="#escaping">エスケープ</a>してからたて棒でつなぐ。
895 : 	    フラグは0ならオフライン、1以上ならオンラインである。
896 : 	    この値はどのサーバの人がオンラインかを見分ける為に，
897 : 	    setup.cfにて servernumber=n のnの値が送信される。
898 : 	    特別な機能として、名前のトークンが空の場合は、以前のAB
899 : 	    コマンドで受信した名前の情報を更新せずにレベルとライフの
900 : 	    項目だけを更新する。というのは名前の情報はキャラ作成後
901 : 	    2度と更新されないからだ。
902 : 		
903 :       </dl>
904 :       <br>
905 :       <hr>
906 :       <br>
*/

	if( strcmp( funcname , "AB" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_AB_recv( fd,data);
		return 0;
	}
/*
907 :   <LI><a name="SC_ABI"><font color=blue>servertoclient ABI( int num, string data );</font></a><br>
908 : 	ABの単体版。num には自分のアドレスブックの何番目かと言う事が入っている。
909 : 	dataの中身はABに準ずる。<br>
910 :       <br>
911 :       <br>
912 :       <hr>
913 :       <br>AB的单体版。num自己的地址簿的第几话进入着。data的内容是AB准
914 : 
*/

	if( strcmp( funcname , "ABI" ) == 0 ){
		int num;
		char* data;
		num = lssproto_demkstr_int( lssproto.token_list[2] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_ABI_recv( fd,num,data);
		return 0;
	}
/*
1005 :     <LI><a name="S2CTK"><font color=blue>servertoclient TK( int index,string message ,int color);<br></font></a>
1006 :       <dl>
1007 : 	<dt>index
1008 : 	<dd>しゃべったキャラのインデックス。-1 の時は、ふきだしの必
1009 : 	    要のないシステムメッセージ<br>
1010 : 	<dt>string
1011 : 	<dd>string の内容は, "|" で2つに区切られている。ひとつめのトー
1012 : 	    クンの文字によって2つめのトークンの内容が変更される。2つめ
1013 : 	    のトークンは<a href="#escaping">エスケープ</a>されている。
1014 : 	    ひとつめのトークンは以下のパターンがある。
1015 : 	    <ul>
1016 : 	      <li>"P" の時。<br>
1017 : 		  チャットメッセージ。自分のメッセージも含まれる。とり
1018 : 		  あえずローカルエコーはなし。例は以下のとおり
1019 : 		  <pre>
1020 : 		  "P|あほあほあ"
1021 : 		  </pre>
1022 : 	      <li>"F"の時。<br>
1023 : 	          <pre>
1024 : 	          F|charaindex|メッセージ
1025 : 	          </pre>
1026 : 	          吹き出し文字のメッセージ。指定されたcharaindexに吹き出しが出て
1027 : 	          メッセージを表示する。<br>
1028 : 	          
1029 : 	    </ul>
1030 : 	<dt>color
1031 : 	<dd>キャプション色。色の定義は?
1032 :       </dl>
1033 :       <br>
1034 :       <hr>
1035 :       <br>
1036 : //讲话发送
*/

	if( strcmp( funcname , "TK" ) == 0 ){
		int index;
		char* message;
		int color;
		index = lssproto_demkstr_int( lssproto.token_list[2] );
		message = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		color = lssproto_demkstr_int( lssproto.token_list[4] );
		lssproto_TK_recv( fd,index,message,color);
		return 0;
	}
/*
1037 :   <LI><a name="SC_MC"><font color=blue>servertoclient MC(int fl , int x1 , int y1 , int x2, int y2, int tilesum, int objsum, int eventsum,string data );<br></font></a>
1038 :       マップを送る前にマップのチェックサムを送る。クライアントは自分のローカルのマップデータのチェックサムを計算し，
1039 :       これと比べて合っていなかったらMを送信する。合っている場合はMを送らずにクライアントのマップデータをそのまま使う。
1040 :       <dl>
1041 : 	<dt>int fl
1042 : 	<dd>フロア番号
1043 : 	<dt>int x1
1044 : 	<dd>左上X
1045 : 	<dt>int y1
1046 : 	<dd>左上Y
1047 : 	<dt>int x2
1048 : 	<dd>右下X
1049 : 	<dt>int y2
1050 : 	<dd>右下Y
1051 : 	<dt>int timesum
1052 : 	<dd>タイルのチェックサム
1053 : 	<dt>int objsum
1054 : 	<dd>オブジェクトのチェックサム
1055 : 	<dt>int eventsum
1056 : 	<dd>イベントオブジェクトのチェックサム。イベントオブジェクトはもしも重なっていたら１番上（マップリンクデータ上で）のものが使用される。
1057 : 	<dt>string data
1058 : 	<dd>フロアの名前文字列（クライアントがMを送らなかった時に使用する）
1059 : 	
1060 :       </dl>
1061 :       <br>
1062 :       <hr>  根据服务端发送的走路封包 进行图档移动变化 是走路封包
1063 :       <br>地图送前地图的检查和送。客户自己的本地的地图数据的检查和计算，
1064 :       与这个比没有互相的话M发送。符合场合M不送客户直接用的地图数据。  服务端不接受
*/

	if( strcmp( funcname , "MC" ) == 0 ){
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;
		int tilesum;
		int objsum;
		int eventsum;
		char* data;
		fl = lssproto_demkstr_int( lssproto.token_list[2] );
		x1 = lssproto_demkstr_int( lssproto.token_list[3] );
		y1 = lssproto_demkstr_int( lssproto.token_list[4] );
		x2 = lssproto_demkstr_int( lssproto.token_list[5] );
		y2 = lssproto_demkstr_int( lssproto.token_list[6] );
		tilesum = lssproto_demkstr_int( lssproto.token_list[7] );
		objsum = lssproto_demkstr_int( lssproto.token_list[8] );
		eventsum = lssproto_demkstr_int( lssproto.token_list[9] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[9] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[10] ));
		lssproto_MC_recv( fd,fl,x1,y1,x2,y2,tilesum,objsum,eventsum,data);
		return 0;
	}
/*
1085 :   <LI><a name="SC_M"><font color=blue>servertoclient M(int fl , int x1 , int y1 , int x2, int y2 ,string data );<br></font></a>
1086 :       マップを送信する。どんな矩形でも送信できる。もちろん最大サイズ
1087 :       はきまっていてよい。これはサーバーからのみ送信し、クライアント
1088 :       が要求することはない。送信のタイミングは、キャラにとってあたら
1089 :       しい部分が見えるようになった瞬間や、地形が変更された瞬間である。
1090 :       <br><br>
1091 :       <dl>
1092 : 	<dt>int fl
1093 : 	<dd>キャラのいるフロア番号
1094 : 	<dt>int x1
1095 : 	<dd>フロアマップの中の絶対位置。左上X
1096 : 	<dt>int y1
1097 : 	<dd>フロアマップの中の絶対位置。左上Y
1098 : 	<dt>int x2
1099 : 	<dd>フロアマップの中の絶対位置。右下X
1100 : 	<dt>int y2
1101 : 	<dd>フロアマップの中の絶対位置。右下Y
1102 : 	<dt>string data
1103 : 	<dd>フロアの見える文字列|タイル|オブジェクト|イベントオブジェクト
1104 : 	    になっている。
1105 : 	    dataはマップタイル番号で"76,76,77,78,98,90,1,1,1,2"という
1106 : 	    ように必要な要素数並んでいる。スペースでくぎるとエスケープ
1107 : 	    の関係上、量がおおくなるので、コンマでくぎる。階段などに進
1108 : 	    んだ場合、全画面分を送信して、一歩あるく場合は一歩分だけお
1109 : 	    くる。こういう判断はサーバーでやるしかない。クライアントは
1110 : 	    この関数でマップをうけとると、それをディスクに保存して、オー
1111 : 	    トマップ用の情報をたくえる。この文字列は<a
1112 : 	    href="#escaping">エスケープ</a>する必要がある。
1113 :       	    イベントオブジェクトはもしも重なっていたら１番上（マップリンクデータ上で）のものが使用される。
1114 :       </dl>
1115 :       <br>
1116 :       <br>
1117 :       <hr>
1118 :       <br>
1119 : 
1120 : 
1121 : 
*/

	if( strcmp( funcname , "M" ) == 0 ){
		int fl;
		int x1;
		int y1;
		int x2;
		int y2;
		char* data;
		fl = lssproto_demkstr_int( lssproto.token_list[2] );
		x1 = lssproto_demkstr_int( lssproto.token_list[3] );
		y1 = lssproto_demkstr_int( lssproto.token_list[4] );
		x2 = lssproto_demkstr_int( lssproto.token_list[5] );
		y2 = lssproto_demkstr_int( lssproto.token_list[6] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[6] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[7] ));
		lssproto_M_recv( fd,fl,x1,y1,x2,y2,data);
		return 0;
	}
/*
1129 :   <LI><a name="S2CC"><font color=blue>servertoclient C( string data );<bR></font></a>
1130 :       (Characters)
1131 :       クライアントは CA が来て、そのキャラについて調なかった時は、 C 
1132 :       でこの関数を要求できる。サーバはチェックしない。
1133 :       <br><br>
1134 :       <dl>
1135 : 	<dt>string data
1136 : 	<dd>	  フォーマットは 以下の3種類の項目をコンマでならべた
1137 : 	    ものである.それぞれの項目の中身はさらにたて棒'|'でくぎられ
1138 : 	    ている。
1139 : 
1140 : 	    <ul>
1141 : 	      <li>たて棒でくぎられたトークンが12個の場合<br>
1142 : 		  トークンの内容は
1143 : 
1144 : 		  <code>WHICHTYPE|CHARINDEX|X|Y|DIR|BASEIMG|LEVEL|NAMECOLOR|NAME|SELFTITLE|WALKABLE|HEIGHT</code>
1145 : 		  このオブジェクトはキャラクターであるということである。<br>
1146 : 		  
1147 : 		  WHICHTYPE は，このキャラクターがどういった種類のもの
1148 : 		  であるか。これはサーバーでは以下のように定義されている。<br>
1149 : 		  <pre>
1150 : 		typedef enum
1151 : 		{
1152 : 		    CHAR_TYPENONE,          何でもない
1153 : 		    CHAR_TYPEPLAYER,        プレイヤー
1154 : 		    CHAR_TYPEENEMY,         敵
1155 : 		    CHAR_TYPEPET,           ペット
1156 : 		    CHAR_TYPEDOOR,          ドア
1157 : 		    CHAR_TYPEBOX ,          宝箱
1158 : 		    CHAR_TYPEMSG ,          看板
1159 : 		    CHAR_TYPEWARP ,         ワープゾーン
1160 : 		    CHAR_TYPESHOP ,         店
1161 : 		    CHAR_TYPEHEALER ,       ヒーラー
1162 : 		    CHAR_TYPEOLDMAN ,       長老
1163 : 		    CHAR_TYPEROOMADMIN,     不動産屋
1164 : 		    CHAR_TYPETOWNPEOPLE,    まちのひと
1165 : 		    CHAR_TYPEDENGON,        伝言版
1166 : 		    CHAR_TYPEADM,           伝言版
1167 : 		    CHAR_TYPETEMPLE,        Temple master
1168 : 		    CHAR_TYPESTORYTELLER,   語り部
1169 : 		    CHAR_TYPERANKING,       不動産ランキング表示ＮＰＣ
1170 : 		    CHAR_TYPEOTHERNPC,      その他の検索対象にならないNPC
1171 : 		    CHAR_TYPEPRINTPASSMAN,  ドアのパスワード表示するNPC
1172 : 		    CHAR_TYPENPCENEMY,      固定敵
1173 : 		    CHAR_TYPEACTION,        アクションに反応するNPC
1174 : 		    CHAR_TYPEWINDOWMAN,     ウィンドウ表示するNPC（テストかも)
1175 : 		    CHAR_TYPESAVEPOINT,     セーブポイント
1176 : 		    CHAR_TYPEWINDOWHEALER,  ウインドウタイプのヒーラー
1177 : 		    CHAR_TYPEITEMSHOP,	    お店
1178 : 		    CHAR_TYPESTONESHOP,	    石盤屋（ペットの技屋）
1179 : 		    CHAR_TYPEDUELRANKING,   DUELランキングNPC
1180 : 		    CHAR_TYPENUM,
1181 : 		}CHAR_TYPE;
1182 : 		  </pre>
1183 : 		  
1184 : 		  それは他のプレイヤーやNPCや敵である。サーバーが送信
1185 : 		  するときは見える範囲について全部送信する。つま
1186 : 		  りクライアントは、このパケットを受けとったときにこの
1187 : 		  パケットに書かれていないキャラを持っていたら消してし
1188 : 		  まってよいということである。また、マウスカーソルをあ
1189 : 		  わせたときに表示する情報はこの情報のみに基いている。
1190 : 		  だから、マウスカーソルをあわせたときに表示することが
1191 : 		  変更された場合は、サーバーは、この関数を能動的に呼び
1192 : 		  ださなければならない。 SELFTITLEについては、デリミタ
1193 : 		  であるたて棒をふくまないようにエスケープされている。
1194 : 		  クライアントはアクションのコマンド(CA)がくるまでは立
1195 : 		  ちで表示する。CHARINDEXサーバー内の一意にキャラを特
1196 : 		  定できる番号、BASEIMGは表示のための番号、LEVELはキャ
1197 : 		  ラのレベル(0なら表示しない。この値はNPCなどに使う。)
1198 : 		  WALKABLEは1のときその上を通過することができ、0なら通
1199 : 		  過することができない。HEIGHTは高さをもつものかそうで
1200 : 		  ないのかの指定。
1201 : 		  
1202 : 		  キャラクターの名前と自由称号は、<a href="#escaping">
1203 : 		  エスケープ</a>されなければならない。'|'でトークンを
1204 : 		  取りだしてからエスケープを解除する。エスケープすると'
1205 : 		  |'がほかの文字におきかわるので、最初は単純に'|'をデ
1206 : 		  リミタとしてよい。送信する方も、名前と自由称号をエス
1207 :                   ケープしてからたて棒でつないでから送信する。
1208 :                   また，ペットの場合は自由称号の代わりにユーザーが設定
1209 :                   したペットの名前が送信されてくる。<br>
1210 : 		  
1211 : 	      <Li>たて棒でくぎられたトークンが6個の場合<br>
1212 : 		  トークンの内容は
1213 : 		  <code>INDEX|X|Y|BASEIMG|LEVEL|ITEM1LINEINFO</code>
1214 : 		  で地面に落ちているアイテムについての情報である。
1215 : 		  INDEXはキャラのインデクッスとかぶらないINDEXである。
1216 : 		  そのアイテムを消す時に使用する。X,Yはアイテムのグロー
1217 : 		  バル位置。BASEIMGは画像の番号。ITEM1LINEINFOは1行
1218 : 		  infoに表示するための情報である。アイテムウインドウ内
1219 : 		  の表示用の情報は別の方法で用意する。アイテムに関して
1220 : 		  はCAは来ない。ITEM1LINEINFOは<a href="#escape">エスケー
1221 : 		  プ</a>される。このエスケープの方法は上の項目を参照。
1222 : 
1223 : 	      <Li>たて棒でくぎられたトークンが4個の場合<br>
1224 : 		  トークンの内容は
1225 : 		  <code>INDEX|X|Y|VALUE</code>
1226 : 		  で地面に落ちているお金についての情報である。内容はす
1227 : 		  べて数字。INDEX,X,Y はアイテムと同じ。VALUE はどれだ
1228 : 		  けの量かという事である。アイテムについての情報である。
1229 : 	      <li>たて棒でくぎられたトークンが1個の場合<br>
1230 : 		  <code>INDEX</code>
1231 : 		  このキャラのCは教えられない。
1232 : 	    </ul>
1233 :       </dl>
1234 :       <br>
1235 :       <br>
1236 :       <hr>
1237 :       <br> 每个坐标的数据？
1238 : 
1239 : 
1240 :       
*/

	if( strcmp( funcname , "C" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_C_recv( fd,data);
		return 0;
	}
/*
1241 :   <LI><a name="SC_CA"><font color=blue>servertoclient CA( string data );<br></font></a>
1242 :       (CharacterAction)
1243 :       見える範囲にいるキャラのアクション状態を更新する。
1244 :       サーバーからクライアントに一方的に送信する。
1245 :       各キャラの1アクションごとに送信。サーバーはアクションをできるだ
1246 :       け圧縮すること。<br><br>
1247 : 
1248 :       <dl>
1249 : 	<dt>string data
1250 : 	<dd>CHARINDEX|X|Y|ACTION|PARAM1|PARAM2|PARAM3|PARAM4|....をコ
1251 : 	    ンマでくぎったものにする。PARAMはアクションごとに個数も
1252 : 	    使いかたもことなる。以下はアクション一覧。X,Yは位置でど
1253 : 	    のアクションでも、あたらしい位置を指定する。
1254 : 	    この文字列はエスケープされない。
1255 : 	    <table border>
1256 : 	      <tr><td>ACTION</td><td>PARAM1</td><td>PARAM2</td>
1257 : 	      <td>PARAM3</td><td>PARAM4</td></tr>
1258 : 	      <tr><td>Stand:0</td><td>方向0~7</td><td></td><td></td><td></td></tr>
1259 : 	      <tr><td>Walk:1</td><td>方向0~7</td><td></td><td></td><td></td></tr>
1260 : 	      <tr><td>Attack:2</td><td>方向0~7</td><td></td><td></td><td></td></tr>
1261 : 	      <tr><td>Throw:3</td><td>方向0~7</td><td></td><td></td><td></td></tr>
1262 : 	      <tr><td>Damage:4</td><td>方向0~7</td><td></td><td></td></tr>
1263 : 	      <tr><td>Dead:5</td><td>方向0~7</td><td></td><td></td><td></td></tr>
1264 : 	      <tr><td>UseMagic:6</td><td>方向0~7</td></tr>
1265 : 	      <tr><td>UseItem:7</td><td>方向0~7</td></tr>
1266 : 	      <tr><td>Effect:8</td><td>方向0~7</td><td>エフェクト番号</td></tr>
1267 : 	      <tr><td>Down:10 (倒れる)</td><td>方向0~7</td></tr>
1268 : 	      <tr><td>Sit:11 (座る)</td><td>方向0~7</td></tr>
1269 : 	      <tr><td>Hand:12 (手を振る)</td><td>方向0~7</td></tr>
1270 : 	      <tr><td>Pleasure:13 (喜ぶ)</td><td>方向0~7</td></tr>
1271 : 	      <tr><td>Angry:14 (怒る)</td><td>方向0~7</td></tr>
1272 : 	      <tr><td>Sad:15 (悲しむ)</td><td>方向0~7</td></tr>
1273 : 	      <tr><td>Guard:16 (ガード)</td><td>方向0~7</td></tr>
1274 : 	      <tr><td>walk:17 (アクション用歩き)</td><td>方向0~7</td></tr>
1275 : 	      <tr><td>nod:18 (うなずく)</td><td>方向0~7</td></tr>
1276 : 	      <tr><td>Battle:20 (戦闘情報)</td><td>方向0~7</td><td>BattleNo(-1 なら表示消す）</td><td>SideNo</td><td>HelpNo（１なら助けを呼ぶCA表示，０なら消す，または無し）</td></tr>
1277 : 	      <tr><td>Leader:21 (リーダー情報)</td><td>方向0~7</td><td>0:表示消す 1:表示</td></tr>
1278 : 	      <tr><td>Watch:22 (戦闘観戦)</td><td>方向0~7</td><td>0:表示消す 1:表示</td></tr>
1279 : 	      <tr><td>Turn:30(方向変換)</td><td>方向0~7</td><td></td></tr>
1280 : 	      <tr><td>Warp:31(ワープ)</td><td>方向0~7</td><td></td></tr>
1281 : 	      
1282 : 	</table>
1283 : 	ACTIONの値は整数で、内容は上の表の左端の項目である。
1284 :   </dl>
1285 :   <br>
1286 :   <br>
1287 :       <hr>
1288 :       <br>
1289 : 
1290 : 
*/

	if( strcmp( funcname , "CA" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_CA_recv( fd,data);
		return 0;
	}
/*
1291 :   <LI><a name="CD"><font color=blue>servertoclient CD( string data );<bR></font></a>
1292 :       (CharacterDelete)
1293 : 	data はデリミタ（,）で区切られたインデックス。
1294 : 	複数件送る事が出来る。
1295 :       このIDを持っているキャラクタが消えた時にサーバからクライアントへ
1296 :       と伝わる。
1297 :       <br>
1298 :       <hr>
1299 :       <br>
1300 :   
*/

	if( strcmp( funcname , "CD" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_CD_recv( fd,data);
		return 0;
	}
/*
1301 :   <LI><a name="R"><font color=blue>servertoclient R( string data );<br></font></a>
1302 :       (Radar)
1303 :       レーダーの内容をクライアントに送信する。クライアントはこの送信を
1304 :       要求することはない。サーバーが適当なタイミングで送信する。
1305 :       たとえば10歩あるくごととか、1分ごととか。<br><br>
1306 : 
1307 :       <dl>
1308 : 	<dt>string data
1309 : 	<dd>x ,y, kind ,の順に値を'|'でくぎってならべたものを
1310 : 	    さらに'|'でならべたもの。
1311 : 	    x,yはキャラからの絶対位置。kindは以下にしめす数値ではない
1312 : 	    記号。レーダーにうつる物の種類を指定する。送られてくるもの
1313 : 	    の種類は、それまでに唱えた魔法とか、スキルで変化し、それは
1314 : 	    サーバーが考える。
1315 : 	    <table border>
1316 : 	      <tr><td>kindの値</td><td>内容</td></tr>
1317 : 	      <tr><td>E</td><td>敵</td></tr>
1318 : 	      <tr><td>P</td><td>プレイヤー</td></tr>
1319 : 	      <tr><td>S</td><td>魂</td></tr>
1320 : 	      <tr><td>G</td><td>お金</td></tr>
1321 : 	      <tr><td>I</td><td>アイテム(魂以外)</td></tr>
1322 : 	</table>
1323 :   </dl>
1324 :   dataの例：<code>"12|22|E|13|24|P|14|28|P"</code>
1325 :   この文字列はエスケープされない。
1326 :   <br>
1327 :   <br>
1328 :       <hr>
1329 :       <br>
1330 : 
*/

	if( strcmp( funcname , "R" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_R_recv( fd,data);
		return 0;
	}
/*
1355 :   <LI><a name="SC_S"><font color=blue>servertoclient S( string data );<br></font></a>
1356 :       (Status)
1357 :       キャラのステータスを送信する。
1358 :       データは カテゴリ記号文字(一文字)内容 となっている。つまり最初の
1359 :       1文字を見れば何のステータスか分る。内容は2文字目からである。
1360 :       内容は以下のフォーマットにしたがう。たて棒記号'|'がデリミタである。
1361 :       2個目以降のトークンが内容である。<br>
1362 :       <br>
1363 :       <UL>
1364 : 	<LI>P 全パラメータ<br><br>
1365 : 	    kubun hp maxhp mp maxmp str tough exp maxexp exp 
1366 : 	    level attackpower deffencepower 
1367 : 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
1368 : 	    gold 付けている
1369 : 	    称号のindex 名前 自己称号 <br>
1370 : 
1371 : 	    デリミタは '|' である。また名前と自己称号は、
1372 : 	    <a href="#escaping">エスケープ</a>したものを
1373 : 	    たて棒でつなげたものが通信される。<br>
1374 : 	    それぞれの値の型は以下。<br>
1375 : 	    kubun に１が入っていると前パラメータを送信，
1376 : 	    ２以上だと，各ビットの立っているもののパラメータが順番に入って送信されている。
1377 : 	   例えば，３が来たらhpとMaxhpがkubun 以降のトークンに納められている。<br>
1378 : 	    <table border>
1379 : 	      <tr><td>kunun</td><td>int</td></tr>
1380 : 	      <tr><td>Hp(体力)hp</td><td>int</td></tr>
1381 : 	      <tr><td>MaxHp</td><td>int</td></tr>
1382 : 	      <tr><td>Mp(呪術パワー)</td><td>int</td></tr>
1383 : 	      <tr><td>MaxMp</td><td>int</td></tr>
1384 : 	      <tr><td>Vital(体力)</td><td>int</td></tr>
1385 : 	      <tr><td>Str(腕力)</td><td>int</td></tr>
1386 : 	      <tr><td>Tough(丈夫さ)</td><td>int</td></tr>
1387 : 	      <tr><td>Dex(素早さ)</td><td>int</td></tr>
1388 : 	      <tr><td>Exp(経験値)exp</td><td>int</td></tr>
1389 : 	      <tr><td>MaxExp(次までの経験値)</td><td>int</td></tr>
1390 : 	      <tr><td>Level(レベル)</td><td>int</td></tr>
1391 : 	      <tr><td>Attack(攻撃力)</td><td>int</td></tr>
1392 :      	      <tr><td>Defense(守備力)</td><td>int</td></tr>
1393 : 	      <tr><td>fQuick(最終素早さ)</td><td>int</td></tr>
1394 : 	      <tr><td>fCharm(魅力)</td><td>int</td></tr>
1395 : 	      <tr><td>fLuck(運)</td><td>int</td></tr>
1396 : 	      <tr><td>fEarth(地)fixearthat</td><td>int</td></tr>
1397 : 	      <tr><td>fWater(水)</td><td>int</td></tr>
1398 : 	      <tr><td>fFire(火)</td><td>int</td></tr>
1399 : 	      <tr><td>fWid(風)fixwindat</td><td>int</td></tr>
1400 : 	      <tr><td>Gold(お金)</td><td>int</td></tr>
1401 : 	      <tr><td>称号のindex</td><td>int</td></tr>
1402 : 	      <tr><td>デュエルポイント</td><td>int</td></tr>
1403 : 	      <tr><td>名前</td><td>文字列</td></tr>
1404 : 	      <tr><td>自己称号</td><td>文字列</td></tr>
1405 : 	    </table>
1406 : 	    具体例( char の配列そのまま書く )<br>
1407 : 	    <code>
1408 : 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|へんぱ|abc|def
1409 : 	    </code>
1410 : 	<br><br>
1411 : 	<li>C 座標<br><br>
1412 : 	    floor maxx maxy x y<br>
1413 : 	    を  | で区切って送る。<br>
1414 : 	    具体例( char の配列そのまま書く )<br>
1415 : 	    <code>
1416 : 	    C1024|100|100|10|20
1417 : 	    </code>
1418 : 	    上記の例では、フロアID1024, サイズ100x100 X 10,Y 20のとこ
1419 : 	    ろにキャラクターがいるという内容になる。このマイキャラのフ
1420 : 	    ロアもふくめた座標をクライアントが知ることができるのはこの
1421 : 	    コマンドのみである。歩きの結果はフロア内の位置しかわからな
1422 : 	    い。よって、ゲーム開始時、階段使用のとき、ワープのときなど
1423 : 	    は、このコマンドをつかって正しい値を送信しなくてはならない。
1424 : 	    
1425 : 
1426 : 	<br><br>
1427 : 	<LI>I 全アイテム<br><br>
1428 : 	    (アイテム1)|(アイテム2)...(アイテムn)<br>
1429 : 	    アイテムの内容は以下の方法でパックする。
1430 : 	    ひとつのアイテムは、かならず以下の6つのトークンのセットで
1431 : 	    送信される。ひとつひとつの内容は、<br>
1432 : 	    <br>
1433 : 	    名前|名前2|ステージ|アイテムの説明|アニメ番号|使用可能場所|使用対象|アイテムレベル|フラグ類<br>
1434 : 	    <br>
1435 : 	    <ul>
1436 : 	      <li>アイテム名は識別レベルで自動的に変更される。
1437 : 	      <li>名前2は識別レベルによって、アイテムの能力が入る予定。
1438 : 		  クライアントのアイテム欄の2行目に入れる事。
1439 : 	      <li>ステージは名前の色を替るのに使う。
1440 : 	      <li>アイテムの説明。
1441 : 	      <li>アニメ番号は、画像番号。
1442 : 	      <li>使用可能場所とは，このアイテムがどこで使用する事が可能かが入っている。
1443 : 	          これは，サーバーでは以下のように定義されている。<br>
1444 : 	          <pre>
1445 : 			typedef enum
1446 : 			{
1447 : 				ITEM_FIELD_ALL,			すべての場所で使える
1448 : 				ITEM_FIELD_BATTLE,		戦闘中のみ
1449 : 				ITEM_FIELD_MAP,			通常マップ上のみ
1450 : 
1451 : 			}ITEM_FIELDTYPE;
1452 : 	          </pre>
1453 : 	          <br>
1454 : 	      <li>使用対象とは，このアイテムを使用することの出来る対象が入っている。
1455 : 	          サーバーでは以下のように定義されている。
1456 : 	          <pre>
1457 : 			typedef enum
1458 : 			{
1459 : 				ITEM_TARGET_MYSELF,		自分のみ
1460 : 				ITEM_TARGET_OTHER,		他の人（自分含む)
1461 : 				ITEM_TARGET_ALLMYSIDE,		味方全体
1462 : 				ITEM_TARGET_ALLOTHERSIDE,	相手側全体
1463 : 				ITEM_TARGET_ALL,		全て
1464 : 			}ITEM_TARGETTYPE;
1465 : 		</pre>
1466 :   	        <font size=+1>この数字に100を足すと，死んでいる者も対象となる。</font><br>
1467 : 	      <li>アイテムレベル。このレベル以上の者でないと装備出来ない。
1468 : 	      <li>フラグ類。色んなフラグを送信する。各ビットの対応は以下の通り。<br>
1469 : 	          <table border=1>
1470 : 		    <tr><td>0bit目</td><td>ペットメールで送信可能なアイテムかどうか。送信可能だと１。０だと送信できない。</td></tr>
1471 : 		    <tr><td>1Bit目</td><td>このアイテムが合成出来るかどうか。</td></tr>
1472 : 		    <tr><td>2Bit目</td><td>このアイテムが料理かどうか。1だと料理である。</td></tr>
1473 : 		  </table>
1474 : 		<br>
1475 : 		<br>
1476 : 	    </ul>
1477 : 	    <br>
1478 : 	    ここの中では<a href="#escaping">エスケープ</a>する。<br>
1479 : 	    具体例( char の配列そのまま書く )<br>
1480 : 	    <code>
1481 : 	    Iあほ な 子|str+2|1|あほ\|いじ|10|2|0
1482 : 	    </code>
1483 : 	    ステージの値の意味は、整数値で
1484 : 	    <table border>
1485 : 	      <tr><td>1</td><td>ステージA</td></tr>
1486 : 	      <tr><td>2</td><td>ステージB</td></tr>	      
1487 : 	      <tr><td>3</td><td>ステージC</td></tr>
1488 : 	      <tr><td>4</td><td>ステージD</td></tr>
1489 : 	    </table>
1490 : 	    とする。また、アイテム表の空の部分については、5個のトークンが
1491 : 	    空で、つまりたて棒がならんだ状態で送信されてくる。
1492 : 
1493 : 	<br><br>
1494 : 	<LI>S 全スキル<br><br>
1495 : 	    (スキル0)|(スキル1)|(スキル2) ... (スキルn)<br>
1496 : 	    称号と同じで、キャラがもてる最大スキル数分を送信する。つま
1497 : 	    りないものは ||| となるのである。
1498 : 	    各エントリの内容は、<br>
1499 : 	    スキルの種類コード|レベル<br>
1500 : 	    である。<br>
1501 : 	    具体例( char の配列そのまま書く )<br>
1502 : 	    <code>
1503 : 	    S10|2|20|2|||
1504 : 	    </code>	    
1505 : 
1506 : 	<br><br>
1507 : 	<LI>T 称号<br><br>
1508 : 	    (称号0)|(称号1)|(称号2) ... (称号n)<br><br>
1509 : 	    称号のエントリが空の場合は '|' が連続するので、連続したも
1510 : 	    のを省略してスキャンしてはならない。かならず最大個数分送信
1511 : 	    する。<br>
1512 : 	    ひとつ、ひとつの内容は、<br>
1513 : 	    名前<br>
1514 : 	    のみである。<br>
1515 : 	    具体例( 送られてくる char の配列そのままである )<br>
1516 : 	    <code>
1517 : 	    Tあほ|よっぱらい
1518 : 	    </code>
1519 : 	    これも<a href="#escaping">エスケープ</a>する。<br>
1520 : 
1521 : 	<br>
1522 : 	<LI>M 部分パラメータ<br><br>
1523 : 	    キャラクターのステータスのうち、頻繁に変更されるもの(現在
1524 : 	    値)とそうでないもの(最大値など)がある。どれかのステータス
1525 : 	    が変更されるたびに毎回全部のパラメータを送信するのはネット
1526 : 	    ワークトラフィックを無駄に使うことになるので、特にHP ,
1527 : 	    MP,EXPの値だけにしぼったパラメータ送信コマンドを用意する。
1528 : 	    それがこのMコマンドである。2文字目以降の第一トークンはHPの
1529 : 	    現在値 , 第二トークンはMP、第三トークンはEXPである。
1530 : 	    以下に具体例をしめす。
1531 : 	    <code>
1532 : 	    M54|210|8944909
1533 : 	    </code>
1534 : 	    この例ではHPの現在値が54、MPの現在値が210、経験値が8944909
1535 : 	    になっているのだ。この3種類は特に変更頻度が高いと思われる
1536 : 	    ため独立したコマンド化したが、もちろんこのMコマンドのかわ
1537 : 	    りにPコマンドで全パラメータを送信してもよい。ただし、最適
1538 : 	    にネットワークを使うためには、このコマンドを使うことが推奨
1539 : 	    される。
1540 : 	    
1541 : 	    <br><br>
1542 :         <li>D キャラのID<br><br>
1543 : 	    ログイン時にだけ送るキャラのindex，それと時間を送る。
1544 : 	    <code>D1000|912766409</code>とかで送られる。
1545 : 	<br><br>
1546 : 	<li>E エンカウント確率（下限)|(上限)<br><br>
1547 : 	    エンカウント率を送信する。確率はn/100<br>
1548 : 	    クライアントは，下限の値からスタートして，１歩歩く毎に+1した確率でエンカウントするかを判断する。
1549 : 	    ただし，上限の値を越えないようにする。
1550 : 	    次回にこれが送られてくるまでこの確率でエンカウントを計算する事。
1551 : 	    
1552 : 	<br><br>
1553 : 	<li>J0 ～J6 使える呪術の内容<br><br>
1554 : 	    使える呪術の内容を送信する。<br>
1555 : 	    0 ～6 はそれぞれアイテムの装備箇所に対応している。一部の装備箇所（頭，胴，手，装飾品）
1556 : 	    しか装備出来ない予定だが，今後拡張の為に全ての装備箇所の情報を送る<br>
1557 : 	    <code>J0|kubun|mp|field|target|name|comment</code><br>
1558 : 	    というフォーマットになっている。<br>
1559 : 	    kubun はその欄があるか無いか。０だと無い。それ以降にトークンすらない。１だとある。
1560 : 	    mpは消費気力を表す。<br>
1561 : 	    fieldはどの場所で使えるか。サーバーでは以下の様に定義されている。<br>
1562 : 	    <pre>
1563 : 		typedef enum
1564 : 		{
1565 : 			MAGIC_FIELD_ALL,		すべての場所で使える
1566 : 			MAGIC_FIELD_BATTLE,		戦闘中のみ
1567 : 			MAGIC_FIELD_MAP,		通常マップ上のみ
1568 : 			
1569 : 		}MAGIC_FIELDTYPE;
1570 : 	    </pre>
1571 : 	    targetはどれを対象に出来るか。サーバーでは以下のように定義されている。<br>
1572 : 	    <pre>
1573 : 		typedef enum
1574 : 		{
1575 : 			MAGIC_TARGET_MYSELF,		自分のみ
1576 : 			MAGIC_TARGET_OTHER,		他の人（自分含む)
1577 : 			MAGIC_TARGET_ALLMYSIDE,		味方全体
1578 : 			MAGIC_TARGET_ALLOTHERSIDE,	相手側全体
1579 : 			MAGIC_TARGET_ALL,		全て
1580 : 			MAGIC_TARGET_NONE,		誰も選択出来ない。防御やための時
1581 : 			MAGIC_TARGET_OTHERWITHOUTMYSELF,他の人（自分含まない)
1582 : 			MAGIC_TARGET_WITHOUTMYSELFANDPET, 自分とペット以外
1583 : 			MAGIC_TARGET_WHOLEOTHERSIDE,    片方のサイド全体
1584 : 		}MAGIC_TARGETTYPE;
1585 : 	    </pre>
1586 : 	    <font size=+1>この数字に100を足すと，死んでいる者も対象となる。</font><br>
1587 : 	    <br>
1588 : 	    nameは呪術名。<br>
1589 : 	    commentはこの呪術の説明。<br>
1590 : 	<br>
1591 : 	<br>
1592 : 	<li>N0 ～ N3 仲間のパラメータ<br><br>
1593 : 	    仲間になっている人の情報を送信する。<br>
1594 : 	    <code>N0|kubun|level|charaindex|maxhp|hp|mp|name</code>
1595 : 	    <br>
1596 : 	    kubun はその欄があるか無いか。０だと無い。それ以降にトークンすらない。１全パラメータ。<br>
1597 : 	    2以上だと，各ビットの立っているもののパラメータ（1bit 目 level 2bit 目charaindexなど）
1598 : 	    が送信される。<br>
1599 : 	    charaindex は仲間のcharaindex<br>
1600 : 	    levelはその人のレベル。<br>
1601 : 	    maxhpはその人のMAXHP<br>
1602 : 	    hpはその人の現在のHP<br>
1603 : 	    mpはその人の気力<br>
1604 : 	    name はその人の名前。<br>
1605 : 	<br><br>
1606 : 	
1607 : 	<LI>K0～K4 恐竜パラメータ<br><br>
1608 : 	    hp maxhp mp maxmp str tough exp 
1609 : 	    level attackpower deffencepower 
1610 : 	    fixdex fixcharm fixluck fixfireat fixwaterat fixearthat fixwindat
1611 : 	    名前 status<br>
1612 : 	    Kの後に0～4を指定して、どの恐竜かを指定すること。
1613 : 	    その次のトークンに0が来たらそのペット欄は無いと言う事。
1614 : 	    ある場合は１である。1だと全パラメータ。
1615 : 	    2以上だと，各ビットの立っているもののパラメータ（1bit 目 hp  2bit 目maxhpなど）
1616 : 	    が送信される。<br>
1617 : 	    デリミタは '|' である。また名前と自己称号は、
1618 : 	    <a href="#escaping">エスケープ</a>したものを
1619 : 	    たて棒でつなげたものが通信される。<br>
1620 : 	    それぞれの値の型は以下。
1621 : 	    <table border>
1622 : 	      <tr><td>No.(番号)</td><td>int</td></tr>
1623 : 	      <tr><td>islive(生死)</td><td>int</td></tr>
1624 : 	      <tr><td>GRA(画像番号)</td><td>int</td></tr>
1625 : 	      <tr><td>Hp(耐久力)</td><td>int</td></tr>
1626 : 	      <tr><td>MaxHp(最大耐久力)</td><td>int</td></tr>
1627 : 	      <tr><td>Mp</td><td>int</td></tr>
1628 : 	      <tr><td>MapMp</td><td>int</td></tr>
1629 : 	      <tr><td>Exp(経験値)</td><td>int</td></tr>
1630 : 	      <tr><td>MaxExp(次までの経験値)</td><td>int</td></tr>
1631 : 	      <tr><td>Level(レベル)</td><td>int</td></tr>
1632 : 	      <tr><td>Attack(攻撃力)</td><td>int</td></tr>
1633 : 	      <tr><td>Defense(守備力)</td><td>int</td></tr>
1634 : 	      <tr><td>Quick(素早さ)</td><td>int</td></tr>
1635 : 	      <tr><td>Ai(忠誠度)</td><td>int</td></tr>
1636 : 	      <tr><td>fEarth(地)</td><td>int</td></tr>
1637 : 	      <tr><td>fWater(水)</td><td>int</td></tr>
1638 : 	      <tr><td>fFire(火)</td><td>int</td></tr>
1639 : 	      <tr><td>fWid(風)</td><td>int</td></tr>
1640 : 	      <tr><td>Slot(最大石版数)</td><td>int</td></tr>
1641 : 	      <tr><td>名前変更許可フラグ</td><td>int</td></tr>
1642 : 	      <tr><td>名前</td><td>文字列</td></tr>
1643 : 	      <tr><td>ユーザーペット名</td><td>文字列</td></tr>
1644 : 	    </table>
1645 : 	    <br>
1646 : 	    名前変更許可フラグとは，このペットの名前を変更してよいかどうかのフラグで，
1647 : 	    1 だと変更ＯＫ，０だと変更不可となる。<br>
1648 : 	    <br>
1649 : 	    <br>
1650 : 	    <br>
1651 : 	    具体例( char の配列そのまま書く )<br>
1652 : 	    <code>
1653 : 	    P10|20|10|20|10|10|10|1|2|13|13|1|100|10|へんぱ|PC
1654 : 	    </code>
1655 : 	    
1656 : 	<br><br>
1657 : 
1658 : 	<LI>W0～W4 ペットの技データ<br><br>
1659 : 	  <code>W0|skillid|field|target|name|comment| x 7</code><br>
1660 : 	  W0 ～ W4 はそれぞれのペットに対応している。<br>
1661 : 	  petskillid は，ペットの技の番号。pet_skillinfo.hに定義されている。<br>
1662 : 	  field はその技がどこで使用できるか。サーバーでは以下のように定義されている。<br>
1663 : 	  <pre>
1664 : 		typedef enum
1665 : 		{
1666 : 			PETSKILL_FIELD_ALL,		すべての場所で使える
1667 : 			PETSKILL_FIELD_BATTLE,		戦闘中のみ
1668 : 			PETSKILL_FIELD_MAP,		通常マップ上のみ
1669 : 
1670 : 		}PETSKILL_FIELDTYPE;
1671 : 	  
1672 : 	  </pre>
1673 : 	  target はその技の対象がどういうものか。サーバーでは次のように定義されている。<br>
1674 : 	  <pre>
1675 : 		typedef enum
1676 : 		{
1677 : 			PETSKILL_TARGET_MYSELF,		自分のみ
1678 : 			PETSKILL_TARGET_OTHER,		他の人（自分含む)
1679 : 			PETSKILL_TARGET_ALLMYSIDE,	味方全体
1680 : 			PETSKILL_TARGET_ALLOTHERSIDE,	相手側全体
1681 : 			PETSKILL_TARGET_ALL,		全て
1682 : 			PETSKILL_TARGET_NONE,		誰も選択出来ない。防御やための時
1683 : 			PETSKILL_TARGET_OTHERWITHOUTMYSELF,他の人（自分含まない) 
1684 : 			PETSKILL_TARGET_WITHOUTMYSELFANDPET, 自分とペット以外
1685 : 		}PETSKILL_TARGETTYPE;
1686 : 	  </pre>
1687 : 	  <br>
1688 : 	  name は技の名前。<br>
1689 : 	  commentはその技に対する説明。<br>
1690 : 	  target|name|comment| は技の数の分１行で送られてくる。
1691 : 	  技は一応７つまで。途中で技が抜けている（"|"のみが続く）場合
1692 : 	  はクライアントで詰めて表示する事。<br>
1693 : 	  <br><br>
1694 : 	  
1695 :       </ul>
1696 : 	  
1697 :       <br>
1698 :       <hr>
1699 :       <br>
1700 : 
*/

	if( strcmp( funcname , "S" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_S_recv( fd,data);
		return 0;
	}
/*
1701 :   <LI><a name="D"><font color=blue>servertoclient D( int category , int dx , int dy , string data );<br></font></a>
1702 :       (Display)
1703 :       画面に何か表示する指令。<br><br>
1704 : 
1705 :       <dl>
1706 : 	<dt>int category
1707 : 	<dd>何を表示するか。
1708 : 	    <table border>
1709 : 	      <tr><td>値</td><td>内容</td></tr>
1710 : 	      <tr><td>1</td><td>自分以外に与えたダメージ。dataは文字
1711 : 		  列になった値。</td></tr>
1712 : 		  <tr><td>2</td><td>自分がうけたダメージ。dataは文字列に
1713 : 		      なった値</td></tr>
1714 : 		</table>
1715 : 	<dt>int dx
1716 : 	<dd>マイキャラからのグリッド相対位置X。一般的にはイベントが発生
1717 : 	    した位置。クライアントはこの値から適宜適切な位置を計算して
1718 : 	    描画する。
1719 : 	<dt>int dy
1720 : 	<dd>相対位置Y
1721 : 	<dt>string data 
1722 : 	<dd>表示する内容。内容はcategoryによって決まる。
1723 : 	    この文字列は<a href="#escaping">エスケープ</a>されなければ
1724 : 	    ならない。
1725 :       </dl>
1726 :       <br>
1727 :       <br>
1728 :       <br>
1729 :       <hr>
1730 :       <br>
*/

	if( strcmp( funcname , "D" ) == 0 ){
		int category;
		int dx;
		int dy;
		char* data;
		category = lssproto_demkstr_int( lssproto.token_list[2] );
		dx = lssproto_demkstr_int( lssproto.token_list[3] );
		dy = lssproto_demkstr_int( lssproto.token_list[4] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[4] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[5] ));
		lssproto_D_recv( fd,category,dx,dy,data);
		return 0;
	}
/*
1746 :   <li><a name="SC_FS"><font color=blue>servertoclient FS( int flg);<br></font></a>
1747 :       (FlgSet)PSのの応答。またはログイン時などに自分の状態として送られてくる。<br><br>
1748 :       <dl>
1749 :         <dt>int flg
1750 :         <dd> 0 bit  0: 仲間Off                  1: 仲間On<br>
1751 :              （現在未使用）1 bit  0: 戦闘途中参加off          1: 戦闘途中参加On    <br>
1752 :              2 bit  0: DUEL off                 1: DUEL On<br>
1753 :              3 bit  0: 普通のチャットモード     1: パーティにしかチャットが飛ばないモード<br>
1754 :              4 bit  0: 名刺交換OK               1: 名刺交換拒否<br>
1755 :       </dl>
1756 :       <br>
1757 :       <br>
1758 :       <hr>登录时等自己的状态被送的FlgSet
1759 :       <br>
*/

	if( strcmp( funcname , "FS" ) == 0 ){
		int flg;
		flg = lssproto_demkstr_int( lssproto.token_list[2] );
		lssproto_FS_recv( fd,flg);
		return 0;
	}
/*
1771 :   <li><a name="SC_HL"><font color=blue>servertoclient HL( int flg);<br></font></a>
1772 :       (HeLp)HLのの応答。またはパーティの仲間が戦闘のお助けモードを変更した場合に送られてくる。<br><br>
1773 :       <dl>
1774 :         <dt>int flg
1775 :         <dd> 0: お助けモードOff                  1: お助けモードOn<br>
1776 :       </dl>
1777 :       <br>
1778 :       <br>
1779 :       <hr>
1780 :       <br>派对的同伴战斗的帮助模式变更的场合，被送的
*/

	if( strcmp( funcname , "HL" ) == 0 ){
		int flg;
		flg = lssproto_demkstr_int( lssproto.token_list[2] );
		lssproto_HL_recv( fd,flg);
		return 0;
	}
/*
1794 :   <li><a name="SC_PR"><font color=blue>servertoclient PR( int request, int result);<br></font></a>
1795 :       (PartyRequest)仲間要求の応答。PRをクライアントが送っていなくてもこれを受け取る時がある。
1796 :        パーティが突然解散（親が抜けた）などの時。
1797 :        <br><br>
1798 :       
1799 :       <dl>朋友要求响应。公关客户发送着要我收到的时候。1796:派对突然解散（父母掉了）等的时候。
1800 :       	<dt>int request
1801 :       	<dd>0: 除隊 1:入隊
1802 :         <dt>int result
1803 :         <dd>0: 失敗 1: 成功
1804 :       </dl>
1805 :       <br>
1806 :       <br>
1807 :   <br>
1808 :       <hr>
1809 :       <br> 组队 解散
*/

	if( strcmp( funcname , "PR" ) == 0 ){
		int request;
		int result;
		request = lssproto_demkstr_int( lssproto.token_list[2] );
		result = lssproto_demkstr_int( lssproto.token_list[3] );
		lssproto_PR_recv( fd,request,result);
		return 0;
	}
/*
1821 :   <li><a name="SC_KS"><font color=blue>servertoclient KS( int petarray,int result);<br></font></a>
1822 :        クライアントのKSに対する返答。これ以外にも必要があれば恐竜のパラメータや
1823 :        メッセージなども送られてくるかもしれない。
1824 :        または戦闘に出すと選択しているペットがなんらかしらの事情（そのペットを置いたなど）で選択から外れたときにも
1825 :        このメッセージは送られてくる。
1826 :        <br><br>
1827 :       
1828 :       <dl>
1829 :       	<dt>int petarray
1830 :       	<dd>ペットの番号
1831 :       	<dt>int result
1832 :       	<dd>0: 失敗，または選択ペットから外れた。 1:成功
1833 :       </dl>
1834 :       <br>
1835 :       <br>
1836 :       <hr>
1837 :       <br>
*/

	if( strcmp( funcname , "KS" ) == 0 ){
		int petarray;
		int result;
		petarray = lssproto_demkstr_int( lssproto.token_list[2] );
		result = lssproto_demkstr_int( lssproto.token_list[3] );
		lssproto_KS_recv( fd,petarray,result);
		return 0;
	}
/*
1921 :   <li><a name="SC_PS"><font color=blue>servertoclient PS( int result, int havepetindex, int havepetskill, int toindex);<br></font></a>
1922 :       (PetSkill use result)ペットの技を使った結果クライアントのPSに対応して送られてくる。
1923 :       result 以外はクライアントのPSに対応している。フィールド上からしか呼ばれない。
1924 :       
1925 :       <dl>
1926 : 	<dt>int result
1927 : 	<dd>結果。0: 失敗 1:成功
1928 :         <dt>int havepetindex
1929 :         <dd>何番目のペットが使用したか。
1930 :         <dt>int havepetskill
1931 :         <dd>何番目の技を使用したか。
1932 : 	<dt>int toindex
1933 : 	<dd> 誰に魔法を使用したか。これはオブジェクトやキャラのindexではない。以下の様になっている。
1934 : 	<pre>
1935 : 	  自分    = 0
1936 : 	  ペット  = 1 ～5
1937 : 	  仲間    = 6 ～10 （S N の0～4に対応。自分自身も含まれている）
1938 : 	</pre>
1939 : 	対象が全員，とか分からない，とかの場合は-1で送信する。<br>
1940 : 	<br>
1941 :       	    
1942 :       </dl>
1943 :       
1944 :       <br>
1945 :       <br>
1946 :       <hr>
1947 :       <br>
1948 : 
*/

	if( strcmp( funcname , "PS" ) == 0 ){
		int result;
		int havepetindex;
		int havepetskill;
		int toindex;
		result = lssproto_demkstr_int( lssproto.token_list[2] );
		havepetindex = lssproto_demkstr_int( lssproto.token_list[3] );
		havepetskill = lssproto_demkstr_int( lssproto.token_list[4] );
		toindex = lssproto_demkstr_int( lssproto.token_list[5] );
		lssproto_PS_recv( fd,result,havepetindex,havepetskill,toindex);
		return 0;
	}
/*
1997 :   <LI><a name="S2CSKUP"><font color=blue>servertoclient SKUP( int point );<br></font></a>
1998 :       (SKillUP)
1999 :       スキルアップができる事をサーバが通知する。いくつ上げられるかを指定する。
2000 :       <br>
2001 :       <br>
2002 :       <hr>提高技能能做的事情服务器通知。取得指定几个？。
2003 :       <br>
2004 : 
*/

	if( strcmp( funcname , "SKUP" ) == 0 ){
		int point;
		point = lssproto_demkstr_int( lssproto.token_list[2] );
		lssproto_SKUP_recv( fd,point);
		return 0;
	}
/*
2033 :   <LI><a name="SC_WN"><font color=blue>servertoclient WN( int windowtype, int buttontype, int seqno, int objindex, string data );<br></font></a>
2034 :       (Window)<br>
2035 :       ウィンドウを表示せよとサーバーがクライアントに通知する。<br>
2036 :       <br>窗口显示要通知客户端服务器
2037 :       <dl>
2038 :         <dt>int windowtype
2039 :         <dd>ウィンドウタイプ。どのような形式のウィンドウを表示するか。これとbuttontype の組み合わせで
2040 :             ウィンドウが決定する。<br>
2041 :             サーバーでは以下の様に設定されている。<br>
2042 :           <pre>窗口类型。怎样的形式的窗口显示。这和buttontype的组合2040:窗口决定。< br / >2041:服务器以下被设定的。
2043 : 		typedef enum
2044 : 		{
2045 : 			WINDOW_MESSAGETYPE_MESSAGE,			メッセージのみ
2046 : 			WINDOW_MESSAGETYPE_MESSAGEANDLINEINPUT,		メッセージと一行入力
2047 : 			WINDOW_MESSAGETYPE_SELECT,			選択ウィンドウ
2048 : 			WINDOW_MESSAGETYPE_PETSELECT,			ペット選択ウィンドウ
2049 : 			WINDOW_MESSAGETYPE_PARTYSELECT,			仲間選択ウィンドウ
2050 : 			WINDOW_MESSAGETYPE_PETANDPARTYSELECT,		ペット，仲間選択ウィンドウ
2051 : 			WINDOW_MESSAGETYPE_ITEMSHOPMENU,		お店のメニューウインドウ
2052 : 			WINDOW_MWSSAGETYPE_ITEMSHOPYMAIN,		お店のメインウインドウ
2053 : 			WINDOW_MESSAGETYPE_LIMITITEMSHOPMAIN,		買い取り専門アイテム屋のメインウインドウ
2054 : 			WINDOW_MESSAGETYPE_PETSKILLSHOP,		ペットの技屋さんウインドウ
2055 : 			WINDOW_MESSAGETYPE_WIDEMESSAGE,			メッセージのみ（大きい方）
2056 : 			WINDOW_MESSAGETYPE_WIDEMESSAGEANDLINEINPUT,	メッセージと一行入力（大きい方）
2057 : 
2058 : 		}WINDOW_MESSAGETYPE;
2059 : 	  </pre>
2060 : 
2061 :         <dt>int buttontype
2062 :         <dd>ボタンの形式を指定する。サーバーでは以下の用に定義されている。<br>
2063 :             <pre>
2064 : 		#define		WINDOW_BUTTONTYPE_NONE		(0)
2065 : 		#define		WINDOW_BUTTONTYPE_OK		(1 << 0)
2066 : 		#define		WINDOW_BUTTONTYPE_CANCEL	(1 << 1)
2067 : 		#define		WINDOW_BUTTONTYPE_YES		(1 << 2)
2068 : 		#define		WINDOW_BUTTONTYPE_NO		(1 << 3)
2069 : 		#define		WINDOW_BUTTONTYPE_PREV		(1 << 4)
2070 : 		#define		WINDOW_BUTTONTYPE_NEXT		(1 << 5)
2071 :             </pre>
2072 :             これらの組み合わせで送信する。例えば，YESボタンとNOボタンが欲しい時は<br>
2073 :             WINDOW_BUTTONTYPE_YES | WINDOW_BUTTONTYPE_NO   (=12)<br>
2074 :             で送る。
2075 :             
2076 :         <dt>int seqno
2077 :         <dd>このウィンドウの番号を示す。サーバーが管理する。
2078 :             クライアントはWNにてこのウィンドウの操作結果を返すのに，この番号を添えて返答する。
2079 :             これによってサーバーはどのNPCのどの場面でのウィンドウかを判断出来るようにする。
2080 :         <dt>int objindex
2081 :         <dd>このウィンドウを出せと言ったNPCなどのindexが格納されている。
2082 :             システムが出せと言った場合は-1などが入っている。
2083 :             クライアントは，ウィンドウ入力後のWNプロトコルでこの数値をそのまま返せば良い。
2084 :         <dt>string data
2085 :         <dd>メッセージ内容を示す。内容はエスケープする。"\n"で区切ると改行という意味とする。
2086 :             また，クライアントのウィンドウで表示出来る横幅を越えた際は自動的に改行される。
2087 :             選択肢のあるウィンドウでは，"\n"で区切られた最初のトークンがメッセージの行数
2088 :             となり，次にメッセージのトークンが最初のトークンで指定された個数続き，
2089 :             そのあとのトークン毎が１つずつの選択肢となる。また前から順に 1 から
2090 :             番号を割り当て，選択した場合のWNでの返答の時に返す。
2091 :         <dt>string data(お店用）
2092 :         <dd>メッセージ内容を示す。内容はエスケープする。"\n"で区切ると改行という意味とする。
2093 :             また，クライアントのウィンドウで表示出来る横幅を越えた際は自動的に改行される。
2094 :             データ内は項目毎に"｜"で区切られています。
2095 :    <br><br><dd>?買う?<br>
2096 :  	買い売るフラグ（買う０：売る１）｜前のデータ使うかフラグ（使う０：使わない：1）｜
2097 : 		店の名前｜メッセージ｜店メッセージ｜個数選択メッセージ｜レベル足りないメッセージ｜確認メッセージ｜
2098 : 		アイテムがいっぱいメッセージ｜アイテム名｜買える買えないフラグ（買える：０買えない：１）｜アイテムレベル｜
2099 : 		値段｜画像番号｜説明｜アイテム名｜買える買えないフラグ｜アイテムレベル｜値段｜画像番号｜説明
2100 :       		<br><br>
2101 :       		0|1|サムギルの武器屋|いらっしゃい、何のようだい|いらっしゃいどれにする|何個必要だい|それを装備するにはレベルが
2102 :       		足りないけどいいのかい|本当にいいんだね|おいおいアイテムがいっぱいじゃねえか！｜ブーメラン|
2103 :       		０|１００|１６０００|横一列に攻撃\n効果抜群|薬草|０|５０|１６００２|ＨＰ２０回復\n冒険にはつき物\n安い｜
2104 : 
2105 :         <br><dd><br>?売る?<br>
2106 : 		買い売るフラグ（買う０：売る１）｜前のデータ使うかフラグ（使う０：使わない：1）｜
2107 : 		店の名前｜メッセージ｜店メッセージ｜お金がいっぱいになっちゃうメッセージ|確認メッセージ｜
2108 : 		アイテム名｜売れる売れないフラグ｜値段｜画像番号｜説明｜アイテム名｜売れる売れないフラグ｜
2109 : 		値段｜画像番号｜説明｜
2110 :       
2111 :       	<br><br>
2112 :       		１|1|サムギルの道具屋|いらっしゃい、どるする？｜いらっしゃいどれを売ってくれる。|本当にいいんだね|
2113 :       		それだと、お金がオーバーしちゃうね｜ブーメラン|０|２０|１６０００|
2114 :       		横一列に攻撃\n効果抜群|薬草|０|１０|１６００２|ＨＰ２０回復\n冒険にはつき物\n安い｜
2115 : 
2116 : 
2117 :         <br><dd><br>?その他?<br>
2118 : 		店の名前｜メッセージ
2119 :       
2120 :       	<br><br>
2121 :       		サムギルの道具屋|いらっしゃい、どるする？
2122 : 
2123 : 	<br><br><br>
2124 :         <dt>string data(ペットの技屋さん用）
2125 :         <dd>メッセージ内容を示す。内容はエスケープする。"\n"で区切ると改行という意味とする。
2126 :             また，クライアントのウィンドウで表示出来る横幅を越えた際は自動的に改行される。
2127 :             データ内は項目毎に"｜"で区切られています。
2128 : 	   <br><br><dd>?ペットの技取得?<br>
2129 : 		前の情報使うかどうか？（使う：0　使わない：1）｜店の名前｜メインメッセージ｜
2130 : 		技名｜値段｜技説明｜技名｜値段｜技説明
2131 : 	<br><br>
2132 : 		1｜サムギルの武器屋｜いらっしゃいまっせ。すばらしい技ばっかですよ｜
2133 : 		背水の陣その①｜500｜攻撃力３０％ＵＰ　防御力３０％ＤＯＷＮ｜
2134 : 		地球一週｜1000｜新しい発見があるかも（適当）｜	
2135 :       </dl>
2136 :       <br>
2137 :       <br>
2138 :       <hr>
2139 :       <br>
2140 :       
*/

	if( strcmp( funcname , "WN" ) == 0 ){
		int windowtype;
		int buttontype;
		int seqno;
		int objindex;
		char* data;
		windowtype = lssproto_demkstr_int( lssproto.token_list[2] );
		buttontype = lssproto_demkstr_int( lssproto.token_list[3] );
		seqno = lssproto_demkstr_int( lssproto.token_list[4] );
		objindex = lssproto_demkstr_int( lssproto.token_list[5] );
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[5] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[6] ));
		lssproto_WN_recv( fd,windowtype,buttontype,seqno,objindex,data);
		return 0;
	}
/*
2208 :   <LI><a name="SC_CLIENTLOGIN"><font color=blue>servertoclient ClientLogin(string result);<br></font></a>
2209 :       ClientLoginの返答。
2210 :       <br><br>
2211 :       <dl>
2212 : 	<dt>string result
2213 : 	<dd>"ok" という文字列。この文字列はエスケープされない。
2214 :       </dl>
2215 :       <br>
2216 :       <br>
2217 :       <hr>
2218 :       <br>
2219 : 
2220 :       
*/

	if( strcmp( funcname , "ClientLogin" ) == 0 ){
		char* result;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_ClientLogin_recv( fd,result);
		return 0;
	}
/*
2251 :   <LI><a name="SC_CREATENEWCHAR"><font color=blue>servertoclient CreateNewChar(string result,string data);<br></font></a>
2252 :       CreateNewCharの返答。
2253 :       <br><br>
2254 :       <dl>
2255 : 	<dt>string result
2256 : 	<dd>"successful" か "failed" のいずれか。この文字列はエスケー
2257 : 	    プしない。
2258 : 	<dt>string data
2259 : 	<dd>"failed" の時は理由を示す人間の見て分る文字
2260 : 	    列である。アカウントサーバからの返答そのままである。
2261 : 	    以下の文字列
2262 : 	    <pre>
2263 : 	    "failed bad parameter"
2264 : 	    </pre>
2265 : 	    の場合は、キャラ作成のときに規定のパラメータの範囲を越えて
2266 : 	    いるというとを意味する。これはゲームサーバーが出力するメッ
2267 : 	    セージである。この文字列はエスケープしない。
2268 : 	    
2269 :       </dl>
2270 :       <br>
2271 :       <br>
2272 :       <hr>
2273 :       <br>
2274 : 
*/

	if( strcmp( funcname , "CreateNewChar" ) == 0 ){
		
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CreateNewChar_recv( fd,result,data);
		return 0;
	}
/*
2286 :   <LI><a name="SC_CHARDELETE"><font color=blue>servertoclient CharDelete(string result,string data);<br></font></a>
2287 :       CharDelete の返答。
2288 :       <br><br>
2289 :       <dl>
2290 : 	<dt>string result
2291 : 	<dd>"successful" か "failed" のいずれか。エスケープしない。
2292 : 	<dt>string data
2293 : 	<dd>"failed" の時は理由を示す人間の見て分る文字
2294 : 	    列である。アカウントサーバからの返答そのままである。
2295 : 	    デリミタをふくまないので、エスケープしない。
2296 :       </dl>
2297 :       <br>
2298 :       <br>
2299 :       <hr>
2300 :       <br>
2301 : 
2302 : 
*/

	if( strcmp( funcname , "CharDelete" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharDelete_recv( fd,result,data);
		return 0;
	}
/*
2313 :   <LI><a name="SC_CHARLOGIN"><font color=blue>servertoclient CharLogin(string result,string data);<br></font></a>
2314 :       CharaLoginの返答。
2315 :       <br><br>
2316 :       <dl>
2317 : 	<dt>string result
2318 : 	<dd>"successful" か "failed" のいずれか。エスケープしない。
2319 : 	<dt>string data
2320 : 	<dd>"failed" の時は その理由の文字列。エスケープしない。
2321 :       </dl>
2322 :       <br>
2323 :       <br>
2324 :       <hr>
2325 :       <br>
2326 : 
2327 : 
*/

	if( strcmp( funcname , "CharLogin" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharLogin_recv( fd,result,data);
		return 0;
	}
/*
2333 :   <LI><a name="SC_CHARLIST"><font color=blue>servertoclient CharList(string result,string data);<br></font></a>
2334 :       CharListの返答。
2335 :       <br><br>
2336 :       <dl>
2337 : 	<dt>string result
2338 : 	<dd>"successful" か "failed" のいずれか。エスケープしない
2339 : 	<dt>string data
2340 : 	<dd>resultが"successful"の時は、アカウントサーバーに保存されて
2341 : 	    いるすべてのキャラの名前、オプションををスペースで区切った
2342 : 	    一個の文字列。この文字列を作っているのは、アカウントサーバ。
2343 : 	    result が "failed" の時は理由を示す人間の見て分る文字列で
2344 : 	    ある。成功の時のオプションの中身は以下のとおりである。<br>
2345 : 	    dataplace|faceimage|level|maxhp|atk|def|quick|charm|earth|water|fire|wind|logincount|name|place<br>
2346 : 	    
2347 : 	    <dl>
2348 : 	      <dt>dataplace
2349 : 	      <dd>セーブデータの何番目か。これでキャラクターリストの何番目かを決定する。
2350 : 	      <dt>faceimage
2351 : 	      <dd>顔の画像番号
2352 : 	      <dt>level
2353 : 	      <dd>キャラのレベル
2354 : 	      <dt>maxhp,atk,def,quick,charm
2355 : 	      <dd>各パラメータ。
2356 : 	      <dt>earth.water,fire,wind
2357 : 	      <dd>各属性値
2358 : 	      <dt>logincount
2359 : 	      <dd>ログインカウント
2360 : 	      <dt>name
2361 : 	      <dd>キャラの名前
2362 : 	      <dt>place
2363 : 	      <dd>キャラのいる場所
2364 : 	    </dl>
2365 : 	    "|" で区切られている。 それぞれの項目は、<a
2366 : 	    href="#escaping">エスケープ</a>されている。そのあとたて棒
2367 : 	    でつなげる。<br>
2368 : 	    
2369 :       </dl>
2370 :       <br>
2371 :       <br>
2372 :       <hr>
2373 :       <br>
2374 : 
*/

	if( strcmp( funcname , "CharList" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharList_recv( fd,result,data);
		return 0;
	}
/*
2382 :   <LI><a name="SC_CHARLOGOUT"><font color=blue>servertoclient CharLogout(string result , string data);<br></font></a>
2383 :       Logoutに対する返答。
2384 :       <br><br>
2385 :       <dl>
2386 : 	<dt>string result
2387 : 	<dd>"successful" か "failed" のいずれか。エスケープしない。
2388 : 	<dt>string data
2389 : 	<dd>"failed" の時にのみ意味があり、失敗の理由(状態)を示す人間
2390 : 	    の見て分る文字列である。エスケープしない。
2391 :       </dl>
2392 :       <br>
2393 :       <br>
2394 :       <hr>
2395 :       <br>
*/

	if( strcmp( funcname , "CharLogout" ) == 0 ){
		char* result;
		char* data;
		result = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[2] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[3] ));
		lssproto_CharLogout_recv( fd,result,data);
		return 0;
	}
/*
2402 :   <LI><a name="SC_PROCGET"><font color=blue>servertoclient ProcGet( string data);<br></font></a>
2403 : 	ProcGetの返答。
2404 :       <br><br>
2405 :       <dl>
2406 : 	<dt>string data
2407 : 	<dd>エスケープする。ゲームサーバーの内部情報を送信する。内容はlog/procファイルに書かれる内容と同じ。
2408 :       </dl>
2409 :       <br>
2410 :       <br>
2411 :       <hr>
2412 :       <br>
2413 : 
*/

	if( strcmp( funcname , "ProcGet" ) == 0 ){
		char* data;
		data = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_ProcGet_recv( fd,data);
		return 0;
	}
/*
2420 :   <LI><a name="SC_PLAYERNUMGET"><font color=blue>servertoclient PlayerNumGet( int logincount, int player);<br></font></a>
2421 : 	PlayerNumGetの返答。
2422 :       <br><br>
2423 :       <dl>
2424 : 	<dt>int logincount,player
2425 : 	<dd>
2426 :       </dl>
2427 :       <br>
2428 :       <br>
2429 :       <hr>
2430 :       <br>
2431 : 
*/

	if( strcmp( funcname , "PlayerNumGet" ) == 0 ){
		int logincount;
		int player;
		logincount = lssproto_demkstr_int( lssproto.token_list[2] );
		player = lssproto_demkstr_int( lssproto.token_list[3] );
		lssproto_PlayerNumGet_recv( fd,logincount,player);
		return 0;
	}
/*
2443 :   <LI><a name="SC_ECHO"><font color=blue>servertoclient Echo( string test );<br></font></a>
2444 :       Echoに対する返答。
2445 :       <dl>
2446 : 	<dt>string test
2447 : 	<dd>さきほど入力された文字列。エスケープしない。
2448 :       </dl>
2449 :       <br>
2450 :       <br>
2451 :       <hr>
2452 :       <br>
2453 :   
*/

	if( strcmp( funcname , "Echo" ) == 0 ){
		char* test;
		test = lssproto_wrapStringAddr( lssproto_stringwrapper[1] , lssproto.workbufsize , lssproto_demkstr_string( lssproto.token_list[2] ));
		lssproto_Echo_recv( fd,test);
		return 0;
	}
	return -1;
}
void lssproto_SetClientLogFiles( char *r , char *w )
{
	lssproto_strcpysafe( lssproto_readlogfilename , r , sizeof( lssproto_readlogfilename ) );
	lssproto_strcpysafe( lssproto_writelogfilename , w , sizeof( lssproto_writelogfilename ) );
}
int lssproto_InitClient( int (*writefunc)(int,char*,int) ,int bufsiz ,int fd)
{
	int i;
	if( (void*)writefunc == NULL){lssproto.write_func = lssproto_default_write_wrap;} else {lssproto.write_func = writefunc;}
	lssproto_AllocateCommonWork(bufsiz);
	lssproto_stringwrapper = (char**)malloc(sizeof(char*) * MAXLSRPCARGS);
	if(lssproto_stringwrapper ==NULL)return -1;
	memset( lssproto_stringwrapper , 0, sizeof(char*)*MAXLSRPCARGS);
	for(i=0;i<MAXLSRPCARGS;i++){
		lssproto_stringwrapper[i] = (char*)malloc( bufsiz );
		if( lssproto_stringwrapper[i] == NULL){
			for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);return -1;}
		}
	}
	return 0;
}
void lssproto_CleanupClient( void )
{
	int i;
	free( lssproto.work );
	free( lssproto.arraywork);
	free( lssproto.escapework );
	free( lssproto.val_str);
	free( lssproto.token_list );
	for(i=0;i<MAXLSRPCARGS;i++){free( lssproto_stringwrapper[i]);}
	free( lssproto_stringwrapper );
}
/* end of the generated client code */
