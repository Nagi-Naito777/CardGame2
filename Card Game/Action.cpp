#include "Action.h"
#include "DxLib.h"
#include "Picture.h"
#include "Select.h"
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Player.h"         //プレイヤークラスヘッダー

/*
* 列挙体の中身のコメントアウト
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
*/

//AI対戦モード時の更新処理
bool Action::Update(const MouseState& mouse) {

    // チーム分けのボタン判定枠
    int btnW = 300;
    int btnH = 50;
    int startX = 800;

    // 列挙体全てをループさせる
    for (int i = 0; i < MAX; i++) {
        // 一個前の画面に戻るボタンは全て共通で置く
        if (i == RETURN) {
            isHoverIdx[i] = IsMouseOver(10, 10, 100, 30, mouse);
        }
        // 修行モードはAIの人数選択ボタンと対戦開始ボタンのみ
        
        // 乱闘モードは個人戦、チーム戦用の計5つのボタンと対戦開始ボタン

        // 真剣勝負モードは1vs1なので、対戦開始ボタンのみ

        else {
            // y座標をずらして判定
            isHoverIdx[i] = IsMouseOver(startX, 50 + (i * 150), btnW, btnH, mouse);
        }
        if (mouse.leftClicked) {
            if (isHoverIdx[0]) { selectedOption = 0; }  // 選択された項目を保存
            else if (isHoverIdx[6]) { selectedOption = 6; }
            return true;        // 選択されたので次のシーンへ（または処理確定）
        }
    }

    for (int i = TEAM_RED; i <= TEAM_GREEN; i++) {
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
    const char* firstLabel;
    if (scene == SelectScene::Option::TRANING) { firstLabel = "修行設定"; }
    else if (scene == SelectScene::Option::PVP) { firstLabel = "乱闘設定"; }
    else { firstLabel = "真剣勝負"; }

    DrawGraph(0, 0, Pic.Sel, TRUE);
    DrawFormatString(10, 770, GetColor(0, 0, 0), "Name: %s", player.getName().c_str());
    FontHandle(10, 50, firstLabel, 64, "ＭＳ 明朝");
    DrawGraph(350, 600, Pic.Start_Button, TRUE);
}
