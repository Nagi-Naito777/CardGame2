#include "BattleAI.h"
#include "Battle.h"
#include "BattlePhase.h"
#include "Card.h"
#include <vector>
#include <cstdlib>

void BattleAI::Update(Battle& battle, int humanIdx, bool isHumanTurn) {

    // 人間のターン時のSelectフェーズならAIは行動しない
    if (isHumanTurn && battle.currentPhase == BattlePhase::Select) return;

    Player& turnPlayer = battle.Player_Turn[battle.currentTurnIdx];

    // =============================================================
    // 攻撃フェーズ（AIのターン時）
    // =============================================================
    if (battle.currentPhase == BattlePhase::Select && !isHumanTurn) {

        battle.selectedCards.clear();
        const auto& hand = turnPlayer.GetHand();

        int bestIndex = -1;
        int maxPower = -1;

        // --- 1. 手札から【MPが足りる】かつ【一番強い】武器や魔法を選ぶ ---
        for (int i = 0; i < (int)hand.size(); ++i) {
            CardCategory cat = hand[i].GetCategory();
            // ※もし「Attack」でエラーが出る場合は「CardCategory::Attack」にしてください
            if (cat == Attack || cat == Magic || cat == Bilingual) {
                // MPが足りているかチェック
                if (turnPlayer.getMp() >= hand[i].GetMP()) {
                    // より攻撃力が高いカードを記憶する
                    if (hand[i].GetPower() > maxPower) {
                        maxPower = hand[i].GetPower();
                        bestIndex = i;
                    }
                }
            }
        }

        // --- 2. 攻撃カードが選べた場合 ---
        if (bestIndex != -1) {
            battle.selectedCards.push_back(bestIndex);

            // 生きている敵（自分以外）からランダムにターゲットを選ぶ（元の素晴らしいロジック！）
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
                battle.targetIdx = battle.currentTurnIdx; // フェールセーフ
            }
            battle.playerTarget = true;

            // 攻撃先が決まったら防御選択フェーズへ
            battle.currentPhase = BattlePhase::DefenseSelect;
        }
        else {
            // --- 3. 攻撃できるカードがない場合（パス） ---
            battle.playerTarget = false;
            battle.targetIdx = -1;

            // ★重要：攻撃しないのに防御フェーズに行くとゲームが止まるので、次のターンへ進める
            battle.NextTurn();
        }
    }

    // =============================================================
    // 防御フェーズ（AIがターゲットにされている時）
    // =============================================================
    else if (battle.currentPhase == BattlePhase::DefenseSelect) {

        // 攻撃のターゲットが人間以外（＝AIが防御する番）かチェック
        if (battle.targetIdx != humanIdx && battle.targetIdx >= 0 && battle.targetIdx < (int)battle.Player_Turn.size()) {

            Player& targetPlayer = battle.Player_Turn[battle.targetIdx];
            battle.selectedDefenseCards.clear();

            const auto& hand = targetPlayer.GetHand();
            int bestIndex = -1;
            int maxPower = -1;

            // --- 防御カードを検索（MPが足りる中で一番防御力が高いもの） ---
            for (int i = 0; i < (int)hand.size(); ++i) {
                CardCategory cat = hand[i].GetCategory();
                if (cat == Defense || cat == Bilingual) {
                    // MPチェック
                    if (targetPlayer.getMp() >= hand[i].GetMP()) {
                        if (hand[i].GetPower() > maxPower) {
                            maxPower = hand[i].GetPower();
                            bestIndex = i;
                        }
                    }
                }
            }

            // 防御カードが見つかったらセット
            if (bestIndex != -1) {
                battle.selectedDefenseCards.push_back(bestIndex);
            }

            // 防御できても、カードがなくて防御できなくても Reveal（公開）フェーズへ
            battle.currentPhase = BattlePhase::Reveal;
            battle.revealIndex = 0;
            battle.animationTimer = 15;
        }
    }
}