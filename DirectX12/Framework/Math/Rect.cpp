#include "Rect.h"
namespace Framework {
namespace Math {

Rect::Rect(float x, float y, float width, float height)
    : _x(x), _y(y), _width(width), _height(height) { }

Rect::Rect()
    : Rect(0.0f, 0.0f, 1.0f, 1.0f) { }


Rect::Rect(const Vector2& center, const Vector2& size)
    : Rect(center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y) { }

Rect::~Rect() { }

Vector2 Rect::getPosition() const {
    return Vector2(_x, _y);
}

void Rect::setPosition(const Vector2& position) {
    _x = position.x;
    _y = position.y;
}

Vector2 Rect::getSize() const {
    return Vector2(_width, _height);
}

void Rect::setSize(const Vector2& size) {
    _width = size.x;
    _height = size.y;
}

float Rect::getXMin() const {
    return _x;
}
void Rect::setXMin(float xmin) {
    _width += _x - xmin;
    _x = xmin;
}

float Rect::getXMax() const {
    return _x + _width;
}

void Rect::setXMax(float xmax) {
    _width = xmax - _x;
}

float Rect::getYMin() const {
    return _y;
}
void Rect::setYMin(float ymin) {
    _height += _y - ymin;
    _y = ymin;
}

float Rect::getYMax() const {
    return _y + _height;
}

void Rect::setYMax(float ymax) {
    _height = ymax - _y;
}

Vector2 Rect::getMin() const {
    return Vector2(getXMin(), getYMin());
}

void Rect::setMin(const Vector2& min) {
    setXMin(min.x);
    setYMin(min.y);
}

void Rect::setMin(float xmin, float ymin) {
    setXMin(xmin);
    setYMin(ymin);
}
Vector2 Rect::getMax() const {
    return Vector2(getXMax(), getYMax());
}

void Rect::setMax(const Vector2& max) {
    setXMax(max.x);
    setYMax(max.y);
}

void Rect::setMax(float xmax, float ymax) {
    setXMax(xmax);
    setYMax(ymax);
}

Vector2 Rect::getCenter() const {
    return Vector2(_x + _width * 0.5f, _y + _height * 0.5f);
}

bool Rect::contains(const Vector2& point) const {
    if (point.x < _x) return false;
    if (point.y < _y)return false;
    if (point.x > getXMax())return false;
    if (point.y > getYMax()) return false;
    return true;
}

bool Rect::intersects(const Rect& rect) const {
    if (getXMin() < rect.getXMax() && getXMax() > rect.getXMin()) {
        if (getYMin() < rect.getYMax() && getYMax() > rect.getYMax()) {
            return true;
        }
    }
    return false;
}

} //Math 
} //Framework 
