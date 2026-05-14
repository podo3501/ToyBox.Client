#pragma once

namespace Core::Math
{
    class Matrix
    {
    public:
        float m[4][4];

    public:
        Matrix();
        Matrix(const Matrix& rhs);
        Matrix& operator=(const Matrix& rhs);

        static Matrix Identity();

        static Matrix Translation(float x, float y, float z);
        static Matrix Scale(float x, float y, float z);

        static Matrix RotationX(float rad);
        static Matrix RotationY(float rad);
        static Matrix RotationZ(float rad);

        static Matrix TRS(const Matrix& t, const Matrix& r, const Matrix& s);

        Matrix operator*(const Matrix& rhs) const;
        Matrix& operator*=(const Matrix& rhs);

        bool operator==(const Matrix& rhs) const;
        bool operator!=(const Matrix& rhs) const;

        void SetIdentity();

        const float* Data() const;
        float* Data();

    };
}
