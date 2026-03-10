#pragma once
#include<string>
#include <vector>

//カードの種類を定義する列挙体
enum CardCategory {
	Attack,			// "攻"　攻撃カード
	Defense,		// "守"　防御カード
	Magic,			// "奇"　奇跡(呪文)カード
	Healing,		// "癒"　HP回復カード
	MagicHealing,	// "魔"　MP回復カード
	Buy,			// "買"　買う
	Sell,			// "売"　売る
	Change,			// "換"　ステータス変換
	AddAttack,		// "加"　加算攻撃カード
	UnNull			// "無"　どこにも属さない
};

//カードの構造体
struct Card {
	int ID = 0;				// カードID
	std::string name;		// カード名
	int power;				// 威力
	std::string type;		// 属性
	std::string setumei;	// カード説明
	CardCategory category;	// カード種類
	int money;				// カードの値段
};

//所持カード構造体
struct HaveCard {
	// 手札（枚数が変わる可能性を考慮）
	std::vector<Card> hand;
	// 場に出しているカード（スタック/重ねがけ用）
	std::vector<Card> field_cards;
};

// 文字列を列挙体に変換する補助関数(inline関数にして二重定義を防ぐ)
inline CardCategory StringToCategory(const std::string& str) {
	if (str == "攻") return CardCategory::Attack;
	if (str == "守") return CardCategory::Defense;
	if (str == "奇") return CardCategory::Magic;
	if (str == "癒") return CardCategory::Healing;
	if (str == "魔") return CardCategory::MagicHealing;
	if (str == "買") return CardCategory::Buy;
	if (str == "売") return CardCategory::Sell;
	if (str == "換") return CardCategory::Change;
	if (str == "加") return CardCategory::AddAttack;
	return CardCategory::UnNull;
}

