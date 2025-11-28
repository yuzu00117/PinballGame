#pragma once

#include "gameobject.h"

class Polygon2D : public GameObject
{
private:
    ID3D11Buffer* m_VertexBuffer = nullptr;
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader* m_PixelShader = nullptr;
    ID3D11InputLayout* m_VertexLayout = nullptr;
    ID3D11ShaderResourceView* m_Texture;

public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;
};
