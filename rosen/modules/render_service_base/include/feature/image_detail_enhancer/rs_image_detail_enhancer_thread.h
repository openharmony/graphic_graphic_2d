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

#ifndef RS_IMAGE_DETAIL_ENHANCER_THREAD_H
#define RS_IMAGE_DETAIL_ENHANCER_THREAD_H

#include <functional>
#include <unordered_map>
#include <mutex>

#include "common/rs_common_hook.h"
#include "common/rs_macros.h"
#include "event_handler.h"
#include "pipeline/rs_draw_cmd.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSImageDetailEnhancerThread final {
public:
    static RSImageDetailEnhancerThread& Instance();

    void PostTask(const std::function<void()>& task);
    bool RegisterCallback(const std::function<void(uint64_t)>& callback);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    void ScaleImageAsync(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& dst,
        uint64_t nodeId, uint64_t imageId, const std::shared_ptr<Drawing::Image>& image);
#endif

    void SetOutImage(uint64_t imageId, const std::shared_ptr<Drawing::Image>& image);
    std::shared_ptr<Drawing::Image> GetOutImage(uint64_t imageId) const;
    void ReleaseOutImage(uint64_t imageId);
    void SetProcessStatus(uint64_t imageId, bool flag);
    bool GetProcessStatus(uint64_t imageId) const;
    bool GetEnableStatus() const;
    bool IsEnableImageDetailEnhance(uint64_t nodeId) const;

private:
    RSImageDetailEnhancerThread();
    ~RSImageDetailEnhancerThread() = default;
    RSImageDetailEnhancerThread(const RSImageDetailEnhancerThread&) = delete;
    RSImageDetailEnhancerThread(const RSImageDetailEnhancerThread&&) = delete;
    RSImageDetailEnhancerThread& operator=(const RSImageDetailEnhancerThread&) = delete;
    RSImageDetailEnhancerThread& operator=(const RSImageDetailEnhancerThread&&) = delete;

    void MarkDirty(uint64_t nodeId);
    void ResetStatus(int srcWidth, int srcHeight, int dstWidth, int dstHeight, uint64_t imageId);
    void SetProcessReady(uint64_t imageId, bool flag);
    bool GetProcessReady(uint64_t imageId) const;
    bool IsSizeSupport(int srcWidth, int srcHeight, int dstWidth, int dstHeight);
    bool GetSharpness(RSImageDetailEnhanceAlgoParams& param, float scaleRatio, float& sharpness);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    bool IsTypeSupport(const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::shared_ptr<Drawing::Image> ScaleByHDSampler(int dstWidth, int dstHeight,
        std::shared_ptr<Drawing::Surface>& newSurface, std::shared_ptr<Drawing::Canvas>& newCanvas,
        const std::shared_ptr<Drawing::Image>& srcImage);
    std::shared_ptr<Drawing::Image> ScaleByAAE(sptr<SurfaceBuffer>& dstSurfaceBuffer,
        const std::shared_ptr<Drawing::Image>& srcImage, std::shared_ptr<Drawing::Canvas>& newCanvas);
    void ExecuteTaskAsync(int dstWidth, int dstHeight, const std::shared_ptr<Drawing::Image>& image,
        uint64_t nodeId, uint64_t imageId);
#endif

    bool isParamValidate_ = false;
    RSImageDetailEnhanceParams params_ = {};
    RSImageDetailEnhanceAlgoParams slrParams_{};
    RSImageDetailEnhanceAlgoParams esrParams_{};
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::unordered_map<uint64_t, std::shared_ptr<Drawing::Image>> outImageMap_ = {};
    std::unordered_map<uint64_t, bool> processStatusMap_ = {};
    std::unordered_map<uint64_t, bool> processReadyMap_ = {};
    std::function<void(uint64_t)> callback_{};
    mutable std::mutex mapMutex_{};
};

class RSB_EXPORT DetailEnhancerUtils {
public:
    static DetailEnhancerUtils& Instance();

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    bool SetMemoryName(sptr<SurfaceBuffer>& buffer);
    sptr<SurfaceBuffer> CreateSurfaceBuffer(int width, int height);
    std::shared_ptr<Drawing::Surface> InitSurface(int dstWidth, int dstHeight, sptr<SurfaceBuffer>& dstSurfaceBuffer,
        const std::shared_ptr<Drawing::Image>& image);
    std::shared_ptr<Drawing::Image> MakeImageFromSurfaceBuffer(std::shared_ptr<Drawing::Canvas>& canvas,
        sptr<SurfaceBuffer>& surfaceBuffer, const std::shared_ptr<Drawing::Image>& image);
    Drawing::ColorType GetColorTypeWithVKFormat(VkFormat vkFormat);
#endif
};
} // OHOS
} // Rosen
#endif