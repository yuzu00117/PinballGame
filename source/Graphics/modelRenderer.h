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
// ���f���p�f�[�^�\��
// ------------------------------------------------------------------------------

/// �}�e���A�����
/// - MTL(.mtl) ����ǂݍ��񂾐F/�p�����[�^�iMATERIAL�j��ێ�
/// - map_Kd �Ȃǂ̃e�N�X�`���p�X�iTextureName�j��ێ�
/// - Texture �͓ǂݍ��݌�� SRV�i�����ꍇ�� nullptr�j
struct MODEL_MATERIAL
{
    char                      Name[256];        // �}�e���A�����inewmtl�j
    MATERIAL                  Material;          // �F/�W���iKa/Kd/Ks/Ns/d ���j
    char                      TextureName[256];  // �e�N�X�`���t�@�C���p�X�imap_Kd�j
    ID3D11ShaderResourceView* Texture;           // �e�N�X�`��SRV�i�����ꍇ�� nullptr�j
};

/// �`��T�u�Z�b�g
/// - usemtl ���Ƃɕ������ꂽ�`��P��
/// - IndexBuffer �͈̔́iStartIndex, IndexNum�j�� Material �����
struct SUBSET
{
    unsigned int   StartIndex;   // �J�n�C���f�b�N�X
    unsigned int   IndexNum;     // �C���f�b�N�X��
    MODEL_MATERIAL Material;     // �T�u�Z�b�g�̃}�e���A��
};

/// OBJ �ǂݍ��ݗp�iCPU���j�f�[�^
/// - LoadObj() ����������ꎞ�f�[�^
/// - LoadModel() �� GPU �o�b�t�@��������ɉ�������
struct MODEL_OBJ
{
    VERTEX_3D*     VertexArray;  // ���_�z��
    unsigned int   VertexNum;    // ���_��

    unsigned int*  IndexArray;   // �C���f�b�N�X�z��
    unsigned int   IndexNum;     // �C���f�b�N�X��

    SUBSET*        SubsetArray;  // �T�u�Z�b�g�z��
    unsigned int   SubsetNum;    // �T�u�Z�b�g��
};

/// �`��p�iGPU���j���f��
/// - Vertex/Index �o�b�t�@�ƃT�u�Z�b�g����ێ�
/// - ModelRenderer �̃��f���v�[���ŋ��L�����
struct MODEL
{
    ID3D11Buffer*  VertexBuffer; // ���_�o�b�t�@
    ID3D11Buffer*  IndexBuffer;  // �C���f�b�N�X�o�b�t�@

    SUBSET*        SubsetArray;  // �T�u�Z�b�g�z��
    unsigned int   SubsetNum;    // �T�u�Z�b�g��
};

/// 3D���f����`�悷��R���|�[�l���g
/// - OBJ/MTL ��ǂݍ��݁A�T�u�Z�b�g�P�ʂŃ}�e���A��/�e�N�X�`����ݒ肵�ĕ`�悷��
/// - ���f���͐ÓI�v�[���im_ModelPool�j�ŋ��L���A����t�@�C����1�񂾂����[�h����
/// - �V�F�[�_�[�����ݒ�̏ꍇ�̓f�t�H���g�iBaseLitVS/PS�j��g�p����
class ModelRenderer : public Component
{
public:
    /// �f�t�H���g�R���X�g���N�^
    ModelRenderer() = default;

    /// �f�X�g���N�^
    /// - Uninit() ��Ăяo���ăV�F�[�_�[����������
    ~ModelRenderer() override { Uninit(); }

    // ----------------------------------------------------------------------
    // ���C�t�T�C�N�����\�b�h
    // ----------------------------------------------------------------------
    /// ������
    /// - Owner �� Transform ��Q�ƂƂ��ĕێ�
    /// - �V�F�[�_�[���ݒ�̏ꍇ�̓f�t�H���g�V�F�[�_�[����[�h
    void Init() override;

