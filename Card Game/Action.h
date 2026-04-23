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

		BATTLE_START,	// バトル開始

		MEMBER,			// 修行と乱闘用の人数変更

		PVP,			// 個人戦参加(乱闘)
		TEAM_RED,		// チームレッドで参加(乱闘)
		TEAM_BLUE,		// チームブルーで参加(乱闘)
		TEAM_YELLOW,	// チームイエローで参加(乱闘)
		TEAM_GREEN,		// チームグリーンで参加(乱闘)

		RANKING,		// ランキングUI表示(真剣勝負用)

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

	// 乱闘モードの枠に個人・チームごとの色の枠にして表示する関数
	void DrawPlayerTeam(const std::string & nameStr, int y) {
		//枠の描画処理
		DrawCircle(300, y, 15, GetColor(0, 0, 0), FALSE);
		DrawCircle(700, y, 15, GetColor(0, 0, 0), FALSE);
		DrawBox(300, y - 15, 700, y + 16, GetColor(0, 0, 0), FALSE);
		DrawCircle(300, y, 14, GetColor(255, 255, 255), TRUE);
		DrawCircle(700, y, 14, GetColor(255, 255, 255), TRUE);
		DrawBox(301, y - 14, 699, y + 15, GetColor(255, 255, 255), TRUE);

		// std::string を DXLib 用に変換
		const char* name = nameStr.c_str(); 

		// ユーザーネームのテキスト描画処理
		int centerX = 500;		// 中央揃え用の座標格納変数
		int width = GetDrawStringWidth(name, (int)strlen(name));

		// Y座標も高さに合わせて微調整
		int drawX = centerX - (width / 2);
		int drawY = y - 8;

		

		DrawString(drawX, drawY, name, GetColor(0, 0, 0));
	}

	// 背景を若干黒くするDrawBox関数
	void BlackDrawBox(int x, int y, int x2, int y2) {
		// 半透明に設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);

		// 四角形を描画
		DrawBox(x, y, x2, y2, GetColor(0, 0, 0), TRUE);

		// 元に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	//所属チームの部分だけtrueで返す自作関数
	bool IsTeamAdd(int IsTeam) {
		// 配列の中身を初期化
		for (int i = 0; i < MAX; i++) {
			isTeam[i] = false;
		}
		for (int i = 0; i < MAX; i++) {
			if (i == IsTeam) { return true; }
		}
	}

private:
	std::vector<Player>BattlePlayer;	// 対戦に参加してる人数
	int selectedOption = NONE;			// 現在選ばれている選択肢
	bool isHoverIdx[MAX];				// 各ボタンの上にマウスがあるか
	bool isHoverIdx2[9];				// 人数選択時のマウスの判定枠
	int fontHandle;						// フォントのサイズとかをいじくる変数
	bool isTeam[MAX];					// その対戦形式になってるかの有無を格納する変数
	bool isBattlePlayer[MENBER_MAX];	// 最大対戦人数分の参加の有無を判定する配列
	bool MemberCustom;					// 対戦人数変更ウィンドウがオンになってるか
	int selectedMemberCount = 2;		// 選択したメンバーの数を格納する変数(初期値は2)
};

extern Action Act;

