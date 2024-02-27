/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_PIPELINE_RS_DRAW_FRAME_H
#define RENDER_SERVICE_PIPELINE_RS_DRAW_FRAME_H
#include <condition_variable>
#include <mutex>
#include <list>
#include "pipeline/rs_render_node.h"

namespace OHOS {
namespace Rosen {
class RSDrawFrame {
public:
    RSDrawFrame();
    ~RSDrawFrame() noexcept;

    RSDrawFrame(const RSDrawFrame&) = delete;
    RSDrawFrame(const RSDrawFrame&&) = delete;
    RSDrawFrame& operator=(const RSDrawFrame&) = delete;
    RSDrawFrame& operator=(const RSDrawFrame&&) = delete;

    void PostAndWait();
    void RenderFrame();
    void UnblockMainThread();
    void Sync();
    void FlushFrame();
    void Render();
    void PostProcess();
private:
    std::mutex frameMutex_;
    std::condition_variable frameCV_;
    std::list<std::shared_ptr<RSRenderNode>> nodes;
    bool canUnblockMainThread = false;
};
} // Rosen
} // OHOS

#endif // RENDER_SERVICE_PIPELINE_RS_DRAW_FRAME_H
