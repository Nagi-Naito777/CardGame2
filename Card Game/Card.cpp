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
    // 1. ファイルを通常モードで開く（ANSIにBOMはないのでバイナリモードは不要）
    std::ifstream file(filePath);
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line)) return false; // 1行目（ヘッダー）を読み飛ばし

    // 2. 1行ずつ読み込む
    while (std::getline(file, line)) {
        // 改行コード \r を除去（WindowsのExcel対策）
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        // カンマで分割
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        // 列数チェック
        if (row.size() < 10) continue;

        // デバッグ表示（プロジェクトがマルチバイト設定なら普通の %s で正しく表示されます）
        printfDx("読み込み成功: %s\n", row[1].c_str());

        try {
            cardDatabase.emplace_back(
                std::stoi(row[0]), row[1], std::stoi(row[2]),
                row[3], row[4], row[5],
                (row[6] == "1"),
                std::stoi(row[7]), std::stoi(row[8]), std::stoi(row[9])
            );
        }
        catch (const std::exception& e) {
            std::cerr << "データ変換エラー: " << e.what() << " 行: " << line << std::endl;
        }
    }

    return true;
}