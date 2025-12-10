#include "main.h"
#include "renderer.h"
#include "animationModel.h"
#include <algorithm>

// ------------------------------------------------------------------------------
/// ライフサイクルメソッド
/// -----------------------------------------------------------------------------
// 更新処理（ComponentのUpdate）
void AnimationModel::Update()
{
	if (!m_IsPlaying || m_CurrentAnim.empty())
	{
		return;
	}

	// フレーム進行
	m_CurrentFrame += static_cast<int>(std::round(m_Speed));
	if (m_CurrentFrame < 0) m_CurrentFrame = 0;

	// 既存のアニメーション更新処理を呼び出し
	Update(m_CurrentAnim.c_str(), m_CurrentFrame);
}

// 更新処理（AnimationModelのUpdate）
void AnimationModel::Update(const char* AnimationName1, int Frame1)
{
	if (m_Animation.count(AnimationName1) == 0)
		return;

	if (!m_Animation[AnimationName1]->HasAnimations())
		return;

	// アニメーションデータからボーンマトリクス算出
	aiAnimation* animation = m_Animation[AnimationName1]->mAnimations[0];

	for (auto pair : m_Bone)
	{
		BONE* bone = &m_Bone[pair.first];

		aiNodeAnim* nodeAnim = nullptr;

		for (unsigned int c = 0; c < animation->mNumChannels; c++)
		{
			if (animation->mChannels[c]->mNodeName == aiString(pair.first))
			{
				nodeAnim = animation->mChannels[c];
				break;
			}
		}

		aiQuaternion rot = aiQuaternion();
		aiVector3D pos = aiVector3D(0.0f, 0.0f, 0.0f);
		int f;

		if (nodeAnim)
		{
			f = Frame1 % std::max(1u, nodeAnim->mNumRotationKeys); // 簡易実装
			rot = nodeAnim->mRotationKeys[f].mValue;

			f = Frame1 % std::max(1u, nodeAnim->mNumPositionKeys); // 簡易実装
			pos = nodeAnim->mPositionKeys[f].mValue;
		}

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rot, pos);
	}

	//再帰的にボーンマトリクスを更新
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));

	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	// 頂点変換（CPUスキニング）
	// 頂点更新（スキニング）
	for (unsigned int i = 0; i < m_AiScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = m_AiScene->mMeshes[i];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[i], 0,
			D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			DEFORM_VERTEX& deformVertex = m_DeformVertex[i][v];

			aiMatrix4x4 matrix[4];
			matrix[0] = m_Bone[deformVertex.BoneName[0]].Matrix;
			matrix[1] = m_Bone[deformVertex.BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex.BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex.BoneName[3]].Matrix;

			aiMatrix4x4 outMatrix;
			outMatrix = matrix[0] * deformVertex.BoneWeight[0]
				+ matrix[1] * deformVertex.BoneWeight[1]
				+ matrix[2] * deformVertex.BoneWeight[2]
				+ matrix[3] * deformVertex.BoneWeight[3];

			// 位置
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Position *= outMatrix;

			// モデル全体のスケールを適用
			deformVertex.Position *= m_ModelScale;

			// 法線変換用に平行移動成分を無効化
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;
			outMatrix.d4 = 0.0f;

			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.Normal *= outMatrix;

			// 頂点バッファへ書き込み
			vertex[v].Position.x = deformVertex.Position.x;
			vertex[v].Position.y = deformVertex.Position.y;
			vertex[v].Position.z = deformVertex.Position.z;

			vertex[v].Normal.x = deformVertex.Normal.x;
			vertex[v].Normal.y = deformVertex.Normal.y;
			vertex[v].Normal.z = deformVertex.Normal.z;

			vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[i], 0);
	}
}

