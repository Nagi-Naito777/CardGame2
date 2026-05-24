// BattleInput.h
#pragma once
#include "MouseInput.h"
#include "BattleUI.h"
#include "Player.h"
#include <vector>

class Battle; // 前方宣言

class BattleInput {
public:
    // 毎フレームの入力を処理する大元
    void Update(Battle& battle, const BattleUI& ui);

private:
    // フェーズごとの入力処理に分けるとスッキリします
    void ProcessSelectPhase(Battle& battle, const BattleUI& ui);
    void ProcessTargetPhase(Battle& battle, const BattleUI& ui);

    // マウスの当たり判定用ヘルパー関数
    bool IsMouseInRect(int mouseX, int mouseY, const Rect& rect);
};

