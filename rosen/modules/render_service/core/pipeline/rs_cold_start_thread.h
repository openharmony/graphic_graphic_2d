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

#ifndef RS_CORE_PIPELINE_RS_COLD_START_THREAD_H
#define RS_CORE_PIPELINE_RS_COLD_START_THREAD_H

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <event_handler.h>

#include "include/core/SkImage.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"
#ifdef RS_ENABLE_GL
#include "common/rs_shared_context.h"
#endif
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_render_node.h"

class GrContext;
namespace OHOS {
namespace Rosen {
class DrawCmdList;
class RSColdStartThread final {
public:
    RSColdStartThread(std::weak_ptr<RSSurfaceRenderNode> surfaceRenderNode, NodeId surfaceNodeId);
    ~RSColdStartThread();
    void PostPlayBackTask(std::shared_ptr<DrawCmdList> drawCmdList, float width, float height);
    void Stop();
    void PostTask(std::function<void()> task);

private:
#ifdef RS_ENABLE_GL
    void Run(EGLContext context);
#else
    void Run();
#endif

    std::weak_ptr<RSSurfaceRenderNode> surfaceNode_;
    NodeId surfaceNodeId_;
    std::unique_ptr<std::thread> thread_ = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::atomic_bool isRunning_ = false;
#ifdef RS_ENABLE_GL
    std::shared_ptr<RSSharedContext> context_ = nullptr;
#endif
    sk_sp<GrContext> grContext_;
    sk_sp<SkSurface> skSurface_;
    std::mutex mutex_;
    std::mutex imageMutex_;
    std::queue<sk_sp<SkImage>> images_;
    std::condition_variable cv_;
};

class RSColdStartManager {
public:
    static RSColdStartManager& Instance();
    void PostPlayBackTask(NodeId id, std::shared_ptr<DrawCmdList> drawCmdList, float width, float height);
    bool IsColdStartThreadRunning(NodeId id);
    void StartColdStartThreadIfNeed(std::shared_ptr<RSSurfaceRenderNode> surfaceNode);
    void StopColdStartThread(NodeId id);
    void DestroyColdStartThread(NodeId id);
    void CheckColdStartMap(const RSRenderNodeMap& nodeMap);

private:
    RSColdStartManager() = default;
    ~RSColdStartManager() = default;
    RSColdStartManager(const RSColdStartManager& manager);
    RSColdStartManager(const RSColdStartManager&& manager);
    RSColdStartManager& operator=(const RSColdStartManager& manager);
    RSColdStartManager& operator=(const RSColdStartManager&& manager);

    std::map<NodeId, std::unique_ptr<RSColdStartThread>> coldStartThreadMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_CORE_PIPELINE_RS_COLD_START_THREAD_H
