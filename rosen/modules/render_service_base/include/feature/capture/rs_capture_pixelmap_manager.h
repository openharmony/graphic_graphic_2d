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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CAPTURE_PIXELMAP_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CAPTURE_PIXELMAP_MANAGER_H
#include "pixel_map.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "image/image.h"
namespace OHOS {
namespace Rosen {
using CapturePixelMapMemFunHandle = std::unordered_map<int,
    std::unordered_map<int, std::function<bool(const std::unique_ptr<Media::PixelMap>&, bool)>>>;

using CapturePixelMapCheckFunHandle = std::unordered_map<int,
     std::unordered_map<int, std::function<bool(const Drawing::Rect&, const RSSurfaceCaptureConfig&)>>>;

class RSB_EXPORT RSCapturePixelMapManager {
public:
    RSCapturePixelMapManager(){};
    ~RSCapturePixelMapManager(){};
    static bool SetCapturePixelMapMem(const std::unique_ptr<Media::PixelMap>& pixelmap,
        const SurfaceCaptureType& captureType, const UniRenderEnabledType& uniRenderEnabledType, bool useDma);
    static std::unique_ptr<Media::PixelMap> CreatePixelMap(const Drawing::Rect& areaRect,
        const RSSurfaceCaptureConfig& captureConfig);
    // this func only use service
    static bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
        const std::unique_ptr<Media::PixelMap>& pixelmap, std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr);

    static bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
        const std::unique_ptr<Media::PixelMap>& pixelmap);
    static std::unique_ptr<Media::PixelMap> GetClientCapturePixelMap(const Drawing::Rect& nodeAreaRect,
        const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType& uniRenderEnabledType,
        const Drawing::Rect& specifiedAreaRect = {0.f, 0.f, 0.f, 0.f});

private:
    static CapturePixelMapMemFunHandle funcHandle_;
    static CapturePixelMapCheckFunHandle checkHandle_;
    static bool AttachCommMem(const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedDma = false);
    static bool AttachShareMem(const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedDma = false);
    static bool AttachHeapMem(const std::unique_ptr<Media::PixelMap>& pixelMap);
    static bool AttachUniCommMem(const std::unique_ptr<Media::PixelMap>& pixelMap, bool isUsedDma = false);
    static bool CheckCaptureConfig(const Drawing::Rect& areaRect, const RSSurfaceCaptureConfig& captureConfig,
        const UniRenderEnabledType& uniRenderEnabledType);
    static void LoadCheckFunc();
    static void LoadSetMemFunc();
    static std::unique_ptr<Media::PixelMap> GetCapturePixelMap(const Drawing::Rect& areaRect,
        const RSSurfaceCaptureConfig& captureConfig, const UniRenderEnabledType& uniRenderEnabledType);

    static Drawing::Rect GetCaptureAreaRect(const Drawing::Rect& nodeAreaRect,
        const Drawing::Rect& specifiedAreaRect = {0.f, 0.f, 0.f, 0.f},
        const SurfaceCaptureType& captureType = SurfaceCaptureType::DEFAULT_CAPTURE);
    static bool AttachDmaMem(const std::unique_ptr<Media::PixelMap>& pixelMap);
    static constexpr uint32_t uint32UniRenderDisableType_ =
        static_cast<uint32_t>(UniRenderEnabledType::UNI_RENDER_DISABLED);
    static constexpr uint32_t uint32UniRenderEnableType_ =
        static_cast<uint32_t>(UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    static constexpr uint32_t uint32DefaultCaptureType_ =
        static_cast<uint32_t>(SurfaceCaptureType::DEFAULT_CAPTURE);
    static constexpr uint32_t uint32UiCaptureType_ = static_cast<uint32_t>(SurfaceCaptureType::UICAPTURE);

};

class RSC_EXPORT RSCapturePixelMap {
public:
    RSCapturePixelMap(){};
    ~RSCapturePixelMap(){};
    inline void SetCapturePixelMap(std::unique_ptr<Media::PixelMap> pixelmap) {
        pixelMap_ = std::move(pixelmap);
    }
    std::unique_ptr<Media::PixelMap> pixelMap_ = nullptr;
    const std::unique_ptr<Media::PixelMap>& GetPixelMap() {
        return pixelMap_;
    }
};

} // Rosen
} // OHOS

#endif