/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Matrix {
public:
    // Matrix is a 3x3 float type matrix.
    static constexpr int MATRIX_SIZE = 9;
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
    virtual ~Matrix() {}
    void Rotate(scalar degree, scalar px, scalar py);
    void Translate(scalar dx, scalar dy);
    void Scale(scalar sx, scalar sy, scalar px, scalar py);

    /*
     * @brief         Sets Matrix to Matrix multiplied by Matrix constructed from rotating by degrees about pivot point(0,0).
     * @param degree  Angle of axes relative to upright axes.
     */
    void PreRotate(scalar degree);

    /*
     * @brief     Sets Matrix to Matrix constructed from translation (dx, dy) multiplied by Matrix.
     * @param dx  X-axis translation after applying Matrix.
     * @param dy  Y-axis translation after applying Matrix.
     */
    void PreTranslate(scalar dx, scalar dy);

    /*
     * @brief     Sets Matrix to Matrix multiplied by Matrix constructed from scaling by (sx, sy) about pivot point (0, 0).
     * @param sx  Horizontal scale factor.
     * @param sy  Vertical scale factor.
     */
    void PreScale(scalar sx, scalar sy);

    /*
     * @brief         Sets Matrix to Matrix other multiplied by Matrix.
     * @param other   Matrix on left side of multiply expression.
     */
    void PreConcat(const Matrix& other);

    /*
     * @brief           Sets inverse to the inverse of Matrix.
     * @param inverse   To store to inverse Matrix, may be nullptr.
     * @return          Return true if Matrix can be inverted, otherwise return false.
     */
    bool Invert(Matrix& inverse) const;
    Matrix operator*(const Matrix& other);

    /*
     * @brief         Compares Matrix and other.
     * @param other   To compare Matrix.
     * @return        True if Matrix and other are numerically equal.
     */
    bool operator==(const Matrix& other) const;
    void SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2);
    void MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, uint32_t count) const;

    /*
     * @brief       Sets dst to bounds of src corners mapped by Matrix.
     * @param dst   Storage for bounds of map.
     * @param src   To map.
     * @return      True if dst is equivalent to mapped src.
     */
    bool MapRect(Rect& dst, const Rect& src) const;

    /*
     * @brief         Sets Matrix value.
     * @param index   One of Index.
     * @param value   Scalar to store in Matrix.
     */
    void Set(Index index, scalar value);
    scalar Get(int index) const;

    /*
     * @brief         Copies nine scalar values contained by Matrix into buffer.
     * @param buffer  Storage for nine scalar values
     */
    void GetAll(Buffer& buffer) const;
    template<typename T>
    const std::shared_ptr<T> GetImpl() const
    {
        return matrixImplPtr->DowncastingTo<T>();
    }

private:
    std::shared_ptr<MatrixImpl> matrixImplPtr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
