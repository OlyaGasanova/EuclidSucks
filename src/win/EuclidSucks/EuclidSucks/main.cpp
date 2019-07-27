#include "game.h"

float osDeltaTime;
bool  osClose;
HDC   osDC;

int osGetTimeMS() {
    LARGE_INTEGER Freq, Count;
    QueryPerformanceFrequency(&Freq);
    QueryPerformanceCounter(&Count);
    return int(Count.QuadPart * 1000L / Freq.QuadPart);
}

HGLRC initGL(HWND hWnd) {
    osDC = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
	
    int format = ChoosePixelFormat(osDC, &pfd);
    SetPixelFormat(osDC, format, &pfd);
    HGLRC hRC = wglCreateContext(osDC);
    wglMakeCurrent(osDC, hRC);
    return hRC;
}

void freeGL(HWND hWnd, HGLRC hRC) {
    ReleaseDC(hWnd, osDC);
    wglMakeCurrent(0, 0);
    wglDeleteContext(hRC);
}

InputKey remapKey(WPARAM code) {
    uint32 codes[KEY_MAX] = { 
        VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_CONTROL, VK_MENU, VK_SHIFT, VK_SPACE, VK_RETURN, VK_TAB, VK_ESCAPE,
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    };

    for (int i = 0; i < KEY_MAX; i++) {
        if (codes[i] == code) {
            return InputKey(i);
        }
    }

    return KEY_MAX;
}

InputKey remapMouse(UINT msg) {
    return (msg >= WM_LBUTTONDOWN && msg <= WM_LBUTTONDBLCLK) ? MOUSE_L :
           (msg >= WM_RBUTTONDOWN && msg <= WM_RBUTTONDBLCLK) ? MOUSE_R : MOUSE_M;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        // window
        case WM_SIZE :
            ctx->resize(LOWORD(lParam), HIWORD(lParam));
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN :
        case WM_KEYUP   :
            Input::setDown(remapKey(wParam), msg != WM_KEYUP);
            break;
        case WM_LBUTTONDOWN   :
        case WM_LBUTTONUP     :
        case WM_LBUTTONDBLCLK :
        case WM_RBUTTONDOWN   :
        case WM_RBUTTONUP     :
        case WM_RBUTTONDBLCLK :
        case WM_MBUTTONDOWN   :
        case WM_MBUTTONUP     :
        case WM_MBUTTONDBLCLK : {
            InputKey key = remapMouse(msg);
            bool down = msg != WM_LBUTTONUP && msg != WM_RBUTTONUP && msg != WM_MBUTTONUP;
            Input::setPos(key, vec3((float)(short)LOWORD(lParam), (float)(short)HIWORD(lParam), 0.0f));
            Input::setDown(key, down);
            if (down) {
                SetCapture(hWnd);
            } else {
                ReleaseCapture();
            }
            break;
        }
        case WM_MOUSEMOVE :
            Input::setPos(MOUSE_L, vec3((float)(short)LOWORD(lParam), (float)(short)HIWORD(lParam), 0.0f));
            break;
        default :
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

#ifdef _CONSOLE
int main(int argc, char** argv)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    RECT r = { 0, 0, 854, 480 };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, false);

    int ox = (GetSystemMetrics(SM_CXSCREEN) - (r.right - r.left)) / 2;
    int oy = (GetSystemMetrics(SM_CYSCREEN) - (r.bottom - r.top)) / 2;
    r.left   += ox;
    r.top    += oy;
    r.right  += ox;
    r.bottom += oy;

    HWND hWnd = CreateWindow("static", "EuclidSucks", WS_OVERLAPPEDWINDOW, r.left, r.top, r.right - r.left, r.bottom - r.top, 0, 0, 0, 0);

    HGLRC hRC = initGL(hWnd);

    Game::init();

    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);
    ShowWindow(hWnd, SW_SHOWDEFAULT);

    MSG msg;

    while (!osClose) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) {
                osClose = true;
            }
        } else {
            if (Game::update()) {
                Game::render();
            }
        }
    };

    Game::free();

    freeGL(hWnd, hRC);

    return 0;
}
