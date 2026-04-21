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

	DrawGraph(0, 0, Pic.Sel, TRUE);

	//上下のラインを描画
	DrawBox(0, 0, 1000, 50, GetColor(0, 255, 255), TRUE);
	DrawBox(0, 750, 1000, 800, GetColor(0, 255, 255), TRUE);

	// 手札の描画（以前作ったDrawPlayerHandの中身をここに書くか、呼び出す）
	DrawPlayerHand(player);
}

void Battle::DrawPlayerHand(const Player& player) {
	// 手札を取得
	const auto& hand = player.GetHand();

	// 描画設定
	const int START_X = 25;     // 1枚目のX座標
	const int START_Y = 500;     // 手札を表示するY座標（画面下部）
	const int MARGIN = 5;       // カード同士の隙間

	for (int i = 0; i < hand.size(); ++i) {

		// 各カードのX座標を計算
		int x = START_X + (CARD_CELL + MARGIN) * i;
		int y = START_Y;

		// カードごとの画像インデックスを取得
		int picIdx = hand[i].graphicIndex;

		// カードの背景画像を描画
		DrawGraph(x, y, Pic.Card[picIdx], TRUE);
		DrawBox(x - 1, y - 1, x + 50, y + 50, GetColor(0, 0, 0), FALSE);
		

		// 2. カード名を描画
		DrawFormatString(x, y + 100, GetColor(0, 0, 0), L"%s", hand[i].GetName().c_str());
		

		// 3. 属性や威力を描画
		DrawFormatString(x + 5, y + 40, GetColor(255, 0, 0), _T("攻%2d"), hand[i].GetPower());

		// カテゴリを描画 (StringToCategoryで設定したもの)
		DrawFormatString(x + 5, y + 70, GetColor(0, 0, 255), _T("[%s]"), hand[i].GetType().c_str());
	}
}