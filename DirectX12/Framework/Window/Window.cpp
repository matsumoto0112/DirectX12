#include "Window.h"
#include "Framework/Utility/Debug.h"
#include "Framework/Window/WindowProcedures.h"

namespace {
/**
* @brief �E�B���h�E���쐬����
* @param hInstance �C���X�^���X�n���h��
* @param hWnd �E�B���h�E�n���h��
* @param clientSize �N���C�A���g�̈�̑傫��
* @param position �E�B���h�E�̏ꏊ
* @param name �E�B���h�E�^�C�g��
*/
bool createWindow(HINSTANCE hInstance,
    HWND* hWnd,
    const Framework::Math::Vector2& clientSize,
    const Framework::Math::Vector2& position,
    const std::string& name) {
    // �E�B���h�E�N���X�̃p�����[�^�ݒ�
    WNDCLASSEX wcex;
    //�\���̂̃T�C�Y
    wcex.cbSize = sizeof(WNDCLASSEX);
    //�E�B���h�E�X�^�C��
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    //�E�B���h�E�̃��b�Z�[�W����������R�[���o�b�N�֐��̓o�^
    wcex.lpfnWndProc = Framework::Window::WindowProcedures::mainWndProc;
    //�E�C���h�E�N���X�\���̂̌��Ɋ��蓖�Ă�⑫�o�C�g��
    wcex.cbClsExtra = 0;
    //�E�C���h�E�C���X�^���X�̌��Ɋ��蓖�Ă�⑫�o�C�g��
    wcex.cbWndExtra = 0;
    //���̃N���X�̂��߂̃E�C���h�E�v���V�[�W��������C���X�^���X�n���h��
    wcex.hInstance = hInstance;
    //�A�C�R���̃n���h��
    wcex.hIcon = NULL;
    //�}�E�X�J�[�\���̃n���h��
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //�E�C���h�E�w�i�F
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW - 1);
    //�f�t�H���g���j���[��
    wcex.lpszMenuName = NULL;
    //�E�C���h�E�N���X�ɂ��閼�O
    wcex.lpszClassName = name.c_str();
    //16�~16�̏������T�C�Y�̃A�C�R��
    wcex.hIconSm = NULL;
    //�o�^�Ɏ��s�����玸�s�����ƕԂ�
    if (!RegisterClassEx(&wcex))
        return false;

    //�E�B���h�E�̐��� 
    //�Ƃ肠�����K���ȑ傫���ŃE�B���h�E�����A��ő傫�����C������
    *hWnd = CreateWindow(name.c_str(),
        name.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        0,
        0,
        800,
        600,
        nullptr,
        nullptr,
        wcex.hInstance,
        NULL);
    //�o�^�Ɏ��s�����玸�s�����ƕԂ�
    if (!(*hWnd))
        return false;

    return true;
}
}
namespace Framework {
namespace Window {

Window::Window(HINSTANCE hInstance,
    const Vec2& clientSize,
    const Vec2& position,
    const std::string& windowTitle)
    :mClientSize(clientSize) {

    bool succeed;
    succeed = createWindow(hInstance, &mHWnd, clientSize, position, windowTitle.c_str());
    //�G���[�`�F�b�N
    if (!succeed) {
        MY_ERROR_WINDOW(succeed, "�E�B���h�E�̐����Ɏ��s���܂����B");
        return;
    }

    setWindowSizeAndPosition(clientSize, position);
    ShowWindow(mHWnd, SW_SHOW);
    UpdateWindow(mHWnd);
}

Window::~Window() { }

void Window::setWindowSizeAndPosition(const Vec2& clientSize, const Vec2& position) {
    //�Ō�ɑ傫����ݒ肷��
    SetWindowPos(mHWnd,
        nullptr,
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(clientSize.x),
        static_cast<int>(clientSize.y),
        SWP_NOZORDER);
}

void Window::quit() {
    PostMessage(mHWnd, WM_CLOSE, 0, 0);
}

HWND Window::getHWND() const {
    return mHWnd;
}

bool Window::isClosed() const {
    return IsWindow(mHWnd) == FALSE;
}

void Window::addProcedureEvent(IWindowProc* proc) {
    //���j�[�N�|�C���^������
    WindowProcedures::mWindowProcs.emplace_back(std::unique_ptr<IWindowProc>(proc));
}

bool Window::isActive() const {
    return GetActiveWindow() == mHWnd;
}

} //Window 
} //Framework 