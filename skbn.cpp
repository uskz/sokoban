#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <conio.h>	//（_getch以外の入力取得にすればlinuxでもビルドできると思う）
using namespace std;

typedef unsigned int uint;
typedef struct _sXY{int x; int y;} sXY;	// xy座標
unordered_map<string, char> M =
{
{"NONE",	' '},	// 何もない
{"WALL",	'#'},	// 壁
{"NI",		'o'},	// 荷物
{"NI_ON",	'O'},	// 荷物(ゴールの上)
{"SELF",	't'},	// プレイヤー
{"SELF_ON",	'T'},	// プレイヤー(ゴールの上)
{"GOAL",	'.'},	// ゴール
{"RTN",		'\n'},	// 改行
{"END",		'E'},	// ステージの終わり
};

class SKBN{
public:
	SKBN();						// コンストラクタ
	void inputkey();			// キー入力
	void update();				// 情報更新
	void draw() const;			// 画面描画
	bool isAllClear();			// 全ステージクリアしたか判定
private:
	void initSelfPos();			// プレイヤー位置初期化
	uint pos(int x, int y);		// 指定xy座標に相当するインデックスを返す
	sXY j;						// プレイヤー座標情報
	sXY d;						// 移動方向情報
	uint lv;					// 現在のステージ数
	bool bClear;				// ステージクリアフラグ
	vector<char> Map;			// 現在のステージデータ
	vector< vector<char> > Dat;	// 全ステージデータ
};

SKBN::SKBN():j({0,0}),d({0,0}),lv(0),bClear(false)			// メンバ変数初期化
{
	Map.clear();											// メンバ変数初期化
	Dat.clear();											// メンバ変数初期化
	ifstream ifs("dat_skbn.txt");							// ステージデータファイル読み込み
	if(!ifs){ printf("file open error\n"); exit(1);}		// 読み込めなかった時終了
	
	string str;
	uint max_lv = 0;
	while(getline(ifs, str, M["END"]))						// ステージの終わりまで読む
	{
		Dat.resize(max_lv + 1);								// 全ステージデータ数更新(1ステージ分読むたびに+1)
		for(const char t : str) Dat[max_lv].push_back(t);	// 全ステージデータに読みこんだ1ステージを格納
		max_lv++;
	}
	initSelfPos();											// プレイヤー位置初期化
	draw();													// 画面描画
}

void SKBN::inputkey()
{
	char ch = _getch();					// 入力取得 ※_getch()以外の入力取得にすればlinuxでもビルドできると思う）
	if		(ch == 'w') d = { 0, -1};	// 移動量を設定：上
	else if	(ch == 's') d = { 0,  1};	// 移動量を設定：下
	else if	(ch == 'd') d = { 1,  0};	// 移動量を設定：右
	else if	(ch == 'a') d = {-1,  0};	// 移動量を設定：左
	else exit(0);						// それ以外の入力はとりあえずプログラム終了
}

void SKBN::update()
{
	uint t0 = pos(j.x, j.y);															// プレイヤー位置インデックス取得
	uint t1 = pos(j.x + d.x, j.y + d.y);												// 一歩先インデックス取得
	
	if(Map[t1] == M["GOAL"] || Map[t1] == M["NONE"])									// 一歩先が「ゴール」または「何もない」
	{
		Map[t1] = (Map[t1] == M["NONE"]) ? M["SELF"] : M["SELF_ON"];					// 一歩先を「プレイヤー」または「プレイヤー(ゴールの上)」に更新
		Map[t0] = (Map[t0] == M["SELF"]) ? M["NONE"] : M["GOAL"];						// プレイヤー位置だった場所を「何もない」または「ゴール」に更新
	}
	else if(Map[t1] == M["NI"] || Map[t1] == M["NI_ON"])								// 一歩先が「荷物」または「荷物(ゴールの上)」
	{
		uint t2 = pos(j.x + d.x * 2, j.y + d.y * 2);									// 二歩先インデックス取得
		if(Map[t2] == M["GOAL"] || Map[t2] == M["NONE"])								// 二歩先が「ゴール」または「何もない」
		{
			Map[t2] = (Map[t2] == M["NONE"]) ? M["NI"] : M["NI_ON"];					// 二歩先を「荷物」または「荷物(ゴールの上)」に更新
			Map[t1] = (Map[t1] == M["NI"]) ? M["SELF"] : M["SELF_ON"];					// 一歩先を「プレイヤー」または「プレイヤー(ゴールの上)」に更新
			Map[t0] = (Map[t0] == M["SELF"]) ? M["NONE"] : M["GOAL"];					// プレイヤー位置だった場所を「何もない」または「ゴール」に更新
		}
	}
	
	if(Map[t0] != M["SELF"] && Map[t0] != M["SELF_ON"])									// プレイヤー位置だった場所にプレイヤーがいない(移動した)場合
	{
		j = { j.x + d.x, j.y + d.y };													// プレイヤー位置更新
		bClear = (find(Map.begin(), Map.end(), M["NI"]) == Map.end()) ? true : false;	// ステージ上に1つも「荷物」が無ければ、ステージクリアフラグをtrue
	}
}

void SKBN::draw() const
{
	for(char m : Map) printf("%c",m);		// ステージ描画
	if(bClear) printf("Stage Clear!\n");	// ステージクリアフラグがtrueなら、ステージクリアした旨を表示
}

void SKBN::initSelfPos()
{
	j = {0,0};
	Map = Dat[lv];										// 全ステージデータから現在のステージのデータを取得
	for(char m : Map)
	{
		if(m == M["SELF"] || m == M["SELF_ON"]) break;	// ステージデータからプレイヤー位置を探す
		else if (m == M["RTN"]) j = {0, j.y+1};			// 探している間xy座標を数える
		else j.x++;										// 探している間xy座標を数える
	}
}

bool SKBN::isAllClear()
{
	if(bClear && (++lv != Dat.size()))	// ステージクリアかつ、全てのステージをクリアしていなければ中へ
	{
		bClear = false;					// ステージクリアフラグをfalse
		initSelfPos();					// プレイヤー位置初期化
		draw();							// 画面描画
	}
	return bClear;						// 全クリアならtrue、それ以外ならfalse
}

uint SKBN::pos(int x, int y)	// 指定xy座標に該当するインデックスを一次元データから算出
{
	uint i = 0;
	uint ycnt = 0;
	for(char m : Map)
	{
		if(ycnt == y) break;
		if(m == M["RTN"]) ycnt++;
		i++;
	}
	return (i+x);
}

int main()						// メインルーチン
{
	SKBN skbn;					// 倉庫番インスタンスを生成
	while(!skbn.isAllClear())	// 全ステージクリアまで繰り返す
	{
		skbn.inputkey();		// キー入力
		skbn.update();			// 情報更新
		skbn.draw();			// 画面描画
	}
	return 0;
}
