#pragma once
#include <vector>
#include <string>
#include "Player.h"
#include "Card.h"
#include "BattlePhase.h" // Phaseのenum定義があるヘッダ

// 描画関連の処理をまとめた専用クラス
class BattleUI {
public:
    // プレイヤーステータスの描画
    static void DrawPlayerStatus(const std::vector<Player>& players, bool playerTarget, int targetIdx, const std::vector<bool>& isHoverPlayerIdx);

    // 手札の描画
    static void DrawPlayerHand(const Player& player, const std::vector<Player>& playerTurn, int currentTurnIdx, BattlePhase currentPhase, int hoveredCardIdx, const std::vector<bool>& isHoverCardIdx);

    // 引数に const std::vector<Player>& playerTurn と int currentTurnIdx を追加
    static void DrawSelectedCard(const Player& player, 
        const std::vector<Player>& playerTurn, 
        int currentTurnIdx, 
        const std::vector<int>& selectedCards, 
        float currentYOffset, 
        int totalPower, 
        const std::string& currentAttackElement);
};

