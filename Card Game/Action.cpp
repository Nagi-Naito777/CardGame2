#include "Action.h"
#include "DxLib.h"
#include "Picture.h"
#include "Select.h"
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Player.h"         //プレイヤークラスヘッダー
#include "FontManager.h"    // フォント設定系ヘッダー

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
bool Action::Update(const MouseState& mouse, int sceneValue) {

    // モードの抽出
    SelectScene::Option scene = static_cast<SelectScene::Option>(sceneValue);

    // チーム分けのボタン判定枠
    int btnW = 200;
    int btnH = 100;
    int startX = 750;

    // 列挙体全てをループさせ、判定を初期化
    for (int i = 0; i < MAX; i++) {
        isHoverIdx[i] = false;
    }

    for (int i = 0; i < MAX; i++) {
        if (i == RETURN) {
            isHoverIdx[i] = IsMouseOver(10, 10, 100, 30, mouse);
        }
        else if (i == BATTLE_START) {
            isHoverIdx[i] = IsMouseOver(350, 550, 300, 150, mouse);
        }
        else {
            switch (scene) {
                // 修行画面の場合
            case SelectScene::Option::TRANING:
               
                break;
                // 乱闘画面
            case SelectScene::Option::PVP:
                if (i >= TEAM_RED && i <= TEAM_GREEN) {
                    // 列挙体の最大値からチームカラー分の数値を引いて初期化
                    int num = i - 2;
                    int y = 100 + (num * 104);
                    isHoverIdx[i] = IsMouseOver(startX, y, btnW, btnH, mouse);
                }
                
                break;
                // その他(タイマン)
            default:

                break;
            }
        }
        // モードごとに異なるボタン判定をするためのswitch

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

    //switchでそれぞれの分岐表示
    const char* firstLabel;
    switch (scene)
    {
    case SelectScene::Option::TRANING:
        firstLabel = "修行";
        break;

    case SelectScene::Option::PVP:
        firstLabel = "乱闘";
        break;

    default:
        firstLabel = "真剣勝負";
        break;
    }

    DrawGraph(0, 0, Pic.Sel, TRUE);
    
    //上下のラインを描画
    DrawBox(0, 0, 1000, 50, GetColor(0, 255, 255), TRUE);
    DrawBox(0, 750, 1000, 800, GetColor(0, 255, 255), TRUE);

    for (int i = 0; i < MAX; i++) {
        if (i == RETURN) {
            //マウスが乗っていたら黄色、そうでなければ白にする処理
            unsigned int color = isHoverIdx[i] ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
            DrawBox(10, 10, 100, 40, color, TRUE);
            DrawBox(9, 9, 101, 41, GetColor(0, 0, 0), FALSE);
        }
        else if (i == BATTLE_START) {
            if (isHoverIdx[i]) { Pic.MouseHoverDraw(350, 551, Pic.Start_Button); }
            else { DrawGraph(350, 550, Pic.Start_Button, TRUE); }
        }
        else {
            switch (scene){
                // 修行画面の場合
            case SelectScene::Option::TRANING:
                
                break;
                // 乱闘画面
            case SelectScene::Option::PVP:
                if (i >= TEAM_RED && i <= TEAM_GREEN) {
                    // 画像配列番号格納変数
                    int num = i - 2;
                    int y = 100 + (num * 104);

                    if (isHoverIdx[i]) { Pic.MouseHoverDraw(750, y + 1, Pic.Team_Button[num]); }
                    else { DrawGraph(750, y, Pic.Team_Button[num], TRUE); }
                }
                
                break;
                // その他(タイマン)
            default:

                break;
            }
        }
    }
    DrawString(37, 17, "戻る", GetColor(0, 0, 0));

    // 名前表示
    DrawFormatStringToHandle(
        10, 770,
        GetColor(0, 0, 0),
        Font.Small,
        "Name: %s",
        player.getName().c_str()
    );

    // 設定されたモード名表示
    DrawStringToHandle(110, 10, firstLabel, GetColor(0, 0, 0), Font.Normal);
}
