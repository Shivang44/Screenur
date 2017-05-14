// Screenur.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "Screenur.h"


#define MAX_LOADSTRING 100
#define APPLICATION_INSTANCE_MUTEX_NAME L"{BA29345E-C29A-4359-A07C-52B64B5571AD}"
#define HOTKEY1 1
#define HOTKEY2 2

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
std::wstring        GetRandomFileName();
INT                 GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
PBITMAPINFO         CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp);
INT                 CreateBMPFile(HWND hwnd, LPCTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);
INT                 bitmapToPNG(std::wstring fileName);
HBITMAP             screenCapture(int x, int y, int w, int h);
INT                 postToImgur(std::string);
size_t              write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
void				handleURLResponse(const char * URL);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Make sure at most one instance of Screenur is running
	HANDLE hMutexOneInstance(::CreateMutex(NULL, TRUE, APPLICATION_INSTANCE_MUTEX_NAME));
	bool bAlreadyRunning((::GetLastError() == ERROR_ALREADY_EXISTS));
	WCHAR alreadyRunningText[MAX_LOADSTRING];
	LoadStringW(hInstance, ERROR_ALREADY_RUNNING, alreadyRunningText, MAX_LOADSTRING);

	if (hMutexOneInstance == NULL || bAlreadyRunning)
	{
		if (hMutexOneInstance)
		{
			::ReleaseMutex(hMutexOneInstance);
			::CloseHandle(hMutexOneInstance);
		}
		MessageBox(NULL, alreadyRunningText, _T("Error"), NULL);
		return FALSE;
	}

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCREENUR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCREENUR));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCREENUR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SCREENUR);
	wcex.lpszMenuName	= NULL;		// No menu
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
   int w = GetSystemMetrics(SM_CXSCREEN);
   int h = GetSystemMetrics(SM_CYSCREEN);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
      0, 0, w, h, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

  // ShowWindow(hWnd, nCmdShow);
  // UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
