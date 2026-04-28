#include <string>
#include <algorithm>        // std::sortを使うためにインクルード
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

// 手札並び替え関数
void Player::SortHand() {

    if (hand.empty()) return; // 手札が空なら何もしない

    std::sort(hand.begin(), hand.end(), [](const Card& a, const Card& b) {

        // カードのカテゴリで並び替え
        if (a.GetCategory() != b.GetCategory()) {
            return a.GetCategory() < b.GetCategory();
        }

        // 攻撃カードのみ属性枠を後ろにする
        if (a.GetCategory() == Attack) {
            // 同じカテゴリ内での属性優先度を設定
            auto getAttrPriority = [](const std::string& type) {
                if (type == "" || type == "無") return 0; // 通常カード
                if (type == "炎") return 1;
                if (type == "水") return 2;
                if (type == "木") return 3;
                if (type == "光") return 4;
                if (type == "闇") return 5;
                return 6; // その他
                };

            int priorityA = getAttrPriority(a.GetType());
            int priorityB = getAttrPriority(b.GetType());

            if (priorityA != priorityB) {
                // 数値が小さい方（通常カード）が前、大きい方（属性付き）が後ろ
                return priorityA < priorityB;
            }
        }

        // 威力比較
        if (a.GetPower() != b.GetPower()) {
            return a.GetPower() < b.GetPower();
        }

        // 追加攻撃(Add)フラグで比較（フラグなしを前、ありを後にする場合）
        if (a.GetAdd() != b.GetAdd()) {
            return (int)a.GetAdd() < (int)b.GetAdd();
        }

        // 威力が1かつ無属性の場合のみ、金額で比較する
        if (a.GetPower() == 1 && a.GetType() == "無" && b.GetType() == "無") {
            return a.GetMoney() < b.GetMoney();
        }

        // すべての条件が同じ場合は「false」を返す
        return false;
    });
}