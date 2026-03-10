#include <string>
#include "DxLib.h"
#include "Title.h"
#include "MouseInput.h"         // ここで初めてMouseStateの正体が必要になる
#include "Picture.h"            // Picなどを使っている場合

// 【重要】DXライブラリ初期化後に呼び出す初期化関数
void TITLE::Init() {
    if (inputHandle == -1) {
        // ここでハンドルを作る（日本語を使いたいなら第2引数をTRUEに）
        inputHandle = MakeKeyInput(20, FALSE, FALSE, FALSE);

        // すべての色をセットする関数
    // 第2引数（StrColor）が文字の色、第10引数（CursorColor）がカーソルの色です
        SetKeyInputStringColor(
            inputHandle,             // 01: 対象となる文字の色
            GetColor(0, 0, 0),       // 02: StrColor (入力文字の色：黒)
            GetColor(150, 150, 150), // 03: SepColor (キャンセルした時などの区切り色)
            GetColor(0, 0, 0),       // 04: ChgColor (日本語変換中の文字色)
            GetColor(255, 255, 255), // 05: ChgCurColor (変換中の下線色)
            GetColor(255, 255, 255), // 06: UserColor1 (ユーザーが任意に使える予備色)
            GetColor(255, 0, 0),     // 07: UserColor2 (予備色)
            GetColor(150, 150, 150), // 08: UserColor3 (予備色)
            GetColor(255, 255, 255), // 09: BackColor (文字の背景色)
            GetColor(255, 255, 255)  // 10: CursorColor（点滅するカーソル色）
        );
    }
}

// 更新処理：戻り値が true なら「名前が確定して次へ進む」
bool TITLE::Update(const MouseState& mouse) {
    // マウスが入力ボックスの上にあるか判定
    isHover = IsMouseOver(300, 450, 400, 50, mouse);

    // マウスクリック時のフォーカス処理
    if (mouse.leftClicked) {
        if (isHover) {
            isFocused = true;
            SetActiveKeyInput(inputHandle); // DXライブラリの入力ハンドルを有効化
        }
        else {
            isFocused = false;
            // ボックス外をクリックしたら入力を中断（必要に応じて）
        }
    }

    // 入力確定の判定
    if (isFocused) {
        // CheckKeyInput は入力中なら 0、Enterなら 1、Escなら 2 を返す
        int state = CheckKeyInput(inputHandle);

        if (state == 1) { // Enterキーで確定
            isFocused = false;
            return true;  // 「入力完了」を呼び出し元に伝える
        }
    }

    return false; // まだ入力中、または待機中
}

void TITLE::Draw() {
    DrawGraph(0, 0, Pic.Tit, TRUE);

    // ボックス描画
    unsigned int boxColor = isFocused ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
    DrawBox(300, 450, 700, 500, boxColor, TRUE);
    DrawBox(299, 449, 701, 501, GetColor(0, 0, 0), FALSE);

    //現在の入力した名前を取得
    std::string currentName = GetName();

    // 入力中の文字列を表示する関数（これを忘れると何も見えません）
    if (isFocused) {
        DrawKeyInputString(310, 465, inputHandle);
    }
    else {
        std::string user_name = currentName.empty() ? "ここをクリックして名前入力" : GetName();
        unsigned int fontColor = currentName.empty() ? GetColor(150, 150, 150) : GetColor(0, 0, 0);
        DrawString(310, 465, user_name.c_str(), fontColor);
    }
}

// 入力された名前を取り出すための便利関数
std::string TITLE::GetName() {
    char dest[256];
    // inputHandle はこのクラスのメンバ変数なのでそのまま使える
    GetKeyInputString(dest, inputHandle);
    return std::string(dest);
}