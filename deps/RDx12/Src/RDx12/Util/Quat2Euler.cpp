#include "../Include/RDx12/Util/Quat2Euler.h"

namespace RDx12
{
    ///////////////////////////////
    // Quaternion to Euler
    ///////////////////////////////

    void twoaxisrot(double r11, double r12, double r21, double r31, double r32, double res[]) {
        res[0] = atan2(r11, r12);
        res[1] = acos(r21);
        res[2] = atan2(r31, r32);
    }

    void threeaxisrot(double r11, double r12, double r21, double r31, double r32, double res[]) {
        res[0] = atan2(r31, r32);
        res[1] = asin(r21);
        res[2] = atan2(r11, r12);
    }

    // note: 
    // return values of res[] depends on rotSeq.
    // i.e.
    // for rotSeq zyx, 
    // x = res[0], y = res[1], z = res[2]
    // for rotSeq xyz
    // z = res[0], y = res[1], x = res[2]
    // ...
    void quaternion2Euler(const Quaternion& q, double res[], RotSeq rotSeq)
    {
        switch (rotSeq) {
        case RotSeq::zyx:
            threeaxisrot(2 * (q.x * q.y + q.w * q.z),
                q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                -2 * (q.x * q.z - q.w * q.y),
                2 * (q.y * q.z + q.w * q.x),
                q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                res);
            break;

        case RotSeq::zyz:
            twoaxisrot(2 * (q.y * q.z - q.w * q.x),
                2 * (q.x * q.z + q.w * q.y),
                q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                2 * (q.y * q.z + q.w * q.x),
                -2 * (q.x * q.z - q.w * q.y),
                res);
            break;

        case RotSeq::zxy:
            threeaxisrot(-2 * (q.x * q.y - q.w * q.z),
                q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                2 * (q.y * q.z + q.w * q.x),
                -2 * (q.x * q.z - q.w * q.y),
                q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                res);
            break;

        case RotSeq::zxz:
            twoaxisrot(2 * (q.x * q.z + q.w * q.y),
                -2 * (q.y * q.z - q.w * q.x),
                q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                2 * (q.x * q.z - q.w * q.y),
                2 * (q.y * q.z + q.w * q.x),
                res);
            break;

        case RotSeq::yxz:
            threeaxisrot(2 * (q.x * q.z + q.w * q.y),
                q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                -2 * (q.y * q.z - q.w * q.x),
                2 * (q.x * q.y + q.w * q.z),
                q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                res);
            break;

        case RotSeq::yxy:
            twoaxisrot(2 * (q.x * q.y - q.w * q.z),
                2 * (q.y * q.z + q.w * q.x),
                q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                2 * (q.x * q.y + q.w * q.z),
                -2 * (q.y * q.z - q.w * q.x),
                res);
            break;

        case RotSeq::yzx:
            threeaxisrot(-2 * (q.x * q.z - q.w * q.y),
                q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                2 * (q.x * q.y + q.w * q.z),
                -2 * (q.y * q.z - q.w * q.x),
                q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                res);
            break;

        case RotSeq::yzy:
            twoaxisrot(2 * (q.y * q.z + q.w * q.x),
                -2 * (q.x * q.y - q.w * q.z),
                q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                2 * (q.y * q.z - q.w * q.x),
                2 * (q.x * q.y + q.w * q.z),
                res);
            break;

        case RotSeq::xyz:
            threeaxisrot(-2 * (q.y * q.z - q.w * q.x),
                q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                2 * (q.x * q.z + q.w * q.y),
                -2 * (q.x * q.y - q.w * q.z),
                q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                res);
            break;

        case RotSeq::xyx:
            twoaxisrot(2 * (q.x * q.y + q.w * q.z),
                -2 * (q.x * q.z - q.w * q.y),
                q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                2 * (q.x * q.y - q.w * q.z),
                2 * (q.x * q.z + q.w * q.y),
                res);
            break;

        case RotSeq::xzy:
            threeaxisrot(2 * (q.y * q.z + q.w * q.x),
                q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                -2 * (q.x * q.y - q.w * q.z),
                2 * (q.x * q.z + q.w * q.y),
                q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                res);
            break;

        case RotSeq::xzx:
            twoaxisrot(2 * (q.x * q.z - q.w * q.y),
                2 * (q.x * q.y + q.w * q.z),
                q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                2 * (q.x * q.z + q.w * q.y),
                -2 * (q.x * q.y - q.w * q.z),
                res);
            break;
        default:
            std::cout << "Unknown rotation sequence" << std::endl;
            break;
        }
    }

    ///////////////////////////////
    // Helper functions
    ///////////////////////////////
    Quaternion operator*(Quaternion& q1, Quaternion& q2) {
        Quaternion q;
        q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
        q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
        q.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
        q.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
        return q;
    }

    double rad2deg(double rad) {
        return rad * 180.0 / M_PI;
    }

}