// 描画処理
void AnimationModel::Draw()
{
	// ワールド行列設定
	// Transformがセットされていればそれを使う
	XMMATRIX world = XMMatrixIdentity();
	if (m_Transform)
	{
		world = m_Transform->GetWorldMatrix();
	}
	Renderer::SetWorldMatrix(world);

	// プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;
	Renderer::SetMaterial(material);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];


		// マテリアル設定
		aiString texture;
		aiColor3D diffuse;
		float opacity;

		aiMaterial* aimaterial = m_AiScene->mMaterials[mesh->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);

		if (texture == aiString(""))
		{
			material.TextureEnable = false;
		}
		else
		{
			Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture[texture.data]);
			material.TextureEnable = true;
		}

		material.Diffuse = XMFLOAT4(diffuse.r, diffuse.g, diffuse.b, opacity);
		material.Ambient = material.Diffuse;
		Renderer::SetMaterial(material);


		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer[m], &stride, &offset);

		// インデックスバッファ設定
		Renderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// ポリゴン描画
		Renderer::GetDeviceContext()->DrawIndexed(mesh->mNumFaces * 3, 0, 0);
	}
}

// 終了処理
void AnimationModel::Uninit()
{
	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		m_VertexBuffer[m]->Release();
		m_IndexBuffer[m]->Release();
	}

	delete[] m_VertexBuffer;
	delete[] m_IndexBuffer;

	delete[] m_DeformVertex;


	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : m_Texture)
	{
		pair.second->Release();
	}



	aiReleaseImport(m_AiScene);


	for (std::pair<const std::string, const aiScene*> pair : m_Animation)
	{
		aiReleaseImport(pair.second);
	}
}


// ------------------------------------------------------------------------------
// モデル・アニメーション関連
// ------------------------------------------------------------------------------
// モデル読み込み
void AnimationModel::Load(const char* FileName)
{
	const std::string modelPath(FileName);

	// 追加: Triangulate / SortByPType を明示的に指定し、非三角形やラインを排除
	m_AiScene = aiImportFile(
		FileName,
		aiProcessPreset_TargetRealtime_MaxQuality |
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_SortByPType);
	assert(m_AiScene);

	m_VertexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];
	m_IndexBuffer = new ID3D11Buffer * [m_AiScene->mNumMeshes];

	//変形後頂点配列生成
	m_DeformVertex = new std::vector<DEFORM_VERTEX>[m_AiScene->mNumMeshes];

	//再帰的にボーン生成
	CreateBone(m_AiScene->mRootNode);

	for (unsigned int m = 0; m < m_AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = m_AiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			VERTEX_3D* vertex = new VERTEX_3D[mesh->mNumVertices];

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].Position = XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex[v].Normal = XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				if (mesh->mTextureCoords[0])
				{
					vertex[v].TexCoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				}
				else
				{
					vertex[v].TexCoord = XMFLOAT2(0.0f, 0.0f);
				}
				vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX_3D) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = vertex;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd,
				&m_VertexBuffer[m]);

			delete[] vertex;
		}


		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];

				assert(face->mNumIndices == 3);

				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.pSysMem = index;

			Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer[m]);

			delete[] index;
		}



		//変形後頂点データ初期化
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			DEFORM_VERTEX deformVertex;
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.BoneNum = 0;

			for (unsigned int b = 0; b < 4; b++)
			{
				deformVertex.BoneName[b] = "";
				deformVertex.BoneWeight[b] = 0.0f;
			}

			m_DeformVertex[m].push_back(deformVertex);
		}


		//ボーンデータ初期化
		for (unsigned int b = 0; b < mesh->mNumBones; b++)
		{
			aiBone* bone = mesh->mBones[b];

			m_Bone[bone->mName.C_Str()].OffsetMatrix = bone->mOffsetMatrix;

			//変形後頂点にボーンデータ格納
			for (unsigned int w = 0; w < bone->mNumWeights; w++)
			{
				aiVertexWeight weight = bone->mWeights[w];

				int num = m_DeformVertex[m][weight.mVertexId].BoneNum;

				if (num < 4) {
					m_DeformVertex[m][weight.mVertexId].BoneWeight[num] = weight.mWeight;
					m_DeformVertex[m][weight.mVertexId].BoneName[num] = bone->mName.C_Str();
					m_DeformVertex[m][weight.mVertexId].BoneNum++;
				}
			}
		}
	}



	// 埋め込みテクスチャ
	for (int i = 0; i < m_AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = m_AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture;

		// テクスチャ読み込み
		TexMetadata metadata;
		ScratchImage image;
		LoadFromWICMemory(aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
		CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &texture);
		assert(texture);

		m_Texture[aitexture->mFilename.data] = texture;
	}
}

