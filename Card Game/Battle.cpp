#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <vector>
#include "DxLib.h"
#include "FontManager.h"    // フォント設定系ヘッダー
#include "Player.h"         // プレイヤークラスヘッダー
#include "MouseInput.h"     // マウス入力関係ヘッダー
#include "Picture.h"        // 写真関係ヘッダー
#include "Card.h"           // カード関係ヘッダー
#include "Title.h"          // タイトルシーンヘッダー
#include "Select.h"         // モードセレクトシーンヘッダー
#include "Action.h"         // バトル詳細設定シーンヘッダー
#include "Battle.h"
#include "DamageResult.h"   // ダメージ計算等のクラス

// コンストラクタの実体
Battle::Battle() : currentTurnIdx(0), targetIdx(-1), selectCard(-1), playerTarget(false), selectedOption(NONE), hoveredCardIdx(-1) {
    // ボタン数分のマウス判定変数を初期化
    for (int i = 0; i < MAX; i++) {
        isHoverIdx[i] = false;
    }

    // カードの最大枚数分のマウス判定変数を初期化
    for (int i = 0; i < CARD_MAX; i++) {
        isHoverCardIdx[i] = false;
    }
    
    // プレイヤーの最大人数分のマウス判定変数を初期化
    for (int i = 0; i < PLAYER_MAX; i++) {
        isHoverPlayerIdx[i] = false;
    }
	
}

// プレイヤーをプッシュバックする関数
void Battle::Initialize(const std::vector<Player>& players) {
    // 引数で受け取った全プレイヤー情報をメンバ変数にコピー
    this->Player_Turn = players;

    // 乱数エンジンのセットアップ
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());

    // AI専用の名前リストを作成 (std::string型)
    // プレイヤーが std::string を使用しているため、_Tマクロは不要です
    std::vector<std::string> aiNames = {
        "ｼﾞﾝﾊﾞﾌﾞｴﾄﾞﾙ太郎", "ｷﾐﾉｶｾﾞﾊﾉﾄﾞｶﾗ", "カオスドゥラゴン","破滅した世界","アルコール","ミセスタニシ",
        "木下 明憲", "アンドロイド伊藤", "消しゴムｽﾚｲﾔｰ","マスターゴリラ","ミーティング次郎","ﾏｲｹﾙ･ｼﾞｪｲｸｿﾝ",
        "白川 真昼",  "闇川 影虎","森中 海導","他人の鉛筆","暴虐武人マン","ナギナギ",
        "清水一登太郎","怪盗マラカス","イグラドガネ","コハクンチョス","リンクカネゴン","ネットワーク",
        "System Error 404","バチカン","ブームブーム","シャングリラ","ｴﾝﾄﾞﾙｶﾈｺﾞﾝﾌｨｰﾙﾄﾞ","アサアサ",
        "アラスカの風","ハリwood","マーKING飛高","謝罪サムライ","GPT","EDM",".cpp",
        "膝の上からｶﾝﾊﾟﾆｰ","膝下ｽﾗｲﾃﾞｨﾝｸﾞ渉","ワールドドリフ",".h","Destiny","enum",
        "心の歪み","憎悪","深淵の戦士 ｱｽﾛﾝ","leading","string.h","using",
        "黒魔術師 ﾅｲﾄﾒｱ","神殺しのｱｻﾞﾘｵｽ","闇の管理人","エディション","クラス.h","カネゴンバレー",
        "砂岩ガン","真夏の秋山","真冬の春海","ボンゴバナンザ","ﾗｽﾄｵﾌﾞかねごん","ﾘﾐﾃｯﾄﾞかねごん",
        "Kanegon","ﾀﾞｰｸﾈｽｽﾏｲﾙ","水しぶき","かねごん動詞","かねごん殴って","終焉のかねごん",
        "雑草","かん","prism","野菜","厄災","国王",
        "あまよもぎ","ぁびゃ","ユウキ","中央都市かねごん","かねごん禁忌","かねごん構成",
        "カミヒデ","カラムライア","白川 大輔","しずお","1031","Clover",
        "ナンバーコア","キラ","カンナ","忠犬","79わ","ひんやり茶",
        "SML","あ","ああああああああ","紫陽花","ブーゲンビリア","ﾀ",
        "ツチノコ","ワシじゃよ、ワシ","強すぎて滅","マリオネット","人生楽観思考","雪谷 久代"
    };
    // ※即興で思いついたものがたくさん入ってるので
    // 　別に大したプログラムでは無い

    // リスト自体をシャッフルして、取り出す名前をランダムにする
    std::shuffle(aiNames.begin(), aiNames.end(), engine);

    // 現在参加している「人間プレイヤー」の名前を把握
    std::vector<std::string> humanNames;
    for (const auto& p : players) {
        if (p.getControllerType() == ControllerType::HUMAN) {
            humanNames.push_back(p.getName());
        }
    }

    // AIプレイヤーに名前を割り当てる
    int aiNameIdx = 0;
    for (auto& p : this->Player_Turn) {
        if (p.getControllerType() == ControllerType::AI) {

            // 候補の名前を取り出す
            std::string candidate = aiNames[aiNameIdx];
            aiNameIdx++;

            // 【ソロプレイ用チェック】
            // もし候補の名前がプレイヤー(g_player)の名前と同じだった場合
            // かつ、リストに次の名前があるなら、次の名前を採用する
            if (candidate == g_player.getName() && aiNameIdx < (int)aiNames.size()) {
                candidate = aiNames[aiNameIdx];
                aiNameIdx++; // インデックスをさらに1つ進める
            }

            p.setName(candidate);
        }
    }

    // ターン順をシャッフル
    std::shuffle(this->Player_Turn.begin(), this->Player_Turn.end(), engine);

    // 内部変数の初期化
    this->currentTurnIdx = 0;
    this->targetIdx = -1;
    this->playerTarget = false;
    this->selectedCards.clear();
    this->totalPower = 0;
    this->currentPhase = BattlePhase::Select;
}

