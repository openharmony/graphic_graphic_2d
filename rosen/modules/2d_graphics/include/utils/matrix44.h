/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef MATRIX44_H
#define MATRIX44_H

#include "impl_interface/matrix44_impl.h"
#include "utils/drawing_macros.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DRAWING_API Matrix44 {
public:
    // Matrix44 is a 4x4 float type matrix.
    constexpr static int MATRIX44_SIZE = 16;

    Matrix44();
    virtual ~Matrix44() {}

    /**
     * @brief     Sets Matrix44 to translate by (dx, dy, dz).
     * @param dx  horizontal translation.
     * @param dy  vertical translation.
     * @param dz  z-axis translation.
     */
    void Translate(scalar dx, scalar dy, scalar dz);

    /**
     * @brief     Sets Matrix44 to scale by sx, sy and sz about pivot point at (0, 0, 0).
     * @param sx  horizontal scale factor.
     * @param sy  vertical scale factor.
     * @param sz  z-axis scale factor.
     */
    void Scale(scalar sx, scalar sy, scalar sz);

    void PreTranslate(scalar dx, scalar dy, scalar dz = 0);

    void PostTranslate(scalar dx, scalar dy, scalar dz = 0);

    void PreScale(scalar sx, scalar sy, scalar sz = 1);

    void SetCol(int column, scalar x, scalar y, scalar z, scalar w);
    /**
     * @brief        Gets new Matrix44 to Matrix44  multiplied by Matrix44 other.
     * @param other  on right side of multiply expression.
     * @return       A new calculated Matrix44.
     */
    Matrix44 operator*(const Matrix44& other);

    /**
     * @brief   Converts the Matrix44 to Matrix.
     * @return  A Matrix which converts by Matrix44.
     */
    explicit operator Matrix() const;

    /**
     * @brief         Sets Matrix44 to sixteen values in buffer.
     * @param buffer  a [col][row] array. eg. buffer[0] maps to m00, buffer[1] maps to m10
     */
    using Buffer = std::array<scalar, MATRIX44_SIZE>;
    void SetMatrix44ColMajor(const Buffer& buffer);
    void SetMatrix44RowMajor(const Buffer& buffer);

    /**
     * @brief   Get the adaptation layer instance, called in the adaptation layer.
     * @return  Adaptation Layer instance.
     */
    template<typename T>
    T* GetImpl() const
    {
        return impl_->DowncastingTo<T>();
    }
private:
    std::shared_ptr<Matrix44Impl> impl_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif