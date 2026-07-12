#include "pch.h"
#include "CameraMatrices.h"

namespace Core
{
    Matrix CreateLookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
    {
        Vector3 zaxis = (target - eye).Normalized();
        Vector3 xaxis = up.Cross(zaxis).Normalized();
        Vector3 yaxis = zaxis.Cross(xaxis);

        Matrix r;

        r.m[0][0] = xaxis.x;
        r.m[0][1] = yaxis.x;
        r.m[0][2] = zaxis.x;
        r.m[0][3] = 0.0f;

        r.m[1][0] = xaxis.y;
        r.m[1][1] = yaxis.y;
        r.m[1][2] = zaxis.y;
        r.m[1][3] = 0.0f;

        r.m[2][0] = xaxis.z;
        r.m[2][1] = yaxis.z;
        r.m[2][2] = zaxis.z;
        r.m[2][3] = 0.0f;

        r.m[3][0] = -xaxis.Dot(eye);
        r.m[3][1] = -yaxis.Dot(eye);
        r.m[3][2] = -zaxis.Dot(eye);
        r.m[3][3] = 1.0f;

        return r;
    }

    Matrix CreatePerspectiveFov(float fovY, float aspect, float zn, float zf)
    {
        Matrix r{};

        float yScale = 1.0f / std::tan(fovY * 0.5f);
        float xScale = yScale / aspect;

        r.m[0][0] = xScale;
        r.m[0][1] = 0.0f;
        r.m[0][2] = 0.0f;
        r.m[0][3] = 0.0f;

        r.m[1][0] = 0.0f;
        r.m[1][1] = yScale;
        r.m[1][2] = 0.0f;
        r.m[1][3] = 0.0f;

        r.m[2][0] = 0.0f;
        r.m[2][1] = 0.0f;
        r.m[2][2] = zf / (zf - zn);
        r.m[2][3] = 1.0f;

        r.m[3][0] = 0.0f;
        r.m[3][1] = 0.0f;
        r.m[3][2] = (-zn * zf) / (zf - zn);
        r.m[3][3] = 0.0f;

        return r;
    }
}