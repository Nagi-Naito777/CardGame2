#pragma once

class Picture
{
public:
	int Tit;		//タイトルシーン変数
	int Sel;		//モードセレクトシーン変数
	int Bat;		//戦闘シーン変数
	int Card[];    // カード画像用の配列

	inline void Read() {
		Tit = LoadGraph("./resource/Title.png");

		// 2. もし読み込みに失敗していたらログを出す（デバッグ用）
		if (Tit == -1) {
			printfDx("Title.png の読み込みに失敗しました\n");
		}
		Sel = LoadGraph("./resource/BackGround.png");
		Bat = LoadGraph("");
		//LoadDivGraph("./resource/puchimon.png", CARD_KIND, 20, 5, CARD_CELL, CARD_CELL, Card);
	}
private:
};

extern Picture Pic;

