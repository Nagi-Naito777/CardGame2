#pragma once

// これらが構造体やクラスであることをコンパイラに教える
struct MouseState;
class Player;

class SelectScene
{
public:
	enum Option {
		NONE = -1,		// 何も選択されてない
		TRANING,		// 修行モード
		PVP,			// プレイヤー対戦モード
		RETURN,			// タイトルに戻る
		MAX				// モード選択最大数
	};

	// 更新処理
	bool Update(const MouseState& mouse);

	// 引数でPlayerの情報を受け取る
	void Draw(const Player& player);

	// 選ばれた番号を外に教える関数
	int getSelectedOption() const {
		return selectedOption;
	}

private:
	int selectedOption = NONE; // 現在選ばれている選択肢
	bool isHoverIdx[MAX];      // 各ボタンの上にマウスがあるか

};

extern SelectScene Sel; // 変数のみ宣言