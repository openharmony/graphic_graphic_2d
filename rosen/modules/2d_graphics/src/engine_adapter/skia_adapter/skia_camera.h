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

#ifndef SKIA_CAMERA_H
#define SKIA_CAMERA_H

#include "include/utils/SkCamera.h"

#include "engine_adapter/impl_interface/camera_impl.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaCamera : public CameraImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaCamera() noexcept;
    ~SkiaCamera() {}
    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    void Save() override;
    void Restore() override;

    void Translate(scalar x, scalar y, scalar z) override;
    void RotateX(scalar deg) override;
    void RotateY(scalar deg) override;
    void RotateZ(scalar deg) override;

    void SetCameraPos(scalar x, scalar y, scalar z) override;
    scalar GetCameraPosX() const override;
    scalar GetCameraPosY() const override;
    scalar GetCameraPosZ() const override;

    void ApplyToMatrix(Matrix& m) override;

private:
    Sk3DView view_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif