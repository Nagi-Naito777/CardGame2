#pragma once

// 前方宣言：MouseStateという構造体があることだけ教える
struct MouseState;

class TITLE {
public:
    TITLE() : isHover(false), isFocused(false), inputHandle(-1) {}
    void Init();
    // 構造体の実体ではなく「参照(&)」で受け取るなら、前方宣言だけでOK
    bool Update(const MouseState& mouse);
    void Draw();
    void GetUserName(char* dest);

private:
    bool isHover;
    bool isFocused;
    int inputHandle;
};

extern TITLE Tit;