#pragma once

#define MENBER_MAX 9        // 最大ゲーム可能人数

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

		MENBER,			// 修行と乱闘用の人数変更

		PVP,			// 個人戦参加(乱闘)
		TEAM_RED,		// チームレッドで参加(乱闘)
		TEAM_BLUE,		// チームブルーで参加(乱闘)
		TEAM_YELLOW,	// チームイエローで参加(乱闘)
		TEAM_GREEN,		// チームグリーンで参加(乱闘)

		RANKING,		// ランキングUI表示(真剣勝負用)

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

	void DrawPlayerTeam(const std::string & nameStr, int y) {
		//枠の描画処理
		DrawCircle(300, y, 15, GetColor(0, 0, 0), FALSE);
		DrawCircle(700, y, 15, GetColor(0, 0, 0), FALSE);
		DrawBox(300, y - 15, 700, y + 16, GetColor(0, 0, 0), FALSE);
		DrawCircle(300, y, 14, GetColor(255, 255, 255), TRUE);
		DrawCircle(700, y, 14, GetColor(255, 255, 255), TRUE);
		DrawBox(301, y - 14, 699, y + 15, GetColor(255, 255, 255), TRUE);

		
		const char* name = nameStr.c_str(); // std::string を DXLib 用に変換

		// ユーザーネームのテキスト描画処理
		int centerX = 500;		// 中央揃え用の座標格納変数
		int width = GetDrawStringWidth(name, (int)strlen(name));

		// Y座標も高さに合わせて微調整
		int drawX = centerX - (width / 2);
		int drawY = y - 8;

		DrawString(drawX, drawY, name, GetColor(0, 0, 0));
	}

private:
	int selectedOption = NONE;			// 現在選ばれている選択肢
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
	int fontHandle;						// フォントのサイズとかをいじくる変数
	bool isTeam[MAX];					// その対戦形式になってるかの有無を格納する変数
	bool isBattlePlayer[MENBER_MAX];	// 最大対戦人数分の参加の有無を判定する配列
};

extern Action Act;

