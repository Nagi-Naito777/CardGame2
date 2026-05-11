#pragma once

//カードの最大所持枚数
#define CARD_MAX 18

#include <string>
#include <algorithm>        // std::sortを使うためにインクルード
#include "DxLib.h"
#include "Card.h"

class SelectScene;
class Action;
class Battle;

// AIかプレイヤーか判断する列挙体
enum class ControllerType {
    HUMAN,
    AI
};

//プレイヤークラス
class Player {
private:
    // 状態異常用の列挙体
    enum Condition {
        NONE = -1,      // 何もない
        Dead,           // 気絶状態(戦闘不能)
        Poison,         // 毒状態
        Mist,           // 霧(相手のステータスが見えなくなる)
        Rock,           // 相手のカードをランダムに1枚使用不可にする
        Flash,          // 防御カードを1枚しか使用できなくする
        Darkness        // 確率ダメージを確定ヒットにする
    };

    int ID;                 // ユーザー識別の個別ID(後にオンライン化するために必須)
    std::string name;      // ユーザーネーム
    bool isAI;              // AIかどうかの判定
    int team;               // チームはどこか
    int hp;                 // HP
    int mp;                 // MP
    int money;              // お金
    // 状態異常のフラグ変数
    bool dead;
    bool poison;
    bool mist;
    bool rock;
    bool flash;
    bool darkness;

    // 手札（枚数が変わる可能性を考慮）
    std::vector<Card> hand;

    ControllerType controlType;

public:
    // プレイヤーの初期値
    Player():hp(40),mp(10),money(20){}

    // AIかどうかの判定系関数
    void setControllerType(ControllerType type) { controlType = type; }
    ControllerType getControllerType() const { return controlType; }

    // --- Getter (取得用) ---
    std::string getName() const;
    int getHp() const;
    int getMp() const;
    int getMoney() const;

    // 状態異常フラグ系セッター
    bool isDead()const;
    bool isPoison() const;
    bool isMist() const;
    bool isRock() const;
    bool isFlash() const;
    bool isDarkness() const;

    // --- Setter (設定用) ---
    void setName(const std::string& newName);
    void setHp(int newHp);
    void setMp(int newMp);
    void setMoney(int newMoney);

    // 状態異常フラグ系セッター
    void setDead(bool value);
    void setPoison(bool value);
    void setMist(bool value);
    void setRock(bool value);
    void setFlash(bool value);
    void setDarkness(bool value);
   
    // 手札にカードを追加（ドロー）
    void AddHand(const Card& newCard) {
        if (hand.size() < CARD_MAX) {
            //printfDx("カードID: %d\n", newCard.GetID()); // デバッグ
            hand.push_back(newCard);
        }
    }

    // カードを使用する（インデックス指定）
    void RemoveHand(int index) {
        if (index >= 0 && index < hand.size()) {
            hand.erase(hand.begin() + index);
        }
    }

    // 手札のカードを消去する処理
    void DeleteHand() {
        while (hand.size()) {
            // 手札がなくなったらループ終了
            if (!hand.size()) {
                break;
            }
            hand.erase(hand.begin());
        }
    }
    // プレイヤーの手札を並べ替える関数
    void SortHand();

    // 全手札を取得（描画やAIの判断用）
    const std::vector<Card>& GetHand() const { return hand; }

    // 手札の枚数を取得
    int GetHandCount() const { return (int)hand.size(); }

    // 属性関係

    // ダメージ計算式

    // 全体ダメージ処理

    // 回復処理

    // ステータス変換処理

    // アイテム購入処理

    // アイテム売却処理

    // 奇跡の処理

    // プレイヤーターン処理

    // カードの追加処理

    // 戦闘開始準備処理
    void BattleInit(Player);

};

extern Player g_player;