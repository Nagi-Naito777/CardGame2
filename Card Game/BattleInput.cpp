// BattleInput.cpp
#include "BattleInput.h"
#include "Battle.h"
#include "BattlePhase.h"
#include "Card.h"
#include <algorithm>

bool BattleInput::Update(Battle& battle, const MouseState& mouse, Player& humanPlayer, int humanIdx, bool isHumanTurn) {

    // =============================================================
    // プレイヤーの入力処理（UI・ボタン類）
    // =============================================================
    
    // マウスホバー状態の初期化
    for (int i = 0; i < Battle::MAX; i++) battle.isHoverIdx[i] = false;

    // -------------------------------------------------------------
    // 【降参（サレンダー）確認画面の処理】
    // -------------------------------------------------------------
    if (battle.isSurrenderConfirm) {
        // ★ GIVE_UP を Battle::GIVE_UP に修正
        battle.isHoverIdx[Battle::GIVE_UP] = IsMouseOver(425, 300, 150, 50, mouse);

        // 確認画面を閉じる条件（再度戻るボタンを押す or ウィンドウ外をクリック）
        bool clickedReturnAgain = (mouse.leftClicked && IsMouseOver(10, 10, 100, 30, mouse));
        bool clickedOutside = (mouse.leftClicked && !IsMouseOver(300, 200, 400, 200, mouse));

        // 降参キャンセル
        if (clickedReturnAgain || clickedOutside) {
            battle.isSurrenderConfirm = false;
        }
        else if (mouse.leftClicked && battle.isHoverIdx[Battle::GIVE_UP]) {
            // 降参決定：バトルの選択状態をすべてクリアして終了処理へ
            battle.selectedCards.clear();
            battle.selectedDefenseCards.clear();
            battle.playerTarget = false;
            battle.targetIdx = -1;
            battle.totalPower = 0;
            battle.isSurrenderConfirm = false;
            battle.selectedOption = Battle::RETURN;
            return true; // 降参によるバトルループ終了
        }
        return false; // 確認画面中は他の入力を受け付けない
    }

    Player& turnPlayer = battle.Player_Turn[battle.currentTurnIdx];

    // -------------------------------------------------------------
    // 【戻る（降参）ボタンの処理】
    // -------------------------------------------------------------
    battle.isHoverIdx[Battle::RETURN] = IsMouseOver(10, 10, 100, 30, mouse);
    // 降参確認画面を展開
    if (mouse.leftClicked && battle.isHoverIdx[Battle::RETURN]) {
        battle.isSurrenderConfirm = true;
    }

    const int DECISION_AREA_W = 150;
    const int DECISION_AREA_H = 40;
    const int ATK_BTN_X = 250;
    const int ATK_BTN_Y = 150;
    const int DEF_BTN_X = 250;
    const int DEF_BTN_Y = 220;

    // -------------------------------------------------------------
    // 【攻撃フェーズ時の決定ボタン処理】
    // -------------------------------------------------------------
    // 条件：攻撃選択フェーズ ＆ カードが1枚以上選ばれている ＆ 自分のターン
    if (battle.currentPhase == BattlePhase::Select && !battle.selectedCards.empty() && isHumanTurn) {
        battle.isHoverIdx[Battle::ATTACK] = IsMouseOver(ATK_BTN_X, ATK_BTN_Y, DECISION_AREA_W, DECISION_AREA_H, mouse);

        if (mouse.leftClicked && battle.isHoverIdx[Battle::ATTACK]) {
            battle.selectedOption = Battle::ATTACK;

            // オートターゲット機能（ターゲット未指定のまま決定を押した場合の救済処理）
            if (!battle.playerTarget || battle.targetIdx == -1) {
                CardCategory firstCardCat = Attack;
                if (!battle.selectedCards.empty() && battle.selectedCards[0] < (int)turnPlayer.GetHand().size()) {
                    firstCardCat = turnPlayer.GetHand()[battle.selectedCards[0]].GetCategory();
                }

                bool isHeal = (firstCardCat == Healing || firstCardCat == MagicHealing);
                if (isHeal) {
                    // 回復カードなら自動的に自分をターゲットにする
                    battle.targetIdx = battle.currentTurnIdx;
                }
                else {
                    // 攻撃カードなら「生存している敵」からランダムにターゲットを決定
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
                        // 敵がいない場合（フェイルセーフとして自分を指定）
                        battle.targetIdx = battle.currentTurnIdx;
                    }
                }
                // 攻撃が確定したので、防御側プレイヤーの選択フェーズへ移行
                battle.playerTarget = true;
            }

            battle.currentPhase = BattlePhase::DefenseSelect;
            return false; // フェーズ移行したため以降の入力処理をスキップ
        }
    }
    // -------------------------------------------------------------
    // 【防御フェーズ時の決定ボタン処理】
    // -------------------------------------------------------------
    // 条件：防御選択フェーズ ＆ 自分が攻撃のターゲットにされている
    else if (battle.currentPhase == BattlePhase::DefenseSelect && battle.targetIdx == humanIdx) {
        battle.isHoverIdx[Battle::ATTACK] = IsMouseOver(DEF_BTN_X, DEF_BTN_Y, DECISION_AREA_W, DECISION_AREA_H, mouse);

        if (mouse.leftClicked && battle.isHoverIdx[Battle::ATTACK]) {
            // 防御が確定したので、カードオープン（演出）フェーズへ移行
            battle.currentPhase = BattlePhase::Reveal;
            battle.revealIndex = 0;         // 演出用のインデックス初期化
            battle.animationTimer = 15;     // 演出用のタイマー初期化
            return false; // フェーズ移行したため以降の入力処理をスキップ
        }
    }

    // =============================================================
    // ターゲット選択判定（ステータスUIのクリック）
    // =============================================================
    // 自分のターンの攻撃選択フェーズのみ、手動でのターゲット切り替えを許可
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
                battle.targetIdx = i;           // クリックしたプレイヤーをターゲットに設定
                battle.playerTarget = true;     // 手動ターゲット指定フラグをON
            }
        }
    }

    // =============================================================
    // 手札のカード選択判定
    // =============================================================
    battle.hoveredCardIdx = -1;

    // 手札描画レイアウト
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
        // カードの描画座標を計算(グリッド配置)
        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;
        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

        // マウスホバー判定
        if (mouse.x >= x && mouse.x <= x + CARD_W &&
            mouse.y >= y && mouse.y <= y + CARD_H + 25) {
            battle.hoveredCardIdx = i;
        }

        // -------------------------------------------------------------
        // カードが選択可能かどうかの判定 (フェーズとカード種類による制限)
        // -------------------------------------------------------------
        bool isSelectable = true;

        if (battle.currentPhase == BattlePhase::Select) {
            if (isHumanTurn) {
                // 攻撃フェーズ中は防御専用カードは選択不可
                int cat = hand[i].GetCategory();
                if (cat == Defense) isSelectable = false;
            }
            else {
                // 相手のターンの攻撃フェーズは何も選択できない
                isSelectable = false;
            }
        }
        else if (battle.currentPhase == BattlePhase::DefenseSelect) {
            if (battle.targetIdx == humanIdx) {
                // 防御フェーズ中は防具系カードのみ選択可能
                int cat = hand[i].GetCategory();
                if (cat != Defense && cat != Bilingual) isSelectable = false;
            }
            else {
                // 他人が攻撃されている時は選択不可
                isSelectable = false;
            }
        }
        else {
            // その他フェーズ中も選択不可
            isSelectable = false;
        }

        // -------------------------------------------------------------
        // カードがクリックされた時の処理（選択・解除・コンボ判定）
        // -------------------------------------------------------------
        if (isSelectable) {
            battle.isHoverCardIdx[i] = IsMouseOver(x, y, CARD_W, CARD_H, mouse);

            if (mouse.leftClicked && battle.isHoverCardIdx[i]) {
                const auto& currentHand = humanPlayer.GetHand();
                // 加算可能か判定
                bool isClickedAddable = currentHand[i].GetAdd();

                CardCategory clickedCat = currentHand[i].GetCategory();
                bool isClickedHeal = (clickedCat == Healing || clickedCat == MagicHealing);

                std::vector<int>& activeSelection = (battle.currentPhase == BattlePhase::Select) ? battle.selectedCards : battle.selectedDefenseCards;

                auto it = std::find(activeSelection.begin(), activeSelection.end(), i);
                
                // 【パターンA】既に選択済みのカードをクリックした場合は「選択解除」
                if (it != activeSelection.end()) {
                    if (it == activeSelection.begin()) {
                        // 1枚目（ベース）を解除した場合は、後続の追加カードもすべてリセット
                        activeSelection.clear();
                        if (battle.currentPhase == BattlePhase::Select) battle.currentAttackElement = "無";
                    }
                    else {
                        // 追加の武器などを解除した場合は、そのカードだけを外し、属性を再計算
                        activeSelection.erase(it);
                        if (battle.currentPhase == BattlePhase::Select) battle.RecalculateAttackElement(currentHand);
                    }
                }
                // 【パターンB】未選択のカードをクリックした場合は「選択追加」の判定
                else {
                    if (activeSelection.empty()) {
                        // まだ1枚も選ばれていない場合：無条件でベースとして選択
                        activeSelection.push_back(i);
                        if (battle.currentPhase == BattlePhase::Select) {
                            std::string baseType = currentHand[i].GetType();
                            battle.currentAttackElement = (baseType == "") ? "無" : baseType;
                        }
                    }
                    else {
                        // 既にベースカードが選ばれている場合：コンボ（追加）できるか判定
                        int baseIdx = activeSelection[0];
                        const auto& baseCard = currentHand[baseIdx];

                        CardCategory baseCat = baseCard.GetCategory();
                        bool isBaseHeal = (baseCat == Healing || baseCat == MagicHealing);

                        // クリックしたカードがBilingual(攻防)かどうか
                        bool isClickedBilingual = (clickedCat == Bilingual);
                        
                        // 「追加不可フラグ」または「回復カード」または「攻撃フェーズでの攻防カード」なら、
                        // 既存の選択を破棄して、今回クリックしたカードを新たなベースにする
                        if (!isClickedAddable || isClickedHeal || (battle.currentPhase == BattlePhase::Select && isClickedBilingual)) {
                            // 既存の選択をすべて破棄し、今回クリックしたカードを新たなベースとして選択し直す
                            activeSelection.clear();
                            activeSelection.push_back(i);
                            if (battle.currentPhase == BattlePhase::Select) {
                                std::string baseType = currentHand[i].GetType();
                                battle.currentAttackElement = (baseType == "") ? "無" : baseType;
                            }
                        }
                        else {
                            // 追加できる場合の条件チェック
                            if (baseCat == All || isBaseHeal) {
                                // ベースが「全体攻撃」または「回復」の場合は武器の追加を許可しない（何もしない）
                            }
                            else {
                                // 条件クリア：武器などの追加を許可し、属性を再計算
                                activeSelection.push_back(i);
                                if (battle.currentPhase == BattlePhase::Select) {
                                    battle.RecalculateAttackElement(currentHand);
                                }
                            }
                        }
                    }
                }

                // -------------------------------------------------------------
                // 最終的な威力の再計算
                // -------------------------------------------------------------
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
