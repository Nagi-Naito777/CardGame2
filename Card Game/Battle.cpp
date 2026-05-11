#include <iostream>
#include <string>
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

// コンストラクタの実体
Battle::Battle() : currentTurnIdx(0), selectedOption(NONE), hoveredCardIdx(-1) {
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
    this->Player_Turn = players; // 設定画面で決まったリストをコピー
    this->currentTurnIdx = 0;    // 最初のプレイヤーから開始
}

// 更新処理
bool Battle::Update(const MouseState& mouse, const Player& player) {
    // 列挙体全てをループさせ、判定を初期化
    for (int i = 0; i < MAX; i++) {
        isHoverIdx[i] = false;
    }

    for (int j = 0; j < MAX; j++) {
        if (j == RETURN) {
            isHoverIdx[j] = IsMouseOver(10, 10, 100, 30, mouse);
        }

        if (mouse.leftClicked && isHoverIdx[j]) {
            selectedOption = j; // 選択された項目を保存
            return true;        // 選択されたので次のシーンへ（または処理確定）
        }
    }

    // 現在ターンが回ってきているプレイヤーを取得
    Player& turnPlayer = GetCurrentPlayer();

    // ターンプレイヤーのアクション処理などをここに書く
    //if (turnPlayer.IsActionFinished()) { // ※仮の関数です
      //  NextTurn(); // 行動が終わったら次の人のターンへ
    //}

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
    const auto& hand = player.GetHand();
    for (int i = 0; i < hand.size(); ++i) {
        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;
        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

        if (mouse.x >= x && mouse.x <= x + CARD_W &&
            mouse.y >= y && mouse.y <= y + CARD_H + 25) { // テキストエリアも含めて判定
            hoveredCardIdx = i;
        }

        // カードのマウス判定
        isHoverCardIdx[i] = IsMouseOver(x, y, CARD_W, CARD_H + 25, mouse);

        // カードの左クリック判定
        if (mouse.leftClicked && isHoverCardIdx[i]) {
            selectCard = i;     // 選択されたカードの項目を保存
            return true;        // 選択されたので次のシーンへ（または処理確定）
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

    // プレイヤーの名前を上に表示する関数
    DrawTurnPlayerName(player);

    // ターゲット指定された時に相手側の名前を表示する
    if (playerTarget) {
        DrawTargetPlayerName(player);
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

}

void Battle::DrawTargetPlayerName(const Player& player) {

}

// ターンを次のプレイヤーに回す関数（Battle.cpp 内に実装）
void Battle::NextTurn() {
    if (Player_Turn.empty()) return; // エラー防止

    // インデックスを1進め、現在の人数で割った余りを代入する
    // 例: 3人プレイなら、0 -> 1 -> 2 -> (2+1)%3=0 -> 1 ... とループする
    currentTurnIdx = (currentTurnIdx + 1) % Player_Turn.size();
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
    const int startY = 200;       // 1人目のY開始点
    const int marginY = 50;       // プレイヤーごとの間隔（枠の高さ + 余白）

    for (size_t i = 0; i < players.size(); ++i) {
        // i番目のプレイヤーのY座標を計算
        int currentY = startY + (int)i * marginY;

        // 枠の描画（黒い縁取り）
        DrawCircle(startX, currentY, 15, GetColor(0, 0, 0), FALSE);
        DrawCircle(startX + 275, currentY, 15, GetColor(0, 0, 0), FALSE);
        DrawBox(startX, currentY - 15, startX + 275, currentY + 16, GetColor(0, 0, 0), FALSE);

        // 枠の描画（白い中身）
        DrawCircle(startX, currentY, 14, GetColor(255, 255, 255), TRUE);
        DrawCircle(startX + 275, currentY, 14, GetColor(255, 255, 255), TRUE);
        DrawBox(startX, currentY - 14, startX + 275, currentY + 15, GetColor(255, 255, 255), TRUE);

        // 名前表示
        DrawFormatStringToHandle(
            startX, currentY - 7,
            GetColor(0, 0, 0),
            Font.Small,
            _T("Name: %s"),
            players[i].getName().c_str()
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


    for (int i = 0; i < hand.size(); ++i) {

        int col = i % MAX_CARDS_PER_ROW;
        int row = i / MAX_CARDS_PER_ROW;

        // X, Y座標の計算
        int x = START_X + (CARD_W + MARGIN) * col;
        int y = START_Y + (ROW_SPACING * row);

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
            _stprintf_s(buf, hand[i].GetAdd() ? _T("+攻%d") : _T("攻%d"), hand[i].GetPower());
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
            DrawFormatString(textX, textY + 20, GetColor(0, 0, 0), _T("守%d"), card.GetPower());
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