
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "main.h"
#include "renderer.h"
#include "modelRenderer.h"
#include "gameobject.h"

using namespace DirectX;

// 静的メンバ変数の初期化
std::unordered_map<std::string, MODEL *> ModelRenderer::m_ModelPool;

// ------------------------------------------------------------
// ライフサイクルメソッド
// ------------------------------------------------------------
void ModelRenderer::Init()
{
	// OwnerからTransformコンポーネントを取得
	m_Transform = m_Owner->GetComponent<Transform>();

	// 念の為アサーション
	assert(m_Transform && "ModelRenderer::Init: Transform component not found in Owner GameObject");

	// まだシェーダーが設定されていなければデフォルトシェーダーを読み込む
	if (!m_VertexShader || !m_PixelShader || !m_VertexLayout)
	{
		LoadShader(kDefaultVSPath, kDefaultPSPath);
	}
}

void ModelRenderer::Uninit()
{
	// アサーション
	assert(m_VertexShader && "ModelRenderer::Uninit: VertexShader is null");
	assert(m_PixelShader && "ModelRenderer::Uninit: PixelShader is null");
	assert(m_VertexLayout && "ModelRenderer::Uninit: VertexLayout is null");

	// シェーダー解放
	m_VertexShader->Release();
	m_VertexShader = nullptr;

	m_PixelShader->Release();
	m_PixelShader = nullptr;

	m_VertexLayout->Release();
	m_VertexLayout = nullptr;

	// モデルはプールで管理されているため解放しない
	m_Model = nullptr;
}

// ------------------------------------------------------------
// シェーダー読み込み
// ------------------------------------------------------------
void ModelRenderer::LoadShader(const char *vsFilePath, const char *psFilePath)
{
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, vsFilePath);
	Renderer::CreatePixelShader(&m_PixelShader, psFilePath);
}

// ------------------------------------------------------------
// モデル描画
// ------------------------------------------------------------
void ModelRenderer::Draw()
{
	// アサーション
	assert(m_Model && "ModelRenderer::Draw: Model is null");
	assert(m_Transform && "ModelRenderer::Draw: Transform is null");

	auto* ctx = Renderer::GetDeviceContext();

	// ワールド行列設定
	XMMATRIX localScaleMatrix = XMMatrixScaling(m_LocalScale.x, m_LocalScale.y, m_LocalScale.z);
	XMMATRIX worldMatrix = localScaleMatrix * m_Transform->GetWorldMatrix();
	Renderer::SetWorldMatrix(worldMatrix);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, &m_Model->VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	ctx->IASetIndexBuffer(m_Model->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// シェーダー・レイアウト設定（設定されていれば）
	if (m_VertexLayout) ctx->IASetInputLayout(m_VertexLayout);
	if (m_VertexShader) ctx->VSSetShader(m_VertexShader, nullptr, 0);
	if (m_PixelShader) ctx->PSSetShader(m_PixelShader, nullptr, 0);

	// サブセットごとにマテリアルとテクスチャを設定して描画
	for (unsigned int i = 0; i < m_Model->SubsetNum; i++)
	{
		// マテリアル設定
		Renderer::SetMaterial(m_Model->SubsetArray[i].Material.Material);

		// テクスチャ設定
		if (m_Model->SubsetArray[i].Material.Texture)
		{
			ctx->PSSetShaderResources(0, 1, &m_Model->SubsetArray[i].Material.Texture);
		}
		else
		{
			ID3D11ShaderResourceView* nullSrv = nullptr;
			ctx->PSSetShaderResources(0, 1, &nullSrv);
		}

		// ポリゴン描画
		ctx->DrawIndexed(
			m_Model->SubsetArray[i].IndexNum,
			m_Model->SubsetArray[i].StartIndex,
			0);
	}
}

// ------------------------------------------------------------
// モデルプール関連
// -----------------------------------------------------------

// モデルの事前読み込み
void ModelRenderer::Preload(const char *FileName)
{
	if (m_ModelPool.count(FileName) > 0)
	{
		return;
	}

	MODEL *model = new MODEL;
	LoadModel(FileName, model);

	m_ModelPool[FileName] = model;
}

// モデルプールの全解放
void ModelRenderer::UnloadAll()
{
	for (std::pair<const std::string, MODEL *> pair : m_ModelPool)
	{
		pair.second->VertexBuffer->Release();
		pair.second->IndexBuffer->Release();

		for (unsigned int i = 0; i < pair.second->SubsetNum; i++)
		{
			if (pair.second->SubsetArray[i].Material.Texture)
				pair.second->SubsetArray[i].Material.Texture->Release();
		}

		delete[] pair.second->SubsetArray;

		delete pair.second;
	}

	m_ModelPool.clear();
}

// ------------------------------------------------------------
// モデル読み込み
// ------------------------------------------------------------
void ModelRenderer::Load(const char *FileName)
{
	if (m_ModelPool.count(FileName) > 0)
	{
		m_Model = m_ModelPool[FileName];
		return;
	}

	m_Model = new MODEL;
	LoadModel(FileName, m_Model);

	m_ModelPool[FileName] = m_Model;
}

void ModelRenderer::LoadModel(const char *FileName, MODEL *Model)
{

	MODEL_OBJ modelObj;
	LoadObj(FileName, &modelObj);

	// 頂点バッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * modelObj.VertexNum;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = modelObj.VertexArray;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &Model->VertexBuffer);
	}

	// インデックスバッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * modelObj.IndexNum;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = modelObj.IndexArray;

		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &Model->IndexBuffer);
	}

	// サブセット設定
	{
		Model->SubsetArray = new SUBSET[modelObj.SubsetNum];
		Model->SubsetNum = modelObj.SubsetNum;

		for (unsigned int i = 0; i < modelObj.SubsetNum; i++)
		{
			Model->SubsetArray[i].StartIndex = modelObj.SubsetArray[i].StartIndex;
			Model->SubsetArray[i].IndexNum = modelObj.SubsetArray[i].IndexNum;

			Model->SubsetArray[i].Material.Material = modelObj.SubsetArray[i].Material.Material;
			Model->SubsetArray[i].Material.Texture = nullptr;

			// テクスチャ読み込み（名前が空ならスキップ）
			const char *texName = modelObj.SubsetArray[i].Material.TextureName;
			if (texName[0] != '\0')
			{
				TexMetadata metadata{};
				ScratchImage image{};
				wchar_t wc[256] = {};
				mbstowcs(wc, texName, _countof(wc));
				if (SUCCEEDED(LoadFromWICFile(wc, WIC_FLAGS_NONE, &metadata, image)))
				{
					CreateShaderResourceView(
						Renderer::GetDevice(),
						image.GetImages(), image.GetImageCount(), metadata,
						&Model->SubsetArray[i].Material.Texture);
				}
			}

			// TextureEnable 設定
			Model->SubsetArray[i].Material.Material.TextureEnable =
				(Model->SubsetArray[i].Material.Texture != nullptr);
		}
	}

	delete[] modelObj.VertexArray;
	delete[] modelObj.IndexArray;
	delete[] modelObj.SubsetArray;
}

