#pragma once

enum class BattlePhase {
	Select,         // 攻撃側のカード選択中
	DefenseSelect,  // 防御側のカード選択中（AI戦ならプレイヤーが操作）
	Reveal,         // カード公開アニメーション
	Effect,         // 重ね掛け・スキル発動演出
	Damage,         // ダメージ・防御判定演出
	Idle            // 待機中
};