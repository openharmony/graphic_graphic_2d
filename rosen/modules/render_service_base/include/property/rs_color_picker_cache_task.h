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

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "render/rs_color_picker.h"
#include "render/rs_shadow.h"
#include "include/effects/SkRuntimeEffect.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSColorPickerCacheTask final {
public:
    static const bool ColorPickerPartialEnabled;
    static bool PostPartialColorPickerTask(std::shared_ptr<RSColorPickerCacheTask> colorPickerTask,
        std::shared_ptr<Drawing::Image> imageSnapshot);
    static std::function<void(std::weak_ptr<RSColorPickerCacheTask>)> postColorPickerTask;
#ifdef IS_OHOS
    static std::function<void(std::shared_ptr<Drawing::Image> &&,
        std::shared_ptr<Drawing::Surface> &&,
        std::shared_ptr<OHOS::AppExecFwk::EventHandler>,
        std::weak_ptr<std::atomic<bool>>,
        std::weak_ptr<std::mutex>)> saveImgAndSurToRelease;
#endif

    RSColorPickerCacheTask() = default;
    ~RSColorPickerCacheTask()
    {
        ReleaseColorPicker();
    }
    bool InitSurface(Drawing::GPUContext* gpuContext);
    bool Render();

    CacheProcessStatus GetStatus() const
    {
        return cacheProcessStatus_.load();
    }

    void SetStatus(CacheProcessStatus cacheProcessStatus)
    {
        cacheProcessStatus_.store(cacheProcessStatus);
    }

    bool InitTask(const std::shared_ptr<Drawing::Image> imageSnapshot);

    void Reset();

    void Notify()
    {
        cvParallelRender_.notify_one();
    }

    bool GetColor(RSColor& color);

    bool GpuScaleImage(std::shared_ptr<RSPaintFilterCanvas> cacheCanvas,
        const std::shared_ptr<Drawing::Image> threadImage, std::shared_ptr<Drawing::Pixmap>& dst);
#ifdef IS_OHOS
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetHandler()
    {
        return handler_;
    }
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetInitHandler()
    {
        return initHandler_;
    }
#endif
    void CalculateColorAverage(RSColor& ColorCur);

    void GetColorAverage(RSColor& color);

    bool GetFirstGetColorFinished();

    void SetDeviceSize(int& deviceWidth, int& deviceHeight);

    void SetIsShadow(bool isShadow);

    void SetShadowColorStrategy(int shadowColorStrategy);

    void SetWaitRelease(bool waitRelease);

    bool GetDeviceSize(int& deviceWidth, int& deviceHeight) const;

    bool GetWaitRelease() const;

    void ReleaseColorPicker();

private:
    std::shared_ptr<Drawing::Surface> cacheSurface_ = nullptr;
    Drawing::BackendTexture cacheBackendTexture_;
    Drawing::RectI surfaceSize_;
    bool valid_ = false;
    bool firstGetColorFinished_ = false;
    bool isShadow_ = false;
    int shadowColorStrategy_ = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    uint32_t* pixelPtr_ = nullptr;
    std::atomic<CacheProcessStatus> cacheProcessStatus_ = CacheProcessStatus::WAITING;
    std::shared_ptr<std::atomic<bool>> waitRelease_ = std::make_shared<std::atomic<bool>>(false);
    std::shared_ptr<Drawing::Image> imageSnapshotCache_ = nullptr;
    RSColor color_;
    std::vector<RSColor> colorArray_;
    std::vector<bool> colorArrayValid_;
    RSColor colorAverage_;
    std::mutex parallelRenderMutex_;
    std::mutex colorMutex_;
    std::shared_ptr<std::mutex> grBackendTextureMutex_ = std::make_shared<std::mutex>();
    std::condition_variable cvParallelRender_;
    std::optional<int> deviceWidth_;
    std::optional<int> deviceHeight_;
#ifdef IS_OHOS
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> initHandler_ = nullptr;
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_PROPERTY_RS_COLOR_PICKER_CACHE_TASK_H