    /// �I��
    /// - �V�F�[�_�[/���̓��C�A�E�g����
    /// - ���f�����̂̓v�[���Ǘ��̂��߂����ł͉�����Ȃ�
    void Uninit() override;

    // ----------------------------------------------------------------------
    // �ÓI����i���f���v�[���j
    // ----------------------------------------------------------------------
    /// ���f������O�ǂݍ��݂���
    /// - ���Ƀv�[���ɑ��݂���ꍇ�͉�����Ȃ�
    static void Preload(const char* FileName);

    /// ���f���v�[����S�������
    /// - Vertex/IndexBuffer
    /// - SubsetArray
    /// - �T�u�Z�b�g�� Texture(SRV)
    static void UnloadAll();

    // ----------------------------------------------------------------------
    // �C���X�^���X����
    // ----------------------------------------------------------------------
    using Component::Component;

    /// ���f����ǂݍ��ށi�v�[���Q�Ƃ�擾�j
    /// - ���Ƀv�[���ɑ��݂���ꍇ�͎Q�Ƃ�擾���邾��
    /// - ���݂��Ȃ��ꍇ�̓��[�h���ăv�[���֓o�^
    void Load(const char* FileName);

    /// ���f���p�V�F�[�_�[��ݒ肷��
    /// - ���_�V�F�[�_�[/���̓��C�A�E�g/�s�N�Z���V�F�[�_�[��쐬���ĕێ�
    void LoadShader(const char* vsFilePath, const char* psFilePath);

    /// ���[�J���X�P�[����ݒ肷��iTransform�̃X�P�[���Ƃ͕ʘg�j
    void SetLocalScale(float x, float y, float z) { m_LocalScale = { x, y, z }; }

    /// ���f���`��
    /// - WorldMatrix �ݒ� �� VB/IB �ݒ� �� �T�u�Z�b�g���Ƃ� Material/Texture ��ݒ肵�ĕ`��
    void Draw() override;

private:
    // ----------------------------------------------------------------------
    // ���f���ǂݍ��ݓ������
    // ----------------------------------------------------------------------
    /// ���f������[�h���� GPU �o�b�t�@�𐶐�����
    static void LoadModel(const char* FileName, MODEL* Model);

    /// OBJ�t�@�C����ǂݍ��ށiCPU���f�[�^�쐬�j
    static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj);

    /// MTL�t�@�C����ǂݍ��ށi�}�e���A���z��쐬�j
    static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

private:
    // ----------------------------------------------------------------------
    // �f�t�H���g�̃V�F�[�_�[�p�X
    // ----------------------------------------------------------------------
    static constexpr const char* kDefaultVSPath = "shader\\bin\\BaseLitVS.cso"; // �f�t�H���g���_�V�F�[�_�[
    static constexpr const char* kDefaultPSPath = "shader\\bin\\BaseLitPS.cso"; // �f�t�H���g�s�N�Z���V�F�[�_�[

private:
    // ----------------------------------------------------------------------
    // �Q��/���
    // ----------------------------------------------------------------------
    Transform* m_Transform = nullptr;                         // �񏊗L�FOwner �� Transform �Q��
    Vector3    m_LocalScale = { 1.0f, 1.0f, 1.0f };          // ���[�J���X�P�[��

    // ----------------------------------------------------------------------
    // ���f���v�[��
    // ----------------------------------------------------------------------
    static std::unordered_map<std::string, MODEL*> m_ModelPool; // ���L�F���[�h�ς݃��f���̋��L�v�[��
    MODEL* m_Model = nullptr;                                   // �񏊗L�F���ݎQ�Ƃ��Ă��郂�f��

    // ----------------------------------------------------------------------
    // ���f���p�V�F�[�_�[
    // ----------------------------------------------------------------------
    ID3D11VertexShader* m_VertexShader = nullptr;  // ���_�V�F�[�_�[
    ID3D11PixelShader*  m_PixelShader  = nullptr;  // �s�N�Z���V�F�[�_�[
    ID3D11InputLayout*  m_VertexLayout = nullptr;  // ���̓��C�A�E�g
};
