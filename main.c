#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void ConvertToPng(int width, int height, unsigned char *pixels) {
    char buffer[255];
    time_t t = time(NULL);
    struct tm date = *localtime(&t);
    snprintf(buffer, sizeof(buffer), "screenshot-%d-%02d-%02d %02d_%02d_%02d.png",
        date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
    stbi_write_png(buffer, width, height, 4, pixels, width * 4);
}

void RotateBitmap(unsigned char *pixels, int width, int height) {
    int rowSize = width * 4;
    for (int y = 0; y < height / 2; y++) {
        BYTE* top = pixels + y * rowSize;
        BYTE* bottom = pixels + (height - 1 - y) * rowSize;
        for (int x = 0; x < rowSize; x++) {
            BYTE temp = top[x];
            top[x] = bottom[x];
            bottom[x] = temp;
        }
    }
}

void SwapChannels(unsigned char *pixels, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        BYTE* p = pixels + i * 4;
        BYTE temp = p[0];
        p[0] = p[2];
        p[2] = temp;
    }
}

void SaveBitmap(HBITMAP hBitmap, HDC hDC) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader = {0};
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER);

    BITMAPINFOHEADER bi = {0};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    DWORD dwBmpSize = ((bmp.bmWidth * bi.biBitCount) / 32) * 4 * bmp.bmHeight;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    LPVOID lpbitmap = GlobalLock(hDIB);

    GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    unsigned char* pixels = (unsigned char*)lpbitmap;
    RotateBitmap(pixels, bmp.bmWidth, bmp.bmHeight);
    SwapChannels(pixels, bmp.bmWidth, bmp.bmHeight);
    ConvertToPng(bmp.bmWidth, bmp.bmHeight, pixels);

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
}


__declspec(dllexport) void TakeScreenshot() {
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    SaveBitmap(hBitmap, hMemoryDC);

    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
}
