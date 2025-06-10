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

#ifndef RS_IMAGE_MANAGER_H
#define RS_IMAGE_MANAGER_H

#include <memory>
#include <mutex>
#include <queue>
#include <variant>
#include <unordered_map>
#include <surface.h>

#ifdef RS_ENABLE_VK
#include "native_window.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "common/rs_common_def.h"
#include "draw/surface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#endif // RS_ENABLE_VK

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES/gl.h"
#include "sync_fence.h"

#include "render_context/render_context.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {
class ImageResource {
public:
    virtual ~ImageResource() = default;

    pid_t GetThreadIndex() const
    {
        return threadIndex_;
    }

    void SetThreadIndex(const pid_t threadIndex)
    {
        threadIndex_ = threadIndex;
    }

#ifdef RS_ENABLE_VK
    // Vulkan virtual functions
    virtual const Drawing::BackendTexture& GetBackendTexture() const
    {
        static Drawing::BackendTexture invalidBackendTexture;
        return invalidBackendTexture;
    }
    virtual NativeBufferUtils::VulkanCleanupHelper* RefCleanupHelper() { return nullptr; }
#endif // RS_ENABLE_VK

    // EGL virtual functions
    virtual GLuint GetTextureId() const { return 0; }

protected:
    ImageResource() = default;
    pid_t threadIndex_;
};

class RSImageManager {
public:
    static std::shared_ptr<RSImageManager> Create(std::shared_ptr<RenderContext>& renderContext);

    virtual ~RSImageManager() = default;

    virtual void UnMapImageFromSurfaceBuffer(int32_t seqNum) = 0;
    virtual std::shared_ptr<Drawing::Image> CreateImageFromBuffer(
        RSPaintFilterCanvas& canvas, const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& acquireFence,
        const uint32_t threadIndex, const std::shared_ptr<Drawing::ColorSpace>& drawingColorSpace) = 0;

    // Vulkan specific functions
#ifdef RS_ENABLE_VK
    virtual void DumpVkImageInfo(std::string &dumpString) { return; }
    virtual std::shared_ptr<ImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer> buffer,
        const sptr<SyncFence>& acquireFence) { return nullptr; }
#endif // RS_ENABLE_VK

    // EGL specific functions
    virtual void ShrinkCachesIfNeeded(bool isForUniRedraw = false) { return; }
    virtual std::unique_ptr<ImageResource> CreateImageCacheFromBuffer(const sptr<OHOS::SurfaceBuffer>& buffer,
        const sptr<SyncFence>& acquireFence) { return nullptr; }
protected:
    RSImageManager() = default;
    mutable std::mutex opMutex_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_IMAGE_MANAGER_H
