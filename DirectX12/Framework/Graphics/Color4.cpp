#include "Color4.h"
#include "Framework/Math/MathUtility.h"

namespace Framework {
namespace Graphics {

const Color4 Color4::BLACK = Color4(0.0f, 0.0f, 0.0f, 1.0f);
const Color4 Color4::WHITE = Color4(1.0f, 1.0f, 1.0f, 1.0f);

Color4::Color4()
    :r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }

Color4::Color4(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a) { }

Color4::Color4(float color[4])
    : r(color[0]), g(color[1]), b(color[2]), a(color[3]) { }

Color4::Color4(const Color4& c)
    : Color4(c.r, c.g, c.b, c.a) { }

Color4::~Color4() { }

Color4& Color4::operator=(const Color4& c) {
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    return *this;
}

bool Color4::operator==(const Color4& c) {
    return (r == c.r && g == c.g && b == c.b && a == c.a);
}

bool Color4::operator!=(const Color4& c) {
    return (r != c.r || g != c.g || b != c.b || a != c.a);
}

Color4 Color4::operator+() {
    return Color4(r, g, b, a);
}

Color4 Color4::operator-() {
    return Color4(1.0f - r, 1.0f - g, 1.0f - b, a);
}

void Color4::get(float c[4]) const {
    c[0] = r;
    c[1] = g;
    c[2] = b;
    c[3] = a;
}

std::array<float, 4> Color4::get() const {
    return std::array<float, 4>{r, g, b, a};
}

Color4 Color4::lerp(const Color4& a, const Color4& b, float t) {
    t = Math::MathUtil::clamp(t, 0.0f, 1.0f);
    float oneMinusT = 1.0f - t;
    float nr = a.r * oneMinusT + b.r * t;
    float ng = a.g * oneMinusT + b.g * t;
    float nb = a.b * oneMinusT + b.b * t;
    float na = a.a * oneMinusT + b.a * t;
    return Color4(nr, ng, nb, na);
}

} //Graphics 
} //Framework 
