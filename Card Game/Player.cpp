#include <string>
#include "Player.h"

// --- Getter (取得用) ---
    // constで「この関数は値を読み取るだけで、書き換えない」ことを保証
std::string Player::getName() const { return name; }
int Player::getHp() const { return hp; }
int Player::getMp() const { return mp; }
int Player::getMoney() const { return money; }
int Player::getCondition() const { return condition; }

// --- Setter (設定用) ---
void Player::setName(const std::string& newName) { name = newName; }
void Player::setHp(int newHp) {
    if (newHp < 0) newHp = 0;           // 0より小さくはしない
    if (newHp > 99) newHp = 99;         //99よりも大きくしない
    hp = newHp;
}
void Player::setMp(int newMp) {
    if (newMp < 0) newMp = 0;           // 0より小さくはしない
    if (newMp > 99) newMp = 99;         //99よりも大きくしない
    mp = newMp;
}
void Player::setMoney(int newMoney) {
    if (newMoney < 0) newMoney = 0;     // 0より小さくはしない
    if (newMoney > 99) newMoney = 99;   //99よりも大きくしない
    money = newMoney;
}
void Player::setCondition(int newCondition) { condition = newCondition; }