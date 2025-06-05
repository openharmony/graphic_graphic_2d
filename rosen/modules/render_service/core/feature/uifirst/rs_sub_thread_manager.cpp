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

#include "rs_sub_thread_manager.h"
#include <chrono>
#include "rs_sub_thread_cache.h"
#include "rs_trace.h"

#include "common/rs_singleton.h"
#include "common/rs_optional_trace.h"
#include "feature/uifirst/rs_uifirst_manager.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_task_dispatcher.h"
#include "memory/rs_memory_manager.h"

#undef LOG_TAG
#define LOG_TAG "RSSubThreadManager"

namespace OHOS::Rosen {
static constexpr uint32_t SUB_THREAD_NUM = 3;
static constexpr uint32_t SUB_VIDEO_THREAD_TASKS_NUM_MAX = 2;
static constexpr uint32_t WAIT_NODE_TASK_TIMEOUT = 5 * 1000; // 5s
constexpr const char* RELEASE_RESOURCE = "releaseResource";
constexpr const char* RELEASE_TEXTURE = "releaseTexture";

RSSubThreadManager* RSSubThreadManager::Instance()
{
    static RSSubThreadManager instance;
    return &instance;
}

void RSSubThreadManager::Start(RenderContext *context)
{
    if (!threadList_.empty()) {
        return;
    }
    if (context) {
        for (uint32_t i = 0; i < SUB_THREAD_NUM; ++i) {
            auto curThread = std::make_shared<RSSubThread>(context, i);
            auto tid = curThread->Start();
            threadIndexMap_.emplace(tid, i);
            reThreadIndexMap_.emplace(i, tid);
            threadList_.push_back(curThread);
            auto taskDispatchFunc = [i](const RSTaskDispatcher::RSTask& task, bool isSyncTask = false) {
                RSSubThreadManager::Instance()->PostTask(task, i, isSyncTask);
            };
            RSTaskDispatcher::GetInstance().RegisterTaskDispatchFunc(tid, taskDispatchFunc);
        }
    }
}

void RSSubThreadManager::PostTask(const std::function<void()>& task, uint32_t threadIndex, bool isSyncTask)
{
    if (threadIndex >= threadList_.size()) {
        RS_LOGE("taskIndex geq thread num");
        return;
    }
    if (isSyncTask) {
        threadList_[threadIndex]->PostSyncTask(task);
    } else {
        threadList_[threadIndex]->PostTask(task);
    }
}

void RSSubThreadManager::DumpMem(DfxString& log)
{
    if (threadList_.empty()) {
        return;
    }
    for (auto subThread : threadList_) {
        if (!subThread) {
            continue;
        }
        subThread->DumpMem(log);
    }
}

std::shared_ptr<Drawing::GPUContext> RSSubThreadManager::GetGrContextFromSubThread(pid_t tid)
{
    auto iter = threadIndexMap_.find(tid);
    if (iter == threadIndexMap_.end()) {
        return nullptr;
    }
    auto index = iter->second;
    if ((index >= 0) && (index < SUB_THREAD_NUM)) {
        return threadList_[index]->GetGrContext();
    }
    return nullptr;
}

float RSSubThreadManager::GetAppGpuMemoryInMB()
{
    if (threadList_.empty()) {
        return 0.f;
    }
    float total = 0.f;
    for (auto& subThread : threadList_) {
        if (!subThread) {
            continue;
        }
        total += subThread->GetAppGpuMemoryInMB();
    }
    return total;
}

void RSSubThreadManager::WaitNodeTask(uint64_t nodeId)
{
    RS_TRACE_NAME_FMT("RSSubThreadManager::WaitNodeTask for node %" PRIu64, nodeId);
    std::unique_lock<std::mutex> lock(parallelRenderMutex_);
    cvParallelRender_.wait_for(lock, std::chrono::milliseconds(WAIT_NODE_TASK_TIMEOUT), [&]() {
        return !nodeTaskState_[nodeId];
    });
}

void RSSubThreadManager::NodeTaskNotify(uint64_t nodeId)
{
    {
        std::unique_lock<std::mutex> lock(parallelRenderMutex_);
        nodeTaskState_[nodeId] = 0;
    }
    cvParallelRender_.notify_one();
}

void RSSubThreadManager::ResetSubThreadGrContext()
{
    if (threadList_.empty()) {
        return;
    }
    if (!needResetContext_) {
        ReleaseTexture();
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->PostTask(
            [subThread]() { subThread->ResetGrContext(); },
            RELEASE_RESOURCE);
    }
    needResetContext_ = false;
    needCancelTask_ = true;
}

void RSSubThreadManager::CancelReleaseResourceTask()
{
    if (!needCancelTask_) {
        return;
    }
    if (threadList_.empty()) {
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->RemoveTask(RELEASE_RESOURCE);
    }
    needCancelTask_ = false;
}

void RSSubThreadManager::ReleaseTexture()
{
    if (threadList_.empty()) {
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->PostTask(
            [subThread]() {
                subThread->ThreadSafetyReleaseTexture();
            },
            RELEASE_TEXTURE);
    }
    needCancelReleaseTextureTask_ = true;
}

void RSSubThreadManager::TryReleaseTextureForIdleThread()
{
    if (threadList_.empty()) {
        return;
    }

    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        if (subThread->GetDoingCacheProcessNum() != 0) {
            continue;
        }
        subThread->PostTask([subThread]() { subThread->ThreadSafetyReleaseTexture(); }, RELEASE_TEXTURE);
    }
    needCancelReleaseTextureTask_ = true;
}