int startX, startY, endX, endY;
HBITMAP screenshot;
BOOL draw = false;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{


	PAINTSTRUCT ps;
	HDC hdc;
	HDC hdcMem;
	HBITMAP hbmOld;

	int fullW = GetSystemMetrics(SM_CXSCREEN);
	int fullH = GetSystemMetrics(SM_CYSCREEN);
	HBRUSH hBrush, hOldBrush;
	HGDIOBJ hPen, hOldPen;
	int w, h;
	PBITMAPINFO pbi;
	TCHAR wCharFilePath[MAX_PATH];
	std::wstring filePath;
	std::wstring fileName;
	std::string debug;

	switch (message)                  /* handle the messages */
	{
		case WM_CREATE:
		{
			/* Register hotkeys */
			if (!RegisterHotKey(hwnd, HOTKEY1, MOD_CONTROL | MOD_SHIFT, 0x33)) {
				MessageBox(NULL, _T("Could not register hotkey 1."), _T("Error"), NULL);
			}

			if (!RegisterHotKey(hwnd, HOTKEY2, MOD_CONTROL | MOD_SHIFT, 0x34)) {
				MessageBox(NULL, _T("Could not register hotkey 1."), _T("Error"), NULL);
			}
		}
		break;
		case WM_HOTKEY:
		{
			if (wParam == HOTKEY1) {
				// Full screenshot
				screenshot = screenCapture(0, 0, fullW, fullH);

				// Save BMP
				hdc = GetDC(hwnd);
				pbi = CreateBitmapInfoStruct(hwnd, screenshot);
				GetTempPath(MAX_PATH, wCharFilePath);
				std::wstring filePath(wCharFilePath);
				fileName = GetRandomFileName();
				CreateBMPFile(hwnd, (filePath + fileName + L".bmp").c_str(), pbi, screenshot, hdc);
				bitmapToPNG((filePath + fileName));

				CStringA sFullImagePNG((filePath + fileName + L".png").c_str());
				postToImgur(std::string(sFullImagePNG));


				DeleteFile((filePath + fileName + L".bmp").c_str());
				DeleteFile((filePath + fileName + L".png").c_str());
				ReleaseDC(hwnd, hdc);

			}
			else if (wParam == HOTKEY2) {

				// Region screenshot
				screenshot = screenCapture(0, 0, fullW, fullH);
				ShowWindow(hwnd, SW_NORMAL);
				SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, fullW, fullH, SWP_SHOWWINDOW);
				SetForegroundWindow(hwnd);
				SetFocus(hwnd);

				// Draw the screen at the time on a window
				hdc = BeginPaint(hwnd, &ps);
				hdcMem = CreateCompatibleDC(hdc);
				hbmOld = (HBITMAP)SelectObject(hdcMem, screenshot);
				BitBlt(hdc, 0, 0, fullW, fullH, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);
				EndPaint(hwnd, &ps);




			}
		}
		break;
		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				//SendMessage(hwnd, WM_CLOSE, 0, 0);
				ShowWindow(hwnd, SW_HIDE);
			}
		}
		break;
		case WM_LBUTTONDOWN:
		{
			startX = GET_X_LPARAM(lParam);
			startY = GET_Y_LPARAM(lParam);
			draw = true;
			//debug = "Start: ( " + patch::to_string(startX) + ", " + patch::to_string(startY) + ")";
			//std::cout << debug;
		}
		break;
		case WM_LBUTTONUP:
		{
			draw = false;
			// Do we need to get endX & endY again since mousemove gets them?
			endX = GET_X_LPARAM(lParam);
			endY = GET_Y_LPARAM(lParam);
			w = abs(startX - endX);
			h = abs(startY - endY);

			// Our screenshot function requires top-left point of rectangle
			// If either our final x or y are decreased, choose them as our start point for screenshot
			if (endX < startX) {
				startX = endX;
			}
			if (endY < startY) {
				startY = endY;
			}

			// Paint over previous rectangle
			hdc = GetDC(hwnd);
			hdcMem = CreateCompatibleDC(hdc);
			hbmOld = (HBITMAP)SelectObject(hdcMem, screenshot);
			BitBlt(hdc, 0, 0, fullW, fullH, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, hbmOld);
			ReleaseDC(hwnd, hdc);
			DeleteDC(hdcMem);

			screenshot = screenCapture(startX, startY, w, h);
			ShowWindow(hwnd, SW_HIDE);

			// Save BMP
			hdc = GetDC(hwnd);
			pbi = CreateBitmapInfoStruct(hwnd, screenshot);
			GetTempPath(MAX_PATH, wCharFilePath);
			std::wstring filePath(wCharFilePath);
			fileName = GetRandomFileName();
			std::wstring fullNameBMP = (filePath + fileName + L".bmp");
			CreateBMPFile(hwnd, fullNameBMP.c_str(), pbi, screenshot, hdc);

			bitmapToPNG((filePath + fileName));

			// POST to imgur and handle response
			CStringA sFullImagePNG((filePath + fileName + L".png").c_str());
			postToImgur(std::string(sFullImagePNG));

			DeleteFile((filePath + fileName + L".bmp").c_str());
			DeleteFile((filePath + fileName + L".png").c_str());
			ReleaseDC(hwnd, hdc);
		}
		break;
		case WM_MOUSEMOVE:
		{
			if (draw) {
				// Get current mouse position
				endX = GET_X_LPARAM(lParam);
				endY = GET_Y_LPARAM(lParam);

				// Paint over previous rectangle
				hdc = GetDC(hwnd);
				hdcMem = CreateCompatibleDC(hdc);
				hbmOld = (HBITMAP)SelectObject(hdcMem, screenshot);
				BitBlt(hdc, 0, 0, fullW, fullH, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, hbmOld);

				// Change to NULL_BRUSH (no fill)
				hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

				// Change to red pen
				hPen = GetStockObject(DC_PEN);
				hOldPen = SelectObject(hdc, hPen);
				SetDCPenColor(hdc, RGB(255, 0, 0));

				// Draw new rectangle
				Rectangle(hdc, startX, startY, endX, endY);

				// Restore original brush and brush
				SelectObject(hdc, hOldBrush);
				SelectObject(hdc, hOldPen);

				ReleaseDC(hwnd, hdc);
				DeleteDC(hdcMem);
			}
		}
		break;
		case WM_DESTROY:
		{
			UnregisterHotKey(hwnd, HOTKEY1);
			UnregisterHotKey(hwnd, HOTKEY2);
			PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
			break;
		}
		default:                      /* for messages that we don't deal with */
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}
	}




    return 0;
}

