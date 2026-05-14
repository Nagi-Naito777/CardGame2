#pragma once

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

struct DamageResult {
    int finalDamage;
    bool isHit;
    bool isInstantDeath;
    int statusEffectID; // 0ならなし
};

struct TotalAttack {
    int power = 0;          // 合計威力
    std::string type = "";  // 属性（炎、水など）
    int hitPercent = 100;   // 命中率（全体攻撃などの場合）
    bool isAll = false;     // 全体攻撃フラグ
};

// DamageResolver.h
class DamageResolver {
public:

    // 1. 防御が成立するかどうかを判定するヘルパー関数
    static bool IsValidGuard(const std::string& atkAttr, const std::string& defAttr) {
        // --- 1. 光属性の盾（最強の盾） ---
        // 「光属性はどの属性も守れる」
        if (defAttr == "光") return true;

        // --- 2. 闇属性攻撃の判定 ---
        // 「闇属性は基本的に無属性でも属性付の守りカードでも守れる」
        if (atkAttr == "闇") {
            // 光以外の盾（無・炎・水・木）でも、盾さえ出せればガード判定自体は成功。
            // ※ただし、威力不足で1ダメージでも通ると死ぬロジックは ResolveDamage 側で処理。
            return true;
        }

        // --- 3. 三すくみ・その他の相性判定 ---
        // ※光以外の盾は、有利属性のみガード可能（同属性も不可）

        if (atkAttr == "炎") {
            return (defAttr == "水"); // 炎には水のみ
        }

        if (atkAttr == "水") {
            return (defAttr == "木"); // 水には木のみ
        }

        if (atkAttr == "木") {
            return (defAttr == "炎"); // 木には炎のみ
        }

        // --- 4. 光属性攻撃の判定 ---
        if (atkAttr == "光") {
            // 光の攻撃は、光の盾（一番最初のifで判定済み）以外では守れない
            return false;
        }

        // 無属性攻撃など
        if (atkAttr == "無" || atkAttr == "") {
            return true;
        }

        return false;
    }

    // 2. ダメージ計算と適用を行うメイン関数
    // （defenseCard が nullptr の場合は「無防備（ガードなし）」として扱う）
    static void ExecuteAttack(Player& target, const Card& attackCard, const Card* defenseCard = nullptr) {

        // ① 全体攻撃(All)の命中判定
        if (attackCard.GetCategory() == All) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(1, 100);

            // 設定された確率(Percent)より乱数が大きければ「Miss」
            if (dist(gen) > attackCard.GetPercent()) {
                // TODO: ここで「Miss演出」のフラグを立てる等の処理
                return; // ダメージ計算に入らず終了
            }
        }

        // 基本ダメージ量
        int finalDamage = attackCard.GetPower();

        // ② 防御カードの適用
        if (defenseCard != nullptr) {
            // 防御相性のチェック
            if (IsValidGuard(attackCard.GetType(), defenseCard->GetType())) {
                // 防御成立：威力を引き算
                finalDamage -= defenseCard->GetPower();
            }
            else {
                // 防御不成立（相性悪や、光属性に他属性でガードした場合）
                // 防御力0として扱い、引き算はしない
                // TODO: 演出用に「属性貫通！」などのフラグを出しても面白いです
            }
        }

        // ダメージがマイナスにならないよう下限を0にする
        if (finalDamage < 0) finalDamage = 0;

        // ③ 闇属性の特殊ルール（即死判定）
        if (attackCard.GetType() == "闇" && finalDamage > 0) {
            // 防御しきれず1ダメージでも食らったら即死
            target.setHp(0);
            target.setDead(true);
            return; // これ以上計算不要なので終了
        }

        // ④ HPの更新と死亡判定 (PlayerのSetterを正しく使用)
        if (finalDamage > 0) {
            // Playerの getter/setter を使ってHPを減らす
            int currentHp = target.getHp();
            target.setHp(currentHp - finalDamage);

            // もしHPが0になっていたら死亡フラグを立てる
            if (target.getHp() == 0) {
                target.setDead(true);
            }
        }

        // ⑤ 状態異常の付与判定（将来用の拡張スペース）
        // if (finalDamage > 0 && attackCard.hasStatusEffect()) {
        //     if (attackCard.GetEffectType() == "毒") target.setPoison(true);
        // }
    }
};
