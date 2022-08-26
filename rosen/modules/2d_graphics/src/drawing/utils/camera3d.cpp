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

#include "utils/camera3d.h"

#include "engine_adapter/impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Camera3D::Camera3D() noexcept : impl_(ImplFactory::CreateCameraImpl()) {}
Camera3D::~Camera3D() {}

void Camera3D::Save()
{
    impl_->Save();
}

void Camera3D::Restore()
{
    impl_->Restore();
}

void Camera3D::Translate(scalar x, scalar y, scalar z)
{
    impl_->Translate(x, y, z);
}

void Camera3D::RotateXDegrees(scalar deg)
{
    impl_->RotateX(deg);
}

void Camera3D::RotateYDegrees(scalar deg)
{
    impl_->RotateY(deg);
}

void Camera3D::RotateZDegrees(scalar deg)
{
    impl_->RotateZ(deg);
}

void Camera3D::SetCameraPos(scalar x, scalar y, scalar z)
{
    impl_->SetCameraPos(x, y, z);
}

scalar Camera3D::GetCameraPosX() const
{
    return impl_->GetCameraPosX();
}

scalar Camera3D::GetCameraPosY() const
{
    return impl_->GetCameraPosY();
}

scalar Camera3D::GetCameraPosZ() const
{
    return impl_->GetCameraPosZ();
}

void Camera3D::ApplyToMatrix(Matrix& m)
{
    impl_->ApplyToMatrix(m);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS