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

#include "rs_parallel_render_manager.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include "rs_trace.h"
#include "EGL/egl.h"
#include "rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "render_context/render_context.h"
#include "pipeline/rs_main_thread.h"
#include "rs_parallel_render_ext.h"
#include "pipeline/rs_uni_render_visitor.h"

namespace OHOS {
namespace Rosen {

static constexpr uint32_t PARALLEL_THREAD_NUM = 3;

RSParallelRenderManager* RSParallelRenderManager::Instance()
{
    static RSParallelRenderManager instance;
    return &instance;
}

RSParallelRenderManager::RSParallelRenderManager()
    : parallelHardwareComposer_(std::make_unique<RSParallelHardwareComposer>())
{
    if (parallelHardwareComposer_) {
        parallelHardwareComposer_->Init(PARALLEL_THREAD_NUM);
    }
}

void RSParallelRenderManager::SetParallelMode(bool parallelMode)
{
    parallelMode_ = parallelMode;
    TryEnableParallelRendering();
}

// You should always use GetParallelModeSafe() instead of GetParallelMode()
// except initialize variable 'isParallel' in 'rs_uni_render_visitor.cpp'
bool RSParallelRenderManager::GetParallelModeSafe() const
{
    return GetParallelRenderingStatus() == ParallelStatus::ON;
}

bool RSParallelRenderManager::GetParallelMode() const
{
    ParallelStatus status = GetParallelRenderingStatus();
    return (status == ParallelStatus::ON) || (status == ParallelStatus::FIRSTFLUSH);
}
 
void RSParallelRenderManager::StartSubRenderThread(uint32_t threadNum, RenderContext *context)
{
    if (GetParallelRenderingStatus() == ParallelStatus::OFF) {
        expectedSubThreadNum_ = threadNum;
        flipCoin_ = std::vector<uint8_t>(expectedSubThreadNum_, 0);
        firstFlush_ = true;
        renderContext_ = context;
        if (context) {
            for (uint32_t i = 0; i < threadNum; ++i) {
                auto curThread = std::make_unique<RSParallelSubThread>(context, renderType_, i);
                curThread->StartSubThread();
                threadList_.push_back(std::move(curThread));
            }
        }
        processTaskManager_.Initialize(threadNum);
        prepareTaskManager_.Initialize(threadNum);
    }
}

void RSParallelRenderManager::EndSubRenderThread()
{
    if (GetParallelRenderingStatus() == ParallelStatus::ON) {
        for (uint32_t i = 0; i < expectedSubThreadNum_; ++i) {
            flipCoin_[i] = 1;
        }
        readySubThreadNum_ = expectedSubThreadNum_ = 0;
        cvParallelRender_.notify_all();
        packVisitor_ = nullptr;
        packVisitorPrepare_ = nullptr;
        for (auto &thread : threadList_) {
            thread->WaitSubMainThreadEnd();
        }
        std::vector<std::unique_ptr<RSParallelSubThread>>().swap(threadList_);
        uniVisitor_ = nullptr;
    }
}

void RSParallelRenderManager::ReadySubThreadNumIncrement()
{
    ++readySubThreadNum_;
}

ParallelStatus RSParallelRenderManager::GetParallelRenderingStatus() const
{
    if (expectedSubThreadNum_ == 0) {
        return ParallelStatus::OFF;
    } else if (expectedSubThreadNum_ == readySubThreadNum_) {
        return ParallelStatus::ON;
    } else if (firstFlush_) {
        return ParallelStatus::FIRSTFLUSH;
    } else {
        return ParallelStatus::WAITFIRSTFLUSH;
    }
}

void RSParallelRenderManager::CopyVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node)
{
    packVisitor_ = std::make_shared<RSParallelPackVisitor>(visitor);
    displayNode_ = node.shared_from_this();
    processTaskManager_.Reset();
    packVisitor_->ProcessDisplayRenderNode(node);
    uniVisitor_ = &visitor;
    taskType_ = TaskType::PROCESS_TASK;
}

void RSParallelRenderManager::CopyPrepareVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node)
{
    packVisitorPrepare_ = std::make_shared<RSParallelPackVisitor>();
    uniVisitor_ = &visitor;
    displayNode_ = node.shared_from_this();
    prepareTaskManager_.Reset();
    packVisitorPrepare_->PrepareDisplayRenderNode(node);
    taskType_ = TaskType::PREPARE_TASK;
}

