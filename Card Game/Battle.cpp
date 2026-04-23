#include <iostream>
#include <string>
#include <random>
#include <vector>
#include "DxLib.h"
#include "FontManager.h"    // フォント設定系ヘッダー
#include "Player.h"         // プレイヤークラスヘッダー
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Picture.h"        // 写真関係ヘッダー
#include "Card.h"           // カード関係ヘッダー
#include "Title.h"          // タイトルシーンヘッダー
#include "Select.h"         // モードセレクトシーンヘッダー
#include "Action.h"         // バトル詳細設定シーンヘッダー
#include "Battle.h"

// コンストラクタの実体
Battle::Battle() : currentTurnIdx(0), selectedOption(NONE) {
	for (int i = 0; i < MAX; i++) {
		isHoverIdx[i] = false;
	}
}

void Battle::Draw(const Player& player) {

	DrawGraph(0, 0, Pic.Bat, TRUE);

	//上下のラインを描画
	DrawBox(0, 0, 1000, 50, GetColor(0, 255, 255), TRUE);
	DrawBox(0, 750, 1000, 800, GetColor(0, 255, 255), TRUE);

	// 手札の描画（以前作ったDrawPlayerHandの中身をここに書くか、呼び出す）
	DrawPlayerHand(player);

	// 名前表示
	DrawFormatStringToHandle(
		10, 770,
		GetColor(0, 0, 0),
		Font.Small,
		_T("Name: %s"),
		player.getName().c_str()
	);
}

void Battle::DrawPlayerHand(const Player& player) {
	// 手札を取得
	const auto& hand = player.GetHand();

	// 描画設定
	const int START_X = 25;     // 1枚目のX座標
	const int START_Y = 500;     // 手札を表示するY座標（画面下部）
	const int MARGIN = 5;       // カード同士の隙間

	// 改行用の変数
	const int MAX_CARDS_PER_ROW = 10;   // 1段に並べる最大枚数
	const int ROW_SPACING = 75;        // 段ごとの縦の間隔（カードの高さ＋余白）

	for (int i = 0; i < hand.size(); ++i) {

		int col = i % MAX_CARDS_PER_ROW; // 列番号 (0?9)
		int row = i / MAX_CARDS_PER_ROW; // 行番号 (10枚目までは0、11枚目からは1)

		// 列(col)を使ってX座標を、行(row)を使ってY座標を計算する
		int x = START_X + (CARD_CELL + MARGIN) * col;
		int y = START_Y + (ROW_SPACING * row);

		// カードごとの画像インデックスを取得
		int picIdx = hand[i].graphicIndex;

		// カードの背景画像を描画
		DrawGraph(x, y, Pic.Card[picIdx], TRUE);
		DrawBox(x, y, x + 50, y + 50, GetColor(0, 0, 0), FALSE);
		if (hand[i].GetCategory() == Attack ||
			hand[i].GetCategory() == Defense) {
			DrawBox(x, y+50, x + 50, y + 70, GetColor(255, 255, 200), TRUE);
		}

		// 2. カード名を描画
		//DrawFormatString(x, y + 100, GetColor(0, 0, 0), L"%s", hand[i].GetName().c_str());
		

		// 属性ごとにフォントの色を分ける処理
		int Col = GetColor(0, 0, 0);
		if(hand[i].GetType() == "炎"){ Col = GetColor(255, 0, 0); }
		else if (hand[i].GetType() == "水") { Col = GetColor(0, 0, 255); }
		else if (hand[i].GetType() == "木") { Col = GetColor(0, 155, 0); }
		else if (hand[i].GetType() == "光") { Col = GetColor(155, 155, 0); }
		else if (hand[i].GetType() == "闇") { Col = GetColor(255, 100, 255); }

		// 攻撃と防御カードの時に文字を表示させる処理
		switch (hand[i].GetCategory()) {
			int w;
		case Attack:
			if(hand[i].GetAdd()){
				w = GetDrawFormatStringWidth(_T("+攻%d"), hand[i].GetPower());
				DrawFormatString(x + (50 - w) / 2, y + 51, Col, _T("+攻%d"), hand[i].GetPower());
			}
			else {
				w = GetDrawFormatStringWidth(_T("攻%d"), hand[i].GetPower());
				DrawFormatString(x + (50 - w) / 2, y + 51, Col, _T("攻%d"), hand[i].GetPower()); }
			break;
		case Defense:
			w = GetDrawFormatStringWidth(_T("守%d"), hand[i].GetPower());
			DrawFormatString(x + (50 - w) / 2, y + 51, Col, _T("守%d"), hand[i].GetPower());
		default:
			break;
		}
		

		// カテゴリを描画 (StringToCategoryで設定したもの)
		//DrawFormatString(x + 5, y + 70, GetColor(0, 0, 255), _T("[%s]"), hand[i].GetType().c_str());
	}
}