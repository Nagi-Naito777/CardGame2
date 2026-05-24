#pragma once

// プレイヤー最大人数
#define PLAYER_MAX 9

#include "DxLib.h"
#include "Player.h"
#include "Card.h"
#include <vector>
#include "BattlePhase.h"

// 前方宣言
struct TotalAttack;
struct MouseState;
class Player;
class SelectScene;
class Action;
class Picture;
class BattleInput;
class BattleAI;

class Battle
{
	friend class BattleInput;	// BattleInputからのアクセスを許可
	friend class BattleAI;		// BattleAIからのアクセスを許可
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

	// 攻防ターンを分ける列挙体
	enum class ActionTurn {
		Attack = 0,		// 攻撃ターン
		Defense			// 防御ターン
	};

	Battle();

	void Initialize(const std::vector<Player>& players);

	// 更新処理
	bool Update(const MouseState& mouse, const Player& player);

	// 描画処理
	void Draw(const Player& player);

	void NextTurn();

	void RemovePlayer(int targetIdx);

	Player& GetCurrentPlayer();

	// そのプレイヤーのターン時に名前を表示する関数
	void DrawTurnPlayerName(const Player&player);


	// 選ばれた番号を外に教える関数
	int getSelectedOption() const {
		return selectedOption;
	}

	TotalAttack CalculateTotalAttack(Player& attacker);

	void ResolveDamage(Player& target, const TotalAttack& attack, const Card* defenseCard);

	void SetAIAction(int cardIdx, int target) {
		selectedCards.push_back(cardIdx);
		targetIdx = target;
		currentPhase = BattlePhase::Reveal;
	}

private:

	std::vector<Player>Player_Turn;			// プレイヤーのターンを分別する変数
	int currentTurnIdx;						// 現在のターンプレイヤー添字
	int selectedOption = NONE;				// 現在選ばれている選択肢
	int selectCard = -1;					// 現在選ばれてるカード
	std::vector<int> selectedCards;			// 選んだ手札のインデックスを順番に格納
	std::vector<int> selectedDefenseCards;	// 防御側が選択したカードのインデックス
	int totalPower = 0;						// 重ね掛けした合計威力
	int selectPlayer;						// 現在選ばれてるプレイヤー
	bool isHoverIdx[MAX];					// 各ボタンの上にマウスがあるか
	bool isHoverCardIdx[CARD_MAX];			// カード枠の上にマウスがあるか
	bool isHoverPlayerIdx[PLAYER_MAX];		// どのプレイヤー枠の上にマウスがあるか
	bool playerTarget = false;				// プレイヤーを指定したかどうか
	int hoveredCardIdx;						// マウスカーソルで選択しているカード番号
	int targetIdx;							// マウスでホバーしたり選択した相手の番号
	bool isSurrenderConfirm;				// あきらめる確認ウィンドウが開いているか


	// 現在のバトルフェーズを格納する変数
	BattlePhase currentPhase = BattlePhase::Idle;

	int animationTimer = 0;					// 演出の残り時間をカウントする
	int damageDisplay = 0;					// 表示するダメージ量
	bool isDefenseSuccess = false;			// 防御成功フラグ

	// カード公開アニメーション用の変数
	int revealIndex = 0;					// アニメーションで現在何枚目まで表示したか

	// カード枠の現在の間隔（アニメーション用。初期値は65）
	float currentYOffset = 65.0f;

	std::string currentAttackElement = "無";// 現在選択中の攻撃属性（初期値は無属性）

	// 加算カードが個別にキャンセルされた時に属性を再計算する関数
	void RecalculateAttackElement(const std::vector<Card>& hand);
};

extern Battle Bat;