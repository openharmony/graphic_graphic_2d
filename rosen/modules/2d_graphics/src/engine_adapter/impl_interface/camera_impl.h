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

#ifndef CAMERA_IMPL_H
#define CAMERA_IMPL_H

#include "base_impl.h"

#include "utils/matrix.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class CameraImpl : public BaseImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::BASE_INTERFACE;
    CameraImpl() {}
    virtual ~CameraImpl() {}
    AdapterType GetType() const override
    {
        return AdapterType::BASE_INTERFACE;
    }

    virtual void Save() = 0;
    virtual void Restore() = 0;

    virtual void Translate(scalar x, scalar y, scalar z) = 0;
    virtual void RotateX(scalar deg) = 0;
    virtual void RotateY(scalar deg) = 0;
    virtual void RotateZ(scalar deg) = 0;

    virtual void SetCameraPos(scalar x, scalar y, scalar z) = 0;
    virtual scalar GetCameraPosX() const = 0;
    virtual scalar GetCameraPosY() const = 0;
    virtual scalar GetCameraPosZ() const = 0;

    virtual void ApplyToMatrix(Matrix& m) = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif