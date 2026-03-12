#pragma once

class Picture
{
public:
	int Tit;			// タイトルシーン変数
	int Tit_Button;		// タイトルの開始ボタン
	int Sel;			// モードセレクトシーン変数
	int Bat;			// 戦闘シーン変数
	int Start_Button;	// スタートボタン画像格納変数
	int Card[];			// カード画像用の配列

	inline void Read() {
		Tit = LoadGraph("./resource/Title.png");
		Tit_Button = LoadGraph("./resource/Title_Button.png");

		// もし読み込みに失敗していたらログを出す（デバッグ用）
		if (Tit == -1) {
			printfDx("Title.png の読み込みに失敗しました\n");
		}
		Sel = LoadGraph("./resource/BackGround.png");
		Start_Button = LoadGraph("./resource/Start_Button.png");
		Bat = LoadGraph("");
		//LoadDivGraph("./resource/puchimon.png", CARD_KIND, 20, 5, CARD_CELL, CARD_CELL, Card);
	}

	// 画像の上にマウスが置かれたら色が少し灰色になる関数
	void MouseHoverDraw(int x,int y, int img) {
		SetDrawBright(180, 180, 180);
		DrawGraph(x, y, img, TRUE);
		SetDrawBright(255, 255, 255);
	}

private:
};

extern Picture Pic; // 変数のみ宣言

