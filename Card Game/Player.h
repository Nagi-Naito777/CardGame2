#pragma once
#include <string>

class SelectScene;

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

    std::string name;       // ユーザーネーム
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

public:
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
   
};

extern Player g_player;