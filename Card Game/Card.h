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
	Bilingual,		// "両"　攻撃・防御どちらも可能
	UnNull			// "無"　どこにも属さない
};

//カードの構造体
class Card {


private:
	// カードデータを構造体としてまとめる
	struct Card_Date {
		int ID = 0;				// カードID
		std::string name;		// カード名
		int power;				// 威力
		std::string type;		// 属性
		std::string setumei;	// カード説明
		CardCategory category;	// カード種類
		bool add;				// 追加攻撃可能か
		int money;				// カードの値段
		int MP;					// 消費MP
	}data;	// 実体化

	// 所持カードも構造体としてまとめる
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
		if (str == "両") return CardCategory::Bilingual;
		return CardCategory::UnNull;
	}

public:
	Card(){}
	// Excel(データベース)から読み込んだ文字列や数値をそのまま受け取る
	Card(int id, std::string name, int power, std::string type,
		std::string setumei, std::string categoryStr, int money, int mp) {

		data.ID = id;
		data.name = name;
		data.power = power;
		data.type = type;
		data.setumei = setumei;
		data.money = money;
		data.MP = mp;

		// 文字列を列挙体に変換して格納
		data.category = StringToCategory(categoryStr);
	}

	// ゲッター関数
	int GetID() const { return data.ID; }
	int GetPower() const { return data.power; }
	int GetMoney() const { return data.money; }
	int GetMP() const { return data.MP; }
	CardCategory GetCategory() const { return data.category; }

	// 文字列型
	const std::string& GetName() const { return data.name; }
	const std::string& GetType() const { return data.type; }
	const std::string& GetDescription() const { return data.setumei; }

	bool LoadCardDatabase(const std::string& filePath);
};

extern Card card;