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
#include <list>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "common/rs_common_hook.h"
#include "common/rs_macros.h"
#include "event_handler.h"
#include "pipeline/rs_draw_cmd.h"
#include "render/rs_pixel_map_util.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {
struct RSImageParams {
    std::shared_ptr<Media::PixelMap> mPixelMap;
    uint64_t mNodeId;
    const Drawing::Rect mDst;
    uint64_t mUniqueId;
    std::shared_ptr<Drawing::Image> mImage;
    float mMatrixScaleX;
};

class RSB_EXPORT RSImageDetailEnhancerThread final {
public:
    static RSImageDetailEnhancerThread& Instance();

    void PostTask(const std::function<void()>& task);
    bool RegisterCallback(const std::function<void(uint64_t)>& callback);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    void ScaleImageAsync(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& dst,
        uint64_t nodeId, uint64_t imageId, const std::shared_ptr<Drawing::Image>& image);
    void ImageSamplingDump(uint64_t imageId);
#endif
    // used for ScaleImageAsync
    std::shared_ptr<Drawing::Image> EnhanceImageAsync(bool& isScaledImageAsync, const RSImageParams& RSImageParams);
    void SetScaledImage(uint64_t imageId, const std::shared_ptr<Drawing::Image>& image);
    std::shared_ptr<Drawing::Image> GetScaledImage(uint64_t imageId);
    void ReleaseScaledImage(uint64_t imageId);
    void PushImageList(uint64_t imageId, const std::shared_ptr<Drawing::Image>& image);
    void SetProcessStatus(uint64_t imageId, bool flag);
    bool GetProcessStatus(uint64_t imageId) const;
    bool GetEnabled();
    void ReleaseAllScaledImage();
    bool IsPidEnabled(uint64_t nodeId);

private:
    RSImageDetailEnhancerThread();
    ~RSImageDetailEnhancerThread() = default;
    RSImageDetailEnhancerThread(const RSImageDetailEnhancerThread&) = delete;
    RSImageDetailEnhancerThread(const RSImageDetailEnhancerThread&&) = delete;
    RSImageDetailEnhancerThread& operator=(const RSImageDetailEnhancerThread&) = delete;
    RSImageDetailEnhancerThread& operator=(const RSImageDetailEnhancerThread&&) = delete;

    void MarkScaledImageDirty(uint64_t nodeId);
    void ResetStatus(int srcWidth, int srcHeight, int dstWidth, int dstHeight, uint64_t imageId);
    void SetProcessReady(uint64_t imageId, bool flag);
    bool GetProcessReady(uint64_t imageId) const;
    bool IsSizeSupported(int srcWidth, int srcHeight, int dstWidth, int dstHeight);
    bool GetSharpness(RSImageDetailEnhanceAlgoParams& param, float scaleRatio, float& sharpness);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    bool IsTypeSupport(const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::shared_ptr<Drawing::Image> ScaleByHDSampler(int dstWidth, int dstHeight,
        sptr<SurfaceBuffer>& dstSurfaceBuffer, const std::shared_ptr<Drawing::Image>& srcImage);
    std::shared_ptr<Drawing::Image> ScaleByAAE(sptr<SurfaceBuffer>& dstSurfaceBuffer,
        const std::shared_ptr<Drawing::Image>& srcImage);
    void ExecuteTaskAsync(int dstWidth, int dstHeight, const std::shared_ptr<Drawing::Image>& image,
        uint64_t nodeId, uint64_t imageId);
    void DumpImage(const std::shared_ptr<Drawing::Image>& image, uint64_t imageId);
#endif

    bool isParamValidate_ = false;
    bool ifReleaseAllScaledImage_ = false;
    RSImageDetailEnhanceParams params_ = {};
    RSImageDetailEnhanceAlgoParams slrParams_{};
    RSImageDetailEnhanceAlgoParams esrParams_{};
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::list<std::pair<uint64_t, std::shared_ptr<Drawing::Image>>> imageList_ = {};
    std::unordered_map<uint64_t,
        std::list<std::pair<uint64_t, std::shared_ptr<Drawing::Image>>>::iterator> keyMap_ = {};
    std::unordered_map<uint64_t, bool> processStatusMap_ = {};
    std::unordered_map<uint64_t, bool> processReadyMap_ = {};
    std::function<void(uint64_t)> callback_{};
    mutable std::mutex mapMutex_{};
    float curCache_ = 0.0f;
    long long releaseTime_ = 0;
};

class RSB_EXPORT DetailEnhancerUtils {
public:
    static DetailEnhancerUtils& Instance();

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    bool SetMemoryName(sptr<SurfaceBuffer>& buffer);
    sptr<SurfaceBuffer> CreateSurfaceBuffer(int width, int height);
    std::shared_ptr<Drawing::Surface> InitSurface(int dstWidth, int dstHeight, sptr<SurfaceBuffer>& dstSurfaceBuffer,
        const std::shared_ptr<Drawing::Image>& image);
    std::shared_ptr<Drawing::Image> MakeImageFromSurfaceBuffer(sptr<SurfaceBuffer>& surfaceBuffer,
        const std::shared_ptr<Drawing::Image>& image);
    Drawing::ColorType GetColorTypeWithVKFormat(VkFormat vkFormat);
    void SavePixelmapToFile(Drawing::Bitmap& bitmap, const std::string& dst);
#endif

    float GetImageSize(const std::shared_ptr<Drawing::Image>& image) const;
    long long GetCurTime() const;
};
} // OHOS
} // Rosen
#endif