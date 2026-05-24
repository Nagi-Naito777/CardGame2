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

// 戦闘ロジックの中身を見やすくするために分割したヘッダー
#include "BattleAI.h"
#include "BattleUI.h"
#include "BattleInput.h"
#include "BattleLogic.h"
#include "BattlePhase.h"

// コンストラクタの実体
// ↓ selectedOption((int)BattleOption::NONE) に変更
Battle::Battle() : currentTurnIdx(0), targetIdx(-1), selectCard(-1), playerTarget(false), selectedOption((int)BattleOption::NONE), hoveredCardIdx(-1) {
    // ボタン数分のマウス判定変数を初期化
    // ↓ (int)BattleOption::MAX に変更
    for (int i = 0; i < (int)BattleOption::MAX; i++) {
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

    // 1〜2. 安全ガードとプレイヤー情報の特定 (変更なし)
    if (Player_Turn.empty() || currentTurnIdx < 0 || currentTurnIdx >= (int)Player_Turn.size()) {
        return false;
    }

    int humanIdx = 0;
    for (int i = 0; i < (int)Player_Turn.size(); ++i) {
        if (Player_Turn[i].getName() == player.getName()) {
            humanIdx = i;
            break;
        }
    }
    bool isHumanTurn = (currentTurnIdx == humanIdx);
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
    // 6. AIの自動行動ロジック (BattleAIに委譲)
    // =============================================================
    BattleAI::Update(*this, humanIdx, isHumanTurn);

    // =============================================================
    // 7〜9. 入力処理（BattleInputに委譲！）
    // =============================================================
    if (BattleInput::Update(*this, mouse, humanPlayer, humanIdx, isHumanTurn)) {
        return true; // 降参などでバトルを抜ける場合
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
    // 安全ガード（配列の範囲外アクセスによるクラッシュを完全に防ぐ）
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

    DrawFormatString(0, 0, GetColor(255, 0, 0), "HandCount: %d", humanPlayer.GetHandCount());

    // 自分の手札をBattleUI経由で描画
    BattleUI::DrawPlayerHand(
        humanPlayer,     // 自分の情報
        Player_Turn,     // プレイヤー一覧
        currentTurnIdx,  // 現在のターンインデックス
        currentPhase,    // 現在のフェーズ
        hoveredCardIdx,  // マウスホバー中のカードindex（もしなければ -1）
        isHoverCardIdx   // カードごとのホバー状態配列
    );

    // ============================================================
    // 3. 人間プレイヤーの操作フェーズ（自分が操作できる時だけのUI表示）
    // ============================================================
    bool isHumanOperableTurn = (currentPhase == BattlePhase::Select && isHumanTurn) ||
        (currentPhase == BattlePhase::DefenseSelect && targetIdx == humanIdx);

    if (isHumanOperableTurn) {
        // --- ★ここを修正★ ---
        // フェーズに応じて、描画するカードのリストを切り替える
        const std::vector<int>& targetCards = (currentPhase == BattlePhase::Select) ? selectedCards : selectedDefenseCards;

        BattleUI::DrawSelectedCard(
            humanPlayer,            // プレイヤー情報
            Player_Turn,            // プレイヤーリスト
            currentTurnIdx,         // 現在のターン
            targetCards,            // 選択中のカードリスト
            currentYOffset,         // Yオフセット
            totalPower,             // 合計威力
            currentAttackElement    // 属性
        );
        // --------------------

        // 現在のフェーズに応じて、決定ボタン等の表示判定
        bool hasSelectedCard = (currentPhase == BattlePhase::Select && !selectedCards.empty()) ||
            (currentPhase == BattlePhase::DefenseSelect && !selectedDefenseCards.empty());

        if (hasSelectedCard) {
            // ...（ボタン描画の既存コードはそのまま）
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
    for (int i = 0; i < (int)BattleOption::MAX; i++) {
        // ↓ (int)BattleOption::RETURN に変更
        if (i == (int)BattleOption::RETURN) {
            unsigned int color = isHoverIdx[i] ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
            DrawBox(10, 10, 100, 40, color, TRUE);
            DrawBox(9, 9, 101, 41, GetColor(0, 0, 0), FALSE);
        }
    }

    // 戻るボタンの文字表記
    DrawString(37, 17, _T("戻る"), GetColor(0, 0, 0));

    // プレイヤーのステータス欄（BattleUI経由で詳細なホバー判定込み）
    BattleUI::DrawPlayerStatus(
        Player_Turn,     // プレイヤー一覧
        playerTarget,    // ターゲット選択中フラグ
        targetIdx,       // ターゲットのインデックス
        isHoverPlayerIdx // プレイヤーごとのホバー状態配列
    );

    // 今ターンのプレイヤーの名前を表示
    BattleUI::DrawTurnPlayerName(Player_Turn[currentTurnIdx]);

    // ターゲットの名前と矢印を表示
    if (playerTarget) {
        BattleUI::DrawTargetPlayerName(Player_Turn, currentTurnIdx, targetIdx);
    }

    // 中央下に「守 〇〇」と選択中の防御カードを出す関数
    BattleUI::DrawDefenseCardsText(humanPlayer, currentPhase, targetIdx, humanIdx, selectedDefenseCards, totalPower);

    // 確認ウィンドウがONの時、最前面に描画
    if (isSurrenderConfirm) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
        DrawBox(0, 50, 1000, 750, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        DrawBox(300, 200, 700, 400, GetColor(255, 255, 255), TRUE);
        DrawBox(300, 200, 700, 400, GetColor(0, 0, 0), FALSE);

        DrawString(415, 240, "本当に降参しますか？", GetColor(0, 0, 0));

        unsigned int btnColor = isHoverIdx[(int)BattleOption::GIVE_UP] ? GetColor(255, 100, 100) : GetColor(200, 0, 0);
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
        aiSelectedCards.push_back(hand[bestIndex]);
        
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
        
    }
}