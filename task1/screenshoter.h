#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_UNIX     3

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_MAC
#else
#define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_WINDOWS

#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <windows.h>
#include <gdiplus.h>
#elif PLATFORM == PLATFORM_UNIX
#include <sys/time.h>
#include <unistd.h>

#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <libpng/png.h>

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>
using namespace std;
#if PLATFORM == PLATFORM_WINDOWS


#pragma comment(lib, "GdiPlus.lib") 
using namespace Gdiplus;

static const GUID png =
{ 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
#elif PLATFORM == PLATFORM_UNIX
extern int errno;
#endif
int screenshoter()
{
#if PLATFORM == PLATFORM_UNIX
	Display* display = XOpenDisplay(NULL);
	Window root = DefaultRootWindow(display);

#endif
	for (int i = 1;; i++)
	{


#if PLATFORM == PLATFORM_WINDOWS
		SYSTEMTIME st;
		GetSystemTime(&st);
		WORD d = st.wDay;
		int day = (int)d;
		std::string dayS = std::to_string(day);
		WORD m = st.wMonth;
		int mon = (int)m;
		std::string monS = std::to_string(mon);
		WORD y = st.wYear;
		int year = (int)y;
		std::string yearS = std::to_string(year);
		string nomber = std::to_string(i);
		string name = dayS + "." + monS + "." + yearS + "_" + nomber + ".png";

		//std::cout << name << "\n";

		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		HDC scrdc, memdc;
		HBITMAP membit;
		// Получаем HDC рабочего стола
		// Параметр HWND для рабочего стола всегда равен нулю.
		scrdc = GetDC(0);
		// Определяем разрешение экрана
		int Height, Width;
		Height = GetSystemMetrics(SM_CYSCREEN);
		Width = GetSystemMetrics(SM_CXSCREEN);
		// Создаем новый DC, идентичный десктоповскому и битмап размером с экран.
		memdc = CreateCompatibleDC(scrdc);
		membit = CreateCompatibleBitmap(scrdc, Width, Height);
		SelectObject(memdc, membit);
		// Улыбаемся... Снято!
		BitBlt(memdc, 0, 0, Width, Height, scrdc, 0, 0, SRCCOPY);
		HBITMAP hBitmap;
		hBitmap = (HBITMAP)SelectObject(memdc, membit);
		Gdiplus::Bitmap bitmap(hBitmap, NULL);
		string s = name;
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;

		bitmap.Save(r.c_str(), &png);

		DeleteObject(hBitmap);
#elif PLATFORM == PLATFORM_UNIX
		struct timeval tv;
		struct tm* tm;
		gettimeofday(&tv, NULL);
		tm = localtime(&tv.tv_sec);
		std::string filename = std::to_string(tm->tm_mday) + "." + std::to_string(tm->tm_mon + 1) + "." + std::to_string(tm->tm_year + 1900) + "_" + std::to_string(i) + ".png";
		XWindowAttributes gwa;

		XGetWindowAttributes(display, root, &gwa);
		int width = gwa.width;
		int height = gwa.height;


		XImage* image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);

		unsigned char* array = new unsigned char[width * height * 3];

		unsigned long red_mask = image->red_mask;
		unsigned long green_mask = image->green_mask;
		unsigned long blue_mask = image->blue_mask;

		int code = 0;
		FILE* fp;
		png_structp png_ptr;
		png_infop png_info_ptr;
		png_bytep png_row;

		// Open file
		fp = fopen(filename.c_str(), "wb");
		if (fp == NULL)
		{
			fprintf(stderr, "Could not open file for writing\n");
			code = 1;
		}

		// Initialize write structure
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL)
		{
			fprintf(stderr, "Could not allocate write struct\n");
			code = 1;
		}

		// Initialize info structure
		png_info_ptr = png_create_info_struct(png_ptr);
		if (png_info_ptr == NULL)
		{


			fprintf(stderr, "Could not allocate info struct\n");
			code = 1;
		}

		// Setup Exception handling
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fprintf(stderr, "Error during png creation\n");
			code = 1;
		}

		png_init_io(png_ptr, fp);

		// Write header (8 bit colour depth)
		png_set_IHDR(png_ptr, png_info_ptr, width, height,
			8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		// Set title
		char* title = (char*)"Screenshot";
		if (title != NULL)
		{
			png_text title_text;
			title_text.compression = PNG_TEXT_COMPRESSION_NONE;
			title_text.key = (char*)"Title";
			title_text.text = title;
			png_set_text(png_ptr, png_info_ptr, &title_text, 1);
		}

		png_write_info(png_ptr, png_info_ptr);

		// Allocate memory for one row (3 bytes per pixel - RGB)
		png_row = (png_bytep)malloc(3 * width * sizeof(png_byte));

		// Write image data
		int x, y;
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				unsigned long pixel = XGetPixel(image, x, y);

				unsigned char blue = pixel & blue_mask;
				unsigned char green = (pixel & green_mask) >> 8;
				unsigned char red = (pixel & red_mask) >> 16;
				png_byte* ptr = &(png_row[x * 3]);
				ptr[0] = red;
				ptr[1] = green;
				ptr[2] = blue;
			}
			png_write_row(png_ptr, png_row);
		}

		// End write
		png_write_end(png_ptr, NULL);

		// Free
		fclose(fp);
		if (png_info_ptr != NULL) png_free_data(png_ptr, png_info_ptr, PNG_FREE_ALL, -1);
		if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		if (png_row != NULL) free(png_row);
#endif        
		this_thread::sleep_for(chrono::seconds(60));
	}
	return 0;

}








