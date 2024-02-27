/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "pipeline/rs_uni_render_thread.h"

#include "rs_trace.h"

#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {

RSUniRenderThread* RSUniRenderThread::Instance()
{
    static RSUniRenderThread instance;
    return &instance;
}

RSUniRenderThread::RSUniRenderThread()
{
}

RSUniRenderThread::~RSUniRenderThread() noexcept
{
}

void RSUniRenderThread::Start()
{
    RS_LOGI("RSSuperThread start");
    runner_ = AppExecFwk::EventRunner::Create("RSUniRenderThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

void RSUniRenderThread::Init()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_->GetGlobalRootRenderNode();
    if (!rootNode) {
        RS_LOGE("rootNode is nullptr");
    }
    rootNodeDrawable_ = RSRenderNodeDrawable::Generate(rootNode);
    InitGrContext();
}

void RSUniRenderThread::InitGrContext()
{
    uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    if (!uniRenderEngine_) {
        RS_LOGE("uniRenderEngine_ is nullptr");
    }
    uniRenderEngine_->Init();
}

void RSUniRenderThread::PostTask(const std::function<void()>& task)
{
    if (!handler_) {
        return;
    }
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void RSUniRenderThread::Render()
{
    if (!rootNodeDrawable_) {
        RS_LOGE("rootNodeDrawable is nullptr");
    }
    //rootNodeDrawable_->Render(canvas_); //canvas need to init
}

} // namespace Rosen
} // namespace OHOS
