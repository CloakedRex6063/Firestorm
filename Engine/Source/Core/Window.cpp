#include "Core/Window.hpp"

#include "Core/Engine.hpp"

namespace
{
    HWND g_hwnd = nullptr;
    auto g_window_size = glm::vec2(0.0f);
}

LRESULT CALLBACK WindowProc(HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        FS::GEngine.RequestQuit();
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        g_window_size = { static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam)) };
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void FS::Window::Init()
{
    HINSTANCE h_instance = GetModuleHandle(nullptr);
    constexpr auto CLASS_NAME = "BasicWin32Window";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = h_instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    g_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Firestorm",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, h_instance, nullptr
    );

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
}

void FS::Window::Update(float)
{
    MSG msg;
    while (PeekMessage(&msg, g_hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void FS::Window::Shutdown()
{
}

glm::vec2 FS::Window::GetWindowSize()
{
    return g_window_size;
}

void* FS::Window::GetHandle()
{
    return g_hwnd;
}
