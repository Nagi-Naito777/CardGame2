#pragma once
#include<string>
#include <vector>

//カードの種類を定義する列挙体
enum CardCategory {
	Change,			// "換"　ステータス変換
	Sell,			// "売"　売る
	Buy,			// "買"　買う
	Attack,			// "攻"　攻撃カード
	Bilingual,		// "両"　攻撃・防御どちらも可能
	Defense,		// "守"　防御カード
	Healing,		// "癒"　HP回復カード
	MagicHealing,	// "魔"　MP回復カード
	Magic,			// "奇"　奇跡(呪文)カード
	All,			// "全"　全体に確率攻撃
	UnNull			// "無"　どこにも属さない
};

//カードの構造体
class Card {

private:
	// カードデータを構造体としてまとめる
	struct Card_Date {
		int ID = 0;				// カードID
		std::string name;		// カード名
		int power = 0;			// 威力
		std::string type;		// 属性
		std::string setumei;	// カード説明
		CardCategory category;	// カード種類
		bool add = false;		// 追加攻撃可能か
		int money = 0;			// カードの値段
		int MP = 0;				// 消費MP
		int percent = 100;		// 成功確率(攻撃系カードのみに適用)
	}; // 実体化

	Card_Date data;

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
		if (str == "全") return CardCategory::All;
		return CardCategory::UnNull;
	}

public:
	int graphicIndex; // Pic.Card[graphicIndex] に対応する番号

	Card(){
		graphicIndex = 0; // デフォルトで0番の画像を使う
		data.power = 0;
		data.name = "なし";
		data.type = "なし";
	}
	// Excel(データベース)から読み込んだ文字列や数値をそのまま受け取る
	Card(int id, std::string name, int power, std::string type,
		std::string setumei, std::string categoryStr, bool can_add,int money, int mp, int percent) {

		data.ID = id;
		data.name = name;
		data.power = power;
		data.type = type;
		data.setumei = setumei;
		data.add = can_add;
		data.money = money;
		data.MP = mp;
		data.percent = percent;

		// IDが0番のカードならPic.Card[0]、1番ならPic.Card[1]を使うように紐付ける
		this->graphicIndex = id;

		// 文字列を列挙体に変換して格納
		data.category = StringToCategory(categoryStr);
	}

	// ゲッター関数
	int GetID() const { return data.ID; }
	int GetPower() const { return data.power; }
	int GetMoney() const { return data.money; }
	int GetMP() const { return data.MP; }
	CardCategory GetCategory() const { return data.category; }
	int GetPercent() const { return data.percent; }

	// 文字列型
	const std::string& GetName() const { return data.name; }
	const std::string& GetType() const { return data.type; }
	const std::string& GetDescription() const { return data.setumei; }

	//フラグ判定
	bool GetAdd()const { return data.add; }

	bool LoadCardDatabase(const std::string& filePath);

	// データベースからランダムに1枚取得する（配る用）
	static Card GetRandomCard();
};

extern Card card;