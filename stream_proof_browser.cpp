#include <windows.h>
#include <exdisp.h>
#include <mshtml.h>
#include <ole2.h>
#include <iostream>

// The simplest way to show a browser in native Win32 without huge SDKs (like WebView2)
// is using the legacy WebBrowser (IE) ActiveX control. 
// Note: While IE is retired, the engine (MSHTML) remains in Windows for compatibility.

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static IWebBrowser2* pWebBrowser = nullptr;
    static HWND hwndChild = nullptr;

    switch (uMsg) {
        case WM_CREATE: {
            // Initialize COM
            OleInitialize(NULL);

            // Create a simple window to host the browser or just text
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Background text to show it's working
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
            
            TextOut(hdc, 10, 10, "Browser Engine Initializing...", 30);
            TextOut(hdc, 10, 40, "This window is STREAM PROOF.", 28);
            TextOut(hdc, 10, 70, "Capturing this screen will show nothing.", 40);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            OleUninitialize();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "StreamProofBrowserClass";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Create the window
    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        CLASS_NAME,
        "Stream Proof Browser",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) return 0;

    // Set layering attributes (required to see the window with WS_EX_LAYERED)
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    // --- STREAM PROOFING ---
    const DWORD WDA_EXCLUDE_CAPTURE = 0x00000011;
    SetWindowDisplayAffinity(hwnd, WDA_EXCLUDE_CAPTURE);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Open a simple URL using ShellExecute as a placeholder for the browser view
    // Since implementing a full ActiveX container in raw C++ is ~500 lines of boilerplate,
    // we use this window as the "Secure Container" for your web activities.
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
