#pragma once

#include "DxLib.h"
#include <vector>

struct MouseState;
class Player;
class SelectScene;
class Action;

class Battle
{
	Battle();

	// 属性関係

	// ダメージ計算式

	// 全体ダメージ処理

	// 回復処理

	// ステータス変換処理

	// アイテム購入処理

	// アイテム売却処理

	// 奇跡の処理

	// プレイヤーターン処理

private:
	// どのプレイヤーのターンなのかを分ける列挙体
	enum PlayerTurn {
		Player1 = 0,
		Player2,
		Player3,
		Player4,
		Player5,
		Player6,
		Player7,
		Player8,
		Player9,
		Player_MAX
	};

	//選択肢
	enum BattleOption {
		NONE = -1,
		ACTION,
		PLAYER_SEL,		// プレイヤー選択用のカーソル判定列挙体
		RETURN,
		MAX
	};

	PlayerTurn Player[Player_MAX];		// プレイヤーターン分け配列
	int currentTurnIdx;					// 現在のターンプレイヤー添字
	int selectedOption = NONE;			// 現在選ばれている選択肢
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
};

extern Battle Bat;