// モデル読込////////////////////////////////////////////
void ModelRenderer::LoadObj(const char *FileName, MODEL_OBJ *ModelObj)
{

	char dir[MAX_PATH];
	strcpy(dir, FileName);
	PathRemoveFileSpec(dir);

	XMFLOAT3 *positionArray;
	XMFLOAT3 *normalArray;
	XMFLOAT2 *texcoordArray;

	unsigned int positionNum = 0;
	unsigned int normalNum = 0;
	unsigned int texcoordNum = 0;
	unsigned int vertexNum = 0;
	unsigned int indexNum = 0;
	unsigned int in = 0;
	unsigned int subsetNum = 0;

	MODEL_MATERIAL *materialArray = nullptr;
	unsigned int materialNum = 0;

	char str[256];
	char *s;
	char c;

	FILE *file;
	file = fopen(FileName, "rt");
	assert(file);

	// 要素数カウント
	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;

		if (strcmp(str, "v") == 0)
		{
			positionNum++;
		}
		else if (strcmp(str, "vn") == 0)
		{
			normalNum++;
		}
		else if (strcmp(str, "vt") == 0)
		{
			texcoordNum++;
		}
		else if (strcmp(str, "usemtl") == 0)
		{
			subsetNum++;
		}
		else if (strcmp(str, "f") == 0)
		{
			in = 0;

			do
			{
				fscanf(file, "%s", str);
				vertexNum++;
				in++;
				c = fgetc(file);
			} while (c != '\n' && c != '\r');

			// 四角は三角に分割
			if (in == 4)
				in = 6;

			indexNum += in;
		}
	}

	// usemtlが一度もないobjでもかけるように、最低1サブセット確保
	if (subsetNum == 0)
		subsetNum = 1;

	// メモリ確保
	positionArray = new XMFLOAT3[positionNum];
	normalArray = new XMFLOAT3[normalNum];
	texcoordArray = new XMFLOAT2[texcoordNum];

	ModelObj->VertexArray = new VERTEX_3D[vertexNum];
	ModelObj->VertexNum = vertexNum;

	ModelObj->IndexArray = new unsigned int[indexNum];
	ModelObj->IndexNum = indexNum;

	ModelObj->SubsetArray = new SUBSET[subsetNum];
	ModelObj->SubsetNum = subsetNum;

	// 要素読込
	XMFLOAT3 *position = positionArray;
	XMFLOAT3 *normal = normalArray;
	XMFLOAT2 *texcoord = texcoordArray;

	unsigned int vc = 0;
	unsigned int ic = 0;
	unsigned int sc = 0;

	fseek(file, 0, SEEK_SET);

	// 追加：デフォルトマテリアルを用意（usemtl が来ない場合用）
	MODEL_MATERIAL defaultMat{};
	strcpy(defaultMat.Name, "Default");
	defaultMat.Material.Ambient = XMFLOAT4(0, 0, 0, 1);
	defaultMat.Material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1);
	defaultMat.Material.Specular = XMFLOAT4(0, 0, 0, 1);
	defaultMat.Material.Shininess = 0.0f;
	defaultMat.Material.TextureEnable = false;
	defaultMat.Texture = nullptr;
	defaultMat.TextureName[0] = '\0';

	// 最初のサブセットを仮スタート（usemtl が来れば上書き）
	ModelObj->SubsetArray[0].StartIndex = 0;
	ModelObj->SubsetArray[0].Material = defaultMat;

	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;

		if (strcmp(str, "mtllib") == 0)
		{
			// マテリアルファイル
			fscanf(file, "%s", str);

			char path[256];
			strcpy(path, dir);
			strcat(path, "\\");
			strcat(path, str);

			LoadMaterial(path, &materialArray, &materialNum);
		}
		else if (strcmp(str, "o") == 0)
		{
			// オブジェクト名
			fscanf(file, "%s", str);
		}
		else if (strcmp(str, "v") == 0)
		{
			// 頂点座標
			fscanf(file, "%f", &position->x);
			fscanf(file, "%f", &position->y);
			fscanf(file, "%f", &position->z);
			position++;
		}
		else if (strcmp(str, "vn") == 0)
		{
			// 法線
			fscanf(file, "%f", &normal->x);
			fscanf(file, "%f", &normal->y);
			fscanf(file, "%f", &normal->z);
			normal++;
		}
		else if (strcmp(str, "vt") == 0)
		{
			// テクスチャ座標
			fscanf(file, "%f", &texcoord->x);
			fscanf(file, "%f", &texcoord->y);
			texcoord->x = 1.0f - texcoord->x;
			texcoord->y = 1.0f - texcoord->y;
			texcoord++;
		}
		else if (strcmp(str, "usemtl") == 0)
		{
			// マテリアル
			fscanf(file, "%s", str);

			if (sc != 0)
				ModelObj->SubsetArray[sc - 1].IndexNum = ic - ModelObj->SubsetArray[sc - 1].StartIndex;

			ModelObj->SubsetArray[sc].StartIndex = ic;

			for (unsigned int i = 0; i < materialNum; i++)
			{
				if (strcmp(str, materialArray[i].Name) == 0)
				{
					ModelObj->SubsetArray[sc].Material.Material = materialArray[i].Material;
					strcpy(ModelObj->SubsetArray[sc].Material.TextureName, materialArray[i].TextureName);
					strcpy(ModelObj->SubsetArray[sc].Material.Name, materialArray[i].Name);

					break;
				}
			}

			sc++;
		}
		else if (strcmp(str, "f") == 0)
		{
			// 面
			in = 0;

			do
			{
				fscanf(file, "%s", str);

				s = strtok(str, "/");
				ModelObj->VertexArray[vc].Position = positionArray[atoi(s) - 1];

				// 先に既定UV
				ModelObj->VertexArray[vc].TexCoord = XMFLOAT2(0.0f, 0.0f);

				if (s[strlen(s) + 1] != '/')
				{
					// テクスチャ座標が存在しない場合もある
					s = strtok(nullptr, "/");
					ModelObj->VertexArray[vc].TexCoord = texcoordArray[atoi(s) - 1];
				}
				s = strtok(nullptr, "/");
				ModelObj->VertexArray[vc].Normal = normalArray[atoi(s) - 1];

				ModelObj->VertexArray[vc].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				ModelObj->IndexArray[ic] = vc;
				ic++;
				vc++;

				in++;
				c = fgetc(file);
			} while (c != '\n' && c != '\r');

			// 四角は三角に分割
			if (in == 4)
			{
				ModelObj->IndexArray[ic] = vc - 4;
				ic++;
				ModelObj->IndexArray[ic] = vc - 2;
				ic++;
			}
		}
	}

	if (sc != 0)
	{
		ModelObj->SubsetArray[sc - 1].IndexNum =
			ic - ModelObj->SubsetArray[sc - 1].StartIndex;
	}
	else
	{
		// usemtl が一度も無い OBJ → 全面を 0 番サブセットで描画
		ModelObj->SubsetArray[0].StartIndex = 0;
		ModelObj->SubsetArray[0].IndexNum = ic;
		ModelObj->SubsetNum = 1;
	}

	fclose(file);

	delete[] positionArray;
	delete[] normalArray;
	delete[] texcoordArray;
	delete[] materialArray;
}

