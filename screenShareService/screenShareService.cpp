#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

class ScreenCaptureStream {
public:
    ScreenCaptureStream()
        : running(false), frameRate(100), captureThread(nullptr) {
    }

    ~ScreenCaptureStream() {
        StopCapture();
    }

    void StartCapture(int frameRate) {
        this->frameRate = frameRate;
        running = true;
        captureThread = new std::thread(&ScreenCaptureStream::CaptureLoop, this);
    }

    void StopCapture() {
        running = false;
        if (captureThread) {
            captureThread->join();
            delete captureThread;
            captureThread = nullptr;
        }
    }

    std::vector<uint8_t> GetLastFrame() {
        std::lock_guard<std::mutex> lock(frameMutex);
        return lastFrame;
    }

private:
    void CaptureLoop() {
        int i = 0;
        HDC screenDC = GetDC(NULL);
        SetProcessDPIAware();
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        HDC memDC = CreateCompatibleDC(screenDC);

        HBITMAP bitmap = CreateCompatibleBitmap(screenDC, screenWidth, screenHeight);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

        while (running) {
            if (!BitBlt(memDC, 0, 0, screenWidth, screenHeight, screenDC, 0, 0, SRCCOPY)) {
                DWORD lastError = GetLastError();
                if (lastError == ERROR_INVALID_HANDLE) {
                    // The screen is locked or not accessible, handle it accordingly
                    std::cerr << "BitBlt failed with error: " << lastError << " (Screen locked or not accessible)" << std::endl;
                    // You can add a delay or other logic here if you want to retry the operation
                }
                else {
                    std::cerr << "BitBlt failed with error: " << lastError << std::endl;
                    break;
                }
            }
            i++;
            ProcessCapturedFrame(memDC, screenWidth, screenHeight, bitmap,i);

            Sleep(frameRate);
        }

        SelectObject(memDC, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(memDC);
        ReleaseDC(NULL, screenDC);
    }

    void ProcessCapturedFrame(HDC memDC, int width, int height, HBITMAP bitmap,int i) {
        BITMAPINFOHEADER bmpInfoHeader = { 0 };
        bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfoHeader.biWidth = width;
        bmpInfoHeader.biHeight = -height;  // Negative height for top-down DIB
        bmpInfoHeader.biPlanes = 1;
        bmpInfoHeader.biBitCount = 24;
        bmpInfoHeader.biCompression = BI_RGB;
        bmpInfoHeader.biSizeImage = 0;

        std::vector<uint8_t> frameData(width * height * 3);

        if (!GetDIBits(memDC, bitmap, 0, height, frameData.data(), (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS)) {
            std::cerr << "GetDIBits failed with error: " << GetLastError() << std::endl;
            return;
        }

        // Store the captured frame
        {
            std::lock_guard<std::mutex> lock(frameMutex);
            lastFrame = std::move(frameData);
        }

        // Do something with the captured frame (e.g., save to file)
        ProcessImageData(bitmap,width, height,i);
    }

    void ProcessImageData(HBITMAP bitmap, int width, int height,int i) {
        // Implement your image processing logic here
        // For example, you can save the frame to a file as BMP
        std::string filename = "images\\captured_image" + std::to_string(i) + ".bmp";  // Specify the desired filename and extension
        SaveBitmapToFile(bitmap, width, height, filename);

    }
    void SaveBitmapToFile(HBITMAP bitmap, int width, int height, const std::string& filename) {
        BITMAP bmp;
        GetObject(bitmap, sizeof(BITMAP), &bmp);

        BITMAPFILEHEADER bfh;
        ZeroMemory(&bfh, sizeof(BITMAPFILEHEADER));

        BITMAPINFOHEADER bih;
        ZeroMemory(&bih, sizeof(BITMAPINFOHEADER));

        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biWidth = width;
        bih.biHeight = height;
        bih.biPlanes = 1;
        bih.biBitCount = 32;
        bih.biCompression = BI_RGB;

        bfh.bfType = 0x4D42;
        bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
        bfh.bfSize = bfh.bfOffBits + bmp.bmWidth * 4 * bmp.bmHeight;

        BYTE* bits = new BYTE[bmp.bmWidth * 4 * bmp.bmHeight];
        ZeroMemory(bits, bmp.bmWidth * 4 * bmp.bmHeight);

        HDC hdc = GetDC(NULL);
        //ShowCursor(hdc);
        GetDIBits(hdc, bitmap, 0, bmp.bmHeight, bits, reinterpret_cast<BITMAPINFO*>(&bih), DIB_RGB_COLORS);

        //showCursorToBitmap(bits, bmp);
        ReleaseDC(NULL, hdc);

        FILE* file;
        fopen_s(&file, filename.c_str(), "wb");
        if (file) {
            fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, file);
            fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, file);
            fwrite(bits, bmp.bmWidth * 4 * bmp.bmHeight, 1, file);
            fclose(file);
        }

        delete[] bits;
    }


    std::atomic<bool> running;
    int frameRate;
    std::thread* captureThread;
    std::vector<uint8_t> lastFrame;
    std::mutex frameMutex;
};

int main() {
    ScreenCaptureStream screenShare;
    screenShare.StartCapture(100);  // Capture a frame every 100 milliseconds
    std::this_thread::sleep_for(std::chrono::minutes(1));

    // Do something...

    screenShare.StopCapture();

    return 0;
}