void RSSubThreadManager::CancelReleaseTextureTask()
{
    if (!needCancelReleaseTextureTask_) {
        return;
    }
    if (threadList_.empty()) {
        return;
    }
    for (uint32_t i = 0; i < SUB_THREAD_NUM; i++) {
        auto subThread = threadList_[i];
        subThread->RemoveTask(RELEASE_TEXTURE);
    }
    needCancelReleaseTextureTask_ = false;
}

void RSSubThreadManager::ForceReleaseResource()
{
    needResetContext_ = true;
}

void RSSubThreadManager::ReleaseSurface(uint32_t threadIndex) const
{
    if (threadList_.size() <= threadIndex) {
        return;
    }
    auto subThread = threadList_[threadIndex];
    subThread->PostTask([subThread]() {
        subThread->ReleaseSurface();
    });
}

void RSSubThreadManager::AddToReleaseQueue(std::shared_ptr<Drawing::Surface>&& surface, uint32_t threadIndex)
{
    if (threadList_.size() <= threadIndex) {
        return;
    }
    threadList_[threadIndex]->AddToReleaseQueue(std::move(surface));
}

std::vector<MemoryGraphic> RSSubThreadManager::CountSubMem(int pid)
{
    std::vector<MemoryGraphic> memsContainer;
    if (threadList_.empty()) {
        return memsContainer;
    }

    for (auto& subThread : threadList_) {
        if (!subThread) {
            MemoryGraphic memoryGraphic;
            memsContainer.push_back(memoryGraphic);
            continue;
        }
        memsContainer.push_back(subThread->CountSubMem(pid));
    }
    return memsContainer;
}

std::unordered_map<uint32_t, pid_t> RSSubThreadManager::GetReThreadIndexMap() const
{
    return reThreadIndexMap_;
}

