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

#include "pipeline/rs_draw_frame.h"

#include "rs_trace.h"
#include "pipeline/rs_uni_render_thread.h"

#include "pipeline/rs_main_thread.h"

namespace OHOS {
namespace Rosen {
RSDrawFrame::RSDrawFrame() : unirenderInstance_(RSUniRenderThread::Instance())
{

}

RSDrawFrame::~RSDrawFrame() noexcept
{

}

void RSDrawFrame::SetRenderThreadParams(std::unique_ptr<RSRenderThreadParams>& stagingRenderThreadParams)
{
    stagingRenderThreadParams_ = std::move(stagingRenderThreadParams);
}

void RSDrawFrame::RenderFrame()
{
    RS_TRACE_NAME_FMT("RenderFrame");
    Sync();
    UnblockMainThread();
    Render();
}

void RSDrawFrame::PostAndWait()
{
    RS_TRACE_NAME_FMT("PostAndWait");
    std::unique_lock<std::mutex> frameLock(frameMutex_);
    canUnblockMainThread = false;
    unirenderInstance_.PostTask([this]() {
        RenderFrame();
    });
    frameCV_.wait(frameLock, [this] {return canUnblockMainThread;});
}

void RSDrawFrame::Sync()
{
    RS_TRACE_NAME_FMT("Sync");
    auto& pendingSyncNodes = RSMainThread::Instance()->GetContext().pendingSyncNodes_;
    for (auto& [id, weakPtr] : pendingSyncNodes) {
        if (auto node = weakPtr.lock()) {
            node->Sync();
        }
    }
    pendingSyncNodes.clear();

    unirenderInstance_.Sync(stagingRenderThreadParams_);
}

void RSDrawFrame::UnblockMainThread()
{
    RS_TRACE_NAME_FMT("UnlockMainThread");
    std::unique_lock<std::mutex> frameLock(frameMutex_);
    if (!canUnblockMainThread) {
        canUnblockMainThread = true;
        frameCV_.notify_all();
    }
}

void RSDrawFrame::Render()
{
    unirenderInstance_.Render();
}

} // namespace Rosen
} // namespace OHOS 