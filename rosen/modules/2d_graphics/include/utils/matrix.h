/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <array>
#include <iostream>

#include "drawing/engine_adapter/impl_interface/matrix_impl.h"
#include "utils/drawing_macros.h"
#include "utils/matrix44.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class ScaleToFit {
    FILL_SCALETOFIT,
    START_SCALETOFIT,
    CENTER_SCALETOFIT,
    END_SCALETOFIT,
};

class DRAWING_API Matrix {
public:
    // Matrix is a 3x3 float type matrix.
    static constexpr int MATRIX_SIZE = 9;
    static constexpr size_t ROW0 = 0;
    static constexpr size_t ROW1 = 1;
    static constexpr size_t ROW2 = 2;
    static constexpr size_t ROW3 = 3;
    static constexpr size_t COL0 = 0;
    static constexpr size_t COL1 = 1;
    static constexpr size_t COL2 = 2;
    static constexpr size_t COL3 = 3;
    using Buffer = std::array<scalar, MATRIX_SIZE>;

    enum Index {
        SCALE_X,
        SKEW_X,
        TRANS_X,
        SKEW_Y,
        SCALE_Y,
        TRANS_Y,
        PERSP_0,
        PERSP_1,
        PERSP_2,
    };

    Matrix();
    Matrix(const Matrix& matrix);
    Matrix& operator=(const Matrix& matrix);
    virtual ~Matrix() {}
    static Matrix Skew(scalar kx, scalar ky)
    {
        Matrix m;
        m.SetSkew(kx, ky);
        return m;
    }

    void Rotate(scalar degree, scalar px, scalar py);
    void Translate(scalar dx, scalar dy);
    void Scale(scalar sx, scalar sy, scalar px, scalar py);
    void SetScale(scalar sx, scalar sy);
    void SetScaleTranslate(scalar sx, scalar sy, scalar dx, scalar dy);
    void SetSkew(scalar kx, scalar ky);
    void SetSkew(scalar kx, scalar ky, scalar px, scalar py);
    /**
     * @brief         Sets Matrix to Matrix multiplied by Matrix constructed
     *                from rotating by degrees about pivot point(0,0).
     * @param degree  Angle of axes relative to upright axes.
     */
    void PreRotate(scalar degree);

    void PostRotate(scalar degree);

    /**
     * @brief         Sets Matrix to Matrix constructed from rotating by degrees
     *                about pivot point(px,py), multiplied by Matrix.
     * @param degree  Angle of axes relative to upright axes.
     * @param px      pivot on x-axis
     * @param px      pivot on y-axis
     */
    void PostRotate(scalar degree, scalar px, scalar py);

    /**
     * @brief     Sets Matrix to Matrix constructed from translation (dx, dy) multiplied by Matrix.
     * @param dx  X-axis translation after applying Matrix.
     * @param dy  Y-axis translation after applying Matrix.
     */
    void PreTranslate(scalar dx, scalar dy);

    void PostTranslate(scalar dx, scalar dy);

    /**
     * @brief     Sets Matrix to Matrix multiplied by Matrix constructed
     *            from scaling by (sx, sy) about pivot point (0, 0).
     * @param sx  Horizontal scale factor.
     * @param sy  Vertical scale factor.
     */
    void PreScale(scalar sx, scalar sy);

    void PostScale(scalar sx, scalar sy);

    /**
     * @brief     Sets Matrix to Matrix multiplied by Matrix constructed
     *            from skewing by (kx, ky) about pivot point (0, 0).
     * @param kx  Horizontal skew factor.
     * @param ky  Vertical skew factor.
     */
    void PreSkew(scalar kx, scalar ky);

    /**
     * @brief     Sets Matrix to Matrix constructed from skewing by (kx, ky)
     *            about pivot point(0, 0), multiplied by Matrix.
     * @param kx  Horizontal skew factor.
     * @param ky  Vertical skew factor.
     */
    void PostSkew(scalar kx, scalar ky);

    /**
     * @brief         Sets Matrix to Matrix multiplied by Matrix constructed
     *                from skewing by (kx, ky) about pivot point (px, py).
     * @param sx      horizontal skew factor
     * @param sy      vertical skew factor
     * @param kx      pivot on x-axis
     * @param ky      pivot on y-axis
     */
    void PreSkew(scalar kx, scalar ky, scalar px, scalar py);

