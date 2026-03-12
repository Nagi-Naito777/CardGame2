#include <string>
#include "Player.h"

// --- Getter (取得用) ---
// constで「この関数は値を読み取るだけで、書き換えない」ことを保証
std::string Player::getName() const { return name; }
int Player::getHp() const { return hp; }
int Player::getMp() const { return mp; }
int Player::getMoney() const { return money; }

// 状態異常フラグ系セッター
bool Player::isDead()const { return dead; }
bool Player::isPoison() const { return poison; }
bool Player::isMist() const { return mist; }
bool Player::isRock() const { return rock; }
bool Player::isFlash() const { return  flash; }
bool Player::isDarkness() const { return darkness; }

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

// 状態異常フラグ系セッター
void Player::setDead(bool value) { dead = value; }
void Player::setPoison(bool value) { poison = value; }
void Player::setMist(bool value) { mist = value; }
void Player::setRock(bool value) { rock = value; }
void Player::setFlash(bool value) { flash = value; }
void Player::setDarkness(bool value) { darkness = value; }