#pragma once

#include "gameobject.h"

class Explosion : public GameObject
{
private:
    ID3D11Buffer* m_VertexBuffer;

    ID3D11InputLayout* m_VertexLayout;
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;

    ID3D11ShaderResourceView* m_Texture;

    int m_CurrentFrame = 0;
    int m_TotalFrames = 16;      // 総フレーム数
    int m_FramesPerRow = 4;      // 横方向のフレーム数
    float m_FrameTime = 0.05f;   // 1フレームの表示時間（秒）
    float m_ElapsedTime = 0.0f;  // 経過時間

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;
};
