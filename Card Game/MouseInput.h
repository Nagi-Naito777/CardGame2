#pragma once

struct MouseState {
    int x, y;           // マウスの現在座標
    int input;          // どのボタンが押されたかのフラグ
    bool leftClicked;   // 前のフレームで押してなくて、今押した瞬間(左クリック)
};

// マウスの状態を更新する関数
inline void UpdateMouseState(MouseState& mouse) {
    static int prevInput = 0;
    GetMousePoint(&mouse.x, &mouse.y);
    mouse.input = GetMouseInput();

    // 「今押された瞬間」だけを判定
    mouse.leftClicked = (mouse.input & MOUSE_INPUT_LEFT) && !(prevInput & MOUSE_INPUT_LEFT);
    prevInput = mouse.input;
}

// 「特定の範囲内にマウスがあるか」を判定する汎用関数
inline bool IsMouseOver(int x, int y, int w, int h, const MouseState& mouse) {
    return (mouse.x >= x && mouse.x <= x + w && mouse.y >= y && mouse.y <= y + h);
}