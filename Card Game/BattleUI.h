#pragma once
#include <vector>
#include <string>
#include "Player.h"
#include "Card.h"
#include "BattlePhase.h" // Phaseのenum定義があるヘッダ

struct Rect {
    int x, y, w, h;
};

// 描画関連の処理をまとめた専用クラス
class BattleUI {
public:
    // プレイヤーステータスの描画
    static void DrawPlayerStatus(const std::vector<Player>& players, bool playerTarget, int targetIdx, const bool* isHoverPlayerIdx);

    // 手札の描画
    static void DrawPlayerHand(const Player& player, const std::vector<Player>& playerTurn, int currentTurnIdx, BattlePhase currentPhase, int hoveredCardIdx, const bool* isHoverCardIdx);

    // 引数に const std::vector<Player>& playerTurn と int currentTurnIdx を追加
    static void DrawSelectedCard(const Player& player, 
        const std::vector<Player>& playerTurn, 
        int currentTurnIdx, 
        const std::vector<int>& selectedCards, 
        float currentYOffset, 
        int totalPower, 
        const std::string& currentAttackElement);

    // BattleInput がクリック判定に使うための関数
    Rect GetHandCardRect(int handIndex) const;

    // 今のターンのプレイヤー名表示
    static void DrawTurnPlayerName(const Player& player);

    // ターゲット指定されたプレイヤー名表示
    static void DrawTargetPlayerName(const std::vector<Player>& players, int currentTurnIdx, int targetIdx);

    // 防御側のカードテキスト表示
    static void DrawDefenseCardsText(const Player& player, BattlePhase currentPhase, int targetIdx, int humanIdx, const std::vector<int>& selectedDefenseCards, int totalPower);

private:
    // レイアウト用の定数をメンバとして持っておく
    const float CARD_SCALE = 1.45f;
    const int BASE_CARD_W = 50;
    const int BASE_CARD_H = 50;
    const int HAND_START_X = 10;
    const int HAND_START_Y = 450;
    const int MAX_CARDS_PER_ROW = 9;
    const int ROW_SPACING = (int)(50 * 1.45f) + 30;
    const int CARD_MARGIN = 2;
};

