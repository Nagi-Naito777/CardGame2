//画面サイズ指定マクロ
#define WIN_MAX_X 1000
#define WIN_MAX_Y 800

//カードサイズ
#define CARD_CELL 50

//カードの合計種類数
#define CARD_KIND 100

//カードの最大所持枚数
#define CARD_MAX 20

//ゲームシーン列挙体
enum GAME_SCENE {
    MEN_00_TITLE,		//タイトル画面
    MEN_01_SELECT,		//モードセレクト画面
    MEN_02_ACTION,		//バトル前の設定画面
    MEN_03_BATTLE       //バトル画面
};

//ゲームシーンの初期化
int Scene = GAME_SCENE::MEN_00_TITLE;

#include <iostream>
#include <string>
#include <random>
#include <vector>
#include "DxLib.h"
#include "Player.h"         // プレイヤークラスヘッダー
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Picture.h"        // 写真関係ヘッダー
#include "Card.h"           // カード関係ヘッダー
#include "Title.h"          // タイトルシーンヘッダー
#include "Select.h"         // モードセレクトシーンヘッダー
#include "Action.h"         // バトル詳細設定シーンヘッダー

// ファイルパスを定数として持っておくと管理が楽です
const std::string CSV_PATH = "../x64/Debug/CSV/card_data.csv";

// externで二重定義エラーを回避
Picture Pic;
TITLE Tit;
SelectScene Sel;
Action Act;
Player g_player;
Card card;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    if (DxLib_Init() == -1) return -1;

    SetWindowText("KANEGON FIELD");
    SetGraphMode(WIN_MAX_X, WIN_MAX_Y, 32);
    SetBackgroundColor(255, 255, 255);			//背景色
    SetUseCharSet(DX_CHARSET_SHFTJIS);          //日本語を正しく扱うための関数
    SetDrawScreen(DX_SCREEN_BACK);

    // --- CSV読み込み実行 ---
    if (!card.LoadCardDatabase(CSV_PATH)) {
        // 失敗したら画面にメッセージを出して止める
        printfDx("エラー: %s が見つかりません！\n", CSV_PATH.c_str());
        ScreenFlip();
        WaitKey();
        DxLib_End();
        return -1;
    }


    // 【追加】ここで入力ハンドルを正しく作成させる
    Tit.Init();

    // マウスの状態を保持する変数を宣言
    MouseState mouse;

    //読み込み関数
    Pic.Read();

    while (ScreenFlip() == 0 &&			// 全背景を消す
        ClearDrawScreen() == 0 &&		// 画面に描かれたものを消去する
        ProcessMessage() == 0 ) 		// ウィンドウズのメッセージ処理
    {
        // 毎フレームの最初でマウスの状態を更新
        UpdateMouseState(mouse);

        switch (Scene)
        {
            //シーンが00ならタイトル画面
        case GAME_SCENE::MEN_00_TITLE:
            // Updateの中で「クリック判定」も「Enter判定」も完結している
            if (Tit.Update(mouse)) {
                // Updateがtrueを返した＝名前入力が完了したということ

                // Titから名前を取り出してセットする
                g_player.setName(Tit.GetName()); // プレイヤーに保存する

                //もし名前が0より大きいならシーン切り替え
                if (g_player.getName().size() > 0) {
                    // シーン写し
                    Scene = GAME_SCENE::MEN_01_SELECT;
                }
            }
            Tit.Draw();
            break;
        case GAME_SCENE::MEN_01_SELECT:
            if (Sel.Update(mouse)) {
                int opt = Sel.getSelectedOption();
                //もしタイトルへ戻る列挙体が選ばれたなら
                if (Sel.getSelectedOption() == SelectScene::Option::RETURN ||
                    CheckHitKey(KEY_INPUT_ESCAPE)) {
                    Tit.Init();
                    Scene = GAME_SCENE::MEN_00_TITLE;
                    // ボタンが離されるまで待機する（あるいはフラグで制御）
                    while (GetMouseInput() & MOUSE_INPUT_LEFT) {
                        if (ProcessMessage() != 0) break;
                    }
                }
                else if (opt != SelectScene::Option::NONE) { 
                    // ちゃんとボタンが選ばれているか？
                    Scene = GAME_SCENE::MEN_02_ACTION;
                }
            }
            Sel.Draw(g_player);
            break;
            //シーンが02ならバトル設定画面
        case GAME_SCENE::MEN_02_ACTION:
            Act.Draw(g_player, Sel.getSelectedOption());
            break;
            //シーンが03ならバトル画面
        case GAME_SCENE::MEN_03_BATTLE:
            break;
        }
    }

    WaitKey();//キー入力待ち

    DxLib_End();
    return 0;
}
