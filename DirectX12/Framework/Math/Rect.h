#pragma once
#include "Framework/Utility/Property.h"
#include "Framework/Math/Vector2.h"
namespace Framework {
namespace Math {

/**
* @class Rect
* @brief 矩形クラス
*/
class Rect {
public:
    /**
    * @brief デフォルトコンストラクタ
    */
    Rect();
    /**
    * @brief コンストラクタ
    * @param _x x座標
    * @param _y y座標
    * @param _width 幅
    * @param _height 高さ
    */
    Rect(float x, float y, float width, float height);
    /**
    * @brief コンストラクタ
    * @param center 中心座標
    * @param size 大きさ
    */
    Rect(const Vector2& center, const Vector2& size);
    /**
    * @brief デストラクタ
    */
    ~Rect();
    /**
    * @brief 座標を取得
    */
    Vector2 getPosition() const;
    /**
    * @brief 座標を設定
    */
    void setPosition(const Vector2& position);
    /**
    * @brief 幅と高さを取得
    */
    Vector2 getSize() const;
    /**
    * @brief 幅と高さを設定
    */
    void setSize(const Vector2& size);
    /**
    * @brief 最小x座標を取得
    */
    float getXMin() const;
    /**
    * @brief 最小x座標を設定
    */
    void setXMin(float xmin);
    /**
    * @brief 最大x座標を取得
    */
    float getXMax() const;
    /**
    * @brief 最大x座標を設定
    */
    void setXMax(float xmax);
    /**
    * @brief 最小y座標を取得
    */
    float getYMin() const;
    /**
    * @brief 最小y座標を設定
    */
    void setYMin(float ymin);
    /**
    * @brief 最大y座標を取得
    */
    float getYMax() const;
    /**
    * @brief 最大y座標を設定
    */
    void setYMax(float ymax);
    /**
    * @brief 最小座標を取得
    */
    Vector2 getMin() const;
    /**
    * @brief 最小座標を設定
    */
    void setMin(const Vector2& min);
    /**
    * @brief 最小座標を設定
    */
    void setMin(float xmin, float ymin);
    /**
    * @brief 最大座標を取得
    */
    Vector2 getMax() const;
    /**
    * @brief 最大座標を設定
    */
    void setMax(const Vector2& max);
    /**
    * @brief 最大座標を設定
    */
    void setMax(float xmax, float ymax);
    /**
    * @brief 中心座標を取得
    */
    Vector2 getCenter() const;
    /**
    * @brief 点を含んでいるか
    * @param point 点の座標
    * @return 含んでいたらtrue
    */
    bool contains(const Vector2& point) const;
    /**
    * @brief 別の矩形と重なっているか
    * @param rect 調べる矩形
    * @return 重なっていたらtrue
    */
    bool intersects(const Rect& rect) const;
private:
    float _x; //!< x座標
    float _y; //!< y座標
    float _width; //!< 幅
    float _height; //!< 高さ
public:
    Utility::Property<float> x{ _x };
    Utility::Property<float> y{ _y };
    Utility::Property<float> width{ _width };
    Utility::Property<float> height{ _height };
};

} //Math 
} //Framework 