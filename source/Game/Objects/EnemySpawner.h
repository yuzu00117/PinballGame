#pragma once

#include "GameObject.h"
#include "Vector3.h"
#include <vector>

class Hole;
class EnemyBase;
class MeshRenderer;

/// <summary>
/// エネミースポナークラス
/// 一定間隔でエネミーを出現させる
/// </summary>
class EnemySpawner : public GameObject
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

    /// <summary>
    /// 何フレームごとにすぽーんするかを設定
    /// </summary>
    void SetSpawnInterval(int frames) { m_SpawnInterval = frames; }

    /// <summary>
    /// スポーン位置の範囲を設定（XY平面）
    /// </summary>
    void SetSpawnArea(float xMin, float xMax, float z)
    {
        m_SpawnXMin = xMin;
        m_SpawnXMax = xMax;
        m_SpawnZ = z;
    }

    /// <summary>
    /// ターゲットにするHoleを登録
    /// </summary>
    void AddTargetHole(Hole* hole) { m_TargetHoles.push_back(hole); }

protected:
    // ----------------------------------------------------------------------
    // 関数定義
    // ----------------------------------------------------------------------
    /// <summary>
    /// エネミーをスポーン
    /// </summary>
    void SpawnEnemy();

    /// <summary>
    /// [min, max]の範囲でランダムなfloat値を取得
    /// </summary>
    float GetRandomFloat(float min, float max);

private:
    // ----------------------------------------------------------------------
    // 定数定義
    // ----------------------------------------------------------------------
    static constexpr int   kDefaultSpawnInterval = 60;    // デフォルトスポーン間隔（フレーム数）
    static constexpr float kDefaultSpawnXMin     = -5.0f; // デフォルトスポーン位置X最小値
    static constexpr float kDefaultSpawnXMax     = 5.0f;  // デフォルトスポーン位置X最大値
    static constexpr float kDefaultSpawnZ        = 5.0f;  // デフォルトスポーン位置Z
    
    // シェーダーパス
    static constexpr const char* VertexShaderPath =   // 頂点シェーダのパス
        "shader\\bin\\unlitTextureVS.cso";   
    static constexpr const char* PixelShaderPath  =   // ピクセルシェーダのパス
        "shader\\bin\\unlitTexturePS.cso";  
    
    // ----------------------------------------------------------------------
    // 変数定義
    // ----------------------------------------------------------------------
    int   m_SpawnInterval = kDefaultSpawnInterval;        // スポーン間隔（フレーム数）
    int   m_SpawnTimer    = 0;                            // スポーンタイマー
    float m_SpawnXMin     = kDefaultSpawnXMin;            // スポーン位置X最小値
    float m_SpawnXMax     = kDefaultSpawnXMax;            // スポーン位置X最大値
    float m_SpawnZ        = kDefaultSpawnZ;               // スポーン位置Z
    std::vector<Hole*> m_TargetHoles;                     // ターゲットHoleリスト
    
    MeshRenderer*      m_MeshRenderer   = nullptr;        // メッシュレンダラーコンポーネント
};
