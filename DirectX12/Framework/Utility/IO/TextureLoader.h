#pragma once

#include <wincodec.h>
#include <memory>
#include <vector>
#include <string>

namespace Framework {
namespace Utility {
/**
* @class TextureLoader
* @brief �e�N�X�`���ǂݍ��݋@
*/
class TextureLoader {
public:
    /**
    * @brief �R���X�g���N�^
    */
    TextureLoader();
    /**
    * @brief �f�X�g���N�^
    */
    ~TextureLoader();
    /**
    * @brief �e�N�X�`���ǂݍ���
    * @param filepath �t�@�C���ւ̃p�X
    */
    std::vector<BYTE> load(const std::string& filepath, _Out_ UINT* width, _Out_ UINT* height);
private:
    IWICBitmapDecoder* mDecoder; //!< �f�R�[�_�[
    IWICBitmapFrameDecode* mFrame; //!< �t���[���f�R�[�h
    IWICFormatConverter* mConverter; //!< �t�H�[�}�b�g�ϊ���
    IWICImagingFactory* mFactory; //!< �C���[�W������
};

} //Utility 
} //Framework 
