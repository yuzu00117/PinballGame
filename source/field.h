#pragma once

#include "gameobject.h"
#include "vector3.h"
#include <vector>

/// <summary>
/// 枠組みフィールドクラス
/// </summary>
/// TODO: 後でメッシュコライダーにする

// 壁セグメント構造体
struct WALL_SEGMENT
{
    Vector3 Start;              // 壁の開始位置
    Vector3 End;                // 壁の終了位置
    float Radius = 0.5f;        // 壁の太さ
    float Restitution = 0.8f;   // 反発係数
};

class Field : public GameObject
{
public:
    // --- 関数定義 ---
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    // 物理インターフェース
    void ResolveBallCollision(Vector3& ballPos, Vector3& ballVel, float ballRadius) const;

    // 壁管理
    void ClearWalls();
    void AddWall(const WALL_SEGMENT& wall);
    void BuildDefaultFrame(float halfW = 9.5f, float halfH = 14.5f, float thick = 0.2f);

    const std::vector<WALL_SEGMENT>& GetWalls() const { return m_Walls; }

private:
    // --- 変数定義 ---
    ID3D11Buffer*               m_VertexBuffer  = nullptr;
    ID3D11InputLayout*          m_VertexLayout  = nullptr;
    ID3D11VertexShader*         m_VertexShader  = nullptr;
    ID3D11PixelShader*          m_PixelShader   = nullptr;
    ID3D11ShaderResourceView*   m_Texture       = nullptr;

    std::vector<WALL_SEGMENT>   m_Walls;      // 壁セグメント配列
};
