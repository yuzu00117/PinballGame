#include "Camera.h"
#include "main.h"
#include "renderer.h"
#include "GameManager.h"
#include "Ball.h"
#include "Input.h"

// �V�[����̃{�[����T���w���p�[�֐�
namespace
{
    Ball* FindBall()
    {
        for (auto obj : GameManager::GetGameObjects())
        {
            if (auto ball = dynamic_cast<Ball*>(obj))
            {
                return ball;
            }
        }
        return nullptr;
    }
}

// ����������
void Camera::Init()
{
    // �V�[����ɂ��� Ball ��T���āA�����^�[�Q�b�g��ݒ�
    Ball* ball = FindBall();
    if (!ball) return;

    // Ball �̈ʒu�𒍎��_��
    auto p = ball->GetPosition();
    m_Target = XMFLOAT3(p.x, p.y, p.z);

    // �΂ߏ�Œ�J�����p�̏����ʒu
    const XMFLOAT3 offset = m_CameraOffset;
    m_Transform.Position = {
        p.x + offset.x,
        p.y + offset.y,
        p.z + offset.z
    };

    // Transform����ʒu��擾���ăL���b�V��
    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
}

// �I������
void Camera::Uninit()
{
    // �O�̂��߃J�[�\�������ƕ\������ɖ߂�
    ClipCursor(nullptr);
    ShowCursor(TRUE);
}

// �X�V����
void Camera::Update(float deltaTime)
{
#if defined(_DEBUG)
    // --------------------------------------------------------------
    // �f�o�b�O�J�������[�h�ؑ֏���
    // --------------------------------------------------------------
    // F2�L�[�Ńf�o�b�O�J�������[�h�ؑ�
    if (Input::GetKeyTrigger(VK_F2))
    {
        m_DebugCameraMode = !m_DebugCameraMode;
        m_FirstMouse = true; // ���[�h�؂�ւ����Ƀ}�E�X������

        if (m_DebugCameraMode)
        {
            // ----- �f�o�b�O�J���� ON�F�J�[�\����\�����E�B���h�E��ɌŒ� -----
            ShowCursor(FALSE);

            // �E�B���h�E�̃N���C�A���g�̈��X�N���[�����W�ɕϊ����� ClipCursor
            HWND hWnd = GetActiveWindow(); // �K�v�Ȃ玩�O�� HWND �ɍ����ւ� OK
            if (hWnd)
            {
                RECT rect;
                GetClientRect(hWnd, &rect); // �N���C�A���g���W
                POINT lt{ rect.left,  rect.top    };
                POINT rb{ rect.right, rect.bottom };

                ClientToScreen(hWnd, &lt); // �����X�N���[�����W��
                ClientToScreen(hWnd, &rb); // �E����X�N���[�����W��

                rect.left   = lt.x;
                rect.top    = lt.y;
                rect.right  = rb.x;
                rect.bottom = rb.y;

                ClipCursor(&rect); // ���͈̔͂���o���Ȃ�
            }
        }
        else
        {
            // ----- �f�o�b�O�J���� OFF�F�J�[�\���\�����N���b�v��� -----
            ClipCursor(nullptr); // �������
            ShowCursor(TRUE);
        }
    }

    // �f�o�b�O�J������OFF�̂Ƃ��͒ʏ�̃J���������������ďI��
    if (!m_DebugCameraMode)
    {
        // �V�[����ɂ��� Ball ��T��
        Ball* ball = FindBall();
        if (!ball)
            return;

        Vector3 ballPos = ball->GetPosition();

        // �J�����̃^�[�Q�b�g��{�[���̈ʒu�ɍX�V
        m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

        // �J�����̌Œ�I�t�Z�b�g
        const XMFLOAT3 offset = m_CameraOffset;

        // �J�����ʒu��X�V
        m_Transform.Position = {
            ballPos.x + offset.x,
            ballPos.y + offset.y,
            ballPos.z + offset.z
        };

        // Transform����ʒu��擾���ăL���b�V��
        const XMMATRIX world = m_Transform.GetWorldMatrix();
        XMStoreFloat3(&m_Position, world.r[3]);

        return;    // ���f�o�b�O�J����OFF�Ȃ炱���ŏI���
    }

    // �f�o�b�O�J���� ON ���̏���
    HWND hWnd = GetActiveWindow(); // �K�v�Ȃ�O���� HWND ��g��
    POINT curPos{};
    POINT centerClient{};
    POINT centerScreen{};

    if (hWnd)
    {
        // �N���C�A���g�����̃X�N���[�����W��v�Z
        RECT rect;
        GetClientRect(hWnd, &rect);

        centerClient.x = (rect.right - rect.left) / 2;
        centerClient.y = (rect.bottom - rect.top) / 2;

        centerScreen = centerClient;
        ClientToScreen(hWnd, &centerScreen);

        // ���݂̃J�[�\���ʒu�擾
        GetCursorPos(&curPos);

        // ��������̍������]�ʂƂ��Ďg��
        float dx = float(curPos.x - centerScreen.x) * m_MouseSensitivity;
        float dy = float(curPos.y - centerScreen.y) * m_MouseSensitivity;

        // ���t���[���A�J�[�\���𒆉��ɖ߂��i�����Œ�j
        SetCursorPos(centerScreen.x, centerScreen.y);

        // Yaw: ���E��]
        m_Yaw += dx;

        // Pitch: �㉺��]�i���]�ρj
        m_Pitch -= dy;

        // �s�b�`�p�̐���
        const float MAX_PITCH = XM_PIDIV2 - 0.1f;
        const float MIN_PITCH = -XM_PIDIV2 + 0.1f;
        if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
        if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
    }

    // 2. �J�����̌����x�N�g����v�Z
    XMVECTOR forward = XMVectorSet(
        cosf(m_Pitch) * sinf(m_Yaw),
        sinf(m_Pitch),
        cosf(m_Pitch) * cosf(m_Yaw),
        0.0f
    );
    forward = XMVector3Normalize(forward);

    XMVECTOR right = XMVector3Normalize(
        XMVectorSet(
            sinf(m_Yaw - XM_PIDIV2),
            0.0f,
            cosf(m_Yaw - XM_PIDIV2),
            0.0f
        )
    );

    // 3. �L�[���͂ɂ��ړ��i������j
    XMVECTOR move = XMVectorZero();

    if (Input::GetKeyPress(VK_UP))    move += forward * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_DOWN))  move -= forward * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_RIGHT)) move -= right   * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_LEFT))  move += right   * m_DebugCameraSpeed;

    if (Input::GetKeyPress(VK_PRIOR)) // PageUp
        move += XMVectorSet(0, 1, 0, 0) * m_DebugCameraSpeed;
    if (Input::GetKeyPress(VK_NEXT))  // PageDown
        move -= XMVectorSet(0, 1, 0, 0) * m_DebugCameraSpeed;

    XMFLOAT3 d;
    XMStoreFloat3(&d, move);

    m_Transform.Position.x += d.x;
    m_Transform.Position.y += d.y;
    m_Transform.Position.z += d.z;

    // forward �����փ^�[�Q�b�g��ݒ�
    m_Target.x = m_Transform.Position.x + XMVectorGetX(forward);
    m_Target.y = m_Transform.Position.y + XMVectorGetY(forward);
    m_Target.z = m_Transform.Position.z + XMVectorGetZ(forward);

