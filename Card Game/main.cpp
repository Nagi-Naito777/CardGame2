//画面サイズ指定マクロ
#define WIN_MAX_X 1000
#define WIN_MAX_Y 800

//カードの最大種類数

//カードサイズ
#define CARD_CELL 50

//ゲームシーン列挙体
enum MEN {
    MEN_00_TITLE,		//タイトル画面
    MEN_01_SELECT,		//モードセレクト画面
    MEN_02_ACTION		//バトル画面
};

//ゲームシーンの初期化
int Sce = MEN::MEN_00_TITLE;

#include <iostream>
#include <string>
#include <random>
#include <vector>
#include "DxLib.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    if (DxLib_Init() == -1) return -1;

    SetWindowText("KANEGON FIELD");
    SetGraphMode(WIN_MAX_X, WIN_MAX_Y, 32);
    SetBackgroundColor(255, 255, 255);			//背景色
    SetUseCharSet(DX_CHARSET_SHFTJIS);          //日本語を正しく扱うための関数
    SetDrawScreen(DX_SCREEN_BACK);

    while (ScreenFlip() == 0 &&			//全背景を消す
        ClearDrawScreen() == 0 &&		//画面に描かれたものを消去する
        ProcessMessage() == 0)  		//ウィンドウズのメッセージ処理
    {

    }

    WaitKey();//キー入力待ち

    DxLib_End();
    return 0;
}
