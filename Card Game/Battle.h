#pragma once

#include "DxLib.h"
#include "Player.h"
#include "Card.h"
#include <vector>

struct MouseState;
class Player;
class SelectScene;
class Action;
class Picture;

class Battle
{
public:
	Battle();

	void Draw(const Player& player);

	void DrawPlayerHand(const Player& player);

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

	// 攻防ターンを分ける列挙体
	enum class ActionTurn {
		Attack = 0,		// 攻撃ターン
		Defense			// 防御ターン
	};

	//選択肢
	enum BattleOption {
		NONE = -1,
		ACTION,
		PLAYER_SEL,		// プレイヤー選択用のカーソル判定列挙体
		RETURN,
		MAX
	};

	PlayerTurn player_turn[Player_MAX];	// プレイヤーターン分け配列
	int currentTurnIdx;					// 現在のターンプレイヤー添字
	int selectedOption = NONE;			// 現在選ばれている選択肢
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
};

extern Battle Bat;

