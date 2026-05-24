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

    // BattleInput がクリック判定に使うための関数
    Rect GetHandCardRect(int handIndex) const;

    // 今のターンのプレイヤー名表示
    void DrawTurnPlayerName(const std::string& playerName);

    // ターゲット指定されたプレイヤー名表示
    void DrawTargetPlayerName(const std::string& targetName);

    // ボタン関係の関数(戻るボタンや決定ボタン)
    void DrawOptionButtons(const std::vector<bool>& isHoverOption);

    void DrawRevealCards(/* オープンされた攻撃・防御カードの情報 */);
    void DrawConfirmWindow(bool isSurrenderConfirm); // 降参確認などのウィンドウ

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

