/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PLATFORM_OHOS_BACKEND_SURFACE_BUFFER_UTILS_H
#define RENDER_SERVICE_BASE_PLATFORM_OHOS_BACKEND_SURFACE_BUFFER_UTILS_H

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "surface_buffer.h"
#include "image/image.h"

namespace OHOS::Rosen {
class SurfaceBufferUtils {
public:
    SurfaceBufferUtils() = delete;
    ~SurfaceBufferUtils() = delete;

    static sptr<SurfaceBuffer> CreateCanvasSurfaceBuffer(pid_t pid, int width, int height);
    static Drawing::BackendTexture ConvertSurfaceBufferToBackendTexture(sptr<SurfaceBuffer> buffer);

private:
    static void SetCanvasSurfaceBufferName(sptr<SurfaceBuffer> buffer, pid_t pid, int width, int height);
};
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK
#endif // RENDER_SERVICE_BASE_PLATFORM_OHOS_BACKEND_SURFACE_BUFFER_UTILS_H