// マテリアル読み込み///////////////////////////////////////////////////////////////////
void ModelRenderer::LoadMaterial(const char *FileName, MODEL_MATERIAL **MaterialArray, unsigned int *MaterialNum)
{

	char dir[MAX_PATH];
	strcpy(dir, FileName);
	PathRemoveFileSpec(dir);

	char str[256];

	FILE *file;
	file = fopen(FileName, "rt");
	assert(file);

	MODEL_MATERIAL *materialArray;
	unsigned int materialNum = 0;

	// 要素数カウント
	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;

		if (strcmp(str, "newmtl") == 0)
		{
			materialNum++;
		}
	}

	// メモリ確保
	materialArray = new MODEL_MATERIAL[materialNum];

	// 要素読込
	int mc = -1;

	fseek(file, 0, SEEK_SET);

	// usemtlが来ない場合に備えて、デフォルトマテリアルを用意
	MODEL_MATERIAL defaultMaterial{};
	strcpy(defaultMaterial.Name, "Default");
	defaultMaterial.Material.Ambient = XMFLOAT4(0, 0, 0, 1);
	defaultMaterial.Material.Diffuse = XMFLOAT4(1, 1, 1, 1);
	defaultMaterial.Material.Specular = XMFLOAT4(1, 1, 1, 1);
	defaultMaterial.Material.Emission = XMFLOAT4(0, 0, 0, 1);
	defaultMaterial.Material.Shininess = 0.0f;
	defaultMaterial.Material.TextureEnable = false;
	defaultMaterial.Texture = nullptr;
	defaultMaterial.TextureName[0] = '\0';

	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;

		if (strcmp(str, "newmtl") == 0)
		{
			// マテリアル名
			mc++;
			fscanf(file, "%s", materialArray[mc].Name);
			strcpy(materialArray[mc].TextureName, "");

			materialArray[mc].Material.Ambient = XMFLOAT4(0, 0, 0, 1);
			materialArray[mc].Material.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1);
			materialArray[mc].Material.Specular = XMFLOAT4(0, 0, 0, 1);
			materialArray[mc].Material.Shininess = 0.0f;
			materialArray[mc].Material.TextureEnable = false;
			materialArray[mc].Texture = nullptr;
		}
		else if (strcmp(str, "Ka") == 0)
		{
			// アンビエント
			fscanf(file, "%f", &materialArray[mc].Material.Ambient.x);
			fscanf(file, "%f", &materialArray[mc].Material.Ambient.y);
			fscanf(file, "%f", &materialArray[mc].Material.Ambient.z);
			materialArray[mc].Material.Ambient.w = 1.0f;
		}
		else if (strcmp(str, "Kd") == 0)
		{
			// ディフューズ
			fscanf(file, "%f", &materialArray[mc].Material.Diffuse.x);
			fscanf(file, "%f", &materialArray[mc].Material.Diffuse.y);
			fscanf(file, "%f", &materialArray[mc].Material.Diffuse.z);
			materialArray[mc].Material.Diffuse.w = 1.0f;
		}
		else if (strcmp(str, "Ks") == 0)
		{
			// スペキュラ
			fscanf(file, "%f", &materialArray[mc].Material.Specular.x);
			fscanf(file, "%f", &materialArray[mc].Material.Specular.y);
			fscanf(file, "%f", &materialArray[mc].Material.Specular.z);
			materialArray[mc].Material.Specular.w = 1.0f;
		}
		else if (strcmp(str, "Ns") == 0)
		{
			// スペキュラ強度
			fscanf(file, "%f", &materialArray[mc].Material.Shininess);
		}
		else if (strcmp(str, "d") == 0)
		{
			// アルファ
			fscanf(file, "%f", &materialArray[mc].Material.Diffuse.w);
		}
		else if (strcmp(str, "map_Kd") == 0)
		{
			// テクスチャ
			fscanf(file, "%s", str);

			char path[256];
			strcpy(path, dir);
			strcat(path, "\\");
			strcat(path, str);

			strcat(materialArray[mc].TextureName, path);
		}
	}

	fclose(file);

	*MaterialArray = materialArray;
	*MaterialNum = materialNum;
}