// アニメーション読み込み
void AnimationModel::LoadAnimation(const char* FileName, const char* Name)
{

	m_Animation[Name] = aiImportFile(FileName, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_SortByPType);
	assert(m_Animation[Name]);

}

// 2本のアニメーションをアルファでブレンドして更新
void AnimationModel::UpdateBlend(const char* AnimationNameA, int FrameA, const char* AnimationNameB, int FrameB, float Alpha)
{
	bool hasA = (m_Animation.count(AnimationNameA) != 0) && m_Animation[AnimationNameA]->HasAnimations();
	bool hasB = (m_Animation.count(AnimationNameB) != 0) && m_Animation[AnimationNameB]->HasAnimations();
	if (!hasA && !hasB) return;
	if (hasA && !hasB) { Update(AnimationNameA, FrameA); return; }
	if (!hasA && hasB) { Update(AnimationNameB, FrameB); return; }

	Alpha = std::max(0.0f, std::min(1.0f, Alpha));

	aiAnimation* animA = m_Animation[AnimationNameA]->mAnimations[0];
	aiAnimation* animB = m_Animation[AnimationNameB]->mAnimations[0];

	for (auto pair : m_Bone)
	{
		const std::string& boneName = pair.first;
		BONE* bone = &m_Bone[boneName];

		aiNodeAnim* nodeA = nullptr;
		aiNodeAnim* nodeB = nullptr;

		for (unsigned int c = 0; c < animA->mNumChannels; c++)
		{
			if (animA->mChannels[c]->mNodeName == aiString(boneName.c_str()))
			{
				nodeA = animA->mChannels[c];
				break;
			}
		}
		for (unsigned int c = 0; c < animB->mNumChannels; c++)
		{
			if (animB->mChannels[c]->mNodeName == aiString(boneName.c_str()))
			{
				nodeB = animB->mChannels[c];
				break;
			}
		}

		aiQuaternion rotA = aiQuaternion();
		aiVector3D  posA = aiVector3D(0.0f, 0.0f, 0.0f);
		aiQuaternion rotB = aiQuaternion();
		aiVector3D  posB = aiVector3D(0.0f, 0.0f, 0.0f);

		if (nodeA)
		{
			int fr = FrameA % std::max(1u, nodeA->mNumRotationKeys);
			int fp = FrameA % std::max(1u, nodeA->mNumPositionKeys);
			rotA = nodeA->mRotationKeys[fr].mValue;
			posA = nodeA->mPositionKeys[fp].mValue;
		}
		if (nodeB)
		{
			int fr = FrameB % std::max(1u, nodeB->mNumRotationKeys);
			int fp = FrameB % std::max(1u, nodeB->mNumPositionKeys);
			rotB = nodeB->mRotationKeys[fr].mValue;
			posB = nodeB->mPositionKeys[fp].mValue;
		}

		aiQuaternion rotOut;
		aiQuaternion::Interpolate(rotOut, rotA, rotB, Alpha);
		rotOut.Normalize();
		aiVector3D posOut = posA * (1.0f - Alpha) + posB * Alpha;

		bone->AnimationMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f), rotOut, posOut);
	}

	//再帰的にボーンマトリクスを更新
	aiMatrix4x4 rootMatrix = aiMatrix4x4(aiVector3D(1.0f, 1.0f, 1.0f),
		aiQuaternion((float)AI_MATH_PI, 0.0f, 0.0f), aiVector3D(0.0f, 0.0f, 0.0f));
	UpdateBoneMatrix(m_AiScene->mRootNode, rootMatrix);

	// 頂点更新（スキニング）
	for (unsigned int i = 0; i < m_AiScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = m_AiScene->mMeshes[i];

		D3D11_MAPPED_SUBRESOURCE ms;
		Renderer::GetDeviceContext()->Map(m_VertexBuffer[i], 0,
			D3D11_MAP_WRITE_DISCARD, 0, &ms);

		VERTEX_3D* vertex = (VERTEX_3D*)ms.pData;

		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			DEFORM_VERTEX& deformVertex = m_DeformVertex[i][v];

			aiMatrix4x4 matrix[4];
			matrix[0] = m_Bone[deformVertex.BoneName[0]].Matrix;
			matrix[1] = m_Bone[deformVertex.BoneName[1]].Matrix;
			matrix[2] = m_Bone[deformVertex.BoneName[2]].Matrix;
			matrix[3] = m_Bone[deformVertex.BoneName[3]].Matrix;

			aiMatrix4x4 outMatrix;
			outMatrix = matrix[0] * deformVertex.BoneWeight[0]
				+ matrix[1] * deformVertex.BoneWeight[1]
				+ matrix[2] * deformVertex.BoneWeight[2]
				+ matrix[3] * deformVertex.BoneWeight[3];

			// 位置
			deformVertex.Position = mesh->mVertices[v];
			deformVertex.Position *= outMatrix;

			// モデル全体のスケールを適用
			deformVertex.Position *= m_ModelScale;

			// 法線変換用に平行移動成分を無効化
			outMatrix.a4 = 0.0f;
			outMatrix.b4 = 0.0f;
			outMatrix.c4 = 0.0f;
			outMatrix.d4 = 0.0f;

			deformVertex.Normal = mesh->mNormals[v];
			deformVertex.Normal *= outMatrix;

			// 頂点バッファへ書き込み
			vertex[v].Position.x = deformVertex.Position.x;
			vertex[v].Position.y = deformVertex.Position.y;
			vertex[v].Position.z = deformVertex.Position.z;

			vertex[v].Normal.x = deformVertex.Normal.x;
			vertex[v].Normal.y = deformVertex.Normal.y;
			vertex[v].Normal.z = deformVertex.Normal.z;

			vertex[v].TexCoord.x = mesh->mTextureCoords[0][v].x;
			vertex[v].TexCoord.y = mesh->mTextureCoords[0][v].y;

			vertex[v].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Renderer::GetDeviceContext()->Unmap(m_VertexBuffer[i], 0);
	}
}

