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
    isHover = IsMouseOver(300, 450, 400, 50, mouse);

    // マウスクリックの判定
    if (mouse.leftClicked) {
        if (isHover) {
            isFocused = true;
            SetActiveKeyInput(inputHandle); // 入力を受け付ける状態にする
        }
        else {
            isFocused = false;
            // ボックス外をクリックしたら入力を中断するならここ
        }
    }

    // 画面遷移の判定（Enterキーが押されたか）
    if (isFocused) {
        // CheckKeyInput は入力が終わると 1 (Enter) か 2 (Esc) を返す
        int state = CheckKeyInput(inputHandle);
        if (state == 1) {
            return true; // ここで初めてシーンを切り替える
        }
    }

    return false; // それ以外は画面を維持
}

void TITLE::Draw() {
    DrawGraph(0, 0, Pic.Tit, TRUE);

    // ボックス描画
    unsigned int boxColor = isFocused ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
    DrawBox(300, 450, 700, 500, boxColor, TRUE);
    DrawBox(299, 449, 701, 501, GetColor(0, 0, 0), FALSE);

    // 重要：入力中の文字列を表示する関数（これを忘れると何も見えません）
    // 座標は DrawBox の少し内側に合わせる
    DrawKeyInputString(310, 465, inputHandle);

    if (!isFocused) {
        DrawString(310, 465, "ここをクリックして入力", GetColor(150, 150, 150));
    }
}

// 入力された名前を取り出すための便利関数
void TITLE::GetUserName(char* dest) {
    // inputHandle はこのクラスのメンバ変数なのでそのまま使える
    GetKeyInputString(dest, inputHandle);
}