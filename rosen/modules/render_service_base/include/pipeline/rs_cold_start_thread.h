/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_PIPELINE_RS_COLD_START_THREAD_H
#define RENDER_SERVICE_BASE_PIPELINE_RS_COLD_START_THREAD_H

#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <event_handler.h>

#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#ifdef RS_ENABLE_GL
#include "common/rs_shared_context.h"
#endif

class GrContext;
class SkPicture;
namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNode;
class RSColdStartThread final {
public:
    RSColdStartThread(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode);
    ~RSColdStartThread();
    void PostPlayBackTask(sk_sp<SkPicture> picture, float width, float height);

private:
#ifdef RS_ENABLE_GL
    void Run(EGLContext context);
#else
    void Run();
#endif

    void Stop();

    std::weak_ptr<RSSurfaceRenderNode> surfaceNode_;
    std::unique_ptr<std::thread> thread_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    bool isRunning_ = false;
    sk_sp<SkSurface> surface_ = nullptr;
#ifdef RS_ENABLE_GL
    std::shared_ptr<RSSharedContext> context_ = nullptr;
    std::vector<sk_sp<GrContext>> grContexts_;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_PIPELINE_RS_COLD_START_THREAD_H
