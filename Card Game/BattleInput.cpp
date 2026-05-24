// BattleInput.cpp
#include "BattleInput.h"
#include "Battle.h"
#include "BattlePhase.h"
#include "Card.h"
#include <algorithm>

bool BattleInput::Update(Battle& battle, const MouseState& mouse, Player& humanPlayer, int humanIdx, bool isHumanTurn) {

    // =============================================================
    // 7. プレイヤーの入力処理（UI・ボタン類）
    // =============================================================
    for (int i = 0; i < MAX; i++) battle.isHoverIdx[i] = false;

    if (battle.isSurrenderConfirm) {
        battle.isHoverIdx[GIVE_UP] = IsMouseOver(425, 300, 150, 50, mouse);
        bool clickedReturnAgain = (mouse.leftClicked && IsMouseOver(10, 10, 100, 30, mouse));
        bool clickedOutside = (mouse.leftClicked && !IsMouseOver(300, 200, 400, 200, mouse));

        if (clickedReturnAgain || clickedOutside) {
            battle.isSurrenderConfirm = false;
        }
        else if (mouse.leftClicked && battle.isHoverIdx[GIVE_UP]) {
            battle.selectedCards.clear();
            battle.selectedDefenseCards.clear();
            battle.playerTarget = false;
            battle.targetIdx = -1;
            battle.totalPower = 0;
            battle.isSurrenderConfirm = false;
            battle.selectedOption = RETURN; // ※RETURNマクロ等がある前提
            return true; // 降参によるバトル終了
        }
        return false;
    }

    Player& turnPlayer = battle.Player_Turn[battle.currentTurnIdx];

    battle.isHoverIdx[RETURN] = IsMouseOver(10, 10, 100, 30, mouse);
    if (mouse.leftClicked && battle.isHoverIdx[RETURN]) {
        battle.isSurrenderConfirm = true;
    }

    const int DECISION_AREA_W = 150;
    const int DECISION_AREA_H = 40;
    const int ATK_BTN_X = 250;
    const int ATK_BTN_Y = 150;
    const int DEF_BTN_X = 250;
    const int DEF_BTN_Y = 220;

    // --- 攻撃フェーズ時の決定ボタン ---
    if (battle.currentPhase == BattlePhase::Select && !battle.selectedCards.empty() && isHumanTurn) {
        battle.isHoverIdx[ATTACK] = IsMouseOver(ATK_BTN_X, ATK_BTN_Y, DECISION_AREA_W, DECISION_AREA_H, mouse);

        if (mouse.leftClicked && battle.isHoverIdx[ATTACK]) {
            battle.selectedOption = ATTACK;

            if (!battle.playerTarget || battle.targetIdx == -1) {
                CardCategory firstCardCat = Attack;
                if (!battle.selectedCards.empty() && battle.selectedCards[0] < (int)turnPlayer.GetHand().size()) {
                    firstCardCat = turnPlayer.GetHand()[battle.selectedCards[0]].GetCategory();
                }

                bool isHeal = (firstCardCat == Healing || firstCardCat == MagicHealing);
                if (isHeal) {
                    battle.targetIdx = battle.currentTurnIdx;
                }
                else {
                    std::vector<int> aliveEnemies;
                    for (int i = 0; i < (int)battle.Player_Turn.size(); ++i) {
                        if (i != battle.currentTurnIdx && !battle.Player_Turn[i].isDead()) {
                            aliveEnemies.push_back(i);
                        }
                    }
                    if (!aliveEnemies.empty()) {
                        battle.targetIdx = aliveEnemies[rand() % aliveEnemies.size()];
                    }
                    else {
                        battle.targetIdx = battle.currentTurnIdx;
                    }
                }
                battle.playerTarget = true;
            }

            battle.currentPhase = BattlePhase::DefenseSelect;
            return false; // フェーズ移行したため以降の入力処理をスキップ
        }
    }
    // --- 防御フェーズ時の決定ボタン ---
    else if (battle.currentPhase == BattlePhase::DefenseSelect && battle.targetIdx == humanIdx) {
        battle.isHoverIdx[ATTACK] = IsMouseOver(DEF_BTN_X, DEF_BTN_Y, DECISION_AREA_W, DECISION_AREA_H, mouse);

        if (mouse.leftClicked && battle.isHoverIdx[ATTACK]) {
            battle.currentPhase = BattlePhase::Reveal;
            battle.revealIndex = 0;
            battle.animationTimer = 15;
            return false; // フェーズ移行したため以降の入力処理をスキップ
        }
    }

    // =============================================================
    // 8. ターゲット選択判定
    // =============================================================
    if (battle.currentPhase == BattlePhase::Select && isHumanTurn) {
        const int STATUS_START_X = 700;
        const int STATUS_START_Y = 75;
        const int STATUS_MARGIN_Y = 40;
        const int STATUS_WIDTH = 275;
        const int STATUS_HEIGHT = 30;

        for (int i = 0; i < (int)battle.Player_Turn.size(); ++i) {
            int currentY = STATUS_START_Y + i * STATUS_MARGIN_Y;
            battle.isHoverPlayerIdx[i] = IsMouseOver(STATUS_START_X, currentY - 15, STATUS_WIDTH, STATUS_HEIGHT, mouse);

            if (mouse.leftClicked && battle.isHoverPlayerIdx[i]) {
                battle.targetIdx = i;
                battle.playerTarget = true;
            }
        }
    }

    // =============================================================
    // 9. 手札のカード選択判定
    // =============================================================
    battle.hoveredCardIdx = -1;

    const float SCALE = 1.45f;
    const int CARD_W = (int)(50 * SCALE);
    const int CARD_H = (int)(50 * SCALE);
    const int START_X = 10;
    const int START_Y = 450;
    const int MARGIN = 2;
    const int MAX_CARDS_PER_ROW = 9;
    const int ROW_SPACING = CARD_H + 30;

    const auto& hand = humanPlayer.GetHand();

    for (int i = 0; i < (int)hand.size(); ++i) {
        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;
        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

        if (mouse.x >= x && mouse.x <= x + CARD_W &&
            mouse.y >= y && mouse.y <= y + CARD_H + 25) {
            battle.hoveredCardIdx = i;
        }

        bool isSelectable = true;

        if (battle.currentPhase == BattlePhase::Select) {
            if (isHumanTurn) {
                int cat = hand[i].GetCategory();
                if (cat == Defense) isSelectable = false;
            }
            else {
                isSelectable = false;
            }
        }
        else if (battle.currentPhase == BattlePhase::DefenseSelect) {
            if (battle.targetIdx == humanIdx) {
                int cat = hand[i].GetCategory();
                if (cat != Defense && cat != Bilingual) isSelectable = false;
            }
            else {
                isSelectable = false;
            }
        }
        else {
            isSelectable = false;
        }

        if (isSelectable) {
            battle.isHoverCardIdx[i] = IsMouseOver(x, y, CARD_W, CARD_H, mouse);

            if (mouse.leftClicked && battle.isHoverCardIdx[i]) {
                const auto& currentHand = humanPlayer.GetHand();
                bool isClickedAddable = currentHand[i].GetAdd();

                CardCategory clickedCat = currentHand[i].GetCategory();
                bool isClickedHeal = (clickedCat == Healing || clickedCat == MagicHealing);

                std::vector<int>& activeSelection = (battle.currentPhase == BattlePhase::Select) ? battle.selectedCards : battle.selectedDefenseCards;

                auto it = std::find(activeSelection.begin(), activeSelection.end(), i);
                if (it != activeSelection.end()) {
                    if (it == activeSelection.begin()) {
                        activeSelection.clear();
                        if (battle.currentPhase == BattlePhase::Select) battle.currentAttackElement = "無";
                    }
                    else {
                        activeSelection.erase(it);
                        if (battle.currentPhase == BattlePhase::Select) battle.RecalculateAttackElement(currentHand);
                    }
                }
                else {
                    if (activeSelection.empty()) {
                        activeSelection.push_back(i);
                        if (battle.currentPhase == BattlePhase::Select) {
                            std::string baseType = currentHand[i].GetType();
                            battle.currentAttackElement = (baseType == "") ? "無" : baseType;
                        }
                    }
                    else {
                        int baseIdx = activeSelection[0];
                        const auto& baseCard = currentHand[baseIdx];

                        CardCategory baseCat = baseCard.GetCategory();
                        bool isBaseHeal = (baseCat == Healing || baseCat == MagicHealing);

                        if (!isClickedAddable || isClickedHeal) {
                            activeSelection.clear();
                            activeSelection.push_back(i);
                            if (battle.currentPhase == BattlePhase::Select) {
                                std::string baseType = currentHand[i].GetType();
                                battle.currentAttackElement = (baseType == "") ? "無" : baseType;
                            }
                        }
                        else {
                            if (baseCat == All || isBaseHeal) {
                                // 何もしない
                            }
                            else {
                                activeSelection.push_back(i);
                                if (battle.currentPhase == BattlePhase::Select) {
                                    battle.RecalculateAttackElement(currentHand);
                                }
                            }
                        }
                    }
                }

                battle.totalPower = 0;
                const auto& handForPower = humanPlayer.GetHand();
                for (int idx : activeSelection) {
                    if (idx >= 0 && idx < (int)handForPower.size()) {
                        battle.totalPower += handForPower[idx].GetPower();
                    }
                }
            }
        }
    }
    return false;
}
