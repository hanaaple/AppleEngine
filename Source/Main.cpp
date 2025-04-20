#include <Windows.h>



// 각종 윈도우 관련 메시지(이벤트)를 처리하는 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // 창이 제거될 때 (창 닫기, Alt+F4 등)
    case WM_DESTROY:
        PostQuitMessage(0); // 프로그램 종료
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

// WinMain 함수에 대한 주석 추가
// hInstance: 현재 인스턴스 핸들
// hPrevInstance: 이전 인스턴스 핸들 (항상 NULL)
// lpCmdLine: 명령줄 인수
// nShowCmd: 윈도우 표시 상태
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    // 사용 안하는 파라미터들
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    // 윈도우 클래스 이름 및 타이틀 이름
    constexpr WCHAR WndClassName[] = L"Apple Engine Class";
    constexpr WCHAR WndTitle[] = L"Apple Engine with D3D11";

    // 윈도우 클래스 등록
    WNDCLASS WndClass = { 0, WndProc, 0, 0, hInstance, nullptr, nullptr, nullptr, nullptr, WndClassName };
    RegisterClass(&WndClass);

    // 1024 x 1024로 윈도우 생성
    const HWND hWnd = CreateWindowEx(
        0, WndClassName, WndTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1024, 1024,
        nullptr, nullptr, hInstance, nullptr
    );

    // FPS 제한
    constexpr int TargetFPS = 60;
    constexpr double TargetDeltaTime = 1000.0f / TargetFPS; // 1 FPS의 목표 시간 (ms)

    // 고성능 타이머 초기화
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime;
    QueryPerformanceCounter(&StartTime);

    float Accumulator = 0.0; // Fixed Update에 사용되는 값
    constexpr float FixedTimeStep = 1.0f / TargetFPS;


    // Main Loop
    bool bIsExit = false;
    while (bIsExit == false)
    {
        // DeltaTime 계산 (초 단위)
        const LARGE_INTEGER EndTime = StartTime;
        QueryPerformanceCounter(&StartTime);

        const float DeltaTime =
            static_cast<float>(StartTime.QuadPart - EndTime.QuadPart) / static_cast<float>(Frequency.QuadPart);

        // 누적 시간 추가
        Accumulator += DeltaTime;

        // 메시지(이벤트) 처리
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&msg);

            // 메시지를 등록한 Proc에 전달
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }

        // FixedTimeStep 만큼 업데이트
        while (Accumulator >= FixedTimeStep)
        {
            Accumulator -= FixedTimeStep;
        }

        // FPS 제한
        double ElapsedTime;
        do
        {
            Sleep(0);

            LARGE_INTEGER CurrentTime;
            QueryPerformanceCounter(&CurrentTime);

            ElapsedTime = static_cast<double>(CurrentTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);
        } while (ElapsedTime < TargetDeltaTime);
    }

   return 0;
}