#include "DxLib.h"
#include "Picture.h"
#include "Select.h"
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Player.h"         //プレイヤークラスヘッダー

bool SelectScene::Update(const MouseState& mouse) {
    // ボタンの配置（例：画面中央付近に縦に並べる）
    int btnW = 600;
    int btnH = 100;
    int startX = 200;

    for (int i = 0; i < MAX; i++) {
        if (i == RETURN) {
            isHoverIdx[i] = IsMouseOver(10, 10, 100, 30, mouse);
        }
        else {
            // y座標をずらして判定
            isHoverIdx[i] = IsMouseOver(startX, 50 + (i * 150), btnW, btnH, mouse);
        }

        if (mouse.leftClicked && isHoverIdx[i]) {
            selectedOption = i; // 選択された項目を保存
            return true;        // 選択されたので次のシーンへ（または処理確定）
        }
    }

    return false;
}

// 引数でPlayerの情報を受け取る
void SelectScene::Draw(const Player& player) {
    //背景画像
    DrawGraph(0, 0, Pic.Sel, TRUE);

    //下のラインを描画
    DrawBox(0, 750, 1000, 800, GetColor(0, 255, 255), TRUE);

    //入力した名前を表示
    DrawFormatString(10, 770, GetColor(0, 0, 0), "Name: %s", player.getName().c_str());
    int startX = 200;

    for (int i = 0; i < MAX; i++) {
        int y = 50 + (i * 150);
        
        if (i == RETURN) {
            //マウスが乗っていたら黄色、そうでなければ白にする処理
            unsigned int color = isHoverIdx[i] ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
            DrawBox(10, 10, 100, 40, color, TRUE);
            DrawBox(9, 11, 101, 41, GetColor(0, 0, 0), FALSE);
        }
        else {
            if (isHoverIdx[i]) { Pic.MouseHoverDraw(startX, y + 1, Pic.Sel_Button[i]); }
            else { DrawGraph(startX, y, Pic.Sel_Button[i], TRUE); }
        }
    }
    DrawString(37, 17, "戻る", GetColor(0, 0, 0));
}