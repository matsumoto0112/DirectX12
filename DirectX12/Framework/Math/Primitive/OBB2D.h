#pragma once
#include <array>
#include "Framework/Math/Matrix3x3.h"
#include "Framework/Math/Primitive/AABB2D.h"
#include "Framework/Math/Vector2.h"

namespace Framework {
namespace Math {
/**
* @class OBB2D
* @brief 2D有向境界ボックス
*/
class OBB2D {
public:
    /**
    * @brief コンストラクタ
    */
    OBB2D() :OBB2D(Vector2::ZERO, Vector2::ZERO, 0.0f) { }
    /**
    * @brief コンストラクタ
    * @param position 中心座標
    * @param size 各軸の大きさ
    * @param rotate 回転（度）
    */
    OBB2D(const Vector2& position, const Vector2& size, float rotate)
        :position(position), size(size), rotate(rotate) { }

    /**
    * @brief AABBに変換する
    */
    static AABB2D toAABB2D(const OBB2D& obb) {
        std::array<Vector2, 4> a = obb.getVertices();
        float left = MathUtil::mymin({ a[0].x,a[1].x,a[2].x, a[3].x });
        float top = MathUtil::mymin({ a[0].y,a[1].y, a[2].y, a[3].y });
        float right = MathUtil::mymax({ a[0].x, a[1].x, a[2].x, a[3].x });
        float bottom = MathUtil::mymax({ a[0].y, a[1].y, a[2].y, a[3].y });
        return AABB2D(left, top, right, bottom);
    }

    /**
    * @brief 回転を無視したAABBに変換する
    */
    static AABB2D toAABB2DIgnoreRotate(const OBB2D& obb) {
        return AABB2D(obb.position, obb.size);
    }

    /**
    * @brief 頂点と頂点を結ぶ4直線を取得する
    */
    std::array<Vector2, 4> getVertices() const {
        const Vector2 halfSize = size * 0.5f;
        const float left = position.x - halfSize.x;
        const float right = position.x + halfSize.x;
        const float top = position.y - halfSize.y;
        const float bottom = position.y + halfSize.y;
        Matrix3x3 rotateMat = Matrix3x3::createTranslate(-position) *
            Matrix3x3::createRotate(-rotate) * Matrix3x3::createTranslate(position);
        std::array<Vector2, 4> res;
        res[0] = Vector2(left, top) * rotateMat;
        res[1] = Vector2(left, bottom) * rotateMat;
        res[2] = Vector2(right, bottom) * rotateMat;
        res[3] = Vector2(right, top) * rotateMat;
        return res;
    }
public:
    Vector2 position; //!< 中心座標
    Vector2 size; //!< 幅と高さ
    float rotate; //!< 回転
};


} //Math 
} //Framework 
