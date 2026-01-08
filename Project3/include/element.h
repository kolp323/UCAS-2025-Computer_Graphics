#ifndef ELEMENT_H
#define ELEMENT_H
#include <cmath>
#include <iostream>

// Vec3 类
template<typename T>
class Vec3
{
public:
    T x, y, z;
    // 构造函数
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T a) : x(a), y(a), z(a) {}
    Vec3(T a, T b, T c) : x(a), y(b), z(c) {}
    
    // 基础算术运算符重载
    Vec3 operator * (const T &f) const { return Vec3(x * f, y * f, z * f); }
    Vec3 operator / (const T &f) const { return Vec3(x / f, y / f, z / f); }
    Vec3 operator * (const Vec3 &v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator - (const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator + (const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator - () const { return Vec3(-x, -y, -z); }
    Vec3& operator += (const Vec3 &v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vec3& operator *= (const Vec3 &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }

    // 向量运算：点积、模长、归一化
    T dot(const Vec3 &v) const {
        return x * v.x + y * v.y + z * v.z;
    }
    T length2() const {
        return x * x + y * y + z * z;
    }
    T length() const { 
        return std::sqrt(length2()); 
    }

    Vec3& normalize() {
        T len2 = length2();
        if (len2 > 0) {
            T invLen = 1 / std::sqrt(len2);
            x *= invLen, y *= invLen, z *= invLen;
        }
        return *this;
    }

    Vec3& normal() { 
        return normalize(); 
    }

    // 输出向量信息
    friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v){
        os << "[" << v.x << " " << v.y << " " << v.z << "]";
        return os;
    }
};
typedef Vec3<float> Vec3f;


// Sphere 类
class Sphere
{
public:
    Vec3f center;                           // 球心位置
    float radius, radius2;                  // 半径、半径平方
    Vec3f surfaceColor, emissionColor;      // 表面颜色、自发光颜色
    float transparency, reflectivity;       // 透明度、反射率

    Sphere(
        const Vec3f &c, 
        const float &r, 
        const Vec3f &sc, 
        const float &refl = 0, 
        const float &transp = 0, 
        const Vec3f &ec = 0) : 
        center(c), radius(r), radius2(r * r), surfaceColor(sc), 
        emissionColor(ec), transparency(transp), reflectivity(refl) 
    {}

    // 射线与球体求交逻辑
    // rayorig：光源方向；raydir：光线方向单位向量；t0、t1：返回交点
    bool intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t0, float &t1) const{
    Vec3f l = center - rayorig; // 光源到球心的连线
    float tca = l.dot(raydir); // 投影长度
    if (tca < 0) return false; // 球在射线后方

    float d2 = l.dot(l) - tca * tca; // 垂直距离平方
    if (d2 > radius2) return false; // 距离大于半径，不相交

    float thc = std::sqrt(radius2 - d2); // 到交点的半弦长
    t0 = tca - thc;
    t1 = tca + thc;

    return true;
    }   
};


#endif