std::wstring GetRandomFileName() {
	std::wstring s = L"";
	srand(time(0));
	std::wstring str = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int pos;
	while (s.size() != 15) {
		pos = ((rand() % (str.size() - 1)));
		s += str.substr(pos, 1);
	}

	return L"screenur_" + s;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0) {
		MessageBox(NULL, _T("GDI+ error!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));

	if (pImageCodecInfo == NULL) {
		MessageBox(NULL, _T("GDI+ error!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	MessageBox(NULL, _T("GDI+ error!"), _T("Error"), NULL);
	return -1;  // Failure
}

PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.
	if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		return 0;

	// Convert the color format to a count of bits.
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD
	// data structures.)

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;
	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	// The width must be DWORD aligned unless the bitmap is RLE
	// compressed.
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

int CreateBMPFile(HWND hwnd, LPCTSTR pszFile, PBITMAPINFO pbi,
	HBITMAP hBMP, HDC hDC)
{
	HANDLE hf;                 // file handle
	BITMAPFILEHEADER hdr;       // bitmap file-header
	PBITMAPINFOHEADER pbih;     // bitmap info-header
	LPBYTE lpBits;              // memory pointer
	DWORD dwTotal;              // total count of bytes
	DWORD cb;                   // incremental count of bytes
	BYTE *hp;                   // byte pointer
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) {
		MessageBox(NULL, _T("Error creating BMP file!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	// Retrieve the color table (RGBQUAD array) and the bits
	// (array of palette indices) from the DIB.
	if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS))
	{
		MessageBox(NULL, _T("Error creating BMP file!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	// Create the .BMP file.
	// TODO: Check if changing CreateFile to CreateFileA affected anything
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	if (hf == INVALID_HANDLE_VALUE)
		return 0;
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
								// Compute the size of the entire file.
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.
	if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL))
	{
		MessageBox(NULL, _T("Error creating BMP file!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL)))
	{
		MessageBox(NULL, _T("Error creating BMP file!"), _T("Error"), NULL);
		return -1;  // Failure

	}
	// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL)) {
		MessageBox(NULL, _T("Error creating BMP file!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	// Close the .BMP file.
	if (!CloseHandle(hf)) {
		MessageBox(NULL, _T("Error creating BMP file!"), _T("Error"), NULL);
		return -1;  // Failure
	}

	// Free memory.
	GlobalFree((HGLOBAL)lpBits);
}


int bitmapToPNG(std::wstring fileName) {
	// Initialize GDI+.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	//std::wstring wFileName = std::wstring(fileName.begin(), fileName.end());
	std::wstring wFileNameBMP = fileName + L".bmp";
	Gdiplus::Image * image = new Gdiplus::Image(wFileNameBMP.c_str());

	// Get the CLSID of the PNG encoder.
	GetEncoderClsid(L"image/png", &encoderClsid);

	std::wstring wFileNamePNG = fileName + L".png";
	stat = image->Save(wFileNamePNG.c_str(), &encoderClsid, NULL);

	if (stat != 0) {
		MessageBox(NULL, _T("Error creating PNG file!"), _T("Error"), NULL);
		return -1;
	}

	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

HBITMAP screenCapture(int x, int y, int w, int h) {
	// We use NULL to retrieve the DC for the entire screen
	HDC hdcSource = GetDC(NULL);

	// Obtain a memory DC since we can't write to bitmaps directly,
	// but only once they are loaded into the memory DC using SelectObject
	HDC hdcMemory = CreateCompatibleDC(hdcSource);

	// Create a compatible bitmap to load into the HDC
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, w, h);

	// Swap in the bitmap into memory device context so we can write to it
	// We back up the old object already in the HDC so we can swap it back in later
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

	// This will effectively write to hBitmap (which is loaded in hdcMemory)
	// from our source (the screen DC)
	BitBlt(hdcMemory, 0, 0, w, h, hdcSource, x, y, SRCCOPY);

	// Swap out the written-to bitmap, and swap in the one we backed up
	hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();

	// clean up
	DeleteDC(hdcSource);
	DeleteDC(hdcMemory);

	return hBitmap;
}

//static read_callback();

int postToImgur(std::string file)
{
	curl_global_cleanup();

	CURL *curl;
	CURLcode res;

	// Init winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);

	// Get curl handle
	curl = curl_easy_init();

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;

	if (curl) {
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "image", CURLFORM_FILE, file.c_str(), CURLFORM_END);
		headerlist = curl_slist_append(headerlist, "Expect:");
		headerlist = curl_slist_append(headerlist, "Authorization: Client-ID bea02ebef14af4b");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.imgur.com/3/image");
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			MessageBox(NULL, _T("Could not upload image. Please check your internet connection."), _T("Error"), NULL);
		}

		curl_easy_cleanup(curl);
		curl_formfree(formpost);
		curl_slist_free_all(headerlist);
	}
	return 0;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	// Libcurl is out to get you and doesn't pass a null-terminated char array!
	
	char * responseString = new char[nmemb + 1]();
	strncpy(responseString, ptr, size*nmemb);
	responseString[nmemb] = '\0';
	OutputDebugStringA(responseString);

	// Parse JSON to get the URL
	rapidjson::Document responseJSON;
	responseJSON.Parse(responseString);
	rapidjson::Value& url = responseJSON["data"]["link"];
	
	// Handle the URL Response
	std::string imageLink = url.GetString();
	handleURLResponse(imageLink.c_str());

	return (size * nmemb);

}

void handleURLResponse(const char * URL)
{
	// Open URL in browser
	CStringW wURL(URL);
	ShellExecute(0, 0, wURL, 0, 0, SW_SHOW);

	// Copy URL to clipboard
	const size_t len = strlen(URL) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	memcpy(GlobalLock(hMem), URL, len);
	GlobalUnlock(hMem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}