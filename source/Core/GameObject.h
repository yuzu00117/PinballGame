//------------------------------------------------------------------------------
// GameObject
//------------------------------------------------------------------------------
// 役割:
// ゲーム内に存在するオブジェクトの基底クラス。
// 位置・回転・スケールなどの基本的な Transform 情報を保持し、
// Update / Draw の共通インターフェースを提供する。
//
// 設計意図:
// すべてのゲームオブジェクトを同一のライフサイクルで管理するため、
// Update / Draw を仮想関数として定義している。
// 描画・衝突・物理などの機能は Component として分離し、
// 組み合わせによって多様なオブジェクトを表現できるようにしている。
//
// 構成:
// - Transform              : 位置・回転・スケール情報を保持
// - Component 管理         : Component を複数所有（unique_ptr）
// - 子オブジェクト管理     : GameObject を階層構造で所有可能
// - ライフサイクル         : Init / Update / Draw
// - 衝突イベント受信       : OnCollision / OnTrigger 系コールバック
// - 生存管理               : Destroy による遅延削除フラグ
//
// NOTE:
// GameObject 自体は描画処理を直接持たない。
// 描画が必要な場合は Renderer / Component を介して実装すること。
//------------------------------------------------------------------------------
#pragma once
#include "DebugSettings.h"
#include "CollisionInfo.h"
#include <memory>
#include <vector>
#include <type_traits>

// コンポーネント関連ヘッダ
#include "Transform.h"
#include "Collider.h"
#include "Component.h"
#include "MeshRenderer.h"

// 前方宣言
class Collider;

/// ゲームオブジェクトの基底クラス
/// - Transform 情報を持ち、複数の Component と子オブジェクトを所有できる
/// - Destroy() は削除予約。子は親 Update() 後に回収され破棄される
/// - 継承先では必要に応じて各関数を override する
class GameObject  
{
public:
    /// 仮想デストラクタ（派生クラスを安全に破棄するため）
    virtual ~GameObject() = default;

    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理（子オブジェクト / Component の構築や初期設定を行う）
    virtual void Init() {}  
    /// 終了処理（明示的な解放が必要な場合に実装）
    virtual void Uninit() {}
    /// 更新処理（deltaTimeは秒単位）
    virtual void Update(float deltaTime);
    /// 描画処理
    virtual void Draw();

    // ----------------------------------------------------------------------
    // Component管理
    // ----------------------------------------------------------------------
    /// Componentを追加する
    /// - 所有権：GameObjectが保持（unique_ptrで所有）
    /// - 戻り値：非所有ポインタ
    ///   - GameObject が生存し、かつ当該 Component が保持されている間のみ有効
    ///   - 将来的に Component の削除/差し替えを導入した場合、無効化され得る
    /// - 副作用：追加直後に comp->Init() が呼び出される
    /// NOTE: Init 内で AddComponent する場合、初期化順序に依存しない設計か確認すること

    template <typename T, typename... Args>
    T *AddComponent(Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T *ptr = comp.get();

        // GameObjectとの連携を設定（Ownerは常に設定）
        comp->m_Owner = this;

        // Transformのリンク
        // TODO: Transformが必要なコンポーネントは、各コンポーネント側のInitでリンクする方式へ整理する
        if constexpr (std::is_base_of<Collider, T>::value)
            comp->m_Transform = &m_Transform;
        if constexpr (std::is_base_of<MeshRenderer, T>::value)
            comp->m_Transform = &m_Transform;

        comp->Init();
        m_Components.push_back(std::move(comp));
        return ptr;
    }

    /// 指定タイプのComponentを取得する（例：GetComponent<BoxCollider>()）
    /// 戻り値：見つからない場合は nullptr
    template <typename T>
    T *GetComponent()
    {
        for (auto &c : m_Components)
            if (auto ptr = dynamic_cast<T *>(c.get()))
                return ptr;
        return nullptr;
    }
    
    // ----------------------------------------------------------------------
    // 子オブジェクト管理
    // ----------------------------------------------------------------------
    /// 子オブジェクトを追加する（基本版）
    /// 戻り値：非所有ポインタ（所有は親GameObject側が持つ）
    /// NOTE: 子が Destroy() 済みの場合、親の次回 Update() 後に回収され無効化される
    GameObject* CreateChild();

    /// 子オブジェクトを追加する（派生クラス版）
    /// 戻り値：非所有ポインタ（所有は親GameObject側が持つ）
    /// NOTE: 子が Destroy() 済みの場合、親の次回 Update() 後に回収され無効化される
    template <class T, class... Args>
    T* CreateChild(Args&&... args) {
        static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from GameObject");

        auto child = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = child.get();
        AttachChild(std::move(child));
        return ptr;
    }

    /// 既存のGameObjectを子オブジェクトとしてアタッチする
    void AttachChild(std::unique_ptr<GameObject> child);

    /// すべての子オブジェクトを破棄する（親子関係も解除）
    /// NOTE: この関数は「子を外へ渡す(detach)」ではなく、解除後に即時破棄する
    // TODO: 将来的に DestroyAllChildren / ClearChildren に改名
    void DetachAllChildren();

    // ----------------------------------------------------------------------
    // 衝突イベントコールバック
    // ----------------------------------------------------------------------
    /// 衝突イベント（当たり判定）
    virtual void OnCollisionEnter(const CollisionInfo& info) {}
    virtual void OnCollisionStay(const CollisionInfo& info) {}
    virtual void OnCollisionExit(const CollisionInfo& info) {}
    /// トリガーイベント（接触判定）
    virtual void OnTriggerEnter(const CollisionInfo& info) {}
    virtual void OnTriggerStay(const CollisionInfo& info) {}
    virtual void OnTriggerExit(const CollisionInfo& info) {}

    /// 子オブジェクトも含めてすべての Collider を収集する
    void CollectCollidersRecursive(std::vector<Collider*>& outColliders);

    // ----------------------------------------------------------------------
    // 生存管理
    // ----------------------------------------------------------------------
    /// オブジェクトを削除予定にする（即時削除ではない）
    void Destroy() { m_IsDead = true; }

    /// 削除予定かどうか
    bool IsDead() const { return m_IsDead; }

public:
    // Transform情報
    Transform m_Transform;                                  // オブジェクトのTransform情報

protected:
    // 所有権・参照関係
    GameObject* m_Parent = nullptr;                         // 非所有：親への参照
    
    std::vector<std::unique_ptr<Component>> m_Components;   // 所有：Component群
    std::vector<std::unique_ptr<GameObject>> m_Children;    // 所有：子オブジェクト群

    bool m_IsDead = false;                                  // 削除フラグ
};
