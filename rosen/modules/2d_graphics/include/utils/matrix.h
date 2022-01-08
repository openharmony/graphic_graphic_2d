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

#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

#include "engine_adapter/impl_interface/matrix_impl.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Matrix {
public:
    Matrix();
    virtual ~Matrix() {}
    void Rotate(scalar degree, scalar px, scalar py);
    void Translate(scalar dx, scalar dy);
    void Scale(scalar sx, scalar sy, scalar px, scalar py);
    Matrix operator*(const Matrix& other);
    bool operator==(const Matrix& other);
    void SetMatrix(scalar scaleX, scalar skewX,  scalar transX,
        scalar skewY,  scalar scaleY, scalar transY,
        scalar persp0, scalar persp1, scalar persp2);
    scalar Get(int index);
    template<typename T> const std::shared_ptr<T> GetImpl() const { return matrixImplPtr->DowncastingTo<T>(); }
private:
    std::shared_ptr<MatrixImpl> matrixImplPtr;
};
}
}
}
#endif
