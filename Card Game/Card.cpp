#include "Card.h"
#include "DxLib.h"
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// CSVから読み込んだデータを格納するデータベース
std::vector<Card> cardDatabase;

// GetRandomCardの実装
Card Card::GetRandomCard() {
    // データベースが空の場合は空のカードを返す（バグ防止）
    if (cardDatabase.empty()) {
        return Card();
    }

    // 乱数生成
    static std::mt19937 engine(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<size_t> dist(0, cardDatabase.size() - 1);
    // ランダムな位置のカードを返す
    return cardDatabase[dist(engine)];
}

bool Card::LoadCardDatabase(const std::string& filePath) {
    
    

    // ファイルを開く
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "エラー: ファイルが開けませんでした: " << filePath << std::endl;
        return false;
    }

    std::string line;

    // ヘッダー行（1行目）を読み飛ばす
    if (!std::getline(file, line)) return false;

    // 1行ずつ読み込む
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        // カンマ区切りで各セルを分割する
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        if (row.size() >= 2) {
            // DrawStringを「直後」に呼んでみる（デバッグ用）
            // これが化けていれば、読み込み処理そのものが失敗しています
            printfDx(L"カード名: %s\n", row[1].c_str());
            
        }

        // 列数が足りているかチェック（ID～Percentまで10項目あるか）
        if (row.size() < 10) continue;

        try {
            // 文字列から適切な型に変換して、Cardクラスを生成
            // コンストラクタ: 
            // (int ID, string Name, int Power, string Type, string Desc, string CatStr, int Money, int MP)
            cardDatabase.emplace_back(
                std::stoi(row[0]), // ID
                row[1],            // 名前
                std::stoi(row[2]), // 威力
                row[3],            // 属性
                row[4],            // 説明
                row[5],            // カテゴリ（"攻"など）
                std::stoi(row[6]), // 追加攻撃可能か
                std::stoi(row[7]), // 値段
                std::stoi(row[8]), // MP
                std::stoi(row[9])  // 攻撃成功確率
            );
        }
        catch (const std::exception& e) {
            std::cerr << "データ変換エラー: " << e.what() << " 行: " << line << std::endl;
        }
    }

    file.close();
    return true;
}