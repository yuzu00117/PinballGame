#pragma once

#include <memory>

class GameObject; // 前方宣言

/// <summary>
/// すべてのコンポーネントの基底クラス
/// </summary>
class Component
{
public:
	// --- 関数定義 ---
	// デフォルトコンストラクタ・デストラクタ
	Component() = default;
	virtual ~Component() = default;
	
	/// <summary>
	/// ライフサイクルメソッド
	/// </summary>
	virtual void Init() {};
	virtual void Uninit() {};
	virtual void Update(float deltaTime) {};
	virtual void Draw() {};

	// --- 変数定義 ---
	GameObject* m_Owner = nullptr; // 所属するオブジェクトへのポインタ
};