    /**
     * @brief         Sets Matrix to Matrix constructed from skewing by (kx, ky)
     *                about pivot point(px,py), multiplied by Matrix.
     * @param sx      horizontal skew factor
     * @param sy      vertical skew factor
     * @param kx      pivot on x-axis
     * @param ky      pivot on y-axis
     */
    void PostSkew(scalar kx, scalar ky, scalar px, scalar py);
    
    /**
     * @brief         Sets Matrix to Matrix constructed from scaling by (sx, sy)
     *                about pivot point(px,py), multiplied by Matrix.
     * @param sx      horizontal scale factor
     * @param sy      vertical scale factor
     * @param px      pivot on x-axis
     * @param px      pivot on y-axis
     */
    void PostScale(scalar sx, scalar sy, scalar px, scalar py);

    /**
     * @brief         Sets Matrix to Matrix other multiplied by Matrix.
     * @param other   Matrix on left side of multiply expression.
     */
    void PreConcat(const Matrix& other);

    /**
     * @brief            Sets Matrix to Matrix other multiplied by Matrix44.
     * @param matrix44   Matrix on left side of multiply expression.
     */
    void PreConcat(const Matrix44& matrix44);

    /**
     * @brief         Sets Matrix to Matrix other multiplied by Matrix.
     * @param other   Matrix on right side of multiply expression.
     */
    void PostConcat(const Matrix& other);

    /**
     * @brief         Sets Matrix to Matrix other multiplied by Matrix44.
     * @param other   Matrix on right side of multiply expression.
     */
    void PostConcat(const Matrix44& matrix44);

    /**
     * @brief           Sets inverse to the inverse of Matrix.
     * @param inverse   To store to inverse Matrix, may be nullptr.
     * @return          Return true if Matrix can be inverted, otherwise return false.
     */
    bool Invert(Matrix& inverse) const;
    Matrix operator*(const Matrix& other);

    /**
     * @brief         Compares Matrix and other.
     * @param other   To compare Matrix.
     * @return        True if Matrix and other are numerically equal.
     */
    bool operator==(const Matrix& other) const;
    void SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2);

    /**
     * @brief Sets matrix to scale and translate src rect to dst rect.
     *
     * @param src Rect to map from
     * @param dst Rect to map to
     * @param stf Describes how matrix is constructed to map one rect to another
     * @return Returns true if dst is empty, and sets matrix to:
               | 0 0 0 |
               | 0 0 0 |
               | 0 0 1 |.
     */
    bool SetRectToRect(const Rect& src, const Rect& dst, ScaleToFit stf);
    
    void MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const;

    /**
     * @brief       Sets dst to bounds of src corners mapped by Matrix.
     * @param dst   Storage for bounds of map.
     * @param src   To map.
     * @return      True if dst is equivalent to mapped src.
     */
    bool MapRect(Rect& dst, const Rect& src) const;

    /**
     * @brief       Sets Matrix to map src to dst. count must be zero or greater, and four or less.
     * @param src   Point to map from
     * @param dst   Point to map to
     * @param count Number of Point in src and dst
     * @return      True if Matrix was constructed successfully
     */
    bool SetPolyToPoly(const Point src[], const Point dst[], uint32_t count);

    /**
     * @brief         Sets Matrix value.
     * @param index   One of Index.
     * @param value   Scalar to store in Matrix.
     */
    void Set(Index index, scalar value);
    scalar Get(int index) const;

    /**
     * @brief         Copies nine scalar values contained by Matrix into buffer.
     * @param buffer  Storage for nine scalar values
     */
    void GetAll(Buffer& buffer) const;

    /**
     * @brief         Copies nine scalar values contained by Matrix from buffer.
     * @param buffer  Storage for nine scalar values
     */
    void SetAll(Buffer& buffer);

    template<typename T>
    T* GetImpl() const
    {
        return matrixImplPtr->DowncastingTo<T>();
    }

    template<typename T>
    const T* GetImplPtr() const
    {
        return reinterpret_cast<const T*>(matrixImplPtr.get());
    }

    /**
     * @brief Returns true if matrix is Identity. Identity matrix is:
     *    | 1 0 0 |
     *    | 0 1 0 |
     *    | 0 0 1 |
     * @return true if is identity
     */
    bool IsIdentity() const;

    void PreRotate(scalar degree, scalar px, scalar py);
    void PreScale(scalar sx, scalar sy, scalar px, scalar py);
    void Reset();

    bool GetMinMaxScales(scalar scaleFactors[2]);
    bool HasPerspective() const;

    void Swap(Matrix& target);

private:
    std::shared_ptr<MatrixImpl> matrixImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
