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

#ifndef SURFACE_OHOS_GL_H
#define SURFACE_OHOS_GL_H

#include <surface.h>
#include "surface.h"
#include "surface_ohos.h"
#include "surface_frame_ohos_gl.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class SurfaceOhosGl : public SurfaceOhos {
public:
    explicit SurfaceOhosGl(const sptr<Surface>& producer);
    ~SurfaceOhosGl() override;

    bool IsValid() const override
    {
        return producer_ != nullptr;
    }

    std::unique_ptr<SurfaceFrame> RequestFrame(int32_t width, int32_t height) override;
    std::unique_ptr<SurfaceFrame> NativeRequestFrame(int32_t width, int32_t height) override;
    bool FlushFrame(std::unique_ptr<SurfaceFrame>& frame) override;
    bool NativeFlushFrame(std::unique_ptr<SurfaceFrame>& frame) override;
    SkCanvas* GetSkCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
    Drawing::Canvas* GetCanvas(std::unique_ptr<SurfaceFrame>& frame) override;
private:
    std::unique_ptr<SurfaceFrameOhosGl> frame_;
};
} // namespace Rosen
} // namespace OHOS
#endif
