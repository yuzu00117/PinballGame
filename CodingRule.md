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


## アノテーションコメント規約

本プロジェクトでは、**TodoTree 拡張で検出可能なアノテーションコメント**として
以下のタグのみを使用する。

* `TODO:`
* `FIXME:`
* `HACK:`
* `BUG:`

### 使い分け指針

| 状態       | 使用タグ    |
| -------- | ------- |
| 将来対応予定   | `TODO`  |
| 問題があると認識 | `FIXME` |
| 意図的な暫定実装 | `HACK`  |
| 実害のある不具合 | `BUG`   |