TaskType RSParallelRenderManager::GetTaskType()
{
    return taskType_;
}

void RSParallelRenderManager::PackRenderTask(RSSurfaceRenderNode &node, TaskType type)
{
    switch (type) {
        case TaskType::PREPARE_TASK:
            prepareTaskManager_.PushRenderTask(
                std::make_unique<RSRenderTask>(node, RSRenderTask::RenderNodeStage::PREPARE));
            break;
        case TaskType::PROCESS_TASK:
            processTaskManager_.PushRenderTask(
                std::make_unique<RSRenderTask>(node, RSRenderTask::RenderNodeStage::PROCESS));
            break;
        default:
            break;
    }
}

void RSParallelRenderManager::LoadBalanceAndNotify(TaskType type)
{
    switch (type) {
        case TaskType::PREPARE_TASK:
            prepareTaskManager_.LBCalcAndSubmitSuperTask(displayNode_);
            break;
        case TaskType::PROCESS_TASK:
            processTaskManager_.LBCalcAndSubmitSuperTask(displayNode_);
            break;
        default:
            break;
    }
    for (uint32_t i = 0; i < expectedSubThreadNum_; ++i) {
        flipCoin_[i] = 1;
    }
    cvParallelRender_.notify_all();
}

void RSParallelRenderManager::WaitPrepareEnd(RSUniRenderVisitor &visitor)
{
    for (uint32_t i = 0; i < expectedSubThreadNum_; ++i) {
        WaitSubMainThread(i);
        visitor.CopyForParallelPrepare(threadList_[i]->GetUniVisitor());
    }
}

void RSParallelRenderManager::DrawImageMergeFunc(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    for (unsigned int i = 0; i < expectedSubThreadNum_; ++i) {
        RS_TRACE_BEGIN("Wait Render finish");
        WaitSubMainThread(i);
        RS_TRACE_END();
        if (i < processTaskManager_.GetTaskNum()) {
            RS_TRACE_BEGIN("Wait Fence Ready");
            threadList_[i]->WaitReleaseFence();
            RS_TRACE_END();
            auto texture = threadList_[i]->GetTexture();
            if (texture == nullptr) {
                RS_LOGE("Texture %d is nullptr", i);
                continue;
            }
            if (canvas) {
                canvas->drawImage(texture, 0, 0);
                // For any one subMainThread' sksurface, we just clear transparent color of self drawing
                // surface drawed in larger skSurface, such as skSurface 0 should clear self drawing surface
                // areas drawed in skSurface 1.
                auto clearTransparentColorSurfaceIndex = i + 1;
                ClearSelfDrawingSurface(canvas, clearTransparentColorSurfaceIndex);
            } else {
                RS_LOGE("canvas of main thread is nullptr!");
            }
        }
    }
}

void RSParallelRenderManager::FlushOneBufferFunc()
{
    renderContext_->ShareMakeCurrent(EGL_NO_CONTEXT);
    for (unsigned int i = 0; i < threadList_.size(); ++i) {
        renderContext_->ShareMakeCurrent(threadList_[i]->GetSharedContext());
        RS_TRACE_BEGIN("Start Flush");
        auto skSurface = threadList_[i]->GetSkSurface();
        if (skSurface) {
            skSurface->flush();
        } else {
            RS_LOGE("skSurface is nullptr, thread index:%d", i);
        }
        RS_TRACE_END();
        renderContext_->ShareMakeCurrent(EGL_NO_CONTEXT);
    }
    renderContext_->MakeSelfCurrent();
}

void RSParallelRenderManager::MergeRenderResult(std::shared_ptr<RSPaintFilterCanvas> canvas)
{
    if (GetParallelRenderingStatus() == ParallelStatus::FIRSTFLUSH) {
        firstFlush_ = false;
        for (unsigned int i = 0; i < expectedSubThreadNum_; ++i) {
            WaitSubMainThread(i);
        }
        return;
    }
    if (renderType_ == ParallelRenderType::DRAW_IMAGE) {
        DrawImageMergeFunc(canvas);
    } else {
        FlushOneBufferFunc();
    }
    if (parallelHardwareComposer_) {
        parallelHardwareComposer_->Init(PARALLEL_THREAD_NUM);
    }
}

