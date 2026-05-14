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

    // --- 1. 演出フェーズの処理（入力は受け付けない） ---
    if (currentPhase == BattlePhase::Effect || currentPhase == BattlePhase::Damage) {
        if (animationTimer > 0) {
            animationTimer--;
        }

        // タイマーが0になった時のフェーズ移行処理
        if (animationTimer == 0) {
            if (currentPhase == BattlePhase::Effect) {
                // 【Effect終了】 -> ダメージ計算をしてDamageフェーズへ

                // 1. 攻撃者とターゲットを取得
                Player& attacker = GetCurrentPlayer();
                Player& target = Player_Turn[targetIdx];

                // 2. 攻撃カードの取得（選択された最初のカードを渡す）
                // ※selectedCards には選んだ手札のインデックス(添字)が入っています
                int atkCardIdx = selectedCards[0];
                Card attackCard = attacker.GetHand()[atkCardIdx];

                // 3. ターゲットの防御カード（とりあえず今は nullptr で「無防備」にしておく）
                Card* defenseCard = nullptr;

                // 4. ★ここで一発で計算＆HP減少を適用！★
                DamageResolver::ExecuteAttack(target, attackCard, defenseCard);

                currentPhase = BattlePhase::Damage;
                animationTimer = 90; // ダメージ表示の演出時間
            }
            else if (currentPhase == BattlePhase::Damage) {
                // 【Damage終了】 -> ターン終了、次の人へ

            // （※もしここで target.isDead() が true なら、脱落処理を行う）
                if (Player_Turn[targetIdx].isDead()) {
                    RemovePlayer(targetIdx);
                }

                selectedCards.clear();
                playerTarget = false;
                targetIdx = -1;

                currentPhase = BattlePhase::Select;
                NextTurn();
            }
        }
        // 演出中はクリック処理などに進ませないため、ここで処理を終了する
        return false;
    }

    // 全てのホバー状態を一旦リセット
    for (int i = 0; i < MAX; i++) isHoverIdx[i] = false;

    // --- 2. 確認ウィンドウが表示されている時の処理 ---
    if (isSurrenderConfirm) {
        // ウィンドウ内の「あきらめる」ボタンの判定に GIVE_UP を使用
        isHoverIdx[GIVE_UP] = IsMouseOver(425, 300, 150, 50, mouse);

        // 「戻る(RETURN)」ボタンをもう一度押したか、枠外をクリックで閉じる
        bool clickedReturnAgain = (mouse.leftClicked && IsMouseOver(10, 10, 100, 30, mouse));
        bool clickedOutside = (mouse.leftClicked && !IsMouseOver(300, 200, 400, 200, mouse));

        if (clickedReturnAgain || clickedOutside) {
            isSurrenderConfirm = false;
        }
        // 「あきらめる(GIVE_UP)」をクリックした時
        else if (mouse.leftClicked && isHoverIdx[GIVE_UP]) {
            selectedCards.clear();
            playerTarget = false;
            targetIdx = -1;
            totalPower = 0;
            isSurrenderConfirm = false;

            selectedOption = RETURN;    // 外部（シーン管理側）には「戻る」として通知
            return true;                // シーン終了
        }
        return false; // ウィンドウ表示中は他の（カード選択などの）処理をさせない
    }

    // --- 3. 通常時のボタン判定（RETURN と ATTACK） ---
    // 戻るボタン
    isHoverIdx[RETURN] = IsMouseOver(10, 10, 100, 30, mouse);
    if (mouse.leftClicked && isHoverIdx[RETURN]) {
        isSurrenderConfirm = true; // 確認ウィンドウを開く（ここでは return true しない）
    }

    // 攻撃ボタン
    if (!selectedCards.empty() && playerTarget) {
        isHoverIdx[ATTACK] = IsMouseOver(15, 95, 250, 60, mouse);
        if (mouse.leftClicked && isHoverIdx[ATTACK]) {
            selectedOption = ATTACK;
            currentPhase = BattlePhase::Effect;
            animationTimer = 60;
            return true; // 演出へ移行するために true を返す
        }
    }

    // 現在ターンが回ってきているプレイヤーを取得
    Player& turnPlayer = GetCurrentPlayer();

    // --- ターゲット選択判定 (右側のステータスバー) ---
    const int STATUS_START_X = 700;
    const int STATUS_START_Y = 75;
    const int STATUS_MARGIN_Y = 40;
    const int STATUS_WIDTH = 275;
    const int STATUS_HEIGHT = 30; // 枠の高さ（-15 ～ +15 なので約30）

    // プレイヤー全員分のステータス枠をチェック
    for (int i = 0; i < Player_Turn.size(); ++i) {
        int currentY = STATUS_START_Y + i * STATUS_MARGIN_Y;

        // 判定範囲：DrawPlayerStatusで描画している枠の範囲
        // Y座標は中心から上下に15pxなので、currentY - 15 を開始点にする
        isHoverPlayerIdx[i] = IsMouseOver(STATUS_START_X, currentY - 15, STATUS_WIDTH, STATUS_HEIGHT, mouse);

        if (mouse.leftClicked && isHoverPlayerIdx[i]) {
            targetIdx = i;       // クラスメンバ変数のターゲット添字を更新
            playerTarget = true; // ターゲット指定フラグをONにする

            // ターゲットが決まった時に何かSEを鳴らすならここ
        }
    }

    // カード選択判定の初期化
    hoveredCardIdx = -1;

    // 手札のレイアウト定数(DrawPlayerHandと同じ)
    const float SCALE = 1.45f;                  // 拡大率
    const int CARD_W = (int)(50 * SCALE);       // 横のサイズ
    const int CARD_H = (int)(50 * SCALE);       // 縦のサイズ
    const int START_X = 10;                     // X座標スタート位置
    const int START_Y = 450;                    // Y座標スタート位置
    const int MARGIN = 2;                       // カード同士の横幅
    const int MAX_CARDS_PER_ROW = 9;            // 一列に並ぶカードの最大数
    const int ROW_SPACING = CARD_H + 30;        // カード同士の縦幅

    // 【追加】現在のターンが「自分の攻撃ターン」かどうかを判定
    // (Player_Turn[currentTurnIdx] と 操作中の player の名前を比較)
    bool isAttackTurn = (player.getName() == Player_Turn[currentTurnIdx].getName());

    // プレイヤーの手札を取得
    const auto& hand = player.GetHand();

    // プレイヤーの手札分ループを回す
    for (int i = 0; i < hand.size(); ++i) {
        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;
        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

        // マウスがカード上にあるか判定 (使えないカードでも説明文は読めるようにする)
        if (mouse.x >= x && mouse.x <= x + CARD_W &&
            mouse.y >= y && mouse.y <= y + CARD_H + 25) {
            hoveredCardIdx = i;
        }

        // このカードが今のターンで使えるか判定
        bool isSelectable = true;

        // カード選択フェーズの時だけ、カテゴリによる制限を適用する
        if (currentPhase == BattlePhase::Select) {
            int cat = hand[i].GetCategory();
            if (isAttackTurn) {
                // 攻撃側：防御専用(Defense)は選べない
                if (cat == Defense) isSelectable = false;
            }
            else {
                // 防御側：防御(Defense)か攻防(Bilingual)以外は選べない
                if (cat != Defense && cat != Bilingual) isSelectable = false;
            }
        }

        // 【変更】isSelectable が true ならホバーやクリックを有効にする
        if (isSelectable) {
            isHoverCardIdx[i] = IsMouseOver(x, y, CARD_W, CARD_H, mouse);

            // クリック判定
            if (mouse.leftClicked && isHoverCardIdx[i]) {

                // すでに選んでいるカードを再度クリックしたらキャンセルする処理（お好みで）
                auto it = std::find(selectedCards.begin(), selectedCards.end(), i);
                if (it != selectedCards.end()) {
                    selectedCards.erase(it);
                }
                else {
                    // 【重要】重ね掛けロジック
                    const auto& hand = player.GetHand();

                    // 追加攻撃できるかどうか判定
                    bool isTargetAddable = hand[i].GetAdd();

                    // 選択済みカードの中に加算タイプじゃないカードが混ざってないか確認
                    bool hasUnaddable = false;
                    const auto& currentHand = player.GetHand(); // 現在の手札を確実に取得

                    for (int idx : selectedCards) {
                        // idxが現在の弾数の範囲内かチェックしてからアクセスする
                        if (idx >= 0 && idx < (int)currentHand.size()) {
                            if (!currentHand[idx].GetAdd()) {
                                hasUnaddable = true;
                                break;
                            }
                        }
                        else {
                            // もしここに来たら、前のターンの選択データが残っている証拠
                            printfDx("Error: Old index %d remains in selectedCards!\n", idx);
                        }
                    }

                    // 一枚しか使えないカードを選んだ時
                    if (!isTargetAddable) {
                        if (selectedCards.empty()) {
                            selectedCards.push_back(i);
                        }
                    }
                    else {
                        if (!hasUnaddable) {
                            selectedCards.push_back(i);
                        }
                    }
                }

                // 合計威力の再計算
                totalPower = 0;
                const auto& currentHand = player.GetHand();
                for (int idx : selectedCards) {
                    // 【修正】ここでも範囲チェックを厳重に行う
                    if (idx >= 0 && idx < (int)currentHand.size()) {
                        totalPower += currentHand[idx].GetPower();
                    }
                }
            }
        }
    }
    return false;
}

