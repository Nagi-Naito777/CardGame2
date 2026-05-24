#include "BattleAI.h"
#include "Battle.h"
#include "BattlePhase.h"
#include "Card.h"
#include <vector>
#include <cstdlib>

void BattleAI::Update(Battle& battle, int humanIdx, bool isHumanTurn) {

    // 人間のターンならAIは行動しない（または防御フェーズ待ち）
    if (isHumanTurn && battle.currentPhase == BattlePhase::Select) return;

    Player& turnPlayer = battle.Player_Turn[battle.currentTurnIdx];

    // =============================================================
    // 攻撃フェーズ（AIのターン時）
    // =============================================================
    if (battle.currentPhase == BattlePhase::Select && !isHumanTurn) {

        // --- 1. 手札から武器や魔法を選ぶロジック ---
        // (例: ランダムに攻撃カードを1枚選ぶ)
        battle.selectedCards.clear();
        const auto& hand = turnPlayer.GetHand();
        int selectedCardIdx = -1;

        for (int i = 0; i < (int)hand.size(); ++i) {
            CardCategory cat = hand[i].GetCategory();
            if (cat == Attack || cat == MagicAttack || cat == Bilingual) {
                selectedCardIdx = i;
                break; // とりあえず見つけた攻撃カードを使用
            }
        }

        // --- 2. ターゲットの決定 ---
        if (selectedCardIdx != -1) {
            battle.selectedCards.push_back(selectedCardIdx);

            // 例: 生きている敵（自分以外）からランダムにターゲットを選ぶ
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
                battle.targetIdx = battle.currentTurnIdx; // ターゲットがいないフェールセーフ
            }
            battle.playerTarget = true;
        }
        else {
            // 攻撃できるカードがない場合はパス（空打ち）
            battle.playerTarget = false;
            battle.targetIdx = -1;
        }

        // --- 3. フェーズ移行 ---
        // AIの思考が終わったら防御側の選択フェーズへ
        battle.currentPhase = BattlePhase::DefenseSelect;
    }

    // =============================================================
    // 防御フェーズ（AIがターゲットにされている時）
    // =============================================================
    else if (battle.currentPhase == BattlePhase::DefenseSelect) {

        // AI自身がターゲットにされているかチェック
        if (battle.targetIdx != humanIdx && battle.targetIdx >= 0 && battle.targetIdx < (int)battle.Player_Turn.size()) {

            Player& targetPlayer = battle.Player_Turn[battle.targetIdx];
            battle.selectedDefenseCards.clear();

            // 防御カードを検索して使用するロジック
            const auto& hand = targetPlayer.GetHand();
            for (int i = 0; i < (int)hand.size(); ++i) {
                CardCategory cat = hand[i].GetCategory();
                if (cat == Defense || cat == Bilingual) {
                    battle.selectedDefenseCards.push_back(i);
                    break; // とりあえず1枚出す
                }
            }

            // フェーズ移行（Revealへ）
            battle.currentPhase = BattlePhase::Reveal;
            battle.revealIndex = 0;
            battle.animationTimer = 15;
        }
    }
}