void RSParallelRenderManager::SetFrameSize(int width, int height)
{
    width_ = width;
    height_ = height;
}

void RSParallelRenderManager::GetFrameSize(int &width, int &height)
{
    width = width_;
    height = height_;
}

void RSParallelRenderManager::SubmitSuperTask(uint32_t taskIndex, std::unique_ptr<RSSuperRenderTask> superRenderTask)
{
    if (taskIndex >= threadList_.size()) {
        RS_LOGE("taskIndex geq thread num");
        return;
    }
    threadList_[taskIndex]->SetSuperTask(std::move(superRenderTask));
}

// called by submain threads
void RSParallelRenderManager::SubMainThreadNotify(int threadIndex)
{
    flipCoin_[threadIndex] = 0;
    std::unique_lock<std::mutex> lock(cvParallelRenderMutex_);
    bool isNotify = true;
    for (unsigned int i = 0; i < expectedSubThreadNum_; ++i) {
        if (flipCoin_[i] != 0) {
            return;
        }
        isNotify &= !flipCoin_[i];
    }
    if (isNotify) {
        cvParallelRender_.notify_all();
    }
}

// called by main thread
void RSParallelRenderManager::WaitSubMainThread(uint32_t threadIndex)
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cvParallelRender_.wait(lock, [&]() {
        return !flipCoin_[threadIndex];
    });
}

// called by submain threads
void RSParallelRenderManager::SubMainThreadWait(uint32_t threadIndex)
{
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cvParallelRender_.wait(lock, [&]() {
        return flipCoin_[threadIndex];
    });
}

void RSParallelRenderManager::SetRenderTaskCost(uint32_t subMainThreadIdx, uint64_t loadId, float cost, TaskType type)
{
    switch (type) {
        case TaskType::PREPARE_TASK:
            prepareTaskManager_.SetSubThreadRenderTaskLoad(subMainThreadIdx, loadId, cost);
            break;
        case TaskType::PROCESS_TASK:
            processTaskManager_.SetSubThreadRenderTaskLoad(subMainThreadIdx, loadId, cost);
            break;
        default:
            break;
    }
}

bool RSParallelRenderManager::ParallelRenderExtEnabled()
{
    return processTaskManager_.GetParallelRenderExtEnable();
}

void RSParallelRenderManager::TryEnableParallelRendering()
{
    if (parallelMode_) {
        StartSubRenderThread(PARALLEL_THREAD_NUM,
            RSMainThread::Instance()->GetRenderEngine()->GetRenderContext().get());
    } else {
        EndSubRenderThread();
    }
}

void RSParallelRenderManager::CommitSurfaceNum(int surfaceNum)
{
    if (ParallelRenderExtEnabled()) {
        auto setCoreLevel = reinterpret_cast<void(*)(int)>(RSParallelRenderExt::setCoreLevelFunc_);
        setCoreLevel(surfaceNum);
    }
}

void RSParallelRenderManager::WorkSerialTask(RSSurfaceRenderNode &node)
{
    if (uniVisitor_) {
        uniVisitor_->PrepareSurfaceRenderNode(node);
    }
}

uint32_t RSParallelRenderManager::GetParallelThreadNumber() const
{
    return PARALLEL_THREAD_NUM;
}

void RSParallelRenderManager::AddSelfDrawingSurface(unsigned int subThreadIndex, bool isRRect, RectF rect,
    Vector4f cornerRadius)
{
    if (parallelHardwareComposer_) {
        auto shape = std::make_unique<RSParallelSelfDrawingSurfaceShape>(isRRect, rect, cornerRadius);
        parallelHardwareComposer_->AddTransparentColorArea(subThreadIndex, std::move(shape));
    }
}

void RSParallelRenderManager::ClearSelfDrawingSurface(std::shared_ptr<RSPaintFilterCanvas> canvas,
    unsigned int subThreadIndex)
{
    if (parallelHardwareComposer_) {
        parallelHardwareComposer_->ClearTransparentColor(canvas, subThreadIndex);
    }
}

} // namespace Rosen
} // namespace OHOS