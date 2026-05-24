#pragma once
#include "Player.h"

class Battle;

class BattleAI {
public:
    // AIの行動を決定・実行する関数
    // humanIdx: プレイヤー（人間）のインデックス
    // isHumanTurn: 現在人間のターンかどうか
    static void Update(Battle& battle, int humanIdx, bool isHumanTurn);
};