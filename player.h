#ifndef _PLAYER_H
#define _PLAYER_H

#include "vector3.h"
#include "gameobject.h"

class Player : public GameObject
{
private:
    ID3D11InputLayout* m_VertexLayout;
    ID3D11VertexShader* m_VertexShader;
    ID3D11PixelShader* m_PixelShader;
    class ModelRenderer* m_ModelRenderer;
    ID3D11ShaderResourceView* m_Texture;
    Vector3 m_Position;
    Vector3 m_Rotation;
    Vector3 m_Scale;
public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;
    Vector3 GetPosition() const { return m_Position; }
};

#endif // _PLAYER_H
