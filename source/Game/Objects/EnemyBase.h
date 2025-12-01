#pragma once

#include "GameObject.h"
#include "Vector3.h"

class ColliderGroup;
class MeshRenderer;

/// <summary>
/// エネミー共通の基底クラス
/// 全エネミーはこのクラスを継承して作成される
/// </summary>
class EnemyBase : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ライフサイクルメソッド
    /// </summary>
    void Init() override;
    void Update() override;
    void Draw() override;
    void Uninit() override;

    /// <summary>
    /// 衝突コールバック
    /// フリッパーでボールを弾くための簡易実装
    /// </summary>
    void OnCollisionEnter(const CollisionInfo& info) override;

    /// <summary>
    /// ターゲット位置を設定する
    /// </summary>
    void SetTargetPos(const Vector3& targetPos) { m_TargetPos = targetPos; }

private:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// ターゲットへの正規化方向
    /// </summary>
    Vector3 GetDirToTarget() const;

    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    // シェーダーパス
    static constexpr const char* VertexShaderPath = // 頂点シェーダのパス
        "shader\\bin\\unlitTextureVS.cso";   
    static constexpr const char* PixelShaderPath  = // ピクセルシェーダのパス
        "shader\\bin\\unlitTexturePS.cso";   
    
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    // 共通パラメーター
    float   m_Speed = 0.15f;                        // 移動速度
    int     m_HP = 1;                               // 体力
    int     m_Score = 100;                          // スコア値
    Vector3 m_TargetPos = { 0.0f, 0.0f, 0.0f };     // 目標位置
    Vector3 m_Velocity = { 0.0f, 0.0f, 0.0f };      // 現在速度

    // コンポーネント
    ColliderGroup* m_ColliderGroup = nullptr;      // コライダーグループコンポーネント
    MeshRenderer*   m_MeshRenderer = nullptr;       // メッシュレンダラーコンポーネント
};
