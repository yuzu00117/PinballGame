| 対象           | スタイル              | 例                                   |
| ------------ | ----------------- | ----------------------------------- |
| クラス / 構造体    | PascalCase        | `MeshRenderer`, `BoxCollider`       |
| enum の型名     | PascalCase        | `ColliderType`                      |
| enum の値      | PascalCase        | `ColliderType::Box`                 |
| 関数名          | PascalCase        | `Init()`, `CreateUnitBox()`         |
| ローカル変数       | lowerCamelCase    | `worldMatrix`, `vertexCount`        |
| 関数引数         | lowerCamelCase    | `int vertexCount`                   |
| メンバ変数        | `m_` + PascalCase | `m_VertexBuffer`, `m_Color`         |
| static メンバ変数 | `s_` + PascalCase | `s_ModelPool`                       |
| グローバル変数      | 原則禁止（必要なら `g_`）   | `g_Renderer`                        |
| 定数           | `k` + PascalCase  | `kMaxLights`, `kPi`                 |
| マクロ          | 全大文字 + アンダースコア    | `#define SAFE_RELEASE(p)`           |
| ファイル名        | PascalCase        | `MeshRenderer.h`, `BoxCollider.cpp` |
