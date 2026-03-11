#pragma once
#include <string>

class SelectScene;

// 状態異常用の列挙体
enum Condition {
    NONE=-1,        // 何もない
    Poison,         // 毒状態
    ,
    ,
    ,
};

//プレイヤークラス
class Player {
private:
    std::string name;
    int hp;
    int mp;
    int money;
    int condition;

public:
    // --- Getter (取得用) ---
    std::string getName() const;
    int getHp() const;
    int getMp() const;
    int getMoney() const;
    int getCondition() const;

    // --- Setter (設定用) ---
    void setName(const std::string& newName);
    void setHp(int newHp);
    void setMp(int newMp);
    void setMoney(int newMoney);
    void setCondition(int newCondition);
};

extern Player g_player;