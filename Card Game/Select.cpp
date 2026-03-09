#include "DxLib.h"
#include "Picture.h"
#include "Select.h"
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Player.h"         //プレイヤークラスヘッダー

bool SelectScene::Update(const MouseState& mouse) {
    // ボタンの配置（例：画面中央付近に縦に並べる）
    int btnW = 300;
    int btnH = 50;
    int startX = 350;

    for (int i = 0; i < MAX; i++) {
        // y座標をずらして判定
        isHoverIdx[i] = IsMouseOver(startX, 200 + (i * 70), btnW, btnH, mouse);

        if (mouse.leftClicked && isHoverIdx[i]) {
            selectedOption = i; // 選択された項目を保存
            return true;        // 選択されたので次のシーンへ（または処理確定）
        }
    }

    return false;
}

// 引数でPlayerの情報を受け取る
void SelectScene::Draw(const Player& player) {
    DrawGraph(0, 0, Pic.Sel, TRUE);
    DrawFormatString(10, 10, GetColor(0, 0, 0), "Name: %s", player.getName().c_str());

    const char* label[] = { "修行", "対人戦", "もどる" };
    int btnW = 300;
    int btnH = 50;
    int startX = 350;

    for (int i = 0; i < MAX; i++) {
        int y = 200 + (i * 70);

        // マウスが乗っていたら黄色、そうでなければ白
        unsigned int color = isHoverIdx[i] ? GetColor(255, 255, 100) : GetColor(255, 255, 255);

        DrawBox(startX, y, startX + btnW, y + btnH, color, TRUE);      // ボタン本体
        DrawBox(startX, y, startX + btnW, y + btnH, GetColor(0, 0, 0), FALSE);  // 枠線

        // 文字を中央付近に表示
        DrawString(startX + 80, y + 15, label[i], GetColor(0, 0, 0));
    }
}