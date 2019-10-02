#pragma once
#include "Framework/Utility/Property.h"
#include "Framework/Math/Vector2.h"
namespace Framework {
namespace Math {

/**
* @class Rect
* @brief ��`�N���X
*/
class Rect {
public:
    /**
    * @brief �f�t�H���g�R���X�g���N�^
    */
    Rect();
    /**
    * @brief �R���X�g���N�^
    * @param _x x���W
    * @param _y y���W
    * @param _width ��
    * @param _height ����
    */
    Rect(float x, float y, float width, float height);
    /**
    * @brief �R���X�g���N�^
    * @param center ���S���W
    * @param size �傫��
    */
    Rect(const Vector2& center, const Vector2& size);
    /**
    * @brief �f�X�g���N�^
    */
    ~Rect();
    /**
    * @brief ���W���擾
    */
    Vector2 getPosition() const;
    /**
    * @brief ���W��ݒ�
    */
    void setPosition(const Vector2& position);
    /**
    * @brief ���ƍ������擾
    */
    Vector2 getSize() const;
    /**
    * @brief ���ƍ�����ݒ�
    */
    void setSize(const Vector2& size);
    /**
    * @brief �ŏ�x���W���擾
    */
    float getXMin() const;
    /**
    * @brief �ŏ�x���W��ݒ�
    */
    void setXMin(float xmin);
    /**
    * @brief �ő�x���W���擾
    */
    float getXMax() const;
    /**
    * @brief �ő�x���W��ݒ�
    */
    void setXMax(float xmax);
    /**
    * @brief �ŏ�y���W���擾
    */
    float getYMin() const;
    /**
    * @brief �ŏ�y���W��ݒ�
    */
    void setYMin(float ymin);
    /**
    * @brief �ő�y���W���擾
    */
    float getYMax() const;
    /**
    * @brief �ő�y���W��ݒ�
    */
    void setYMax(float ymax);
    /**
    * @brief �ŏ����W���擾
    */
    Vector2 getMin() const;
    /**
    * @brief �ŏ����W��ݒ�
    */
    void setMin(const Vector2& min);
    /**
    * @brief �ŏ����W��ݒ�
    */
    void setMin(float xmin, float ymin);
    /**
    * @brief �ő���W���擾
    */
    Vector2 getMax() const;
    /**
    * @brief �ő���W��ݒ�
    */
    void setMax(const Vector2& max);
    /**
    * @brief �ő���W��ݒ�
    */
    void setMax(float xmax, float ymax);
    /**
    * @brief ���S���W���擾
    */
    Vector2 getCenter() const;
    /**
    * @brief �_���܂�ł��邩
    * @param point �_�̍��W
    * @return �܂�ł�����true
    */
    bool contains(const Vector2& point) const;
    /**
    * @brief �ʂ̋�`�Əd�Ȃ��Ă��邩
    * @param rect ���ׂ��`
    * @return �d�Ȃ��Ă�����true
    */
    bool intersects(const Rect& rect) const;
private:
    float _x; //!< x���W
    float _y; //!< y���W
    float _width; //!< ��
    float _height; //!< ����
public:
    Utility::Property<float> x{ _x };
    Utility::Property<float> y{ _y };
    Utility::Property<float> width{ _width };
    Utility::Property<float> height{ _height };
};

} //Math 
} //Framework 