#pragma once
// COMPILE: g++ -o quat2EulerTest quat2EulerTest.cpp
#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>

//Code From http://bediyap.com/programming/convert-quaternion-to-euler-rotations/
namespace RDx12
{
    ///////////////////////////////
    // Quaternion struct
    // Simple incomplete quaternion struct for demo purpose
    ///////////////////////////////
    struct Quaternion {
        Quaternion() :x(0), y(0), z(0), w(1) {};
        Quaternion(double x, double y, double z, double w) :x(x), y(y), z(z), w(w) {};

        void normalize() {
            double norm = std::sqrt(x * x + y * y + z * z + w * w);
            x /= norm;
            y /= norm;
            z /= norm;
            w /= norm;
        }

        double norm() {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        double x;
        double y;
        double z;
        double w;

    };

    enum class RotSeq { zyx, zyz, zxy, zxz, yxz, yxy, yzx, yzy, xyz, xyx, xzy, xzx };
    void twoaxisrot(double r11, double r12, double r21, double r31, double r32, double res[]);
    void threeaxisrot(double r11, double r12, double r21, double r31, double r32, double res[]);
    void quaternion2Euler(const Quaternion& q, double res[], RotSeq rotSeq);
    Quaternion operator*(Quaternion& q1, Quaternion& q2);
    std::ostream& operator <<(std::ostream& stream, const Quaternion& q);
    double rad2deg(double rad);

}