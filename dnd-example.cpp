// dnd-example.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "dnd-example.h"
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

class MyDropTarget : public IDropTarget {
public:
    MyDropTarget() : m_cRefCount(1) {}
    ~MyDropTarget() {}

    // IUnknown methods
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppvObject)
    {
        if (iid == IID_IUnknown || iid == IID_IDropTarget) {
            *ppvObject = this;
            AddRef();
            return S_OK;

        }
        else {
            *ppvObject = NULL;
            return E_NOINTERFACE;

        }

    }
    ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_cRefCount);

    }
    ULONG STDMETHODCALLTYPE Release()
    {
        ULONG cRef = InterlockedDecrement(&m_cRefCount);
        if (cRef == 0)
            delete this;
        return cRef;

    }

    // IDropTarget methods
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        // Check if the data object contains files
        std::cout << "DragEnter\n";
        FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg;
        if (SUCCEEDED(pDataObj->GetData(&fmt, &stg))) {
            // The data object contains files, so allow the drop
            *pdwEffect = DROPEFFECT_COPY;
            ReleaseStgMedium(&stg);
            return S_OK;

        }
        else {
            // The data object does not contain files, so disallow the drop
            *pdwEffect = DROPEFFECT_NONE;
            return DRAGDROP_S_CANCEL;

        }

    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        // Allow the drop
        *pdwEffect = DROPEFFECT_COPY;
        return S_OK;

    }
    HRESULT STDMETHODCALLTYPE DragLeave()
    {
        return S_OK;

    }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
    {
        // Process the dropped files
        FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg;
        if (SUCCEEDED(pDataObj->GetData(&fmt, &stg))) {
            HDROP hDrop = (HDROP)stg.hGlobal;

            // Get the number of files dropped
            UINT nFiles = DragQueryFile(hDrop, (UINT)-1, NULL, 0);

            // Process each dropped file
            for (UINT i = 0; i < nFiles; i++) {
                // Get the file name
                TCHAR szFileName[MAX_PATH];
                DragQueryFile(hDrop, i, szFileName, MAX_PATH);

                // Print the file
				std::wcout << "Dropped file: " << szFileName << "\n";

            }

            ReleaseStgMedium(&stg);

        }

        // Signal that the drop has been processed
        *pdwEffect = DROPEFFECT_COPY;
        return S_OK;

    }

private:
    LONG m_cRefCount;

};



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

   FILE* fp;

   // Create  a console so we can hook into STDOUT/ERR
   AllocConsole();
   freopen_s(&fp, "CONIN$", "r", stdin);
   freopen_s(&fp, "CONOUT$", "w", stdout);
   freopen_s(&fp, "CONOUT$", "w", stderr);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DNDEXAMPLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DNDEXAMPLE));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DNDEXAMPLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   OleInitialize(NULL);
   MyDropTarget* pDropTarget = new MyDropTarget();
   RegisterDragDrop(hWnd, pDropTarget);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
} 
