#include <string>
#include <vector>
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
bool Action::Update(const MouseState& mouse, int sceneValue,const Player&player) {

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
            if (!MemberCustom) {
                isHoverIdx[i] = IsMouseOver(350, 550, 300, 150, mouse);
            }
        }
        else {
            switch (scene) {
                // 修行画面の場合
            case SelectScene::Option::TRANING:
                // メンバー変更用
                if (i == MEMBER) {
                    if (MemberCustom && mouse.leftClicked) {
                        // 白いボックスの範囲：x(200～850), y(150～525)
                        // IsMouseOverの引数は (x, y, width, height) なので注意
                        if (!IsMouseOver(200, 150, 650, 375, mouse)) {
                            MemberCustom = false; // 範囲外なら閉じる
                        }
                    }

                    if (!MemberCustom) {
                        isHoverIdx[i] = IsMouseOver(200, 250, 600, 100, mouse);
                    }
                    else {
                        // ウィンドウが開いている間はホバー判定を強制的にfalseにする
                        isHoverIdx[i] = false;
                    }
                    // もじ対戦人数変更ウィンドウがtrueになったら
                    if (MemberCustom) {
                        int j = 0;
                        int member_num = 2; // 2人からスタート
                        for (int y = 0; y < 3; y++) {
                            for (int x = 0; x < 3; x++) {
                                if (member_num > MENBER_MAX) break;

                                int PosX = 250 + (x * 200);
                                int PosY = 200 + (y * 100);


                                isHoverIdx2[j] = IsMouseOver(PosX, PosY, 150, 80, mouse);
                                if (mouse.leftClicked && isHoverIdx2[j]) { // 判定枠内でのクリックかチェック
                                    selectedMemberCount = member_num;
                                    MemberCustom = false; // 選択したら閉じる
                                }
                                member_num++;
                                j++;
                            }
                            
                        }
                    }
                }

                break;
                // 乱闘画面
            case SelectScene::Option::PVP:
                if (i >= TEAM_RED && i <= TEAM_GREEN) {
                    // 列挙体の最大値からチームカラー分の数値を引いて初期化
                    int num = i - 3;
                    // ループを回すためのY座標計算式
                    int y = 100 + (num * 104);
                    isHoverIdx[i] = IsMouseOver(startX, y, btnW, btnH, mouse);
                }
                // 個人での参加を判定するボタン用
                else if (i == PVP) { isHoverIdx[i] = IsMouseOver(50, 100, btnW, btnH, mouse); }
                break;
                // タイマン(真剣勝負)
            case SelectScene::Option::TAIMAN:
                // ランキングUI表示用
                if (i == RANKING) { isHoverIdx[i] = IsMouseOver(50, 100, btnW, btnH, mouse); }
            default:

                break;
            }
        }
        // モードごとに異なるボタン判定をするためのswitch
        
        if (mouse.leftClicked && isHoverIdx[i]) {
            selectedOption = i; // 選択された項目を保存
            switch (selectedOption) {
            case BATTLE_START:
                // 1. 以前のデータをクリア（やり直し対策）
                BattlePlayer.clear();

                // 2. 1人目：自分（人間）を追加
                {
                    Player user;
                    user.setName(player.getName()); // 引数からプレイヤー名を取得
                    user.setControllerType(ControllerType::HUMAN);
                    BattlePlayer.push_back(user);
                }

                // 3. 2人目以降：AIを追加
                // iを1から開始して selectedMemberCount 未満まで回す
                for (int i = 1; i < selectedMemberCount; i++) {
                    Player ai;
                    // 名前を "AI 1", "AI 2"... と設定
                    std::string aiName = "AI " + std::to_string(i);
                    ai.setName(aiName.c_str());
                    ai.setControllerType(ControllerType::AI);
                    BattlePlayer.push_back(ai);
                }

                // 4. バトルシーンへ遷移
                return true;
                
            case MEMBER:
                MemberCustom = true;
                break;
            case PVP:
                isTeam[i] = IsTeamAdd(i);
                break;
            case TEAM_RED:
                isTeam[i] = IsTeamAdd(i);
                break;
            case TEAM_BLUE:
                isTeam[i] = IsTeamAdd(i);
                break;
            case TEAM_YELLOW:
                isTeam[i] = IsTeamAdd(i);
                break;
            case TEAM_GREEN:
                isTeam[i] = IsTeamAdd(i);
                break;
            default:
                MemberCustom = false;
                return true;        // 選択されたので次のシーンへ（または処理確定）
                break;
            }

        }
    }
    return false;
}

