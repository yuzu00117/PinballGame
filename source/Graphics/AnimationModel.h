#pragma once

#include <unordered_map>

// Assimp関連
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")

// コンポーネント関連
#include "component.h"
#include "Transform.h"

/// <summary>
/// アニメーション用のモデル描画コンポーネント
/// </summary>
// ------------------------------------------------------------------------------
// 構造体定義
// ------------------------------------------------------------------------------
//変形後頂点構造体
struct DEFORM_VERTEX
{
	aiVector3D Position;
	aiVector3D Normal;
	int				BoneNum;
	std::string		BoneName[4];//本来はボーンインデックスで管理するべき
	float			BoneWeight[4];
};

//ボーン構造体
struct BONE
{
	aiMatrix4x4 Matrix;
	aiMatrix4x4 AnimationMatrix;
	aiMatrix4x4 OffsetMatrix;
};

/// <summary>
/// アニメーション用のモデル描画コンポーネントクラス
/// </summary>
class AnimationModel : public Component
{
public:
	using Component::Component;

	// ------------------------------------------------------------------------------
	// 関数定義
	// ------------------------------------------------------------------------------
	/// <summary>
	/// モデル読み込み
	/// </summary>
	void Load(const char* FileName);

	/// <summary>
	/// アニメーション読み込み
	/// </summary>
	void LoadAnimation(const char* FileName, const char* Name);

	/// <summary>
	/// ライフサイクルメソッド
	/// </summary>
	void Uninit() override;
	void Update() override; // コンポーネント用の引数なし版
	void Update(const char* AnimationName1, int Frame1);
	void Draw() override;

	/// <summary>
	/// 2本のアニメーションをアルファでブレンドして更新
	/// </summary>
	void UpdateBlend(const char* AnimationNameA, int FrameA, const char* AnimationNameB, int FrameB, float Alpha);

	/// <summary>
	/// アニメーション管理用API
	/// </summary>
	void Play(const char* AnimationName, bool Loop = true, bool ResetFrame = true);
	void Stop();
	void SetSpeed(float Speed) { m_Speed = Speed; }

	// ------------------------------------------------------------------------------
	// 変数定義
	// ------------------------------------------------------------------------------
	// GameObject の Transform 情報を保持するポインタ
	Transform* m_Transform = nullptr;

private:
	// ------------------------------------------------------------------------------
	// 関数定義
	// ------------------------------------------------------------------------------
	/// <summary>
	/// ボーン作成
	/// </summary>
	void CreateBone(aiNode* Node);

	/// <summary>
	/// ボーン行列更新
	/// </summary>
	void UpdateBoneMatrix(aiNode* Node, aiMatrix4x4 Matrix);

	// ------------------------------------------------------------------------------
	// 変数定義
	// ------------------------------------------------------------------------------
	const aiScene* m_AiScene = nullptr;
	std::unordered_map<std::string, const aiScene*> m_Animation;

	ID3D11Buffer** m_VertexBuffer;
	ID3D11Buffer** m_IndexBuffer;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Texture;

	std::vector<DEFORM_VERTEX>* m_DeformVertex;   						  //変形後頂点データ
	std::unordered_map<std::string, BONE> m_Bone; 						  //ボーンデータ（名前で参照）

	float m_ModelScale = 1.0f; 											  //モデルスケール

	// アニメーション制御用
	std::string m_CurrentAnim; 											  // 現在再生中のアニメーション名
	int	  m_CurrentFrame = 0; 											  // 現在のアニメーションフレーム
	float m_Speed = 1.0f; 												  // アニメーション再生速度
	bool  m_Loop = true; 												  // ループ再生フラグ
	bool  m_IsPlaying = false; 											  // 再生中フラグ
};