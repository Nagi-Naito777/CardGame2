#pragma once
#include <string>

// 前方宣言：MouseStateという構造体があることだけ教える
struct MouseState;

class TITLE {
public:
    TITLE() : isHover(false), isFocused(false), inputHandle(-1) {}
    void Init();
    // 構造体の実体ではなく「参照(&)」で受け取るなら、前方宣言だけでOK
    bool Update(const MouseState& mouse);
    bool TitleEnter();
    void Draw();
    std::string GetName();

private:
    bool isHover;       // マウスが入力ボックスの上にあるかどうか
    bool isFocused;     // 入力モードに入っているかどうか
    int inputHandle;
    bool isStartHover;       // マウスがスタートボタンの上にあるかどうか
};

extern TITLE Tit;