// 更新処理
bool Battle::Update(const MouseState& mouse, const Player& player) {

    // =============================================================
    // 1. 安全ガード
    // =============================================================
    if (Player_Turn.empty() || currentTurnIdx < 0 || currentTurnIdx >= (int)Player_Turn.size()) {
        return false;
    }

    // =============================================================
    // 2. プレイヤー情報の特定
    // =============================================================
    int humanIdx = 0;
    for (int i = 0; i < (int)Player_Turn.size(); ++i) {
        if (Player_Turn[i].getName() == player.getName()) {
            humanIdx = i;
            break;
        }
    }
    bool isHumanTurn = (currentTurnIdx == humanIdx);

    // ★修正ポイント★
    // 引数の player は const で手札が更新されないため、実体である Player_Turn のデータを使う
    Player& humanPlayer = Player_Turn[humanIdx];

    // =============================================================
    // 3. カード重なりアニメーション
    // =============================================================
    float targetYOffset = 65.0f;
    if (selectedCards.size() >= 4) {
        targetYOffset = 30.0f;
    }
    if (selectedCards.empty()) {
        currentYOffset = 65.0f;
    }
    else {
        currentYOffset += (targetYOffset - currentYOffset) * 0.1f;
    }

    // =============================================================
    // 4. Reveal（公開演出）フェーズ
    // =============================================================
    if (currentPhase == BattlePhase::Reveal) {
        if (animationTimer > 0) {
            animationTimer--;
        }
        else {
            if (revealIndex < selectedCards.size()) {
                revealIndex++;
                animationTimer = 30;
            }
            else {
                currentPhase = BattlePhase::Effect;
                animationTimer = 60;
            }
        }
        return false;
    }

    // =============================================================
    // 5. Effect / Damage（ダメージ計算・削除・ドロー）フェーズ
    // =============================================================
    if (currentPhase == BattlePhase::Effect || currentPhase == BattlePhase::Damage) {
        if (animationTimer > 0) {
            animationTimer--;
        }

        if (animationTimer == 0) {
            if (currentPhase == BattlePhase::Effect) {
                Player& attacker = GetCurrentPlayer();
                Player& target = Player_Turn[targetIdx];

                TotalAttack attackData = CalculateTotalAttack(attacker);

                Card* defenseCard = nullptr;
                if (!selectedDefenseCards.empty() && selectedDefenseCards[0] < (int)target.GetHand().size()) {
                    defenseCard = const_cast<Card*>(&target.GetHand()[selectedDefenseCards[0]]);
                }

                ResolveDamage(target, attackData, defenseCard);

                currentPhase = BattlePhase::Damage;
                animationTimer = 90;
            }
            else if (currentPhase == BattlePhase::Damage) {
                Player& attacker = Player_Turn[currentTurnIdx];
                Player& target = Player_Turn[targetIdx];

                std::sort(selectedCards.rbegin(), selectedCards.rend());
                for (int idx : selectedCards) {
                    attacker.RemoveHand(idx);
                }

                std::sort(selectedDefenseCards.rbegin(), selectedDefenseCards.rend());
                for (int idx : selectedDefenseCards) {
                    target.RemoveHand(idx);
                }

                while (attacker.GetHandCount() < CARD_MAX) {
                    attacker.AddHand(Card::GetRandomCard());
                }
                if (targetIdx != -1 && !target.isDead()) {
                    while (target.GetHandCount() < CARD_MAX) {
                        target.AddHand(Card::GetRandomCard());
                    }
                }

                if (Player_Turn[targetIdx].isDead()) {
                    RemovePlayer(targetIdx);
                }

                selectedCards.clear();
                selectedDefenseCards.clear();
                playerTarget = false;
                targetIdx = -1;

                currentPhase = BattlePhase::Select;
                NextTurn();
            }
        }
        return false;
    }

    // =============================================================
    // 6. AIの自動行動ロジック
    // =============================================================
    if (currentPhase == BattlePhase::Select && !isHumanTurn) {
        Player& aiPlayer = Player_Turn[currentTurnIdx];
        if (selectedCards.empty() && !aiPlayer.GetHand().empty()) {
            int bestIdx = -1;
            int maxPower = -1;

            for (int i = 0; i < (int)aiPlayer.GetHand().size(); ++i) {
                const Card& c = aiPlayer.GetHand()[i];
                if (c.GetCategory() == Attack || c.GetCategory() == Bilingual) {
                    if (aiPlayer.getMp() >= c.GetMP() && c.GetPower() > maxPower) {
                        maxPower = c.GetPower();
                        bestIdx = i;
                    }
                }
            }

            if (bestIdx != -1) {
                selectedCards.push_back(bestIdx);
            }
            // ★修正ポイント★
            // 攻撃カードがない場合は push_back(0) せずにパス扱いにする
        }

        targetIdx = humanIdx;
        playerTarget = true;
        currentPhase = BattlePhase::DefenseSelect;
        return false;
    }

    if (currentPhase == BattlePhase::DefenseSelect && targetIdx != humanIdx) {
        Player& aiPlayer = Player_Turn[targetIdx];
        selectedDefenseCards.clear();

        int bestIdx = -1;
        int bestPower = -1;

        for (int i = 0; i < (int)aiPlayer.GetHand().size(); ++i) {
            const Card& c = aiPlayer.GetHand()[i];
            if (c.GetCategory() == Defense || c.GetCategory() == Bilingual) {
                if (aiPlayer.getMp() >= c.GetMP() && c.GetPower() > bestPower) {
                    bestPower = c.GetPower();
                    bestIdx = i;
                }
            }
        }

        if (bestIdx != -1) {
            selectedDefenseCards.push_back(bestIdx);
        }

        currentPhase = BattlePhase::Reveal;
        revealIndex = 0;
        animationTimer = 15;
        return false;
    }

    // =============================================================
    // 7. プレイヤーの入力処理（UI・ボタン類）
    // =============================================================
    for (int i = 0; i < MAX; i++) isHoverIdx[i] = false;

    if (isSurrenderConfirm) {
        isHoverIdx[GIVE_UP] = IsMouseOver(425, 300, 150, 50, mouse);
        bool clickedReturnAgain = (mouse.leftClicked && IsMouseOver(10, 10, 100, 30, mouse));
        bool clickedOutside = (mouse.leftClicked && !IsMouseOver(300, 200, 400, 200, mouse));

        if (clickedReturnAgain || clickedOutside) {
            isSurrenderConfirm = false;
        }
        else if (mouse.leftClicked && isHoverIdx[GIVE_UP]) {
            selectedCards.clear();
            selectedDefenseCards.clear();
            playerTarget = false;
            targetIdx = -1;
            totalPower = 0;
            isSurrenderConfirm = false;
            selectedOption = RETURN;
            return true;
        }
        return false;
    }

    Player& turnPlayer = GetCurrentPlayer();

    isHoverIdx[RETURN] = IsMouseOver(10, 10, 100, 30, mouse);
    if (mouse.leftClicked && isHoverIdx[RETURN]) {
        isSurrenderConfirm = true;
    }

    const int DECISION_AREA_W = 150;
    const int DECISION_AREA_H = 40;

    // 攻撃決定ボタンの座標
    const int ATK_BTN_X = 250;
    const int ATK_BTN_Y = 150;

    // ★修正ポイント★ 防御決定ボタンの座標（攻撃と被らないように下にずらす）
    const int DEF_BTN_X = 250;
    const int DEF_BTN_Y = 220;

    // --- 攻撃フェーズ時の決定ボタン ---
    if (currentPhase == BattlePhase::Select && !selectedCards.empty() && isHumanTurn) {
        isHoverIdx[ATTACK] = IsMouseOver(ATK_BTN_X, ATK_BTN_Y, DECISION_AREA_W, DECISION_AREA_H, mouse);

        if (mouse.leftClicked && isHoverIdx[ATTACK]) {
            selectedOption = ATTACK;

            if (!playerTarget || targetIdx == -1) {
                CardCategory firstCardCat = Attack;
                if (!selectedCards.empty() && selectedCards[0] < (int)turnPlayer.GetHand().size()) {
                    firstCardCat = turnPlayer.GetHand()[selectedCards[0]].GetCategory();
                }

                bool isHeal = (firstCardCat == Healing || firstCardCat == MagicHealing);
                if (isHeal) {
                    targetIdx = currentTurnIdx;
                }
                else {
                    std::vector<int> aliveEnemies;
                    for (int i = 0; i < (int)Player_Turn.size(); ++i) {
                        if (i != currentTurnIdx && !Player_Turn[i].isDead()) {
                            aliveEnemies.push_back(i);
                        }
                    }
                    if (!aliveEnemies.empty()) {
                        targetIdx = aliveEnemies[rand() % aliveEnemies.size()];
                    }
                    else {
                        targetIdx = currentTurnIdx;
                    }
                }
                playerTarget = true;
            }

            currentPhase = BattlePhase::DefenseSelect;
            // ★修正ポイント★ trueを返すとバトルが強制終了するためfalseにする
            return false;
        }
    }
    // --- 防御フェーズ時の決定ボタン ---
    else if (currentPhase == BattlePhase::DefenseSelect && targetIdx == humanIdx) {
        isHoverIdx[ATTACK] = IsMouseOver(DEF_BTN_X, DEF_BTN_Y, DECISION_AREA_W, DECISION_AREA_H, mouse);

        if (mouse.leftClicked && isHoverIdx[ATTACK]) {
            currentPhase = BattlePhase::Reveal;
            revealIndex = 0;
            animationTimer = 15;
            // ★修正ポイント★ 同様にfalseにする
            return false;
        }
    }

    // =============================================================
    // 8. ターゲット選択判定
    // =============================================================
    if (currentPhase == BattlePhase::Select && isHumanTurn) {
        const int STATUS_START_X = 700;
        const int STATUS_START_Y = 75;
        const int STATUS_MARGIN_Y = 40;
        const int STATUS_WIDTH = 275;
        const int STATUS_HEIGHT = 30;

        for (int i = 0; i < (int)Player_Turn.size(); ++i) {
            int currentY = STATUS_START_Y + i * STATUS_MARGIN_Y;
            isHoverPlayerIdx[i] = IsMouseOver(STATUS_START_X, currentY - 15, STATUS_WIDTH, STATUS_HEIGHT, mouse);

            if (mouse.leftClicked && isHoverPlayerIdx[i]) {
                targetIdx = i;
                playerTarget = true;
            }
        }
    }

    // =============================================================
    // 9. 手札のカード選択判定
    // =============================================================
    hoveredCardIdx = -1;

    const float SCALE = 1.45f;
    const int CARD_W = (int)(50 * SCALE);
    const int CARD_H = (int)(50 * SCALE);
    const int START_X = 10;
    const int START_Y = 450;
    const int MARGIN = 2;
    const int MAX_CARDS_PER_ROW = 9;
    const int ROW_SPACING = CARD_H + 30;

    // ★修正ポイント★ 古い情報を持つ引数の player ではなく humanPlayer の手札を参照
    const auto& hand = humanPlayer.GetHand();

    for (int i = 0; i < hand.size(); ++i) {
        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;
        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

        if (mouse.x >= x && mouse.x <= x + CARD_W &&
            mouse.y >= y && mouse.y <= y + CARD_H + 25) {
            hoveredCardIdx = i;
        }

        bool isSelectable = true;

        if (currentPhase == BattlePhase::Select) {
            if (isHumanTurn) {
                int cat = hand[i].GetCategory();
                if (cat == Defense) isSelectable = false;
            }
            else {
                isSelectable = false;
            }
        }
        else if (currentPhase == BattlePhase::DefenseSelect) {
            if (targetIdx == humanIdx) {
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
            isHoverCardIdx[i] = IsMouseOver(x, y, CARD_W, CARD_H, mouse);

            if (mouse.leftClicked && isHoverCardIdx[i]) {
                const auto& currentHand = humanPlayer.GetHand();
                bool isClickedAddable = currentHand[i].GetAdd();

                CardCategory clickedCat = currentHand[i].GetCategory();
                bool isClickedHeal = (clickedCat == Healing || clickedCat == MagicHealing);

                std::vector<int>& activeSelection = (currentPhase == BattlePhase::Select) ? selectedCards : selectedDefenseCards;

                auto it = std::find(activeSelection.begin(), activeSelection.end(), i);
                if (it != activeSelection.end()) {
                    if (it == activeSelection.begin()) {
                        activeSelection.clear();
                        if (currentPhase == BattlePhase::Select) currentAttackElement = "無";
                    }
                    else {
                        activeSelection.erase(it);
                        if (currentPhase == BattlePhase::Select) RecalculateAttackElement(currentHand);
                    }
                }
                else {
                    if (activeSelection.empty()) {
                        activeSelection.push_back(i);
                        if (currentPhase == BattlePhase::Select) {
                            std::string baseType = currentHand[i].GetType();
                            currentAttackElement = (baseType == "") ? "無" : baseType;
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
                            if (currentPhase == BattlePhase::Select) {
                                std::string baseType = currentHand[i].GetType();
                                currentAttackElement = (baseType == "") ? "無" : baseType;
                            }
                        }
                        else {
                            if (baseCat == All || isBaseHeal) {
                                // 何もしない
                            }
                            else {
                                activeSelection.push_back(i);
                                if (currentPhase == BattlePhase::Select) {
                                    RecalculateAttackElement(currentHand);
                                }
                            }
                        }
                    }
                }

                totalPower = 0;
                const auto& handForPower = humanPlayer.GetHand();
                for (int idx : activeSelection) {
                    if (idx >= 0 && idx < (int)handForPower.size()) {
                        totalPower += handForPower[idx].GetPower();
                    }
                }
            }
        }
    }
    return false;
}

void Battle::RecalculateAttackElement(const std::vector<Card>& hand) {
    if (selectedCards.empty()) {
        currentAttackElement = _T("無");
        return;
    }

    // 1枚目の属性をベースにする
    std::string baseType = hand[selectedCards[0]].GetType();
    if (baseType == "") baseType = _T("無");

    bool hasNonLightAddition = false;

    // 2枚目以降をチェック
    for (size_t i = 1; i < selectedCards.size(); ++i) {
        std::string addType = hand[selectedCards[i]].GetType();
        if (addType == "") addType = _T("無");

        if (addType != _T("光")) {
            hasNonLightAddition = true;
        }
    }

    // 最終的な表示属性を決定
    if (hasNonLightAddition) {
        currentAttackElement = _T("無");
    }
    else {
        currentAttackElement = baseType;
    }
}

void Battle::Draw(const Player& player) {

    // =============================================================
    // 1. 安全ガード（配列の範囲外アクセスによるクラッシュを完全に防ぐ）
    // =============================================================
    if (Player_Turn.empty() || currentTurnIdx < 0 || currentTurnIdx >= (int)Player_Turn.size()) {
        return;
    }

    // =============================================================
    // 2. プレイヤー情報の特定（リアルタイムデータへの同期）
    // =============================================================
    int humanIdx = 0;
    for (int i = 0; i < (int)Player_Turn.size(); ++i) {
        if (Player_Turn[i].getName() == player.getName()) {
            humanIdx = i;
            break;
        }
    }
    bool isHumanTurn = (currentTurnIdx == humanIdx);

    // 最新の手札・ステータスを持つ実体（humanPlayer）を取得
    const Player& humanPlayer = Player_Turn[humanIdx];

    // 背景画像の描画
    DrawGraph(0, 0, Pic.Bat, TRUE);

    // 上下のラインを描画
    DrawBox(0, 0, 1000, 50, GetColor(0, 255, 255), TRUE);
    DrawBox(0, 750, 1000, 800, GetColor(0, 255, 255), TRUE);

    // ============================================================
    // ★重要修正★ 自分の手札は、誰のターンだろうが、どの演出フェーズだろうが
    // 画面の下部に「常に表示」されている必要があるため、条件分岐の外に出します。
    // ============================================================
    DrawPlayerHand(player);
    // ※ もし上記でカードが透明になってしまう場合は、下の「切り分けの確認」を見てください。

    // ============================================================
    // 3. 人間プレイヤーの操作フェーズ（自分が操作できる時だけのUI表示）
    // ============================================================
    bool isHumanOperableTurn = (currentPhase == BattlePhase::Select && isHumanTurn) ||
        (currentPhase == BattlePhase::DefenseSelect && targetIdx == humanIdx);

    if (isHumanOperableTurn) {
        // 選択中のカードを名前の下などに表示する
        DrawSelectedCard(humanPlayer);

        // 現在のフェーズに応じて、カードが選ばれているかチェック
        bool hasSelectedCard = (currentPhase == BattlePhase::Select && !selectedCards.empty()) ||
            (currentPhase == BattlePhase::DefenseSelect && !selectedDefenseCards.empty());

        // カードが1枚以上選ばれている時だけ決定ボタンを表示
        if (hasSelectedCard) {
            const int DECISION_AREA_X = 250;
            const int DECISION_AREA_W = 150;
            const int DECISION_AREA_H = 40;

            // 攻撃フェーズならY=150、防御フェーズならY=220にボタンを移動
            int decisionY = (currentPhase == BattlePhase::Select) ? 150 : 220;

            // マウスが重なっている（ホバー）かどうかで四角の色を変化させる
            unsigned int boxColor = isHoverIdx[ATTACK] ? GetColor(160, 160, 160) : GetColor(90, 90, 90);

            // 判定枠（四角）の描画
            DrawBox(DECISION_AREA_X, decisionY,
                DECISION_AREA_X + DECISION_AREA_W, decisionY + DECISION_AREA_H,
                boxColor, TRUE);

            // ボタンの文字描画
            DrawString(DECISION_AREA_X + 45, decisionY + 12, _T("決 定"), GetColor(255, 255, 255));
        }
    }
    // ============================================================
    // 4. 決定後（Revealフェーズ以降）のカードオープン演出描画
    // ============================================================
    else if (currentPhase >= BattlePhase::Reveal) {
        // 現在のターンプレイヤー（攻撃側）を取得
        Player& turnPlayer = GetCurrentPlayer();

        // 【攻撃カードのオープン描画】
        for (int i = 0; i < revealIndex; i++) {
            if (selectedCards.empty() || i >= (int)selectedCards.size()) break;

            int cardIdx = selectedCards[i];
            if (cardIdx >= 0 && cardIdx < (int)turnPlayer.GetHand().size()) {
                Card drawCard = turnPlayer.GetHand()[cardIdx];

                int drawX = 350 + (i * 75); // 中央付近に並べる
                int drawY = 350;            // 攻撃カードは Y=350 の位置

                // TODO: お手持ちの「カードを描画する関数」があれば有効化してください
                // drawCard.Draw(drawX, drawY); 
            }
        }

        // 【防御カードのオープン描画】
        if (targetIdx >= 0 && targetIdx < (int)Player_Turn.size()) {
            Player& targetPlayer = Player_Turn[targetIdx];

            for (int i = 0; i < (int)selectedDefenseCards.size(); i++) {
                int cardIdx = selectedDefenseCards[i];
                if (cardIdx >= 0 && cardIdx < (int)targetPlayer.GetHand().size()) {
                    Card drawCard = targetPlayer.GetHand()[cardIdx];

                    int drawX = 350 + (i * 75);
                    int drawY = 220; // 攻撃カード（Y=350）の少し上に並べる

                    // TODO: お手持ちの「カードを描画する関数」があれば有効化してください
                    // drawCard.Draw(drawX, drawY); 
                }
            }
        }
    }

    // ============================================================
    // 5. 共通UI・ステータス類の描画
    // ============================================================
    for (int i = 0; i < MAX; i++) {
        if (i == RETURN) {
            unsigned int color = isHoverIdx[i] ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
            DrawBox(10, 10, 100, 40, color, TRUE);
            DrawBox(9, 9, 101, 41, GetColor(0, 0, 0), FALSE);
        }
    }

    // 戻るボタンの文字表記
    DrawString(37, 17, _T("戻る"), GetColor(0, 0, 0));

    // プレイヤーのステータス欄を左に表示する関数
    DrawPlayerStatus(Player_Turn);

    // 今ターンのプレイヤーの名前を表示
    DrawTurnPlayerName(Player_Turn[currentTurnIdx]);

    // ターゲットの名前を表示する引数を targetIdx に修正
    if (playerTarget && targetIdx >= 0 && targetIdx < (int)Player_Turn.size()) {
        DrawTargetPlayerName(Player_Turn[targetIdx]);
    }

    // 中央下に「守 〇〇」と選択中の防御カードを出す関数
    DrawDefenseCards(humanPlayer);

    // 確認ウィンドウがONの時、最前面に描画
    if (isSurrenderConfirm) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
        DrawBox(0, 50, 1000, 750, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        DrawBox(300, 200, 700, 400, GetColor(255, 255, 255), TRUE);
        DrawBox(300, 200, 700, 400, GetColor(0, 0, 0), FALSE);

        DrawString(415, 240, "本当に降参しますか？", GetColor(0, 0, 0));

        unsigned int btnColor = isHoverIdx[GIVE_UP] ? GetColor(255, 100, 100) : GetColor(200, 0, 0);
        DrawBox(425, 300, 575, 350, btnColor, TRUE);
        DrawString(460, 315, "あきらめる", GetColor(255, 255, 255));
    }

    // 最下部に自分の名前を表示
    DrawFormatStringToHandle(
        10, 770,
        GetColor(0, 0, 0),
        Font.Small,
        _T("Name: %s"),
        humanPlayer.getName().c_str()
    );
}

void Battle::DrawTurnPlayerName(const Player& player) {
    int x = 15;
    int y = 70;
    int boxWidth = 250; // 枠のメイン部分の幅

    // 1. 文字列の横幅を取得
    // TCHAR型（_Tマクロ）を使用しているため、_tcslen で文字数を取得します
    int stringWidth = GetDrawStringWidthToHandle(
        player.getName().c_str(),
        (int)_tcslen(player.getName().c_str()),
        Font.Small
    );

    // 2. 中央揃えのためのX座標計算
    int drawX = x + (boxWidth - stringWidth) / 2;

    // --- 枠線UIの描画 ---
    // 外枠（黒）
    DrawCircle(x, y, 10, GetColor(0, 0, 0), FALSE);
    DrawCircle(x + boxWidth, y, 10, GetColor(0, 0, 0), FALSE);
    DrawBox(x, y - 10, x + boxWidth, y + 11, GetColor(0, 0, 0), FALSE);

    // 中身（白）
    DrawCircle(x, y, 9, GetColor(255, 255, 255), TRUE);
    DrawCircle(x + boxWidth, y, 9, GetColor(255, 255, 255), TRUE);
    DrawBox(x, y - 9, x + boxWidth, y + 10, GetColor(255, 255, 255), TRUE);

    // 3. 計算した drawX を使って名前を表示
    DrawFormatStringToHandle(
        drawX, y - 7,
        GetColor(200, 50, 50),
        Font.Small,
        _T("%s"),
        player.getName().c_str()
    );
}

void Battle::DrawTargetPlayerName(const Player& player) {
    // ターゲットが未定(-1)なら、何も描画せずに処理を終了する
    if (targetIdx < 0) return;

    int x = 350;
    int y = 70;
    int boxWidth = 250;
    int arrowColor = GetColor(0, 0, 0);
    int arrowY = y;

    // --- 1. 名前枠の描画（ターゲットが自分以外の時だけ実行） ---
    if (targetIdx != currentTurnIdx) {
        int stringWidth = GetDrawStringWidthToHandle(
            Player_Turn[targetIdx].getName().c_str(),
            (int)_tcslen(Player_Turn[targetIdx].getName().c_str()),
            Font.Small
        );
        int drawX = x + (boxWidth - stringWidth) / 2;

        // 外枠（黒）
        DrawCircle(x, y, 10, GetColor(0, 0, 0), FALSE);
        DrawCircle(x + boxWidth, y, 10, GetColor(0, 0, 0), FALSE);
        DrawBox(x, y - 10, x + boxWidth, y + 11, GetColor(0, 0, 0), FALSE);

        // 中身（白）
        DrawCircle(x, y, 9, GetColor(255, 255, 255), TRUE);
        DrawCircle(x + boxWidth, y, 9, GetColor(255, 255, 255), TRUE);
        DrawBox(x, y - 9, x + boxWidth, y + 10, GetColor(255, 255, 255), TRUE);

        // 名前表示（相手なので赤系）
        DrawFormatStringToHandle(
            drawX, y - 7,
            GetColor(200, 50, 50),
            Font.Small,
            _T("%s"),
            Player_Turn[targetIdx].getName().c_str()
        );
    }

    // --- 2. 矢印の描画処理（自分自身への矢印もここで制御） ---
    bool isVisibleArrow = true;

    // 将来的に「自分への回復の時は矢印すら消す」なら、ここに条件を追加
    // if (targetIdx == currentTurnIdx && isHealing) isVisibleArrow = false;

    if (isVisibleArrow) {
        if (targetIdx != currentTurnIdx) {
            // --- 相手への矢印 ( → ) ---
            int startX = 295;
            int endX = 320;
            DrawLine(startX, arrowY, endX, arrowY, arrowColor, 2);
            DrawTriangle(endX, arrowY, endX - 10, arrowY - 5, endX - 10, arrowY + 5, arrowColor, TRUE);
        }
        else {
            // --- 自分への矢印 ( ← ) ---
            // 名前枠は消えていても、この「自分を指す矢印」が出ることで選択中だとわかる
            int startX = 320;
            int endX = 295;
            DrawLine(startX, arrowY, endX, arrowY, arrowColor, 2);
            DrawTriangle(endX, arrowY, endX + 10, arrowY - 5, endX + 10, arrowY + 5, arrowColor, TRUE);
        }
    }
}

// 防御時のカード描画関数
void Battle::DrawDefenseCards(const Player& player) {

    // =============================================================
    // 1. 人間プレイヤーのインデックスを特定
    // =============================================================
    int humanIdx = 0;
    for (int i = 0; i < (int)Player_Turn.size(); ++i) {
        if (Player_Turn[i].getName() == player.getName()) {
            humanIdx = i;
            break;
        }
    }

    // =============================================================
    // 2. 自分がターゲットにされている「防御フェーズ」の時だけ描画
    // =============================================================
    if (currentPhase == BattlePhase::DefenseSelect && targetIdx == humanIdx) {

        // 描画開始位置（画面の中央下部・手札の少し上あたり）
        // 画面幅が1000なので、450あたりから描画すると大体真ん中になります
        const int DEF_UI_X = 450;
        const int DEF_UI_Y = 350;
        const int CARD_OFFSET_Y = 25; // カード名を縦に並べる間隔

        // 見出しの描画
        DrawString(DEF_UI_X, DEF_UI_Y - 25, _T("【選択中の防御カード】"), GetColor(255, 255, 0));

        const auto& hand = player.GetHand();

        // ★修正ポイント★ 攻撃用の selectedCards ではなく、防御用の selectedDefenseCards を使う
        for (size_t i = 0; i < selectedDefenseCards.size(); ++i) {
            int idx = selectedDefenseCards[i];

            // 手札の範囲内か安全確認
            if (idx >= 0 && idx < (int)hand.size()) {
                int drawX = DEF_UI_X;
                int drawY = DEF_UI_Y + (i * CARD_OFFSET_Y);

                // 加算カードかどうかで色を変える（加算は緑っぽく、通常は白）
                unsigned int color = hand[idx].GetAdd() ? GetColor(150, 255, 150) : GetColor(255, 255, 255);

                // カード名を描画
                DrawFormatStringToHandle(
                    drawX, drawY, color, Font.Small,
                    _T("%s"),
                    hand[idx].GetName().c_str()
                );
            }
        }

        // =============================================================
        // 3. 防御力の合計値を「守 〇〇」の形で一番下に表示
        // =============================================================
        // 選択したカードの枚数分だけ下にずらす
        int totalDefY = DEF_UI_Y + (selectedDefenseCards.size() * CARD_OFFSET_Y) + 10;

        // 合計防御力（totalPower）の表示
        DrawFormatStringToHandle(
            DEF_UI_X, totalDefY, GetColor(0, 255, 255), Font.Small,
            _T("守 %d"), totalPower
        );
    }
}

// ターンを次のプレイヤーに回す関数（Battle.cpp 内に実装）
void Battle::NextTurn() {
    if (Player_Turn.empty()) return;

    // 次のプレイヤーへ
    currentTurnIdx = (currentTurnIdx + 1) % Player_Turn.size();

    // 前のターンの選択情報を完全にリセット
    selectedCards.clear();
    totalPower = 0;
    playerTarget = false;
    targetIdx = -1;

    currentPhase = BattlePhase::Select;
}

// プレイヤーが脱落した際の処理
void Battle::RemovePlayer(int targetIdx) {
    if (targetIdx < 0 || targetIdx >= Player_Turn.size()) return;

    // プレイヤーをvectorから削除
    Player_Turn.erase(Player_Turn.begin() + targetIdx);

    // 削除によってターンの順番が狂わないように調整する
    if (targetIdx < currentTurnIdx) {
        // 現在のプレイヤーより前の人が抜けた場合、自分のインデックスが1つ前にズレる
        currentTurnIdx--;
    }
    else if (currentTurnIdx >= Player_Turn.size()) {
        // 現在のプレイヤー（最後尾）が抜けて、サイズが縮んだ場合は0番目に戻す
        currentTurnIdx = 0;
    }
}

// 今ターンのプレイヤーの参照を返す便利関数を作っておくと良いです
Player& Battle::GetCurrentPlayer() {
    return Player_Turn[currentTurnIdx];
}

// プレイヤーのステータス表示
void Battle::DrawPlayerStatus(const std::vector<Player>& players) {
    const int startX = 700;       // X開始点
    const int startY = 75;       // 1人目のY開始点
    const int marginY = 40;       // プレイヤーごとの間隔（枠の高さ + 余白）

    for (size_t i = 0; i < players.size(); ++i) {
        int currentY = startY + (int)i * marginY;

        // --- 変更点：ホバー中またはターゲット選択中なら色を変える ---
        unsigned int bgColor = GetColor(255, 255, 255); // 基本は白
        if (isHoverPlayerIdx[i]) {
            bgColor = GetColor(255, 255, 200); // ホバー中は薄黄色
        }
        if (playerTarget && targetIdx == i) {
            bgColor = GetColor(255, 200, 200); // 選択済みのターゲットは薄赤色
        }

        // 枠の描画（黒い縁取り）
        DrawCircle(startX, currentY, 15, GetColor(0, 0, 0), FALSE);
        DrawCircle(startX + 275, currentY, 15, GetColor(0, 0, 0), FALSE);
        DrawBox(startX, currentY - 15, startX + 275, currentY + 16, GetColor(0, 0, 0), FALSE);

        // 枠の描画（背景色：bgColorを使用）
        DrawCircle(startX, currentY, 14, bgColor, TRUE);
        DrawCircle(startX + 275, currentY, 14, bgColor, TRUE);
        DrawBox(startX, currentY - 14, startX + 275, currentY + 15, bgColor, TRUE);

        // 名前表示
        DrawFormatStringToHandle(
            startX, currentY - 7,
            GetColor(0, 155, 155),
            Font.Small,
            _T("%s"),
            players[i].getName().c_str()
        );

        DrawFormatStringToHandle(
            startX+135, currentY - 7,
            GetColor(0, 0, 0),
            Font.Small,
            _T("HP %2d MP %2d ￥ %2d "),
            players[i].getHp(), players[i].getMp(), players[i].getMoney()
        );
    }
}

void Battle::DrawPlayerHand(const Player& player) {
    // 手札を取得
    const auto& hand = player.GetHand();

    // --- サイズ・レイアウト設定 ---
    const float SCALE = 1.45f;                  // 拡大倍率
    const int BASE_W = 50;                      // 元のカード幅
    const int BASE_H = 50;                      // 元のカード高さ
    const int CARD_W = (int)(BASE_W * SCALE);   // 拡大後の幅 (100)
    const int CARD_H = (int)(BASE_H * SCALE);   // 拡大後の高さ (100)

    const int START_X = 10;                     // 1枚目のX座標
    const int START_Y = 450;                    // 手札を表示するY座標（サイズアップに合わせて少し上に調整）
    const int MARGIN = 2;                       // カード同士の隙間（2倍に調整）

    // 改行用の変数
    const int MAX_CARDS_PER_ROW = 9;            // 1段の枚数（大きくなったので10枚だと画面からはみ出す可能性あり。適宜調整してください）
    const int ROW_SPACING = CARD_H + 30;        // 段ごとの縦の間隔


    // ここでも攻撃ターンかどうかを判定
    bool isAttackTurn = (player.getName() == Player_Turn[currentTurnIdx].getName());

    for (int i = 0; i < hand.size(); ++i) {

        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;

        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

        // このカードが今のターンで使えるか判定
        bool isSelectable = false;
        int cat = hand[i].GetCategory();

        if (isAttackTurn) {
            if (cat != Defense) isSelectable = true;
        }
        else {
            if (cat == Defense || cat == Bilingual) isSelectable = true;
        }

        // カード画像の描画
        int picIdx = hand[i].graphicIndex;

        if (picIdx >= 0 && picIdx < 100) {

            // もしマウスカーソルが重なった時は若干白くさせる
            if (isHoverCardIdx[i]) {
                // ブレンドモードを「加算」に設定（0〜255で白さの強さを調節）
                SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
            }
            // DrawExtendGraph(左上X, 左上Y, 右下X, 右下Y, グラフィックハンドル, 透過フラグ)
            DrawExtendGraph(x, y, x + CARD_W, y + CARD_H, Pic.Card[picIdx], TRUE);

            // 描き終わったら必ず「ノーブレンド」に戻す
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        else {
            // エラー時の赤い箱も拡大サイズに合わせる
            DrawBox(x, y, x + CARD_W, y + CARD_H, GetColor(255, 0, 0), TRUE);
            printfDx(_T("Error: CardIndex %d out of range!\n"), picIdx);
        }

        // カードの枠線
        DrawBox(x, y, x + CARD_W, y + CARD_H, GetColor(0, 0, 0), FALSE);

        // --- 属性・数値の描画 ---
        int Col = GetColor(0, 0, 0);
        if (hand[i].GetType() == "炎") { Col = GetColor(255, 0, 0); }
        else if (hand[i].GetType() == "水") { Col = GetColor(0, 0, 255); }
        else if (hand[i].GetType() == "木") { Col = GetColor(0, 155, 0); }
        else if (hand[i].GetType() == "光") { Col = GetColor(155, 155, 0); }
        else if (hand[i].GetType() == "闇") { Col = GetColor(255, 100, 255); }

        // テキストエリアの設定（カードのすぐ下に配置）
        int textAreaY = y + CARD_H;
        int textAreaH = 25; // テキスト背景の高さ

        // カテゴリごとに描画する内容を変更する分岐
        switch (hand[i].GetCategory()) {
            int w;
            TCHAR buf[64]; // フォーマット用バッファ

            // 攻撃カード
        case Attack:
            DrawBox(x, textAreaY, x + CARD_W, textAreaY + textAreaH, GetColor(255, 255, 200), TRUE);
            _stprintf_s(buf, hand[i].GetAdd() ? _T("+攻%d") : _T("攻%d"), hand[i].GetPower());
            w = GetDrawStringWidth(buf, (int)_tcslen(buf));
            DrawString(x + (CARD_W - w) / 2, textAreaY + 4, buf, Col);
            break;

            // 攻防カード
        case Bilingual:
            DrawBox(x, textAreaY, x + CARD_W, textAreaY + textAreaH, GetColor(255, 255, 200), TRUE);

            if (currentPhase == BattlePhase::Select) {
                // 選択フェーズ中は、今の役割（攻or守）に合わせて表示
                if (isAttackTurn) {
                    _stprintf_s(buf, hand[i].GetAdd() ? _T("+攻%d") : _T("攻%d"), hand[i].GetPower());
                }
                else {
                    _stprintf_s(buf, _T("守%d"), hand[i].GetPower());
                }
            }
            else {
                // 通常時は攻の方に合わせる
                _stprintf_s(buf, _T("守%d"), hand[i].GetPower());
            }

            w = GetDrawStringWidth(buf, (int)_tcslen(buf));
            DrawString(x + (CARD_W - w) / 2, textAreaY + 4, buf, Col);
            break;

            // 奇跡カード
        case Magic:
            if (hand[i].GetPower() > 0) {
                DrawBox(x, textAreaY, x + CARD_W, textAreaY + textAreaH, GetColor(255, 255, 200), TRUE);
                _stprintf_s(buf, hand[i].GetAdd() ? _T("+攻%d") : _T("攻%d"), hand[i].GetPower());
                w = GetDrawStringWidth(buf, (int)_tcslen(buf));
                DrawString(x + (CARD_W - w) / 2, textAreaY + 4, buf, Col);
            }
            break;

            // 防御カード
        case Defense:
            DrawBox(x, textAreaY, x + CARD_W, textAreaY + textAreaH, GetColor(255, 255, 200), TRUE);
            _stprintf_s(buf, _T("守%d"), hand[i].GetPower());
            w = GetDrawStringWidth(buf, (int)_tcslen(buf));
            DrawString(x + (CARD_W - w) / 2, textAreaY + 4, buf, Col);
            break;

            // 全体攻撃カード
        case All:
            DrawBox(x, textAreaY, x + CARD_W, textAreaY + textAreaH, GetColor(255, 255, 200), TRUE);
            _stprintf_s(buf, _T("%d%%攻%d"), hand[i].GetPercent(), hand[i].GetPower());
            w = GetDrawStringWidth(buf, (int)_tcslen(buf));
            DrawString(x + (CARD_W - w) / 2, textAreaY + 4, buf, Col);
            break;

        default:
            break;
        }

        // ここも「選択フェーズ」かつ「選択不可」なときだけ実行する
        if (currentPhase == BattlePhase::Select && !isSelectable) {
            // アルファブレンドで半透明の黒を描画 (150/255くらいの濃さがおすすめ)
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);

            // カード本体とテキストエリア(textAreaH=25)をすっぽり覆うように黒を描画
            DrawBox(x, y, x + CARD_W, y + CARD_H, GetColor(0, 0, 0), TRUE);

            // 描画モードを通常に戻す（これを忘れると以降の描画が全て半透明になります）
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

    }

    // マウスカーソルが重なった際に説明文を表示する処理
    if (hoveredCardIdx != -1 && hoveredCardIdx < hand.size()) {
        const auto& card = hand[hoveredCardIdx];

        // --- 属性・数値の描画 ---
        int Col = GetColor(0, 0, 0);
        if (hand[hoveredCardIdx].GetType() == "炎") { Col = GetColor(255, 0, 0); }
        else if (hand[hoveredCardIdx].GetType() == "水") { Col = GetColor(0, 0, 255); }
        else if (hand[hoveredCardIdx].GetType() == "木") { Col = GetColor(0, 155, 0); }
        else if (hand[hoveredCardIdx].GetType() == "光") { Col = GetColor(155, 155, 0); }
        else if (hand[hoveredCardIdx].GetType() == "闇") { Col = GetColor(255, 100, 255); }

        // 選択してるカードの座標を再計算

        // レイアウト定数
        const int BOX_X1 = 685; // 説明ボックスのX開始点
        const int BOX_Y1 = 450; // Y開始点
        const int BOX_X2 = 995; // X終了点
        const int BOX_Y2 = 600; // Y終了点
        const int PADDING = 10; // ボックス内の余白

        // 背景ボックスの描画
        DrawBox(BOX_X1, BOX_Y1, BOX_X2, BOX_Y2, GetColor(255, 255, 200), TRUE);      // 背景
        DrawBox(BOX_X1, BOX_Y1, BOX_X2, BOX_Y2, GetColor(0, 0, 0), FALSE);           // 枠線

        // カード画像の描画変数
        const float img_s = 1.5f;               // 画像拡大率
        const int img_w = (int)(50 * img_s);    // 横幅
        const int img_h = (int)(50 * img_s);    // 縦幅

        // 画像を配置変数
        int imgX = BOX_X1 + PADDING;
        int imgY = BOX_Y1 + PADDING + 25;       // 名前表示の分だけ下げる

        // 画像描画
        DrawExtendGraph(imgX, imgY, imgX + img_w, imgY + img_h, Pic.Card[card.graphicIndex], TRUE);
        DrawBox(imgX, imgY, imgX + img_w, imgY + img_h, GetColor(0, 0, 0), FALSE);

        // カード名テキスト
        DrawFormatString(710, 460, Col, _T("[%s]"), card.GetName().c_str());

        // --- 4. 説明文の描画 (画像の右側に改行して表示) ---
        int textX = imgX + img_w + PADDING;
        int textY = imgY;

        DrawFormatString(textX, textY, GetColor(0, 0, 0), _T("%s"), card.GetDescription().c_str());

        // 金額表示(奇跡のみ表示しない)
        if (card.GetCategory() != Magic) {
            DrawFormatString(textX, textY + 40, GetColor(0, 0, 0), _T("\\%d"), card.GetMoney());
        }
        if (card.GetCategory() == Bilingual) {
            DrawFormatString(textX, textY + 20, GetColor(0, 0, 0), _T("攻%d 守%d"), card.GetPower(), card.GetPower());
        }
        if (card.GetCategory() == Healing) {
            DrawFormatString(textX, textY + 20, GetColor(0, 200, 0), _T("HP+%d"), card.GetPower());
        }
        if (card.GetCategory() == MagicHealing) {
            DrawFormatString(textX, textY + 20, GetColor(50, 50, 255), _T("MP+%d"), card.GetPower());
        }

        // 奇跡の消費MPを表示
        if (card.GetCategory() == Magic) {
            DrawFormatString(textX, textY + 20, GetColor(50, 50, 255), _T("MP-%d"), card.GetMP());
        }

    }
}

void Battle::DrawSelectedCard(const Player& player) {
    // まだカードが選ばれていない、または手札の範囲外なら何もしない
    if (selectedCards.empty()) return;

    const auto& hand = player.GetHand(); // 手札を参照

    // サイズ設定（手札より少し小さめ）
    const float SCALE = 1.0f;
    const int CARD_W = (int)(50 * SCALE);
    const int CARD_H = (int)(50 * SCALE);

    // 描画開始座標
    int startX = 15;
    int startY = 95;

    // --- UIボックスの固定サイズ（元コードの横幅250を基準） ---
    const int boxWidth = 250;

    // -------------------------------------------------------------
    // ★【変更】固定の計算ではなく、Updateで計算されたアニメーション変数を適用
    // -------------------------------------------------------------
    int yOffset = (int)currentYOffset;
    // -------------------------------------------------------------

    // --- 選択されたすべてのカードを縦リストとして描画 ---
    for (int i = 0; i < (int)selectedCards.size(); ++i) {
        int handIdx = selectedCards[i];
        if (handIdx >= 0 && handIdx < (int)hand.size()) {
            const auto& card = hand[handIdx];

            int drawX = startX;
            int drawY = startY + (i * yOffset); // アニメーションするyOffsetで配置

            // 1. 背面のテキストエリア（UIボックス）の描画
            DrawBox(drawX - 5, drawY - 5, drawX + boxWidth, drawY + CARD_H + 5, GetColor(255, 255, 200), TRUE);
            DrawBox(drawX - 5, drawY - 5, drawX + boxWidth, drawY + CARD_H + 5, GetColor(0, 0, 0), FALSE);

            // 2. カード画像の描画
            int picIdx = card.graphicIndex;
            if (picIdx >= 0 && picIdx < 100) {
                DrawExtendGraph(drawX, drawY, drawX + CARD_W, drawY + CARD_H, Pic.Card[picIdx], TRUE);
            }
            DrawBox(drawX, drawY, drawX + CARD_W, drawY + CARD_H, GetColor(0, 0, 0), FALSE);

            int textX = drawX + CARD_W + 15;

            // 3. 属性色の取得
            int Col = GetColor(0, 0, 0);
            if (card.GetType() == "炎") { Col = GetColor(255, 0, 0); }
            else if (card.GetType() == "水") { Col = GetColor(0, 0, 255); }
            else if (card.GetType() == "木") { Col = GetColor(0, 155, 0); }
            else if (card.GetType() == "光") { Col = GetColor(155, 155, 0); }
            else if (card.GetType() == "闇") { Col = GetColor(255, 100, 255); }

            // 4. カテゴリごとの文字描画
            TCHAR buf[64] = _T("");
            bool hasText = true;

            switch (card.GetCategory()) {
            case Attack:
            case Bilingual:
                _stprintf_s(buf, card.GetAdd() ? _T("+攻%d") : _T("攻%d"), card.GetPower());
                break;
            case Magic:
                if (card.GetPower() > 0) {
                    _stprintf_s(buf, card.GetAdd() ? _T("+攻%d") : _T("攻%d"), card.GetPower());
                }
                else {
                    hasText = false;
                }
                break;
            case Defense:
                _stprintf_s(buf, _T("守%d"), card.GetPower());
                break;
            case All:
                _stprintf_s(buf, _T("%d%%攻%d"), card.GetPercent(), card.GetPower());
                break;
            case Healing:
                _stprintf_s(buf, _T("HP+%d"), card.GetPower());
                break;
            case MagicHealing:
                _stprintf_s(buf, _T("MP+%d"), card.GetPower());
                break;
            default:
                hasText = false;
                break;
            }

            if (hasText) {
                DrawFormatString(textX, drawY + 2, Col, _T("[%s]"), card.GetName().c_str());
                DrawString(textX, drawY + 22, buf, Col);
            }
        }
    }

    // =============================================================
    // 合計威力の表示（手札一覧の少し上に表示。ここも連動して滑らかに動きます）
    // =============================================================
    int baseIdx = selectedCards[0];
    bool isBaseHealCard = false;
    if (baseIdx >= 0 && baseIdx < (int)hand.size()) {
        CardCategory baseCat = hand[baseIdx].GetCategory();
        if (baseCat == Healing || baseCat == MagicHealing) {
            isBaseHealCard = true;
        }
    }

    if (!isBaseHealCard) {
        bool isAttackTurn = (player.getName() == Player_Turn[currentTurnIdx].getName());

        int totalDrawX = startX + 100;
        int totalDrawY = 400; // ※手札の少し上の位置に固定

        if (isAttackTurn) {
            int totalCol = GetColor(0, 0, 0);
            if (currentAttackElement == "炎") { totalCol = GetColor(255, 0, 0); }
            else if (currentAttackElement == "水") { totalCol = GetColor(0, 0, 255); }
            else if (currentAttackElement == "木") { totalCol = GetColor(0, 155, 0); }
            else if (currentAttackElement == "光") { totalCol = GetColor(155, 155, 0); }
            else if (currentAttackElement == "闇") { totalCol = GetColor(255, 100, 255); }

            DrawFormatString(totalDrawX, totalDrawY, totalCol, _T("攻 %d"), totalPower);
        }
        else {
            DrawFormatString(totalDrawX, totalDrawY, GetColor(0, 255, 255), _T("守 %d"), totalPower);
        }
    }
}

// 合計攻撃データを計算する関数
TotalAttack Battle::CalculateTotalAttack(Player& attacker) {
    TotalAttack total;
    auto& hand = attacker.GetHand();    // 手札参照

    if (selectedCards.empty()) return total;

    // =============================================================
    // ★ 安全ガード：選択されたカードが手札の範囲内に実在するかチェック
    // =============================================================
    if (selectedCards[0] < 0 || selectedCards[0] >= (int)hand.size()) {
        return total; // 範囲外ならエラーを防ぐため空データを返す
    }

    // 1枚目（ベースカード）の属性を記憶しておく
    std::string baseType = hand[selectedCards[0]].GetType();
    if (baseType == "") baseType = _T("無"); // 空文字なら無属性扱い

    bool hasNonLightAddition = false; // 「光以外の加算カード」があるかどうかの目印

    // 選択されたカードを順に処理
    for (size_t i = 0; i < selectedCards.size(); ++i) {
        int index = selectedCards[i];

        // =============================================================
        // ★ 安全ガード：2枚目以降のカードも範囲内かチェック
        // =============================================================
        if (index < 0 || index >= (int)hand.size()) {
            continue; // 範囲外のインデックスは無視して次へ
        }

        const Card& card = hand[index];

        // 威力を加算
        total.power += card.GetPower();

        // 全体攻撃(All)の判定
        if (card.GetCategory() == All) {
            total.isAll = true;
            total.hitPercent = card.GetPercent();
        }

        // ★ 2枚目以降（加算カード）の属性チェック
        if (i > 0) {
            std::string addType = card.GetType();
            if (addType == "") addType = _T("無");

            // もし光属性「以外」のカードが混ざっていたらフラグをONにする
            if (addType != _T("光")) {
                hasNonLightAddition = true;
            }
        }
    }

    // ★ 属性の最終決定ルール
    if (hasNonLightAddition) {
        // 光以外の加算が1枚でもあれば「無属性」
        total.type = _T("無");
    }
    else {
        // 2枚目以降がすべて光属性（または加算なし）なら、1枚目の属性を引き継ぐ
        total.type = baseType;
    }

    return total;
}

void Battle::ResolveDamage(Player& target, const TotalAttack& attack, const Card* defenseCard) {

    // 1. 命中判定
    if (attack.isAll) {
        if ((rand() % 100) > attack.hitPercent) return;
    }

    int finalDamage = attack.power;

    // 2. ★ガード判定ロジックの追加★
    if (defenseCard != nullptr) {
        // ここで自作の IsValidGuard を呼ぶ
        if (DamageResolver::IsValidGuard(attack.type, defenseCard->GetType())) {
            // ガード成功なら、攻撃力から防御力を引く
            finalDamage -= defenseCard->GetPower();
        }
        else {
            // ガード失敗（相性不良）なら、finalDamage は attack.power のまま
        }
    }

    // ダメージがマイナスにならないように
    if (finalDamage < 0) finalDamage = 0;

    // 3. 闇属性の特殊ルール
    if (attack.type == "闇") {
        if (finalDamage > 0) {
            target.setHp(0);
            target.setDead(true);
        }
    }
    else {
        // 4. 通常ダメージ適用
        target.setHp(target.getHp() - finalDamage);
        if (target.getHp() <= 0) target.setDead(true);
    }
}

// AI関係の関数
// Battleクラス内、またはAI制御関数として作成
// aiPlayer: 敵AIのプレイヤーオブジェクト
// aiSelectedCards: AIが場に出すカードを入れるリスト(std::vector<Card>)
void DecideAIAttackCard(Player& aiPlayer, std::vector<Card>& aiSelectedCards) {
    aiSelectedCards.clear(); // 選択リストをリセット

    int bestIndex = -1;
    int maxPower = -1;

    // AIの手札を取得
    const std::vector<Card>& hand = aiPlayer.GetHand();

    for (int i = 0; i < hand.size(); ++i) {
        const Card& c = hand[i];

        // カードが「攻撃」または「両用」かチェック
        if (c.GetCategory() == CardCategory::Attack || c.GetCategory() == CardCategory::Bilingual) {

            // MPが足りているかチェック
            if (aiPlayer.getMp() >= c.GetMP()) {

                // 一番威力が高いものを更新
                if (c.GetPower() > maxPower) {
                    maxPower = c.GetPower();
                    bestIndex = i;
                }
            }
        }
    }

    // 使えるカードが見つかった場合
    if (bestIndex != -1) {
        // 選んだカードを選択リストに追加
        aiSelectedCards.push_back(hand[bestIndex]);

        // MPを消費させる（必要であれば）
        // aiPlayer.setMp(aiPlayer.getMp() - hand[bestIndex].GetMP());

        // 手札から削除
        aiPlayer.RemoveHand(bestIndex);
    }
    // ※見つからなかった場合（bestIndex == -1）は「何もしない（パス）」になります
}

// incomingDamage: プレイヤーの攻撃力（受ける予定のダメージ）
void DecideAIDefenseCard(Player& aiPlayer, std::vector<Card>& aiSelectedCards, int incomingDamage) {
    aiSelectedCards.clear();

    int bestIndex = -1;
    int bestPower = -1;

    const std::vector<Card>& hand = aiPlayer.GetHand();

    for (int i = 0; i < hand.size(); ++i) {
        const Card& c = hand[i];

        // カードが「防御」または「両用」かチェック
        if (c.GetCategory() == CardCategory::Defense || c.GetCategory() == CardCategory::Bilingual) {

            // MPが足りているかチェック
            if (aiPlayer.getMp() >= c.GetMP()) {

                // 一番防御力が高いものを探す
                if (c.GetPower() > bestPower) {
                    bestPower = c.GetPower();
                    bestIndex = i;
                }
            }
        }
    }

    // 使えるカードが見つかった場合
    if (bestIndex != -1) {
        aiSelectedCards.push_back(hand[bestIndex]);
        // aiPlayer.setMp(aiPlayer.getMp() - hand[bestIndex].GetMP());
        aiPlayer.RemoveHand(bestIndex);
    }
}