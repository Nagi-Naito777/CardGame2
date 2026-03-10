#include "Action.h"
#include "DxLib.h"
#include "Picture.h"
#include "Select.h"
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Player.h"         //プレイヤークラスヘッダー

//AI対戦モード時の更新処理
bool Action::Traning_Update(const MouseState& mouse) {
    if (mouse.leftClicked) {
        if (isHoverIdx[0]) { selectedOption = 0; }  // 選択された項目を保存
        else if (isHoverIdx[6]) { selectedOption = 6; }
        return true;        // 選択されたので次のシーンへ（または処理確定）
    }
    return false;
}

//プレイヤー対戦モード時の更新処理
bool Action::PvP_Update(const MouseState& mouse) {
    // チーム分けのボタン判定枠
    int btnW = 300;
    int btnH = 50;
    int startX = 800;

    for (int i = TEAM_RED; i < TEAM_GREEN; i++) {
        // y座標をずらして判定
        isHoverIdx[i] = IsMouseOver(startX, 200 + (i * 70), btnW, btnH, mouse);

        if (mouse.leftClicked && isHoverIdx[i]) {
            selectedOption = i; // 選択された項目を保存
            return true;        // 選択されたので次のシーンへ（または処理確定）
        }
    }

    return false;
}

void Action::Draw(const Player& player, int sceneValue) {
    // モードによって1番目のラベルを変える
    SelectScene::Option scene = static_cast<SelectScene::Option>(sceneValue);
    
    //三項演算子でそれぞれの分岐表示
    const char* firstLabel = (scene == SelectScene::Option::TRANING) ? "修行モード" : "プレイヤー対戦モード";
    const char* secondLabel = (scene == SelectScene::Option::TRANING) ? "修行開始" : "対戦開始";

    DrawGraph(0, 0, Pic.Sel, TRUE);
    DrawFormatString(10, 10, GetColor(0, 0, 0), "Name: %s", player.getName().c_str());
    DrawString(100, 100, firstLabel, GetColor(0, 0, 0));
    DrawString(450, 500, secondLabel, GetColor(0, 0, 0));



}
