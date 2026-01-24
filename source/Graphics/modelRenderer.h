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
// モデル用データ構造
// ------------------------------------------------------------------------------

/// マテリアル情報
/// - MTL(.mtl) から読み込んだ色/パラメータ（MATERIAL）を保持
/// - map_Kd などのテクスチャパス（TextureName）を保持
/// - Texture は読み込み後の SRV（無い場合は nullptr）
struct MODEL_MATERIAL
{
    char                      Name[256];        // マテリアル名（newmtl）
    MATERIAL                  Material;          // 色/係数（Ka/Kd/Ks/Ns/d 等）
    char                      TextureName[256];  // テクスチャファイルパス（map_Kd）
    ID3D11ShaderResourceView* Texture;           // テクスチャSRV（無い場合は nullptr）
};

/// 描画サブセット
/// - usemtl ごとに分割された描画単位
/// - IndexBuffer の範囲（StartIndex, IndexNum）と Material を持つ
struct SUBSET
{
    unsigned int   StartIndex;   // 開始インデックス
    unsigned int   IndexNum;     // インデックス数
    MODEL_MATERIAL Material;     // サブセットのマテリアル
};

/// OBJ 読み込み用（CPU側）データ
/// - LoadObj() が生成する一時データ
/// - LoadModel() で GPU バッファ化した後に解放される
struct MODEL_OBJ
{
    VERTEX_3D*     VertexArray;  // 頂点配列
    unsigned int   VertexNum;    // 頂点数

    unsigned int*  IndexArray;   // インデックス配列
    unsigned int   IndexNum;     // インデックス数

    SUBSET*        SubsetArray;  // サブセット配列
    unsigned int   SubsetNum;    // サブセット数
};

/// 描画用（GPU側）モデル
/// - Vertex/Index バッファとサブセット情報を保持
/// - ModelRenderer のモデルプールで共有される
struct MODEL
{
    ID3D11Buffer*  VertexBuffer; // 頂点バッファ
    ID3D11Buffer*  IndexBuffer;  // インデックスバッファ

    SUBSET*        SubsetArray;  // サブセット配列
    unsigned int   SubsetNum;    // サブセット数
};

/// 3Dモデルを描画するコンポーネント
/// - OBJ/MTL を読み込み、サブセット単位でマテリアル/テクスチャを設定して描画する
/// - モデルは静的プール（m_ModelPool）で共有し、同一ファイルは1回だけロードする
/// - シェーダーが未設定の場合はデフォルト（BaseLitVS/PS）を使用する
class ModelRenderer : public Component
{
public:
    /// デフォルトコンストラクタ
    ModelRenderer() = default;

    /// デストラクタ
    /// - Uninit() を呼び出してシェーダー等を解放する
    ~ModelRenderer() override { Uninit(); }

    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化
    /// - Owner の Transform を参照として保持
    /// - シェーダー未設定の場合はデフォルトシェーダーをロード
    void Init() override;

    /// 終了
    /// - シェーダー/入力レイアウトを解放
    /// - モデル自体はプール管理のためここでは解放しない
    void Uninit() override;

    // ----------------------------------------------------------------------
    // 静的操作（モデルプール）
    // ----------------------------------------------------------------------
    /// モデルを事前読み込みする
    /// - 既にプールに存在する場合は何もしない
    static void Preload(const char* FileName);

    /// モデルプールを全解放する
    /// - Vertex/IndexBuffer
    /// - SubsetArray
    /// - サブセットの Texture(SRV)
    static void UnloadAll();

    // ----------------------------------------------------------------------
    // インスタンス操作
    // ----------------------------------------------------------------------
    using Component::Component;

    /// モデルを読み込む（プール参照を取得）
    /// - 既にプールに存在する場合は参照を取得するだけ
    /// - 存在しない場合はロードしてプールへ登録
    void Load(const char* FileName);

    /// モデル用シェーダーを設定する
    /// - 頂点シェーダー/入力レイアウト/ピクセルシェーダーを作成して保持
    void LoadShader(const char* vsFilePath, const char* psFilePath);

    /// ローカルスケールを設定する（Transformのスケールとは別枠）
    void SetLocalScale(float x, float y, float z) { m_LocalScale = { x, y, z }; }

    /// モデル描画
    /// - WorldMatrix 設定 → VB/IB 設定 → サブセットごとに Material/Texture を設定して描画
    void Draw() override;

private:
    // ----------------------------------------------------------------------
    // モデル読み込み内部処理
    // ----------------------------------------------------------------------
    /// モデルをロードして GPU バッファを生成する
    static void LoadModel(const char* FileName, MODEL* Model);

    /// OBJファイルを読み込む（CPU側データ作成）
    static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj);

    /// MTLファイルを読み込む（マテリアル配列作成）
    static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

private:
    // ----------------------------------------------------------------------
    // デフォルトのシェーダーパス
    // ----------------------------------------------------------------------
    static constexpr const char* kDefaultVSPath = "shader\\bin\\BaseLitVS.cso"; // デフォルト頂点シェーダー
    static constexpr const char* kDefaultPSPath = "shader\\bin\\BaseLitPS.cso"; // デフォルトピクセルシェーダー

private:
    // ----------------------------------------------------------------------
    // 参照/状態
    // ----------------------------------------------------------------------
    Transform* m_Transform = nullptr;                         // 非所有：Owner の Transform 参照
    Vector3    m_LocalScale = { 1.0f, 1.0f, 1.0f };          // ローカルスケール

    // ----------------------------------------------------------------------
    // モデルプール
    // ----------------------------------------------------------------------
    static std::unordered_map<std::string, MODEL*> m_ModelPool; // 所有：ロード済みモデルの共有プール
    MODEL* m_Model = nullptr;                                   // 非所有：現在参照しているモデル

    // ----------------------------------------------------------------------
    // モデル用シェーダー
    // ----------------------------------------------------------------------
    ID3D11VertexShader* m_VertexShader = nullptr;  // 頂点シェーダー
    ID3D11PixelShader*  m_PixelShader  = nullptr;  // ピクセルシェーダー
    ID3D11InputLayout*  m_VertexLayout = nullptr;  // 入力レイアウト
};
