#pragma once

// プレイヤー最大人数
#define PLAYER_MAX 9

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
		ATTACK,			// 攻撃決定判定枠
		DEFENSE,		// 防御決定判定枠
		RETURN,
		GIVE_UP,		// あきらめるボタンの判定枠
		MAX
	};

	Battle();

	void Initialize(const std::vector<Player>& players);

	// 更新処理
	bool Update(const MouseState& mouse, const Player& player);

	// 描画処理
	void Draw(const Player& player);

	// プレイヤーステータス描画関数
	void DrawPlayerStatus(const std::vector<Player>& players);

	void NextTurn();

	void RemovePlayer(int targetIdx);

	Player& GetCurrentPlayer();

	// そのプレイヤーのターン時に名前を表示する関数
	void DrawTurnPlayerName(const Player&player);

	// ターゲットになったプレイヤーの名前を表示
	void DrawTargetPlayerName(const Player& player);

	// プレイヤーの手札描画関数
	void DrawPlayerHand(const Player& player);

	// 選択したカードを自分の名前の下に描画する処理
	void DrawSelectedCard(const Player& player);

	// 選ばれた番号を外に教える関数
	int getSelectedOption() const {
		return selectedOption;
	}

private:

	// 攻防ターンを分ける列挙体
	enum class ActionTurn {
		Attack = 0,		// 攻撃ターン
		Defense			// 防御ターン
	};

	enum class BattlePhase {
		Idle,       // 何もしていない（通常表示）
		Select,     // カード選択中（攻撃・防御の制限をかける）
		Action      // 行動演出中（通常表示、またはクリック不可）
	};

	std::vector<Player>Player_Turn;		// プレイヤーのターンを分別する変数
	int currentTurnIdx;					// 現在のターンプレイヤー添字
	int selectedOption = NONE;			// 現在選ばれている選択肢
	int selectCard = -1;				// 現在選ばれてるカード
	int selectPlayer;					// 現在選ばれてるプレイヤー
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
	bool isHoverCardIdx[CARD_MAX];		// カード枠の上にマウスがあるか
	bool isHoverPlayerIdx[PLAYER_MAX];	// どのプレイヤー枠の上にマウスがあるか
	bool playerTarget = false;			// プレイヤーを指定したかどうか
	int hoveredCardIdx;					// マウスカーソルで選択しているカード番号
	int targetIdx;						// マウスでホバーしたり選択した相手の番号

	// 現在のバトルフェーズを格納する変数
	BattlePhase currentPhase = BattlePhase::Idle;
};

extern Battle Bat;