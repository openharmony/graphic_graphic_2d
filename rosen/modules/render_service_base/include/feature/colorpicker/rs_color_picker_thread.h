/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_THREAD_H
#define RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_THREAD_H

#include <functional>

#include "event_handler.h"
#include "common/rs_macros.h"

#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include "image/gpu_context.h"
#endif

namespace OHOS::Rosen {
class RenderContext;

class RSB_EXPORT RSColorPickerThread final {
public:
using NodeDirtyCallback = std::function<void(uint64_t)>;

    static RSColorPickerThread& Instance();
    void PostTask(const std::function<void()>& task, int64_t delayTime = 0);
    void RegisterNodeDirtyCallback(const NodeDirtyCallback& callback);
    void NotifyNodeDirty(uint64_t nodeId);

#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    void InitRenderContext(std::shared_ptr<RenderContext> context);
    std::shared_ptr<Drawing::GPUContext> GetShareGPUContext() const;
#endif

private:
    RSColorPickerThread();
    ~RSColorPickerThread() = default;
    RSColorPickerThread(const RSColorPickerThread&);
    RSColorPickerThread(const RSColorPickerThread&&);
    RSColorPickerThread& operator=(const RSColorPickerThread&);
    RSColorPickerThread& operator=(const RSColorPickerThread&&);

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    NodeDirtyCallback callback_ = nullptr;

#if defined(RS_ENABLE_UNI_RENDER) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
    std::shared_ptr<RenderContext> renderContext_ = nullptr;
    std::shared_ptr<Drawing::GPUContext> CreateShareGPUContext();
    std::shared_ptr<Drawing::GPUContext> gpuContext_ = nullptr;
#endif
};
}
#endif // RENDER_SERVICE_BASE_FEATURE_COLOR_PICKER_RS_COLOR_PICKER_THREAD_H