void Action::Draw(const Player& player, int sceneValue) {

    // ループで人数分trueにする
    for (int i = 0; i < MENBER_MAX; i++) {
        if (BattlePlayer.size() < i)
            isBattlePlayer[i] = true;
    }

    // モードによって1番目のラベルを変える
    SelectScene::Option scene = static_cast<SelectScene::Option>(sceneValue);

    //switchでそれぞれの分岐表示
    const char* firstLabel;
    switch (scene)
    {
    case SelectScene::Option::TRANING:
        firstLabel = _T("修行");
        break;

    case SelectScene::Option::PVP:
        firstLabel = _T("乱闘");
        break;

    default:
        firstLabel = _T("真剣勝負");
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
                if (i == MEMBER) {
                    if (isHoverIdx[i]) {
                        Pic.MouseHoverDraw(200, 251, Pic.AI_Button);
                        DrawFormatStringToHandle(
                            275, 266,
                            GetColor(0, 0, 0),
                            Font.Big,
                            _T("対戦人数 : %d人"),
                            selectedMemberCount
                        );
                    }
                    else {
                        DrawGraph(200, 250, Pic.AI_Button, TRUE);
                        // 選択された人数の数を表示
                        DrawFormatStringToHandle(
                            275, 265,
                            GetColor(0, 0, 0),
                            Font.Big,
                            _T("対戦人数 : %d人"),
                            selectedMemberCount
                        );
                    }
                    // もじ対戦人数変更ウィンドウがtrueになったら
                    if (MemberCustom) {
                        // 裏描画を黒くするためのBlackDrawBox(自作関数)を配置
                        Act.BlackDrawBox(0, 50, 1000, 750);
                        // 白い四角を下地に描画
                        DrawBox(200, 150, 850, 525, GetColor(255, 255, 255), TRUE);
                        // 対戦人数初期値
                        int member_num = 2;
                        int j = 0;
                        // 3×3サイズのループを作成(ただし最後の一枠は無し)
                        for (int y = 0; y < 3; y++) {
                            for (int x = 0; x < 3; x++) {
                                // 最大人数を越したら表示ループ終了
                                if (member_num > MENBER_MAX) { break; }
                                // 画像表示の原点を決める
                                int PosX = 250 + (x * 200);
                                int PosY = 200 + (y * 100);
                                // 画像表示

                                if (isHoverIdx2[j]) {
                                    Pic.MouseHoverDraw(PosX, PosY + 1, Pic.Member);
                                    DrawFormatStringToHandle(
                                        PosX + 30, PosY + 6,
                                        GetColor(0, 0, 0),
                                        Font.Big,
                                        _T("%d人"),
                                        member_num
                                    );
                                }
                                else {
                                    DrawGraph(PosX, PosY, Pic.Member, TRUE);
                                    // 選択人数の表示
                                    DrawFormatStringToHandle(
                                        PosX + 30, PosY + 5,
                                        GetColor(0, 0, 0),
                                        Font.Big,
                                        _T("%d人"),
                                        member_num
                                    );
                                }
                                // ループ用に最後に1ずつ増やす
                                member_num++;
                                j++;
                            }
                        }
                    }
                }
                break;
                // 乱闘画面
            case SelectScene::Option::PVP:
                // 4チーム分のループ
                if (i >= TEAM_RED && i <= TEAM_GREEN) {
                    // 画像配列番号格納変数
                    int num = i - 3;
                    int y = 100 + (num * 104);

                    if (isHoverIdx[i]) { Pic.MouseHoverDraw(750, y + 1, Pic.Team_Button[num]); }
                    else { DrawGraph(750, y, Pic.Team_Button[num], TRUE); }
                }
                else if (i == PVP) {
                    if (isHoverIdx[i]) { Pic.MouseHoverDraw(50, 100, Pic.Solo_Button); }
                    else { DrawGraph(50, 100, Pic.Solo_Button, TRUE); }
                }
                // 最大参加人数分の枠をループ処理
                for (int j = 0; j < MENBER_MAX; j++) {
                    if (isBattlePlayer[j]) {
                        DrawPlayerTeam(player.getName(), 100 + (j * 40));
                    }
                    else {
                        DrawPlayerTeam(_T(" "), 100 + (j * 40));
                    }
                }
                break;
                // その他(タイマン)
            default:

                break;
            }
        }
    }
    DrawString(37, 17, _T("戻る"), GetColor(0, 0, 0));

    // 名前表示
    DrawFormatStringToHandle(
        10, 770,
        GetColor(0, 0, 0),
        Font.Small,
        _T("Name: %s"),
        player.getName().c_str()
    );

    // 設定されたモード名表示
    DrawStringToHandle(110, 10, firstLabel, GetColor(0, 0, 0), Font.Normal);
}

// 「Actionクラスの」「getBattlePlayersという名前の」「定数(const)関数」であることを明示する
std::vector<Player> Action::getBattlePlayers() const {
    return BattlePlayer;
}