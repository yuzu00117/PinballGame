#include "SpriteSheet.h"

// ------------------------------------------------------------------------------
// ライフサイクル処理
// ------------------------------------------------------------------------------
// - インスタンスの初期化および終了時のリソース解放等を行う

void SpriteSheet::Init()
{
    // 親クラスの初期化
    Sprite::Init();

    // デフォルト値
    m_Columns = 1;
    m_Rows = 1;
    m_Index = 0;
    m_IsGridMode = true;
}

void SpriteSheet::Uninit()
{
    // 親クラスの終了処理
    Sprite::Uninit();
}

// ------------------------------------------------------------------------------
// 描画処理
// ------------------------------------------------------------------------------
// - 事前にUV座標等を再計算した上で親クラスの描画処理を呼び出す
void SpriteSheet::Draw()
{
    // 自動（グリッド）モードの場合のみ、描画直前にUVを算出して反映する
    // 手動モード(SetUVRect)の場合は既に計算済みのSetUVが設定されている
    if (m_IsGridMode)
    {
        UpdateGridUV();
    }
    
    // 親クラスの描画処理
    Sprite::Draw();
}

// ------------------------------------------------------------------------------
// スプライトシート設定
// ------------------------------------------------------------------------------
// - グリッド分割数や描画するインデックス、直接のUV指定など、表示対象のコマを設定する

void SpriteSheet::SetGrid(int cols, int rows)
{
    // 0 除算防止のため 1 以上を保証する
    m_Columns = (cols > 0) ? cols : 1;
    m_Rows    = (rows > 0) ? rows : 1;
    m_IsGridMode = true;
}

// 表示するコマのインデックスを設定する
void SpriteSheet::SetIndex(int index)
{
    if (!m_IsGridMode) return;

    int maxIndex = m_Columns * m_Rows - 1;
    
    // 範囲外のインデックスはクランプする
    if (index < 0)
    {
        m_Index = 0;
    }
    else if (index > maxIndex)
    {
        m_Index = maxIndex;
    }
    else
    {
        m_Index = index;
    }
}

// ピクセル単位で直接UV座標を計算して設定する
void SpriteSheet::SetUVRect(float x, float y, float width, float height, float texWidth, float texHeight)
{
    // 手動モードにする
    m_IsGridMode = false;

    if (texWidth <= 0 || texHeight <= 0) return;

    // ピクセル座標から 0.0?1.0 の UV 座標に変換
    float u0 = x / texWidth;
    float v0 = y / texHeight;
    float u1 = (x + width) / texWidth;
    float v1 = (y + height) / texHeight;

    // Sprite の UV 設定関数を呼ぶ
    SetUV(u0, v0, u1, v1);
}

// ------------------------------------------------------------------------------
// 計算処理
// ------------------------------------------------------------------------------
// - グリッド分割時のインデックスから各コマのUV座標を算出する
void SpriteSheet::UpdateGridUV()
{
    // 1コマあたりの UV サイズ
    float uvWidth  = 1.0f / static_cast<float>(m_Columns);
    float uvHeight = 1.0f / static_cast<float>(m_Rows);

    // インデックスから現在の列と行を計算
    int col = m_Index % m_Columns;
    int row = m_Index / m_Columns;

    // UV の左上と右下の座標を計算
    float u0 = static_cast<float>(col) * uvWidth;
    float v0 = static_cast<float>(row) * uvHeight;
    float u1 = u0 + uvWidth;
    float v1 = v0 + uvHeight;

    // 親クラス (Sprite) の UV 設定メソッドを呼び出す
    SetUV(u0, v0, u1, v1);
}
