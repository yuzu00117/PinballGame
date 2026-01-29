// modelRenderer.h
#pragma once

#include "main.h"
#include "component.h"
#include "Transform.h"
#include "vector3.h"
#include <string>
#include <unordered_map>

using namespace DirectX;

class GameObject;

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

struct MODEL_MATERIAL
{
    char                      Name[256];
    MATERIAL                  Material;
    char                      TextureName[256];
    ID3D11ShaderResourceView* Texture;
};

struct SUBSET
{
    unsigned int   StartIndex;
    unsigned int   IndexNum;
    MODEL_MATERIAL Material;
};

struct MODEL_OBJ
{
    VERTEX_3D*     VertexArray;
    unsigned int   VertexNum;

    unsigned int*  IndexArray;
    unsigned int   IndexNum;

    SUBSET*        SubsetArray;
    unsigned int   SubsetNum;
};

struct MODEL
{
    ID3D11Buffer*  VertexBuffer;
    ID3D11Buffer*  IndexBuffer;

    SUBSET*        SubsetArray;
    unsigned int   SubsetNum;
};

class ModelRenderer : public Component
{
public:
    ModelRenderer() = default;

    ~ModelRenderer() override { Uninit(); }

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    void Init() override;

    void Uninit() override;

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    static void Preload(const char* FileName);

    /// - Vertex/IndexBuffer
    /// - SubsetArray
    static void UnloadAll();

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    using Component::Component;

    void Load(const char* FileName);

    void LoadShader(const char* vsFilePath, const char* psFilePath);

    void SetLocalScale(float x, float y, float z) { m_LocalScale = { x, y, z }; }

    void Draw() override;

private:
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    static void LoadModel(const char* FileName, MODEL* Model);

    static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj);

    static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

private:
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    static constexpr const char* kDefaultVSPath = "shader\\bin\\BaseLitVS.cso";
    static constexpr const char* kDefaultPSPath = "shader\\bin\\BaseLitPS.cso";

private:
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    Transform* m_Transform = nullptr;
    Vector3    m_LocalScale = { 1.0f, 1.0f, 1.0f };

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    static std::unordered_map<std::string, MODEL*> m_ModelPool;
    MODEL* m_Model = nullptr;

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    ID3D11VertexShader* m_VertexShader = nullptr;
    ID3D11PixelShader*  m_PixelShader  = nullptr;
    ID3D11InputLayout*  m_VertexLayout = nullptr;
};
