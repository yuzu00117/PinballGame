//------------------------------------------------------------------------------
// Component
//------------------------------------------------------------------------------
// 役割:
// GameObject に付与できる振る舞い（機能）を表す基底クラス。
// Init / Update / Draw のフックを提供し、派生コンポーネントで機能を実装する。
//
// 設計意図:
// GameObject 本体に描画・衝突・物理などを直接持たせず、Component として分離することで
// 組み合わせ（合成）で多様なオブジェクトを表現できるようにする。
// Component は所有（破棄）を GameObject 側に委ね、Owner 参照を通じて連携する。
//
// 構成:
// - ライフサイクル         : Init / Uninit / Update / Draw
// - 所有関係               : GameObject が unique_ptr で所有（想定）
// - 参照関係               : m_Owner で親 GameObject を参照（非所有）
//
// NOTE:
// - Component 単体では機能は成立しない（Owner にアタッチされて初めて意味を持つ）
// - m_Owner の寿命は「Owner(GameObject)が生存している間のみ」有効
// - Init / Uninit の呼び出し責務は管理側（通常は GameObject）にある
//------------------------------------------------------------------------------
#pragma once

class GameObject;

/// Component の基底クラス
/// - GameObject に付与できる「振る舞い」を表す
/// - Update / Draw の呼び出し順は管理側（通常は GameObject）に依存する
class Component
{
public:
    Component() = default;
    virtual ~Component() = default;

    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理（生成直後に一度だけ呼ばれる想定）
    /// NOTE: Owner 参照（m_Owner）は Init 前に設定される設計を前提とする
    virtual void Init() {}

    /// 終了処理
    virtual void Uninit() {}

    /// 更新処理（deltaTime は秒単位）
    virtual void Update(float deltaTime) {}

    /// 描画処理
    virtual void Draw() {}

public:
    /// 所属する GameObject への参照（非所有）
    /// - 有効期間：Owner(GameObject) が生存している間のみ有効
    /// - 設定責務：アタッチ側（例：GameObject::AddComponent）が必ず設定する
    GameObject* m_Owner = nullptr;
};
