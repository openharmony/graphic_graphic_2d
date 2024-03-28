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

#ifndef RS_OFFSCREEN_RENDER_THREAD_H
#define RS_OFFSCREEN_RENDER_THREAD_H

#include <functional>
#include <event_handler.h>
#include <map>
#include <mutex>
#include <sched.h>

#include "common/rs_common_def.h"

namespace OHOS::Rosen {
class RenderContext;
class RSB_EXPORT RSOffscreenRenderThread {
public:
    static RSOffscreenRenderThread& Instance();
    void PostTask(const std::function<void()>& task);
    void InSertCaptureTask(NodeId nodeId, std::function<void()>& task);
    const std::function<void()> GetCaptureTask(NodeId nodeId);
#ifdef ROSEN_OHOS
    const std::shared_ptr<RenderContext>& GetRenderContext();
    void CleanGrResource();
#endif
private:
    RSOffscreenRenderThread();
    ~RSOffscreenRenderThread() = default;
    RSOffscreenRenderThread(const RSOffscreenRenderThread&);
    RSOffscreenRenderThread(const RSOffscreenRenderThread&&);
    RSOffscreenRenderThread& operator=(const RSOffscreenRenderThread&);
    RSOffscreenRenderThread& operator=(const RSOffscreenRenderThread&&);

    std::mutex mutex_;
    int offscreenRenderNum_ = 0;

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
#ifdef ROSEN_OHOS
    std::shared_ptr<RenderContext> renderContext_ = nullptr;
#endif
    std::map<NodeId, std::function<void()>> taskMap_;
};
}
#endif // RS_OFFSCREEN_RENDER_THREAD_H
