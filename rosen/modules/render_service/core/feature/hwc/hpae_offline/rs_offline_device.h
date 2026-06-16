/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_FEATURE_HWC_OFFLINE_DEVICE_H
#define RS_CORE_FEATURE_HWC_OFFLINE_DEVICE_H

#include <chrono>
#include <memory>
#include <string>
#include "drawable/rs_surface_render_node_drawable.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_process_syncer.h"
#include "feature/hwc/hpae_offline/rs_offline_result.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"
#include "params/rs_surface_render_params.h"

namespace OHOS {
namespace Rosen {

class RSOfflineDevice {
public:
    virtual ~RSOfflineDevice() = default;

    virtual OfflineDeviceType GetDeviceType() const = 0;

    virtual bool IsRSOfflineProcessorReady(std::shared_ptr<RSSurfaceRenderNode> surfaceNode) = 0;

    virtual bool PostProcessOfflineTask(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable,
        offlineTaskId taskId) = 0;

    virtual bool PostProcessOfflineTask(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        offlineTaskId taskId) = 0;

    virtual bool WaitForProcessOfflineResult(offlineTaskId taskId, std::chrono::milliseconds timeout,
        ProcessOfflineResult& processOfflineResult) = 0;

    virtual void CheckAndPostClearOfflineResourceTask() = 0;

    virtual bool CanDeleteDevice() = 0;
    
protected:
    RSOfflineDevice() = default;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_CORE_FEATURE_HWC_OFFLINE_DEVICE_H