#pragma once

#include "main.h"
#include "component.h"
#include "Transform.h"
#include "vector3.h"
#include <string>
#include <unordered_map>

using namespace DirectX;

class GameObject;

// マテリアル構造体
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
	ID3D11ShaderResourceView* Texture;

};

// 描画サブセット構造体
struct SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	MODEL_MATERIAL	Material;
};

// モデル構造体
struct MODEL_OBJ
{
	VERTEX_3D* VertexArray;
	unsigned int	VertexNum;

	unsigned int* IndexArray;
	unsigned int	IndexNum;

	SUBSET* SubsetArray;
	unsigned int	SubsetNum;
};

struct MODEL
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	SUBSET* SubsetArray;
	unsigned int	SubsetNum;
};

/// <summary>
/// 3Dモデルを描画するコンポーネント
/// </summary>
class ModelRenderer : public Component
{
public:
	// ----------------------------------------------------------------------
	// 関数定義
	// ----------------------------------------------------------------------
	/// <summary>
	/// ライフサイクルメソッド
	/// </summary>
	ModelRenderer() = default;
	~ModelRenderer() override { Uninit(); }

	void Init() override;
	void Uninit() override;

	/// <summary>
	/// 静的操作
	/// </summary>
	static void Preload(const char* FileName);
	static void UnloadAll();

	// インスタンス操作
	using Component::Component;

	/// <summary>
	/// モデルを読み込む
	/// </summary>
	void Load(const char* FileName);

	/// <summary>
	/// モデル用シェーダーを設定する
	/// </summary>
	void LoadShader(const char* vsFilePath, const char* psFilePath);

	/// <summary>
	/// ローカルスケールを設定する
	/// </summary>
	void SetLocalScale(float x, float y, float z) { m_LocalScale = { x, y, z }; }

	/// <summary>
	/// モデルの描画
	/// </summary>
	void Draw() override;

private:
	// ----------------------------------------------------------------------
	// 関数定義
	// ----------------------------------------------------------------------
	/// <summary>
	/// モデル読み込み補助関数
	/// </summary>
	static void LoadModel(const char* FileName, MODEL* Model);

	/// <summary>
	/// OBJファイル読み込み
	/// </summary>
	static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

	// ----------------------------------------------------------------------
	// 定数定義
	// ----------------------------------------------------------------------
	static constexpr const char* kDefaultVSPath = "shader\\bin\\BaseLitVS.cso"; 	// デフォルト頂点シェーダーパス
	static constexpr const char* kDefaultPSPath = "shader\\bin\\BaseLitPS.cso"; 	// デフォルトピクセルシェーダーパス

	// ----------------------------------------------------------------------
	// 変数定義
	// ----------------------------------------------------------------------
	Transform* m_Transform = nullptr; 													// 所属するTransformコンポーネントへのポインタ
	Vector3 m_LocalScale = { 1.0f, 1.0f, 1.0f }; 										// ローカルスケール
	static std::unordered_map<std::string, MODEL*> m_ModelPool;							// モデルプール
	MODEL* m_Model{};																	// 所属するモデル

	// モデル用シェーダー
	ID3D11VertexShader *m_VertexShader = nullptr; 										// 頂点シェーダー
	ID3D11PixelShader *m_PixelShader = nullptr;	  										// ピクセルシェーダー
	ID3D11InputLayout *m_VertexLayout = nullptr;  										// 頂点レイアウト
};