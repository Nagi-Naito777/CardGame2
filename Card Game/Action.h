#pragma once
#include "DxLib.h"

struct MouseState;
class Player;
class SelectScene;

class Action
{
public:
	//詳細設定列挙体
	enum BattleOption {
		NONE = -1,		// 何も選択されてない

		MENBER,			// AI人数選択

		PVP,			// 個人戦参加(PVPのみ)
		TEAM_RED,		// チームレッドで参加(PVPのみ)
		TEAM_BLUE,		// チームブルーで参加(PVPのみ)
		TEAM_YELLOW,	// チームイエローで参加(PVPのみ)
		TEAM_GREEN,		// チームグリーンで参加(PVPのみ)

		BATTLE_START,	// バトル開始
		RETURN,			// 一個前の画面に戻る
		MAX				// 詳細設定選択最大数
	};

	// 更新処理
	bool Update(const MouseState& mouse, int sceneValue);

	//描画処理
	void Draw(const Player& player, int sceneValue);

	// 選ばれた番号を外に教える関数
	int getSelectedOption() const {
		return selectedOption;
	}


private:
	int selectedOption = NONE;			// 現在選ばれている選択肢
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
	int fontHandle;						// フォントのサイズとかをいじくる変数
	
};

extern Action Act;

