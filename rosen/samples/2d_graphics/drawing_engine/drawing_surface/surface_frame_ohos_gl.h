/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SURFACE_FRAME_OHOS_GL_H
#define SURFACE_FRAME_OHOS_GL_H

#include "EGL/egl.h"
#include "EGL/eglext.h"

#include <display_type.h>
#include <surface.h>
#include "surface_type.h"
#include "surface_frame.h"
#include "surface_frame_ohos.h"

namespace OHOS {
namespace Rosen {
class SurfaceFrameOhosGl : public SurfaceFrameOhos {
public:
    SurfaceFrameOhosGl(int32_t width, int32_t height);
    ~SurfaceFrameOhosGl() override;
    void SetDamageRegion(int32_t left, int32_t top, int32_t width, int32_t height) override {};
    void SetSurface(EGLSurface surface);
    void SetColorSpace(ColorGamut colorSpace) override;
    ColorGamut GetColorSpace() const override;
private:
    EGLSurface eglSurface_;
    ColorGamut colorSpace_;
};
} // namespace Rosen
} // namespace OHOS
#endif
