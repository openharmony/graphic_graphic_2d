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

namespace OHOS {
namespace Rosen {
RSDrawFrame::RSDrawFrame()
{

}

RSDrawFrame::~RSDrawFrame() noexcept
{

}
void RSDrawFrame::RenderFrame()
{
    RS_TRACE_NAME_FMT("RenderFrame");
    Sync();
    UnblockMainThread();
    Render();
    FlushFrame();
    PostProcess();
}

void RSDrawFrame::PostAndWait()
{
    std::unique_lock<std::mutex> frameLock(frameMutex_);
    canUnblockMainThread = false;
    RSUniRenderThread::Instance()->PostTask([this]() {
        RenderFrame();
    });
    frameCV_.wait(frameLock, [this] {return canUnblockMainThread;});
}

void RSDrawFrame::Sync()
{
    RS_TRACE_NAME_FMT("Sync");
    for(auto node : nodes) {
        if (!node) {
            continue;
        }
        //node->Sync();
    }
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
    RSUniRenderThread::Instance()->Render();
}

void RSDrawFrame::FlushFrame()
{
    //todo
}

void RSDrawFrame::PostProcess()
{
    //todo
}

} // namespace Rosen
} // namespace OHOS 