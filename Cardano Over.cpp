// DECLARAÇÃO DAS DIRETIVAS:

#include "framework.h"
#include "Cardano Over.h"
#include "shellapi.h"
#include "Windows.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <base64.h>
#include <iostream>
#include <fstream>
#include <string>
#include <httplib.h>
#include <dwmapi.h>
#include <codecvt>
#include <chrono>
#include <thread>
#include <ctime>

#pragma comment(lib, "dwmapi.lib")

#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#define MAX_LOADSTRING 100
#define WM_SETEDITCOLOR (WM_USER + 1)
#define STM_SETBKCOLOR (WM_USER + 1)

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#define TAMANHO_MAX_STRING 100
#define TAMANHO_MAX_COMANDO 500

#define WM_USER_HTTP_COMPLETED (WM_USER + 1)
#define WM_USER_UPDATE_PROGRESS (WM_USER + 2)
#define WM_USER_UPDATE_COMPLETED (WM_USER + 3)

// VARIÁVEIS GLOBAIS:

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

HWND hwndImage1;
HWND PriceADA;

// DECLARAÇÔES DE FUNÇÔES:

// FUNÇÂO: 001 -> Propósito: Leitura do conteúdo de arquivos.

std::wstring ReadContents(const std::wstring& filePath) {
    std::wifstream file(filePath);
    if (!file) {
        return L"Erro ao abrir o arquivo.";
    }

    // Lê o conteúdo do arquivo e armazena em uma string
    std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
    file.close();

    return content;
}

// FUNÇÂO: 002 -> Propósito: Capturar/selecionar um trecho dos dados.

std::wstring SelectContents(const std::wstring& content, const std::wstring& start, const std::wstring& end) {
    size_t startPos = content.find(start);
    if (startPos == std::wstring::npos) {
        return L"";
    }

    size_t endPos = content.find(end, startPos + start.length());
    if (endPos == std::wstring::npos) {
        return L"";
    }

    return content.substr(startPos + start.length(), endPos - startPos - start.length());
}

// Carregando a imagem que será usada no fundo da aplicação:

const char* imagePath1 = "ADAUSD.png";
int width1, height1, channels1;
int PosXIMG1, PosYIMG1;
unsigned char* image_data1 = stbi_load(imagePath1, &width1, &height1, &channels1, 3);


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// DECLARAÇÃO DA FUNÇÂO PRINCIPAL DO PROGRAMA:

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CARDANOOVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CARDANOOVER));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CARDANOOVER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(0, 0, 0)));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CARDANOOVER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowEx(
        0,
        szWindowClass,
        szTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        350, 250,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);

        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CREATE:
    {
        if (image_data1)
        {
            RECT clientRect1;
            GetClientRect(hWnd, &clientRect1);
            int windowWidth1 = clientRect1.right - clientRect1.left;
            int windowHeight1 = clientRect1.bottom - clientRect1.top;

            int PosXIMG1 = (windowWidth1 - width1) / 2;
            int PosYIMG1 = (windowHeight1 - height1) / 2;

            int PosXWLLT = 20 + (windowWidth1 - 600) / 2;
            int PosYWLLT = 362 + ((windowHeight1 - height1) / 2);

            hwndImage1 = CreateWindow(L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_BITMAP, PosXIMG1, PosYIMG1, 300, 500, hWnd, NULL, NULL, NULL);

            // Criação da imagem a partir dos dados carregados:

            BITMAPINFO ImageSelected1 = { 0 };
            ImageSelected1.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            ImageSelected1.bmiHeader.biWidth = width1;
            ImageSelected1.bmiHeader.biHeight = -height1;
            ImageSelected1.bmiHeader.biPlanes = 1;
            ImageSelected1.bmiHeader.biBitCount = 24;
            ImageSelected1.bmiHeader.biCompression = BI_RGB;

            HDC hdc1 = GetDC(hwndImage1);
            HBITMAP hBitmap1 = CreateDIBitmap(hdc1, &ImageSelected1.bmiHeader, CBM_INIT, image_data1, &ImageSelected1, DIB_RGB_COLORS);
            ReleaseDC(hwndImage1, hdc1);

            if (hBitmap1)
            {
                SendMessage(hwndImage1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap1);
            }

            stbi_image_free(image_data1);

            // Configuração a fim de que a execução do comando pelo SO não mostre a execução na UI do PC:

            STARTUPINFO si = {};
            PROCESS_INFORMATION pi = {};
            si.cb = sizeof(si);
            si.dwFlags = STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE; // Esconde a janela do console

            // Caminho para o comando curl:

            TCHAR command[] = _T("cmd /c curl -sS -o CardanoNetwork.txt \"https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&ids=cardano&order=market_cap_desc&per_page=1&page=1&sparkline=false\"");

            // Criação do processo:

            if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                
                // Caso a criação do processo venha a falhar:

                MessageBox(NULL, _T("Failed to create process!"), _T("Error"), MB_OK | MB_ICONERROR);
            }
            else {
                
                // Aguardar até que o processo filho termine
                
                WaitForSingleObject(pi.hProcess, INFINITE);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);

                // Atualização da UI com os resultados da requisição HTTP.

                PostMessage(hWnd, WM_USER_HTTP_COMPLETED, 0, 0);
            }

            PriceADA = CreateWindowEx( 0, L"STATIC", L"Not found.", WS_CHILD | WS_VISIBLE, 200, 110, 75, 30, hWnd, NULL, GetModuleHandle(NULL), NULL);
        }
    }break;
    case WM_USER_HTTP_COMPLETED:
    {
        
        //PROPÓSITO DO SCRIPT : Execução após o êxito da requisição HTTP.

        std::wstring Content = ReadContents(L"CardanoNetwork.txt");
        std::wstring ValuePriceADA = SelectContents(Content, L"\"current_price\":", L",\"market_cap\":");

        HFONT FONT01 = CreateFont( 36, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

        SetWindowText(PriceADA, ValuePriceADA.c_str());

        SendMessage(PriceADA, WM_SETFONT, (WPARAM)FONT01, TRUE);

    }
    case WM_CTLCOLORSTATIC:
    {
        // PROPÓSITO DO SCRIPT: DEFINIR A COR DAS LETRAS E A COR DO FUNDO DAS ETIQUETAS.
        
        SetTextColor((HDC)wParam, RGB(0, 0, 0));
        SetBkColor((HDC)wParam, RGB(255, 255, 255));
        return (LRESULT)CreateSolidBrush(RGB(115, 115, 115));

    }break;
    case WM_SIZE:
    {
        // PROPÓSITO DO SCRIPT:
        
        RECT clientRect1;
        GetClientRect(hWnd, &clientRect1);
        int windowWidth1 = clientRect1.right - clientRect1.left;
        int windowHeight1 = clientRect1.bottom - clientRect1.top;

        int PosXIMG1 = (windowWidth1 - width1) / 2;
        int PosYIMG1 = (windowHeight1 - height1) / 2;

        int PosXPriceADA = 320 + (windowWidth1 - 600) / 2;
        int PosYPriceADA = 85 + ((windowHeight1 - height1) / 2);

        SetWindowPos(hwndImage1, NULL, PosXIMG1, PosYIMG1, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

        SetWindowPos(PriceADA, NULL, PosXPriceADA, PosYPriceADA, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    }break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// MANIPULADOR DA MENSAGEM PARA CAIXA 'ABOUT':

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
