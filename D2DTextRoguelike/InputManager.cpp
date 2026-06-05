#include "pch.h"
#include "InputManager.h"


// 로그라이크는 rawinput에 대한 입력처리는 필요없기 때문에 rawinput 관련 코드 제거
bool InputManager::Initialize(HWND hwnd)
{
    return true;
}

// 매 프레임 씬 업데이트 후 호출되어 단발성 입력 상태를 초기화
void InputManager::Update()
{
    // 마우스의 이전 프레임 상태 갱신
    m_PrevMouse = m_CurMouse;

    // 키보드의 단발성 입력 상태 초기화
    for (auto& edge : m_keyEdge)
    {
        edge.pressed = false;
        edge.released = false;
    }
}

bool InputManager::IsKeyDown(UINT vk) const
{
    if (vk >= 256) return false;
    return m_keyDown[vk];
}

bool InputManager::IsKeyPressed(UINT vk) const
{
    if (vk >= 256) return false;
    return m_keyEdge[vk].pressed;
}

bool InputManager::IsKeyReleased(UINT vk) const
{
    if (vk >= 256) return false;
    return m_keyEdge[vk].released;
}

POINT InputManager::GetMousePos() const
{
    return m_CurMouse.pos;
}

bool InputManager::IsMouseLeftDown() const
{
    return m_CurMouse.leftPressed;
}

bool InputManager::IsMouseLeftClicked() const
{
    // 현재 눌려있고, 이전 프레임에는 안 눌려있었다면 막 클릭된 상태
    return m_CurMouse.leftPressed && !m_PrevMouse.leftPressed;
}

bool InputManager::IsMouseLeftReleased() const
{
    return !m_CurMouse.leftPressed && m_PrevMouse.leftPressed;
}

bool InputManager::OnHandleMessage(const MSG& msg)
{
    switch (msg.message)
    {
    case WM_KEYDOWN:
    {
        HandleMsgKeyDown(msg.wParam, msg.lParam);
    }
    break;

    case WM_KEYUP:
    {
        HandleMsgKeyUp(msg.wParam, msg.lParam);
    }
    break;

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    {
        HandleMsgMouse(msg);
    }
    break;

    default:
        return false; // Unhandled message
    }

    return true;
}

void InputManager::HandleMsgKeyDown(WPARAM wParam, LPARAM lParam)
{
    assert(wParam < 256);

    bool wasDown = (lParam & (1 << 30)) != 0;
    m_keyDown[wParam] = true;

    if (!wasDown)
    {
        m_keyEdge[wParam].pressed = true;   // 처음 눌렸을 때만 true
    }

    m_keyEdge[wParam].released = false; // 눌린 상태이므로 released = false

#ifdef _VERBOSE_KEYBOARD
    char buf[128];
    std::snprintf(buf, sizeof(buf),
        "KEY DOWN  VK=0x%02X  (wasDown=%d)\n",
        static_cast<unsigned int>(wParam), wasDown);

    std::cout << buf;
#endif
}

void InputManager::HandleMsgKeyUp(WPARAM wParam, LPARAM lParam)
{
    assert(wParam < 256);

    m_keyDown[wParam] = false; // 현재 키가 눌리지 않음을 기록
    m_keyEdge[wParam].released = true; // 키가 떼어졌음을 기록

#ifdef _VERBOSE_KEYBOARD
    char buf[64];
    std::snprintf(buf, sizeof(buf),
        "KEY UP    VK=0x%02X\n",
        static_cast<unsigned int>(wParam));

    std::cout << buf;
#endif
}

void InputManager::HandleMsgMouse(const MSG& msg)
{
    int x = GetXFromLParam(msg.lParam);
    int y = GetYFromLParam(msg.lParam); // [오류 수정]

    m_CurMouse.pos = { x, y };

    if (msg.message == WM_LBUTTONDOWN)
    {
        m_CurMouse.leftPressed = true;
        SetCapture(msg.hwnd);
    }
    else if (msg.message == WM_RBUTTONDOWN)
    {
        m_CurMouse.rightPressed = true;
        SetCapture(msg.hwnd);
    }
    else if (msg.message == WM_LBUTTONUP)
    {
        m_CurMouse.leftPressed = false;
        ReleaseCapture();
    }
    else if (msg.message == WM_RBUTTONUP)
    {
        m_CurMouse.rightPressed = false;
        ReleaseCapture();
    }

#ifdef _VERBOSE_MOUSE
    char buf[128];
    std::snprintf(buf, sizeof(buf),
        "MOUSE (%d,%d)  left=%d right=%d\n",
        x, y, m_CurMouse.leftPressed, m_CurMouse.rightPressed);

    std::cout << buf;
#endif
}
