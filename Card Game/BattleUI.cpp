#include "BattleUI.h"
#include "DxLib.h"
#include "Picture.h"   // Picture構造体の定義
#include "FontManager.h" // FontManager構造体の定義

extern Picture Pic;
extern FontManager Font;


void BattleUI::DrawPlayerStatus(const std::vector<Player>& players, bool playerTarget, int targetIdx, const std::vector<bool>& isHoverPlayerIdx) {
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
            startX + 135, currentY - 7,
            GetColor(0, 0, 0),
            Font.Small,
            _T("HP %2d MP %2d ￥ %2d "),
            players[i].getHp(), players[i].getMp(), players[i].getMoney()
        );
    }
}

void BattleUI::DrawPlayerHand(const Player& player, const std::vector<Player>& playerTurn, int currentTurnIdx, BattlePhase currentPhase, int hoveredCardIdx, const std::vector<bool>& isHoverCardIdx) {
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
    bool isAttackTurn = (player.getName() == playerTurn[currentTurnIdx].getName());

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
                // ブレンドモードを「加算」に設定（0?255で白さの強さを調節）
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

void BattleUI::DrawSelectedCard(const Player& player,
    const std::vector<Player>& playerTurn,
    int currentTurnIdx,
    const std::vector<int>& selectedCards,
    float currentYOffset,
    int totalPower,
    const std::string& currentAttackElement) {
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
        bool isAttackTurn = (player.getName() == playerTurn[currentTurnIdx].getName());

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