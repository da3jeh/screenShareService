//#include <Windows.h>
//#include <iostream>
//#include <string>
//#include <vector>
//#include <thread>
//#include <atomic>
//#include <mutex>
//#include "screenShareService.cpp"
//
////class ScreenCaptureStream {
////    // ... (unchanged)
////};
//
//// Global variable for the service status
//SERVICE_STATUS serviceStatus;
//SERVICE_STATUS_HANDLE serviceStatusHandle;
//
//// Function prototypes
//void WINAPI ServiceMain(DWORD argc, LPWSTR* argv);
//void WINAPI ServiceCtrlHandler(DWORD ctrl);
//
//int main() {
//    // Convert the string to a wide string literal
//    LPWSTR serviceName = const_cast<LPWSTR>(L"MyCaptureService");
//
//    SERVICE_TABLE_ENTRY serviceTable[] = {
//        { serviceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
//        { nullptr, nullptr }
//    };
//
//    if (StartServiceCtrlDispatcher(serviceTable) == FALSE) {
//        DWORD error = GetLastError();
//        std::cerr << "StartServiceCtrlDispatcher failed. Error code: " << error << std::endl;
//
//        switch (error) {
//        case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
//            std::cerr << "ERROR_FAILED_SERVICE_CONTROLLER_CONNECT: The process that hosts the service (the service control manager) is not available." << std::endl;
//            break;
//            // Add more cases as needed...
//
//        default:
//            break;
//        }
//
//        return -1;
//    }
//
//    return 0;
//}
//
//
//
//
//void WINAPI ServiceMain(DWORD argc, LPWSTR* argv) {
//    // Register the service control handler
//    serviceStatusHandle = RegisterServiceCtrlHandler(L"MyCaptureService", ServiceCtrlHandler);
//
//    if (!serviceStatusHandle) {
//        std::cerr << "RegisterServiceCtrlHandler failed." << std::endl;
//        return;
//    }
//
//    // Initialize the service status
//    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
//    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
//    SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//    // Perform service-specific initialization
//    ScreenCaptureStream screenShare;
//    screenShare.StartCapture(100);
//
//    // Update the service status
//    serviceStatus.dwCurrentState = SERVICE_RUNNING;
//    SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//    // Keep the service running until it is stopped
//    while (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
//        std::this_thread::sleep_for(std::chrono::minutes(1));
//    }
//
//    // Perform cleanup
//    screenShare.StopCapture();
//}
//
//void WINAPI ServiceCtrlHandler(DWORD ctrl) {
//    switch (ctrl) {
//    case SERVICE_CONTROL_STOP:
//        // Update the service status
//        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
//        SetServiceStatus(serviceStatusHandle, &serviceStatus);
//
//        // Perform cleanup and stop the service
//        serviceStatus.dwWin32ExitCode = 0;
//        serviceStatus.dwCurrentState = SERVICE_STOPPED;
//        SetServiceStatus(serviceStatusHandle, &serviceStatus);
//        break;
//
//        // Handle other service control codes as needed
//
//    default:
//        break;
//    }
//}
