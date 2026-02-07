#include <windows.h>
#include <exdisp.h>
#include <mshtml.h>
#include <ole2.h>
#include <atlbase.h>
#include <atlwin.h>
#include <atlhost.h>

// The simplest way to show a browser in native Win32 without huge SDKs (like WebView2)
// is using the legacy WebBrowser (IE) ActiveX control. 
// Note: While IE is retired, the engine (MSHTML) remains in Windows for compatibility.

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static CComPtr<IWebBrowser2> pWebBrowser;
    static HWND hwndChild = nullptr;

    switch (uMsg) {
        case WM_CREATE: {
            OleInitialize(NULL);
            AtlAxWinInit();

            hwndChild = CreateWindowExW(
                0,
                L"AtlAxWin",
                L"about:blank",
                WS_CHILD | WS_VISIBLE,
                0,
                0,
                0,
                0,
                hwnd,
                NULL,
                reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance,
                NULL);

            if (hwndChild) {
                CComPtr<IUnknown> pUnk;
                if (SUCCEEDED(AtlAxGetControl(hwndChild, &pUnk)) && pUnk) {
                    pUnk->QueryInterface(IID_PPV_ARGS(&pWebBrowser));
                }

                if (pWebBrowser) {
                    CComVariant empty;
                    pWebBrowser->Navigate(
                        CComBSTR(L"https://example.com"),
                        &empty,
                        &empty,
                        &empty,
                        &empty);
                }
            }

            return 0;
        }
        case WM_SIZE: {
            if (hwndChild) {
                MoveWindow(hwndChild, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            }
            return 0;
        }
        case WM_DESTROY:
            pWebBrowser.Release();
            OleUninitialize();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"StreamProofBrowserClass";
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    // Create the window
    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST,
        CLASS_NAME,
        L"Stream Proof Browser",
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

    // The embedded WebBrowser control renders inside this "secure container" window.
    
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
