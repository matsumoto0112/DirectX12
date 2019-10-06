#pragma once
#include "Framework/Graphics/DX12/Material/IMaterial.h"
#include "Framework/Utility/Property.h"
#include "Framework/Graphics/DX12/Material/CBStruct.h"

namespace Framework {
namespace Graphics {

/**
* @class Material2D
* @brief discription
*/
class Material2D :public IMaterial {
public:
    /**
    * @brief �R���X�g���N�^
    */
    Material2D();
    /**
    * @brief �f�X�g���N�^
    */
    ~Material2D();
    /**
    * @brief �R�}���h���X�g�ɒǉ�����
    */
    virtual void addToCommandList(ID3D12GraphicsCommandList* commandList) override;
public:
    MVPCBuffer mMVP;
    ColorCBuffer mColor;
    UVCBuffer mUV;
};

} //Graphics 
} //Framework 