void Battle::Draw(const Player& player) {

	DrawGraph(0, 0, Pic.Bat, TRUE);

	//上下のラインを描画
	DrawBox(0, 0, 1000, 50, GetColor(0, 255, 255), TRUE);
	DrawBox(0, 750, 1000, 800, GetColor(0, 255, 255), TRUE);

	// 手札の描画（以前作ったDrawPlayerHandの中身をここに書くか、呼び出す）
	DrawPlayerHand(player);

    for (int i = 0; i < MAX; i++) {
        if (i == RETURN) {
            //マウスが乗っていたら黄色、そうでなければ白にする処理(三項演算子)
            unsigned int color = isHoverIdx[i] ? GetColor(255, 255, 100) : GetColor(255, 255, 255);
            DrawBox(10, 10, 100, 40, color, TRUE);
            DrawBox(9, 9, 101, 41, GetColor(0, 0, 0), FALSE);
        }
    }

    // 戻るボタンの文字表記
    DrawString(37, 17, _T("戻る"), GetColor(0, 0, 0));

    // プレイヤーのステータス欄を左に表示する関数
    DrawPlayerStatus(Player_Turn);

    // 引数の player ではなく、「今ターンのプレイヤー」を渡す
    DrawTurnPlayerName(Player_Turn[currentTurnIdx]);

    // 選んだカードを名前の下に表示する
    DrawSelectedCard(player);

    // ターゲット指定された時に相手側の名前を表示する
    if (playerTarget) {
        // ここも同様に修正（引数に引きずられないようにするため）
        DrawTargetPlayerName(Player_Turn[currentTurnIdx]);
    }

    // 確認ウィンドウがONの時、最前面に描画
    if (isSurrenderConfirm) {
        // 暗転
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
        DrawBox(0, 50, 1000, 750, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        // 白いボックス
        DrawBox(300, 200, 700, 400, GetColor(255, 255, 255), TRUE);
        DrawBox(300, 200, 700, 400, GetColor(0, 0, 0), FALSE);

        DrawString(415, 240, "本当に降参しますか？", GetColor(0, 0, 0));

        // 「あきらめる」ボタン（GIVE_UP のホバー状態によって色を変える）
        unsigned int btnColor = isHoverIdx[GIVE_UP] ? GetColor(255, 100, 100) : GetColor(200, 0, 0);
        DrawBox(425, 300, 575, 350, btnColor, TRUE);
        DrawString(460, 315, "あきらめる", GetColor(255, 255, 255));
    }

	// 名前表示
	DrawFormatStringToHandle(
		10, 770,
		GetColor(0, 0, 0),
		Font.Small,
		_T("Name: %s"),
		player.getName().c_str()
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

    // 描画座標
    int x = 15;
    int y = 95;         // 名前枠の下あたり
    int offset = 20;    // カード重ね時のずらし幅

    // --- 背面のテキストエリアの描画 ---
    int boxWidth = 250 + (int)(selectedCards.size() - 1) * offset;
    DrawBox(x - 5, y - 5, x + boxWidth, y + CARD_H + 5, GetColor(255, 255, 200), TRUE);
    DrawBox(x - 5, y - 5, x + boxWidth, y + CARD_H + 5, GetColor(0, 0, 0), FALSE);

    // --- 1. 選択されたすべてのカード画像を表示 ---
    for (int i = 0; i < (int)selectedCards.size(); ++i) {
        int handIdx = selectedCards[i];

        // 【重要】配列の範囲チェック（クラッシュ防止）
        if (handIdx >= 0 && handIdx < (int)hand.size()) {
            const auto& cardInLoop = hand[handIdx];
            int drawX = x + (i * offset);

            int picIdx = cardInLoop.graphicIndex;
            if (picIdx >= 0 && picIdx < 100) {
                DrawExtendGraph(drawX, y, drawX + CARD_W, y + CARD_H, Pic.Card[picIdx], TRUE);
            }
            DrawBox(drawX, y, drawX + CARD_W, y + CARD_H, GetColor(0, 0, 0), FALSE);
        }
    }

    // --- 詳細情報の描画（最後に選んだカード） ---
    int lastIdx = selectedCards.back();
    if (lastIdx >= 0 && lastIdx < (int)hand.size()) {
        const auto& lastCard = hand[lastIdx]; // 変数名を統一

        // テキスト表示開始位置（カード群の右側）
        int textX = x + (int)(selectedCards.size() * offset) + 35;

        // --- 属性色の取得 (修正箇所：card -> lastCard) ---
        int Col = GetColor(0, 0, 0);
        if (lastCard.GetType() == "炎") { Col = GetColor(255, 0, 0); }
        else if (lastCard.GetType() == "水") { Col = GetColor(0, 0, 255); }
        else if (lastCard.GetType() == "木") { Col = GetColor(0, 155, 0); }
        else if (lastCard.GetType() == "光") { Col = GetColor(155, 155, 0); }
        else if (lastCard.GetType() == "闇") { Col = GetColor(255, 100, 255); }

        // カテゴリごとの文字描画 (修正箇所：card -> lastCard)
        TCHAR buf[64] = _T("");
        bool hasText = true;

        switch (lastCard.GetCategory()) {
        case Attack:
        case Bilingual:
            _stprintf_s(buf, lastCard.GetAdd() ? _T("+攻%d") : _T("攻%d"), lastCard.GetPower());
            break;
        case Magic:
            if (lastCard.GetPower() > 0) {
                _stprintf_s(buf, lastCard.GetAdd() ? _T("+攻%d") : _T("攻%d"), lastCard.GetPower());
            }
            else {
                hasText = false;
            }
            break;
        case Defense:
            _stprintf_s(buf, _T("守%d"), lastCard.GetPower());
            break;
        case All:
            _stprintf_s(buf, _T("%d%%攻%d"), lastCard.GetPercent(), lastCard.GetPower());
            break;
        default:
            hasText = false;
            break;
        }

        if (hasText) {
            // カード名の表示
            DrawFormatString(textX, y + 2, Col, _T("[%s]"), lastCard.GetName().c_str());
            // そのカード単体の威力を表示
            DrawString(textX, y + 22, buf, Col);
        }
    }

    // 合計威力の表示
    DrawFormatString(x, y + 70, GetColor(255, 255, 0), _T("攻 %d"), totalPower);
}

TotalAttack Battle::CalculateTotalAttack(Player& attacker) {
    TotalAttack total;
    auto& hand = attacker.GetHand();

    for (int index : selectedCards) {
        const Card& card = hand[index];

        // 最初の1枚目の属性をベースにする
        if (total.type == "") {
            total.type = card.GetType();
        }

        // 威力を加算（ダメージの合算ロジック）
        total.power += card.GetPower();

        // カテゴリが全体攻撃(All)ならフラグを立てる
        if (card.GetCategory() == All) {
            total.isAll = true;
            total.hitPercent = card.GetPercent();
        }
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