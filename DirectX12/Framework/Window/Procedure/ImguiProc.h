#pragma once

#include "Framework/Window/Procedure/IWindowProc.h"

namespace Framework {
namespace Window {
/**
* @class ImguiProc
* @brief ImGUI�p�v���V�[�W��
*/
class ImguiProc :public IWindowProc {
public:
    /**
    * @brief �f�X�g���N�^
    */
    virtual ~ImguiProc();
    /**
    * @brief �E�B���h�E�v���V�[�W��
    */
    virtual LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL* isReturn) override;
};

} //Window 
} //Framework 
