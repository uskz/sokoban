using System;
using System.IO;
using System.Collections.Generic;

namespace SokobanCS
{
    class Sokoban
    {
        (int x, int y) j = (0, 0);                              // プレイヤー座標情報
        (int x, int y) d = (0, 0);                              // 移動方向情報
        int lv = 0;                                             // 現在のステージ数
        bool bClear = false;                                    // ステージクリアフラグ
        List<char> Map = new List<char>();                      // 現在のステージデータ
        readonly List<List<char>> Dat = new List<List<char>>(); // 全ステージデータ
        readonly Dictionary<string, char> M = new Dictionary<string, char>()
        {
            {"NONE",    ' '},	// 何もない
            {"WALL",    '#'},	// 壁
            {"NI",      'o'},	// 荷物
            {"NI_ON",   'O'},	// 荷物(ゴールの上)
            {"SELF",    't'},	// プレイヤー
            {"SELF_ON", 'T'},	// プレイヤー(ゴールの上)
            {"GOAL",    '.'},	// ゴール
            {"RTN",     '\n'},	// 改行
            {"END",     'E'},	// ステージの終わり
        };

        public Sokoban()
        {
            Dat.Add(new List<char>());                              // メンバ変数初期化
            using (var r = new StreamReader(@".\dat_skbn.txt"))     // ステージデータファイル読み込み
            {
                foreach (var t in r.ReadToEnd())
                {
                    if (t == M["END"]) Dat.Add(new List<char>());   // ステージを最後まで読んだら次のステージ枠確保
                    else Dat[Dat.Count-1].Add(t);                   // ステージデータ格納
                }
            }
            InitSelfPos();                                          // プレイヤー位置初期化
            Draw();													// 画面描画
        }

        public bool IsAllClear()
        {
            if (bClear && (++lv != Dat.Count))  // 全ステージをクリアしていないか
            {
                bClear = false;                 // ステージクリアフラグをfalse
                InitSelfPos();                  // プレイヤー位置初期化
                Draw();                         // 画面描画
            }
            return bClear;						// 全ステージをクリアしていればtrueを返す
        }

        public void Inputkey()
        {
            switch (Console.ReadKey(true).Key)   // 入力取得
            {
                case ConsoleKey.W: d = (0, -1); break;  // 移動量を設定：上
                case ConsoleKey.S: d = (0, 1); break;   // 移動量を設定：下
                case ConsoleKey.D: d = (1, 0); break;   // 移動量を設定：右
                case ConsoleKey.A: d = (-1, 0); break;  // 移動量を設定：左
                default: Environment.Exit(0); break;    // それ以外の入力はとりあえずプログラム終了
            }
        }

        public void Update()
        {
            int t0 = Pos(j.x, j.y);                                             // プレイヤー位置インデックス取得
            int t1 = Pos(j.x + d.x, j.y + d.y);                                 // 一歩先インデックス取得

            if (Map[t1] == M["GOAL"] || Map[t1] == M["NONE"])                   // 一歩先が「ゴール」または「何もない」
            {
                Map[t1] = (Map[t1] == M["NONE"]) ? M["SELF"] : M["SELF_ON"];    // 一歩先を「プレイヤー」または「プレイヤー(ゴールの上)」に更新
                Map[t0] = (Map[t0] == M["SELF"]) ? M["NONE"] : M["GOAL"];       // プレイヤー位置だった場所を「何もない」または「ゴール」に更新
            }
            else if (Map[t1] == M["NI"] || Map[t1] == M["NI_ON"])               // 一歩先が「荷物」または「荷物(ゴールの上)」
            {
                int t2 = Pos(j.x + d.x * 2, j.y + d.y * 2);                     // 二歩先インデックス取得
                if (Map[t2] == M["GOAL"] || Map[t2] == M["NONE"])               // 二歩先が「ゴール」または「何もない」
                {
                    Map[t2] = (Map[t2] == M["NONE"]) ? M["NI"] : M["NI_ON"];    // 二歩先を「荷物」または「荷物(ゴールの上)」に更新
                    Map[t1] = (Map[t1] == M["NI"]) ? M["SELF"] : M["SELF_ON"];  // 一歩先を「プレイヤー」または「プレイヤー(ゴールの上)」に更新
                    Map[t0] = (Map[t0] == M["SELF"]) ? M["NONE"] : M["GOAL"];   // プレイヤー位置だった場所を「何もない」または「ゴール」に更新
                }
            }

            if (Map[t0] != M["SELF"] && Map[t0] != M["SELF_ON"])    // プレイヤー位置だった場所にプレイヤーがいない(移動した)場合
            {
                j = (j.x + d.x, j.y + d.y);                         // プレイヤー位置更新
                bClear = (0 > Map.IndexOf(M["NI"]));                // ステージ上に1つも「荷物」が無ければ、ステージクリアフラグをtrue
            }
        }

        public void Draw()
        {
            foreach (char m in Map) Console.Write(m);       // ステージ描画
            if (bClear) Console.WriteLine("Stage Clear!");	// ステージクリアフラグがtrueなら、ステージクリアした旨を表示
        }

        void InitSelfPos()      // プレイヤー位置初期化
        {
            j = (0, 0);
            Map = Dat[lv];      // 全ステージデータから現在のステージのデータを取得
            foreach (char m in Map)
            {
                if (m == M["SELF"] || m == M["SELF_ON"]) break; // ステージデータからプレイヤー位置を探す
                else if (m == M["RTN"]) j = (0, j.y + 1);       // 探している間xy座標を数える
                else j.x++;                                     // 探している間xy座標を数える
            }
        }

        int Pos(int x, int y)  // 指定xy座標に相当するインデックスを返す
        {
            int i = 0;
            uint ycnt = 0;
            foreach (char m in Map)
            {
                if (ycnt == y) break;
                if (m == M["RTN"]) ycnt++;
                i++;
            }
            return (i + x);
        }
    }

    class Skbn
    {
        static void Main()
        {
            var skbn = new Sokoban();   // 倉庫番インスタンスを生成
            while (!skbn.IsAllClear())  // 全ステージクリアまで繰り返す
            {
                skbn.Inputkey();        // キー入力
                skbn.Update();          // 情報更新
                skbn.Draw();            // 画面描画
            }
        }
    }
}
