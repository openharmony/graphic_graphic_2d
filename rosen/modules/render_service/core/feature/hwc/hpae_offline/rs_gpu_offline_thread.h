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

#ifndef RS_CORE_FEATURE_HWC_GPU_OFFLINE_THREAD_H
#define RS_CORE_FEATURE_HWC_GPU_OFFLINE_THREAD_H

#include <memory>
#include <string>

#include "common/rs_macros.h"
#include "event_handler.h"
#include "engine/rs_base_render_engine.h"

namespace OHOS {
namespace Rosen {

class RSGPUOfflineThread {
public:
    RSGPUOfflineThread();
    ~RSGPUOfflineThread();

    bool Start();
    void Stop();
    bool PostTask(const std::function<void()>& task, const std::string& name = std::string());
    std::shared_ptr<RSBaseRenderEngine> GetRenderEngine() const
    {
        return renderEngine_;
    }

private:
    void InitRenderEngine();

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<RSBaseRenderEngine> renderEngine_ = nullptr;
    bool isInitialized_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_CORE_FEATURE_HWC_GPU_OFFLINE_THREAD_H