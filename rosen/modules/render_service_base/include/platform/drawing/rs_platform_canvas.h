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

#ifndef FOUNDATION_ROSEN_SURFACE_H
#define FOUNDATION_ROSEN_SURFACE_H

#include <surface.h>
#include <mutex>

#include "common/rs_macros.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

namespace OHOS{
namespace Rosen{
class RS_EXPORT PlatformCanvas {
public:
    explicit PlatformCanvas();
    SkCanvas* AcquireCanvas();
    void FlushBuffer();
    void SetSurfaceSize(int width, int height);
    void SetSurface(Surface* surface);
    void Disable();
private:
    std::mutex mutex_;
    bool enable_ = true;
    BufferRequestConfig config_;
    sptr<Surface> surface_;
    sptr<SurfaceBuffer> buffer_;

    sk_sp<SkSurface> sk_surface_;
};
} // namespace Rosen
} // namespace OHOS
#endif //FOUNDATION_ROSEN_SURFACE_H