void RSSubThreadManager::ScheduleRenderNodeDrawable(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable)
{
    if (UNLIKELY(!nodeDrawable)) {
        RS_LOGE("ScheduleRenderNodeDrawable nodeDrawable nullptr");
        return;
    }
    const auto& param = nodeDrawable->GetRenderParams();
    if (UNLIKELY(!param)) {
        RS_LOGE("ScheduleRenderNodeDrawable param nullptr");
        return;
    }

    const auto& rtUniParam = RSUniRenderThread::Instance().GetRSRenderThreadParams();
    // rtUniParam will not be updated before UnblockMainThread
    if (UNLIKELY(!rtUniParam)) {
        RS_LOGE("ScheduleRenderNodeDrawable renderThread param nullptr");
        return;
    }

    auto minDoingCacheProcessNum = threadList_[defaultThreadIndex_]->GetDoingCacheProcessNum();
    minLoadThreadIndex_ = defaultThreadIndex_;
    for (unsigned int j = 0; j < SUB_THREAD_NUM; j++) {
        if (j == defaultThreadIndex_) {
            continue;
        }
        if (minDoingCacheProcessNum > threadList_[j]->GetDoingCacheProcessNum()) {
            minDoingCacheProcessNum = threadList_[j]->GetDoingCacheProcessNum();
            minLoadThreadIndex_ = j;
        }
    }
    auto nowIdx = minLoadThreadIndex_;
    auto& rsSubThreadCache = nodeDrawable->GetRsSubThreadCache();
    if (threadIndexMap_.count(rsSubThreadCache.GetLastFrameUsedThreadIndex()) != 0) {
        nowIdx = threadIndexMap_[rsSubThreadCache.GetLastFrameUsedThreadIndex()];
    } else {
        defaultThreadIndex_++;
        if (defaultThreadIndex_ >= SUB_THREAD_NUM) {
            defaultThreadIndex_ = 0;
        }
    }

    auto subThread = threadList_[nowIdx];
    auto tid = reThreadIndexMap_[nowIdx];
    {
        std::unique_lock<std::mutex> lock(parallelRenderMutex_);
        nodeTaskState_[param->GetId()] = 1;
    }
    auto submittedFrameCount = RSUniRenderThread::Instance().GetFrameCount();
    subThread->DoingCacheProcessNumInc();
    rsSubThreadCache.SetCacheSurfaceProcessedStatus(CacheProcessStatus::WAITING);
    subThread->PostTask([subThread, nodeDrawable, tid, submittedFrameCount,
                            uniParam = new RSRenderThreadParams(*rtUniParam)]() mutable {
        if (UNLIKELY(!uniParam)) {
            RS_LOGE("ScheduleRenderNodeDrawable subThread param is nullptr");
            return;
        }

        // The destructor of GPUCompositonCacheGuard, a memory release check will be performed
        RSMainThread::GPUCompositonCacheGuard guard;
        std::unique_ptr<RSRenderThreadParams> uniParamUnique(uniParam);
        /* Task run in SubThread, the uniParamUnique which is copyed from uniRenderThread will sync to SubTread */
        RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(std::move(uniParamUnique));
        nodeDrawable->GetRsSubThreadCache().SetLastFrameUsedThreadIndex(tid);
        nodeDrawable->GetRsSubThreadCache().SetTaskFrameCount(submittedFrameCount);
        subThread->DrawableCache(nodeDrawable);
        RSRenderThreadParamsManager::Instance().SetRSRenderThreadParams(nullptr);
    });
    needResetContext_ = true;
}

void RSSubThreadManager::ScheduleReleaseCacheSurfaceOnly(
    std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable> nodeDrawable)
{
    if (!nodeDrawable) {
        return;
    }
    auto& param = nodeDrawable->GetRenderParams();
    if (!param) {
        return;
    }
    auto bindThreadIdx = nodeDrawable->GetRsSubThreadCache().GetLastFrameUsedThreadIndex();
    if (!threadIndexMap_.count(bindThreadIdx)) {
        RS_LOGE("ScheduleReleaseCacheSurface invalid thread idx");
        return;
    }
    auto nowIdx = threadIndexMap_[bindThreadIdx];

    auto subThread = threadList_[nowIdx];
    subThread->PostTask([subThread, nodeDrawable]() { subThread->ReleaseCacheSurfaceOnly(nodeDrawable); });
}

void RSSubThreadManager::GetGpuMemoryForReport(std::unordered_map<pid_t, size_t>& gpuMemoryOfPid)
{
    if (OHOS::Rosen::RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return;
    }
    for (auto& subThread : threadList_) {
        if (!subThread) {
            continue;
        }
        std::unordered_map<pid_t, size_t> gpuMemOfPid = subThread->GetGpuMemoryOfPid();
        for (auto& [pid, memSize] : gpuMemOfPid) {
            gpuMemoryOfPid[pid] += memSize;
            if (memSize == 0) {
                subThread->ErasePidOfGpuMemory(pid);
            }
        }
    }
}
} // namespace OHOS::Rosen