#else
    // -----------------------------------------------
    // Release �r���h��p�F�ʏ�J�����̂�
    // -----------------------------------------------
    // �f�o�b�O�J�����͎g��Ȃ�
    m_DebugCameraMode = false;

    // �V�[����ɂ��� Ball ��T��
    Ball* ball = FindBall();
    if (!ball)
        return;

    Vector3 ballPos = ball->GetPosition();

    // �J�����̃^�[�Q�b�g��{�[���̈ʒu�ɍX�V
    m_Target = XMFLOAT3(ballPos.x, ballPos.y, ballPos.z);

    // �J�����̌Œ�I�t�Z�b�g
    const XMFLOAT3 offset = m_CameraOffset;

    // �J�����ʒu��X�V
    m_Transform.Position = {
        ballPos.x + offset.x,
        ballPos.y + offset.y,
        ballPos.z + offset.z
    };

    // Transform����ʒu��擾���ăL���b�V��
    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);
#endif
}



// �`�揈��
void Camera::Draw()
{
    // �v���W�F�N�V�����}�g���N�X
    m_Projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        (float)SCREEN_WIDTH / SCREEN_HEIGHT,
        1.0f,
        1000.0f
    );

    Renderer::SetProjectionMatrix(m_Projection);

    // �O�̈�Transform����ʒu��Ď擾
    const XMMATRIX world = m_Transform.GetWorldMatrix();
    XMStoreFloat3(&m_Position, world.r[3]);

    // �r���[�E�}�g���N�X
    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    m_View = XMMatrixLookAtLH(
        XMLoadFloat3(&m_Position),
        XMLoadFloat3(&m_Target),
        XMLoadFloat3(&up)
    );

    Renderer::SetViewMatrix(m_View);
}

// �J�����̃s�b�`��]�𐧌�
void Camera::RotatePitch(float angle)
{
    m_Pitch += angle;
    
    // �p�̐����i�J�������n�ʂɐ��荞�܂Ȃ��悤�����j
    const float MAX_PITCH = 1.3f;  // ��75�x
    const float MIN_PITCH = -0.3f; // ��-17�x
    
    if (m_Pitch > MAX_PITCH) m_Pitch = MAX_PITCH;
    if (m_Pitch < MIN_PITCH) m_Pitch = MIN_PITCH;
}

// �J�����̋�����ݒ�
void Camera::SetDistance(float distance)
{
    m_Distance = distance;
    
    // �����̐���
    const float MIN_DISTANCE = 2.0f;
    const float MAX_DISTANCE = 10.0f;
    
    if (m_Distance < MIN_DISTANCE) m_Distance = MIN_DISTANCE;
    if (m_Distance > MAX_DISTANCE) m_Distance = MAX_DISTANCE;
}