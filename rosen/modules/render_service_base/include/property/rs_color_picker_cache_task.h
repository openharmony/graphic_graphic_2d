/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H

#include <condition_variable>
#include <mutex>
#ifdef IS_OHOS
#include "event_handler.h"
#endif
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkImageInfo.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#include "include/gpu/GrBackendSurface.h"
#endif

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_color_picker.h"
#include "include/effects/SkRuntimeEffect.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSColorPickerCacheTask final {
public:
    static const bool ColorPickerPartialEnabled;
    static bool PostPartialColorPickerTask(std::shared_ptr<RSColorPickerCacheTask> colorPickerTask,
        sk_sp<SkImage> imageSnapshot);
    static std::function<void(std::weak_ptr<RSColorPickerCacheTask>)> postColorPickerTask;

    RSColorPickerCacheTask() = default;
    ~RSColorPickerCacheTask() = default;
#ifndef USE_ROSEN_DRAWING
    bool InitSurface(GrRecordingContext* grContext);
#endif
    bool Render();

    CacheProcessStatus GetStatus() const
    {
        return cacheProcessStatus_.load();
    }

    void SetStatus(CacheProcessStatus cacheProcessStatus)
    {
        cacheProcessStatus_.store(cacheProcessStatus);
    }

#ifndef USE_ROSEN_DRAWING
    bool InitTask(const sk_sp<SkImage> imageSnapshot);
#endif

    void Reset()
    {
        imageSnapshotCache_.reset();
    }

    void ResetGrContext();

    void Notify()
    {
        cvParallelRender_.notify_one();
    }

    bool GetColor(RSColor& color);

    bool GpuScaleImage(std::shared_ptr<RSPaintFilterCanvas> cacheCanvas,
        const sk_sp<SkImage> threadImage, std::shared_ptr<SkPixmap>& dst);
#ifdef IS_OHOS
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetHandler()
    {
        return handler_;
    }
#endif
    void CalculateColorAverage(RSColor& ColorCur);

    void GetColorAverage(RSColor& color);

    bool GetFirstGetColorFinished();

    void SetDeviceSize(int& deviceWidth, int& deviceHeight);

    void SetIsShadow(bool isShadow);

    void SetShadowColorStrategy(int shadowColorStrategy);

    bool GetDeviceSize(int& deviceWidth, int& deviceHeight) const;


private:
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> cacheSurface_ = nullptr;
    GrBackendTexture cacheBackendTexture_;
    SkISize surfaceSize_;
#endif
    bool valid_ = false;
    bool firstGetColorFinished_ = false;
    bool isShadow_ = false;
    int shadowColorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    uint32_t* pixelPtr_ = nullptr;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::WAITING;
    sk_sp<SkImage> imageSnapshotCache_ = nullptr;
    RSColor color_;
    std::vector<RSColor> colorArray_;
    std::vector<bool> colorArrayValid_;
    RSColor colorAverage_;
    std::mutex parallelRenderMutex_;
    std::mutex colorMutex_;
    std::condition_variable cvParallelRender_;
    std::optional<int> deviceWidth_;
    std::optional<int> deviceHeight_;
#ifdef IS_OHOS
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H
