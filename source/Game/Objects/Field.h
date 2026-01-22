//------------------------------------------------------------------------------
// Field
//------------------------------------------------------------------------------
// ピンボールゲームのフィールド（台全体）を表すモジュール。
// - GameObject 階層のルートとして機能する
// - フィールド内に存在するすべてのオブジェクトの親となる
//
// 構成：
// - 配置定義：FieldLayout
// - 生成処理：FieldBuilder
// - 階層・制御の起点：Field
//
// 本ファイルは「フィールド全体の概念」を定義し、
// 具体的な生成や配置の詳細は他モジュールに委譲する。
//------------------------------------------------------------------------------
#pragma once

#include "gameobject.h"
#include "vector3.h"
#include "FieldBuilder.h"

// 前方宣言
class MeshRenderer;
class ColliderGroup;

//------------------------------------------------------------------------------
// Field
//------------------------------------------------------------------------------
// ピンボール台（フィールド全体）を表すルート GameObject。
// - 床/壁などの環境オブジェクトを生成し、フィールド内の子 GameObject 群を保持する
// - フィールド内オブジェクトの生成（Hole/Flipper/Bumper/Spawner など）は FieldBuilder に委譲する
//
// 設計方針：
// - 配置定義（FieldLayout）と生成処理（FieldBuilder）を分離し、Field は階層の起点として振る舞う
// - Field::Init では「環境（床/壁）」の構築と「レイアウトに基づく子オブジェクト生成」を行う
//
// 注意：
// - 子オブジェクトの所有権は GameObject 階層（親が子を所有）に従う
// - m_Level は生成済みオブジェクトへの非所有参照の集合であり、寿命は Field の子として存在する間に限る
class Field : public GameObject
{
public:
    // ----------------------------------------------------------------------
    // ライフサイクルメソッド
    // ----------------------------------------------------------------------
    /// 初期化処理
    /// - 床/壁（環境）を生成する
    /// - FieldLayout を作成し、FieldBuilder により子オブジェクトを生成する
    /// 注意：FieldBuilder::Build は生成後に各オブジェクトの Init を呼び出す
    void Init() override;

    /// 終了処理
    /// - 参照ポインタを無効化する（所有しているリソースの解放は GameObject 側に従う）
    void Uninit() override;

    /// 更新処理
    /// - 現状は基底クラスの更新（子の更新）に委譲する
    void Update(float deltaTime) override;

    /// 描画処理
    /// - 現状は基底クラスの描画（子の描画）に委譲する
    void Draw() override;

private:
    // ----------------------------------------------------------------------
    // レイアウト作成
    // ----------------------------------------------------------------------
    /// ステージ01のフィールドレイアウトを作成する
    /// - FieldBuilder に渡す初期配置定義（FieldLayout）を組み立てる
    static FieldLayout MakeStage01Layout();

    // ----------------------------------------------------------------------
    // フィールド寸法
    // ----------------------------------------------------------------------
    static constexpr float          kHalfWidth  = 9.5f;                               // フィールドの幅の半分
    static constexpr float          kHalfHeight = 14.5f;                              // フィールドの高さの半分
    static constexpr float          kWallThick  = 1.0f;                               // 壁の厚さ
    static constexpr float          kWallHeight = 2.0f;                               // 壁の高さ

    // ----------------------------------------------------------------------
    // リソース
    // ----------------------------------------------------------------------
    static constexpr const wchar_t* kFieldTexturePath = L"asset\\texture\\Field.png"; // フィールドテクスチャのパス
    static constexpr const wchar_t* kWallTexturePath  = L"asset\\texture\\Wall2.png"; // 壁テクスチャのパス
    static constexpr const char*    kVertexShaderPath = "shader\\bin\\BaseLitVS.cso"; // 頂点シェーダのパス
    static constexpr const char*    kPixelShaderPath  = "shader\\bin\\BaseLitPS.cso"; // ピクセルシェーダのパス

    // ----------------------------------------------------------------------
    // コンポーネントの参照ポインタ
    // ----------------------------------------------------------------------
    MeshRenderer*  m_Floor = nullptr;         // 非所有：床メッシュ描画（Init で設定 / Uninit で無効化）
    ColliderGroup* m_ColliderGroup = nullptr; // 非所有：床/壁の当たり判定（Init で設定 / Uninit で無効化）
    LevelObjects   m_Level;                   // 非所有：生成済みレベルオブジェクト参照の集合
};
