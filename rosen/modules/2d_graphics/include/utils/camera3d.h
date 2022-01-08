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

#ifndef CAMERA3D_H
#define CAMERA3D_H

#include "engine_adapter/impl_interface/camera_impl.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Camera3D {
public:
    Camera3D() noexcept;
    ~Camera3D();

    void Save();
    void Restore();

    void Translate(scalar x, scalar y, scalar z);
    void RotateXDegrees(scalar deg);
    void RotateYDegrees(scalar deg);
    void RotateZDegrees(scalar deg);

    void SetCameraPos(scalar x, scalar y, scalar z);
    scalar GetCameraPosX() const;
    scalar GetCameraPosY() const;
    scalar GetCameraPosZ() const;

    void ApplyToMatrix(Matrix& m);
private:
    std::unique_ptr<CameraImpl> impl_;
};
}
}
}
#endif