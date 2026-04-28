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

	//選択肢
	enum BattleOption {
		NONE = -1,
		ACTION,
		PLAYER_SEL,		// プレイヤー選択用のカーソル判定列挙体
		RETURN,
		GIVE_UP,		// あきらめるボタンの判定枠
		MAX
	};

	Battle();

	// 更新処理
	bool Update(const MouseState& mouse, const Player& player);

	// 描画処理
	void Draw(const Player& player);

	// プレイヤーの手札描画関数
	void DrawPlayerHand(const Player& player);

	// 選ばれた番号を外に教える関数
	int getSelectedOption() const {
		return selectedOption;
	}

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


	PlayerTurn player_turn[Player_MAX];	// プレイヤーターン分け配列
	int currentTurnIdx;					// 現在のターンプレイヤー添字
	int selectedOption = NONE;			// 現在選ばれている選択肢
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
	int hoveredCardIdx;					// マウスカーソルで選択しているカード番号
};

extern Battle Bat;