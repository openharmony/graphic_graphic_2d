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

#include "skia_camera.h"

#include "skia_matrix.h"
namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaCamera::SkiaCamera() noexcept : view_() {}

void SkiaCamera::Save()
{
    view_.save();
}

void SkiaCamera::Restore()
{
    view_.restore();
}

void SkiaCamera::Translate(scalar x, scalar y, scalar z)
{
    view_.translate(x, y, z);
}

void SkiaCamera::RotateX(scalar deg)
{
    view_.rotateX(deg);
}

void SkiaCamera::RotateY(scalar deg)
{
    view_.rotateY(deg);
}

void SkiaCamera::RotateZ(scalar deg)
{
    view_.rotateZ(deg);
}

void SkiaCamera::SetCameraPos(scalar x, scalar y, scalar z)
{
#if defined(USE_CANVASKIT0310_SKIA)
    return;
#else
    view_.setCameraLocation(x, y, z);
#endif
}

scalar SkiaCamera::GetCameraPosX() const
{
#if defined(USE_CANVASKIT0310_SKIA)
    return 0;
#else
    return view_.getCameraLocationX();
#endif
}

scalar SkiaCamera::GetCameraPosY() const
{
#if defined(USE_CANVASKIT0310_SKIA)
    return 0;
#else
    return view_.getCameraLocationY();
#endif
}

scalar SkiaCamera::GetCameraPosZ() const
{
#if defined(USE_CANVASKIT0310_SKIA)
    return 0;
#else
    return view_.getCameraLocationZ();
#endif
}

void SkiaCamera::ApplyToMatrix(Matrix& m)
{
    SkMatrix& matrix = m.GetImpl<SkiaMatrix>()->ExportMatrix();
    view_.getMatrix(&matrix);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS