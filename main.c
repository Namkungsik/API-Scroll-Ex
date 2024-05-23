#include <windows.h>
#include <windef.h>

#define NUMLINES	30

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    static CHAR szAppName[] = "GP";
    HWND         hwnd;
    MSG          msg;
    WNDCLASS     wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    RegisterClass(&wndclass);

    hwnd = CreateWindow(szAppName,                  // 윈도우 클래스 이름
        "스크롤 예제",               // 윈도우 켑션
        WS_OVERLAPPEDWINDOW,        // 윈도우 모양
        CW_USEDEFAULT,              // X 출력위치
        CW_USEDEFAULT,              // Y 출력위치
        300,						  // 윈도우 넓이
        300,						  // 윈도우 높이
        NULL,                       // 부모 윈도우 핸들
        NULL,                       // 윈도우 메뉴 핸들
        hInstance,                  // 인스턴트 핸들
        NULL);                     // 사용되지 않음

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC			hdc;	// DC에 대한 핸들
    PAINTSTRUCT ps;
    char* szText = "스크롤";
    static int	cxChar = 0, cyChar = 0, nVscrollPos = 0, nHscrollPos = 0, cxClient = 0, cyClient = 0;
    static SIZE size;
    char		strBuff[255];
    int			x, y, i;
    TEXTMETRIC  tm;
    SCROLLINFO si;

    switch (message)
    {
    case WM_CREATE:

        hdc = GetDC(hwnd);
        GetTextMetrics(hdc, &tm);
        cxChar = tm.tmAveCharWidth;
        cyChar = tm.tmHeight;
        ReleaseDC(hwnd, hdc);

        SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
        SetScrollPos(hwnd, SB_VERT, nVscrollPos, FALSE);

        SetScrollRange(hwnd, SB_HORZ, 0, NUMLINES - 1, FALSE);
        SetScrollPos(hwnd, SB_HORZ, nHscrollPos, FALSE);

        return 0;

    case WM_SIZE:
    {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        cxClient = clientRect.right - clientRect.left;
        cyClient = clientRect.bottom - clientRect.top;

        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin = 0;
        si.nMax = NUMLINES - 1;
        si.nPage = min(cyClient / cyChar, NUMLINES);

        nVscrollPos = min(nVscrollPos, NUMLINES - cyClient / cyChar);
        if (nVscrollPos < 0)
            nVscrollPos = 0;
        si.nPos = nVscrollPos;

        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        nHscrollPos = min(nHscrollPos, NUMLINES - cxClient / cxChar);
        if (nHscrollPos < 0)
            nHscrollPos = 0;
        si.nMax = NUMLINES - 1;
        si.nPage = min(cxClient / cxChar, NUMLINES);
        si.nPos = 0;

        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

        return 0;
    }

    case WM_VSCROLL:
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            nVscrollPos -= 1;
            break;

        case SB_LINEDOWN:
            nVscrollPos += 1;
            break;

        case SB_PAGEUP:
            nVscrollPos -= cyClient / cyChar;
            break;

        case SB_PAGEDOWN:
            nVscrollPos += cyClient / cyChar;
            break;

        case SB_THUMBPOSITION:
            nVscrollPos = HIWORD(wParam);
            break;

        default:
            break;
        }

        if (nVscrollPos >= 0)
        {
            if (nVscrollPos >= NUMLINES - cyClient / cyChar)
                nVscrollPos = NUMLINES - cyClient / cyChar;
            nVscrollPos = nVscrollPos;
        }
        else
        {
            nVscrollPos = 0;
        }

        if (nVscrollPos == GetScrollPos(hwnd, 0))
        {
            return 0;
        }

        SetScrollPos(hwnd, SB_VERT, nVscrollPos, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);

        return 0;

    case WM_HSCROLL:
    {
        switch (LOWORD(wParam))
        {
        case SB_LINELEFT:
            nHscrollPos -= 1;
            break;

        case SB_LINERIGHT:
            nHscrollPos += 1;
            break;

        case SB_PAGELEFT:
            nHscrollPos -= cyClient / cyChar;
            break;

        case SB_PAGERIGHT:
            nHscrollPos += cyClient / cyChar;
            break;

        case SB_THUMBPOSITION:
            nHscrollPos = HIWORD(wParam);
            break;

        default:
            break;
        }

        if (nHscrollPos >= 0)
        {
            if (nHscrollPos >= (size.cx - cxClient) / cxChar)
                nHscrollPos = (size.cx - cxClient) / cxChar;
            nHscrollPos = nHscrollPos;
        }
        else
        {
            nHscrollPos = 0;
        }

        if (nHscrollPos == GetScrollPos(hwnd, 0))
        {
            return 0;
        }

        SetScrollPos(hwnd, SB_HORZ, nHscrollPos, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);
    }

    case WM_PAINT:
    {
        HDC hdc = GetDC(hwnd);
        SIZE textSize;
        x = 0 - nHscrollPos * cxChar;

        for (i = 0; i < NUMLINES; i++)
        {
            y = cyChar * (i - nVscrollPos);

            wsprintf(strBuff, "Scrollbar %2d, HSPos %2d VSPos %2d, y %3d", i, nHscrollPos, nVscrollPos, y);

            GetTextExtentPoint(hdc, strBuff, strlen(strBuff), &textSize);
            TextOut(hdc, x, y, strBuff, strlen(strBuff));
        }

        ReleaseDC(hwnd, hdc);

        size.cx = textSize.cx + 10;

        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}