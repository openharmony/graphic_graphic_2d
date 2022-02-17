/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef MATRIX_IMPL_H
#define MATRIX_IMPL_H

#include <vector>

#include "base_impl.h"

#include "utils/point.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Matrix;
class MatrixImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    MatrixImpl() {}
    virtual ~MatrixImpl() {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void Rotate(scalar degree, scalar px, scalar py) = 0;
    virtual void Translate(scalar dx, scalar dy) = 0;
    virtual void Scale(scalar sx, scalar sy, scalar px, scalar py) = 0;
    virtual void Multiply(const Matrix& a, const Matrix& b) = 0;
    virtual bool Equals(const Matrix& a, const Matrix& b) const = 0;
    virtual void SetMatrix(scalar scaleX, scalar skewX, scalar transX, scalar skewY, scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2) = 0;
    virtual void MapPoints(std::vector<Point>& dst, const std::vector<Point>& src, int count) const;
    virtual scalar Get(int index) const = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