// ------------------------------------------------------------------------------
// ボーン関連
// ------------------------------------------------------------------------------
// ボーン作成
void AnimationModel::CreateBone(aiNode* node)
{
	BONE bone;

	m_Bone[node->mName.C_Str()] = bone;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		CreateBone(node->mChildren[n]);
	}

}

// ボーン行列更新
void AnimationModel::UpdateBoneMatrix(aiNode* node, aiMatrix4x4 matrix)
{
	BONE* bone = &m_Bone[node->mName.C_Str()];

	aiMatrix4x4 worldMatrix;
	worldMatrix = matrix * bone->AnimationMatrix;

	bone->Matrix = worldMatrix * bone->OffsetMatrix;

	for (unsigned int n = 0; n < node->mNumChildren; n++)
	{
		UpdateBoneMatrix(node->mChildren[n], worldMatrix);
	}
}

// ------------------------------------------------------------------------------
// アニメーション管理用API
// ------------------------------------------------------------------------------
// アニメーション再生
void AnimationModel::Play(const char* animName, bool loop, bool resetFrame)
{
	// 登録されていないアニメーションなら何もしない
	if (m_Animation.count(animName) == 0)
	{
		m_IsPlaying = false;
		return;
	}

	// アニメーション再生開始
	if (resetFrame || m_CurrentAnim != animName)
	{
		m_CurrentFrame = 0;
	}

	m_CurrentAnim = animName;
	m_Loop 		  = loop;
	m_IsPlaying   = true;
}

// アニメーション停止
void AnimationModel::Stop()
{
	m_IsPlaying = false;
}

// 