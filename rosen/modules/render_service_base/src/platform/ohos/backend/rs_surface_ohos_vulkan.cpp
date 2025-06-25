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

#include "rs_surface_ohos_vulkan.h"

#include <atomic>
#include <memory>
#include "common/rs_exception_check.h"
#include "memory/rs_tag_tracker.h"
#include "native_buffer_inner.h"
#include "native_window.h"
#include "vulkan/vulkan_core.h"
#include "platform/common/rs_log.h"
#include "window.h"
#include "platform/common/rs_system_properties.h"
#include "drawing/engine_adapter/skia_adapter/skia_gpu_context.h"
#include "engine_adapter/skia_adapter/skia_surface.h"
#include "rs_trace.h"

#if defined(ROSEN_OHOS) && defined(ENABLE_HPAE_BLUR)
#include "cpp/ffrt_dynamic_graph.h"
#include "hpae_base/rs_hpae_ffrt_pattern_manmger.h"
#include "hpae_base/rs_hpae_scheduler.h.h"
#include "hpae_base/rs_hpae_log.h"
#include "hpae_base/rs_hpae_perf_thread.h"
#include "hpae_base/rs_hpae_hianimation.h"
#endif

#ifdef USE_M133_SKIA
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/vk/GrVkBackendSemaphore.h"
#else
#include "include/gpu/GrDirectContext.h"
#include "include/gpu/GrBackendSemaphore.h"
#endif

namespace OHOS {
namespace Rosen {

static constexpr int64_t PROTECTEDBUFFERSIZE = 2;
static constexpr uint16_t FFRTEVENTLEN = 2;

RSSurfaceOhosVulkan::RSSurfaceOhosVulkan(const sptr<Surface>& producer) : RSSurfaceOhos(producer)
{
    bufferUsage_ = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_MEM_DMA;
}

RSSurfaceOhosVulkan::~RSSurfaceOhosVulkan()
{
    mSurfaceMap.clear();
    mSurfaceList.clear();
    DestoryNativeWindow(mNativeWindow);
    mNativeWindow = nullptr;
    if (mReservedFlushFd != -1) {
        fdsan_close_with_tag(mReservedFlushFd, LOG_DOMAIN);
        mReservedFlushFd = -1;
    }
}

void RSSurfaceOhosVulkan::SetNativeWindowInfo(int32_t width, int32_t height, bool useAFBC, bool isProtected)
{
    if (width != mWidth || height != mHeight) {
        for (auto &[key, val] : mSurfaceMap) {
            NativeWindowCancelBuffer(mNativeWindow, key);
        }
        mSurfaceMap.clear();
        mSurfaceList.clear();
    }
    NativeWindowHandleOpt(mNativeWindow, SET_FORMAT, pixelFormat_);
#ifdef RS_ENABLE_AFBC
    if (RSSystemProperties::GetAFBCEnabled() && !isProtected) {
        int32_t format = 0;
        NativeWindowHandleOpt(mNativeWindow, GET_FORMAT, &format);
        if (format == GRAPHIC_PIXEL_FMT_RGBA_8888 && useAFBC) {
            bufferUsage_ =
                (BUFFER_USAGE_HW_RENDER | BUFFER_USAGE_HW_TEXTURE | BUFFER_USAGE_HW_COMPOSER | BUFFER_USAGE_MEM_DMA);
        }
    }
#endif

    if (isProtected) {
        bufferUsage_ |= BUFFER_USAGE_PROTECTED;
    } else {
        bufferUsage_ &= (~BUFFER_USAGE_PROTECTED);
    }
    NativeWindowHandleOpt(mNativeWindow, SET_USAGE, bufferUsage_);
    NativeWindowHandleOpt(mNativeWindow, SET_BUFFER_GEOMETRY, width, height);
    NativeWindowHandleOpt(mNativeWindow, GET_BUFFER_GEOMETRY, &mHeight, &mWidth);
    NativeWindowHandleOpt(mNativeWindow, SET_COLOR_GAMUT, colorSpace_);
}


void RSSurfaceOhosVulkan::CreateVkSemaphore(
    VkSemaphore& semaphore, RsVulkanContext& vkContext, NativeBufferUtils::NativeSurfaceInfo& nativeSurface)
{
    VkSemaphoreCreateInfo semaphoreInfo;
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    semaphoreInfo.flags = 0;
    auto& vkInterface = vkContext.GetRsVulkanInterface();
    auto res = vkInterface.vkCreateSemaphore(vkInterface.GetDevice(), &semaphoreInfo, nullptr, &semaphore);
    if (res != VK_SUCCESS) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: CreateVkSemaphore vkCreateSemaphore failed %{public}d", res);
        semaphore = VK_NULL_HANDLE;
        nativeSurface.fence->Wait(-1);
        return;
    }

    VkImportSemaphoreFdInfoKHR importSemaphoreFdInfo;
    importSemaphoreFdInfo.sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR;
    importSemaphoreFdInfo.pNext = nullptr;
    importSemaphoreFdInfo.semaphore = semaphore;
    importSemaphoreFdInfo.flags = VK_SEMAPHORE_IMPORT_TEMPORARY_BIT;
    importSemaphoreFdInfo.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_SYNC_FD_BIT;
    importSemaphoreFdInfo.fd = nativeSurface.fence->Dup();
    res = vkInterface.vkImportSemaphoreFdKHR(vkInterface.GetDevice(), &importSemaphoreFdInfo);
    if (res != VK_SUCCESS) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: CreateVkSemaphore vkImportSemaphoreFdKHR failed %{public}d", res);
        vkInterface.vkDestroySemaphore(vkInterface.GetDevice(), semaphore, nullptr);
        semaphore = VK_NULL_HANDLE;
        close(importSemaphoreFdInfo.fd);
        nativeSurface.fence->Wait(-1);
    }
}

int32_t RSSurfaceOhosVulkan::RequestNativeWindowBuffer(NativeWindowBuffer** nativeWindowBuffer,
    int32_t width, int32_t height, int& fenceFd, bool useAFBC, bool isProtected)
{
    SetNativeWindowInfo(width, height, useAFBC, isProtected);
    struct timespec curTime = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &curTime);
    // 1000000000 is used for transfer second to nsec
    uint64_t duration = static_cast<uint64_t>(curTime.tv_sec) * 1000000000 + static_cast<uint64_t>(curTime.tv_nsec);
    NativeWindowHandleOpt(mNativeWindow, SET_UI_TIMESTAMP, duration);

    auto res = NativeWindowRequestBuffer(mNativeWindow, nativeWindowBuffer, &fenceFd);
    if (res != OHOS::GSERROR_OK) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: RequestBuffer failed %{public}d", res);
        NativeWindowCancelBuffer(mNativeWindow, *nativeWindowBuffer);
    }
    return res;
}

bool RSSurfaceOhosVulkan::PreAllocateProtectedBuffer(int32_t width, int32_t height)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("PreAllocateProtectedBuffer: create native window");
    }
    for (int num = 0; num < PROTECTEDBUFFERSIZE; num++) {
        NativeWindowBuffer* nativeWindowBuffer = nullptr;
        int fenceFd = -1;
        if (RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd, true, true) != OHOS::GSERROR_OK) {
            RS_TRACE_NAME("PreAllocateProtectedBuffer failed.");
            continue;
        }
        RS_TRACE_NAME("PreAllocateProtectedBuffer protectedSurfaceBufferList_ push back.");
        std::lock_guard<std::mutex> lock(protectedSurfaceBufferListMutex_);
        protectedSurfaceBufferList_.emplace_back(std::make_pair(nativeWindowBuffer, fenceFd));
    }
    return true;
}

std::unique_ptr<RSSurfaceFrame> RSSurfaceOhosVulkan::RequestFrame(
    int32_t width, int32_t height, uint64_t uiTimestamp, bool useAFBC, bool isProtected)
{
    if (mNativeWindow == nullptr) {
        mNativeWindow = CreateNativeWindowFromSurface(&producer_);
        ROSEN_LOGD("RSSurfaceOhosVulkan: create native window");
    }

    if (!mSkContext) {
        ROSEN_LOGE("RSSurfaceOhosVulkan: skia context is nullptr");
        return nullptr;
    }

    NativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    {
        std::lock_guard<std::mutex> lock(protectedSurfaceBufferListMutex_);
        if (isProtected && !protectedSurfaceBufferList_.empty()) {
            RS_TRACE_NAME_FMT("protectedSurfaceBufferList_ size = %lu, addr = %p", protectedSurfaceBufferList_.size(),
                this);
            nativeWindowBuffer = protectedSurfaceBufferList_.front().first;
            fenceFd = protectedSurfaceBufferList_.front().second;
            mSurfaceList.emplace_back(nativeWindowBuffer);
            protectedSurfaceBufferList_.pop_front();
        } else {
            if (RequestNativeWindowBuffer(&nativeWindowBuffer, width, height, fenceFd, useAFBC, isProtected) !=
                OHOS::GSERROR_OK) {
                return nullptr;
            }
            mSurfaceList.emplace_back(nativeWindowBuffer);
        }
    }

    NativeBufferUtils::NativeSurfaceInfo& nativeSurface = mSurfaceMap[nativeWindowBuffer];
#ifdef RS_ENABLE_PREFETCH
    __builtin_prefetch(&(nativeSurface.lastPresentedCount), 0, 1);
#endif
    if (nativeSurface.drawingSurface == nullptr) {
        NativeObjectReference(mNativeWindow);
        nativeSurface.window = mNativeWindow;
        nativeSurface.graphicColorGamut = colorSpace_;
        if (!NativeBufferUtils::MakeFromNativeWindowBuffer(
            mSkContext, nativeWindowBuffer, nativeSurface, width, height, isProtected)
            || !nativeSurface.drawingSurface) {
            ROSEN_LOGE("RSSurfaceOhosVulkan: skSurface is null, return");
            mSurfaceList.pop_back();
            NativeWindowCancelBuffer(mNativeWindow, nativeWindowBuffer);
            if (fenceFd != -1) {
                close(fenceFd);
                fenceFd = -1;
            }
            mSurfaceMap.erase(nativeWindowBuffer);
            return nullptr;
        } else {
            ROSEN_LOGD("RSSurfaceOhosVulkan: skSurface create success %{public}zu", mSurfaceMap.size());
        }
    }

    nativeSurface.drawingSurface->ClearDrawingArea();

    if (fenceFd >= 0) {
        nativeSurface.fence = std::make_unique<SyncFence>(fenceFd);
        auto status = nativeSurface.fence->GetStatus();
        if (status != SIGNALED) {
            auto& vkContext = RsVulkanContext::GetSingleton();
            VkSemaphore semaphore = VK_NULL_HANDLE;
            CreateVkSemaphore(semaphore, vkContext, nativeSurface);
            if (semaphore != VK_NULL_HANDLE) {
                nativeSurface.drawingSurface->Wait(1, semaphore);
            }
        }
    }
    int32_t bufferAge;
    if (nativeSurface.lastPresentedCount == -1) {
        bufferAge = 0;
    } else {
        bufferAge = mPresentCount - nativeSurface.lastPresentedCount;
    }
    std::unique_ptr<RSSurfaceFrameOhosVulkan> frame =
        std::make_unique<RSSurfaceFrameOhosVulkan>(nativeSurface.drawingSurface, width, height, bufferAge);
    std::unique_ptr<RSSurfaceFrame> ret(std::move(frame));
    mSkContext->BeginFrame();
    return ret;
}

sptr<SurfaceBuffer> RSSurfaceOhosVulkan::GetCurrentBuffer()
{
    if (mSurfaceList.empty()) {
        return nullptr;
    }
    return mSurfaceList.back()->sfbuffer;
}

void RSSurfaceOhosVulkan::SetUiTimeStamp(const std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
}
#if defined(ROSEN_OHOS)&& defined(ENABLE_HPAE_BLUR)
bool RSSurfaceOhosVulkan::NeedSubmitWithFFTS()
{
    return RSHpaeFfrtPatternManager::Instance().IsUpdated();
}

int GetFftsSemaphore(const uint64_t& frameId, const MHC_PatternTaskName& taskName, uint64_t* eventId,
    uint16_t eventlen, uint31_t* event_handle, VkSemaphore &semaNotifyFfts)
{
    HPAE_TRACE_NAME_FMT("GetFftsSemaphore. taskName:=%d, frameId:=%u", taskName, frameId);
    
    uint16_t eventId[FFRTEVENTLEN] = {0};
    int ret = -1;
    eventId[0] = RSHpaeFfrtPatternManager::Instance().MHCGetVulkanTaskWaitEvent(
        taskName, frameId);
    eventId[1] = RSHpaeFfrtPatternManager::Instance().MHCGetVulkanTaskNotifyEvent(
        taskName, frameId);
    
    if (eventId[0] == 0 || eventId[1] == 0) {
        // 0 is abnormal
        HPAE_LOGE("GetFftsSemaphore mhc get Event fail. \n");
        return -1;
    }

    HPAE_LOGW("mhc_so: taskName:%{public}d, waitevent:%{public}d, notifyevent:%{public}d,
         frameId=${public}" PRIu64 "\n",
        taskName, eventId[0], eventId[1], frameId);
    
    VkDevice vkDevice = RsVulkanContext::GetSingleton().GetDevice();

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.flags = 0;

    VkSemaphoreExtTypeCreateInfoHUAWEI semaphoreExtTypeCreateInfoHUAWEI = {};
    semaphoreExtTypeCreateInfoHUAWEI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_EXT_CREATE_INFO_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.pNext = nullptr;

    semaphoreExtTypeCreateInfoHUAWEI.semaphoreExtType = VK_SEMAPHORE_EXT_TYPE_FFTS_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.eventId = eventId[0];
    semaphoreInfo.pNext = &senaphoreExtTypeCreateInfoHUAWEI;
    vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &semaWaitFfts);

    semaphoreExtTypeCreateInfoHUAWEI.semaphoreExtType = VK_SEMAPHORE_EXT_TYPE_FFTS_HUAWEI;
    semaphoreExtTypeCreateInfoHUAWEI.eventId = eventId[1];
    semaphoreInfo.pNext = &senaphoreExtTypeCreateInfoHUAWEI;
    vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &semNotifyFfts);

    return 0;
}
void RSSurfaceOhosVulkan::SubmitHpaeTask(const uint64_t& curFrameId)
{
        // Submit HPAE
        auto hpaeItem = RSHpaeScheduler::GetInstance().GetCacheHpaeItem();
        auto hpaeTask = hpaeItem.hpaeTask_;
        if (hpaeTask.taskPtr == nullptr) {
            return;
        }
        HPAE_TRACE_NAME("SubmitHpae:. curFrameId=%u, item.gpFrameId=%u", curFrameId, hpaeItem.gpFrameId_);
        std::lock_guard<std::mutex> lock(taskHandleMapMutex_);
        taskHandleMap_[curFrameId] = hpaeTask.taskPtr;
        HPAE_LOGW("mhc_so shenh: GP before aae submit, curFrameId=%{public}" PRIu64 ",
            taskVec=0x%{public}p, taskPtr=0x%{public}p, taskId: %{public}d",
            curFrameId, &(taskHandleMap_[curFrameId]), hpaeTask.taskPtr, hpaeTask.taskId);
                
        void** taskHandleVec[3] = { &(taskHandleMap_[curFrameId]) }; // max count for multi-blur is 3

                auto hpaePreFunc = [=]() {
                    std::lock_guard<std::mutex> lock(taskHandleMapMutex_);
                    HPAE_TRACE_NAME("shenh: GP aae preFunc[%d]", hpaeTask.taskId);
                    HPAE_LOGW("mhc_so shenh: GP before aae submit, curFrameId=%{public}" PRIu64 ",
                        taskVec=0x%{public}p, taskPtr=0x%{public}p, taskId: %{public}d",
                        curFrameId, &(taskHandleMap_[curFrameId]), taskHandleMap_[curFrameId], hpaeTask.taskId);
                    HianimationManager::GetInstance().HianimationPerfTrack();
                };

                auto hpaePostFunc = [=]() {
                    std::lock_guard<std::mutex> lock(taskHandleMapMutex_);
                    HPAE_LOGW("mhc_so shenh: GP aae postfunc, curFrameId=%{public}" PRIu64 ",
                        taskHandleMapMutex_.size=%{public}lu, taskPtr=0x%{public}p",
                        curFrameId, taskHandleMapMutex_.size(), taskHandleMap_[curFrameId]);
                    HPAE_TRACE_NAME("shenh: GP aae postfunc deinit[%d]", hpaeTask.taskId);
                    HianimationManager::GetInstance().HianimationDestroyTaskAndNotify(hpaeTask.taskId);
                    taskHandleMapMutex_.erase(curFrameId);
                };
                RSHpaeFfrtPatternManager::Instance().MHCubmitTask(curFrameId, MHC_PATTERN_TASK_BLUR_AEE,
                    hpaePreFunc, taskHandleVec, 1, hpaePostFunc);
}

void RSSurfaceOhosVulkan::SubmitGPGpuAndHpaeTask(const uint64_t& preFrameId, const uint64_t& curFrameId)
{
    HPAE_TRACE_NAME_FMT("SubmitGPGpuAndHpaeTask. preFrameId=%u, curFrameId=%u", preFrameId, curFrameId);
    HPAE_LOGW("SubmitGPGpuAndHpaeTask. preFrameId=%{public}" PRIu64 ",
        curFrameId=%{public}" PRIu64 "\n", preFrameId, curFrameId);

    if (preFrameId > 0) {
        // Submit GPU1
        auto gpu1PreFunc = [preFrameId]() {
            HPAE_TRACE_NAME_FMT("GPU1 Task GP preFunc, preFrameId=%d", preFrameId);
            HPAE_LOGW("mhc_so GPU1 Task GP preFunc, preFrameId=%{public}" PRIu64 " ", preFrameId);
        };
        auto gpu1PostFunc = [preFrameId]() {
            HPAE_TRACE_NAME_FMT("GPU1 Task GP preFunc, preFrameId=%d. MHCReleaseEGrapg", preFrameId);
            HPAE_LOGW("mhc_so GPU1 Task GP preFunc, preFrameId=%{public}" PRIu64 ". MHCReleaseEGrapg", preFrameId);
            RSHpaeFfrtPatternManager::Instance().MHCReleaseEGrapg(preFrameId);
        };
        RSHpaeFfrtPatternManager::Instance().MHCubmitTask(preFrameId, MHC_PATTERN_TASK_BLUR_GPU1
            gpu1PreFunc, nullptr, 0, gpu1PostFunc);
    }

    if (curFrameId > 0) {
        // Submit GPU0
        auto gpu0PreFunc = [curFrameId]() {
            HPAE_TRACE_NAME_FMT("GPU0 Task GP preFunc, curFrameId=%d", curFrameId);
            HPAE_LOGW("mhc_so GPU0 Task GP preFunc, curFrameId=%{public}" PRIu64 " ", curFrameId);
        };
        auto gpu0PostFunc = [preFrameId, curFrameId]() {
            HPAE_TRACE_NAME_FMT("GPU0 Task GP preFunc, preFrameId=%d, curFrameId=%d", preFrameId, curFrameId);
            HPAE_LOGW("mhc_so GPU0 Task GP preFunc, preFrameId=%{public}" PRIu64 ",  curFrameId=%{public}" PRIu64 " ",
                preFrameId, curFrameId);
        };
        RSHpaeFfrtPatternManager::Instance().MHCubmitTask(curFrameId, MHC_PatternTaskName::BLUR_GPU0,
            gpu0PreFunc, nullptr, 0, gpu0PostFunc);

            SubmitHpaeTask(curFrameId);
        }
    }
    RSHpaeScheduler::GetInstance().Reset();
    RSHpaeFfrtPatternManager::Instance().ResetUpdateFlag();
}

void SetGpuSemaphore(bool& submitWithFFTS, const uint64_t& curFrameId, const uint64_t& curFrameId,
    std::vector<GrBackendSemaphore>& semphoreVec, NativeBufferUtils::NativeSurfaceInfo& surface)
{
    int ret = -1;
    if (preFrameId > 0) {
        VkSemaphore preNotifySemaphore;
        VkSemaphore preWaitSemaphore;
        ret = GetGPEventAndSemaphoreNew(preFrameId, MHC_PATTERN_TASK_BLUR_GPU1,
            eventId, FFRTEVENTLEN, &event_handle, preNotifySemaphore, preWaitSemaphore);
        if (ret != -1) {
            GrBackendSemaphore htsSemaphore;
            htsSemaphore.initVulkan(preNotifySemaphore);
            semphoreVec.emplace_back(std::move(htsSemaphore));
            surface.drawingSurface->Wait(1, preWaitSemaphore);
        } else {
            submitWithFFTS = false;
        }
    }

    if (curFrameId > 0) {
        VkSemaphore notifySemaphore;
        VkSemaphore waitSemaphore;
        ret = GetGPEventAndSemaphoreNew(curFrameId, MHC_PATTERN_TASK_BLUR_GPU1,
            eventId, FFRTEVENTLEN, &event_handle, notifySemaphore, waitSemaphore);
        if (ret != -1) {
            GrBackendSemaphore htsSemaphore;
            htsSemaphore.initVulkan(notifySemaphore);
            semphoreVec.emplace_back(std::move(htsSemaphore));
            surface.drawingSurface->Wait(1, waitSemaphore);
        } else {
            submitWithFFTS = false;
        }
    }
}
#endif

bool RSSurfaceOhosVulkan::FlushFrame(std::unique_ptr<RSSurfaceFrame>& frame, uint64_t uiTimestamp)
{
    if (mSurfaceList.empty()) {
        return false;
    }
#ifdef RS_ENABLE_PREFETCH
    __builtin_prefetch(&mSkContext, 0, 1);
#endif
    auto& vkContext = RsVulkanContext::GetSingleton().GetRsVulkanInterface();

    VkSemaphore semaphore = vkContext.RequireSemaphore();

#ifdef USE_M133_SKIA
    GrBackendSemaphore backendSemaphore = GrBackendSemaphores::MakeVk(semaphore);
#else
    GrBackendSemaphore backendSemaphore;
    backendSemaphore.initVulkan(semaphore);
#endif

    if (mSurfaceMap.find(mSurfaceList.front()) == mSurfaceMap.end()) {
        ROSEN_LOGE("RSSurfaceOhosVulkan Can not find drawingsurface");
        return false;
    }

    auto& surface = mSurfaceMap[mSurfaceList.front()];

    RSTagTracker tagTracker(mSkContext, RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE);

    auto* callbackInfo = new RsVulkanInterface::CallbackSemaphoreInfo(vkContext, semaphore, -1);

    std::vector<GrBackendSemaphore> semphoreVec = {backendSemaphore};

#if defined(ROSEN_OHOS) && defined(ENABLE_HPAE_BLUR)
    RSHpaeScheduler::GetInstance().WaitBuildTask();
    uint32_t event_handle = 0;
    uint16_t eventId[FFRTEVENTLEN] = {0};
    uint64_t preFrameId = RSHpaeScheduler::GetInstance().GetHpaeFrameId();
    uint64_t curFrameId = RSHpaeFfrtPatternManager::Instance().MHCGetCurFrameId();
    bool submitWithFFTS = NeedSubmitWithFFTS() && (preFrameId > 0 || curFrameId > 0);
    HPAE_TRACE_NAME_FMT("shenh: FlushFrame. submitWithFFTS=%d, preFrameId=%u, curFrameId=%u",
        submitWithFFTS, preFrameId, curFrameId);
    
    if (submitWithFFTS) {
        SetGpuSemaphore(submitWithFFTS, preFrameId, curFrameId, semphoreVec);
    }
#endif
    Drawing::FlushInfo drawingFlushInfo;
    drawingFlushInfo.backendSurfaceAccess = true;
    drawingFlushInfo.numSemaphores = semphoreVec.size();
    drawingFlushInfo.backendSemaphore = static_cast<void*>(semphoreVec.data());
    drawingFlushInfo.finishedProc = [](void *context) {
        RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsFrom2DEngine(context);
    };
    drawingFlushInfo.finishedContext = callbackInfo;
    {
        RS_TRACE_NAME("Flush");
        RSTimer timer("Flush", 50); // 50ms
        surface.drawingSurface->Flush(&drawingFlushInfo);
    }
    {
        RS_TRACE_NAME("Submit");
        RSTimer timer("Submit", 50); // 50ms
        mSkContext->Submit();
        mSkContext->EndFrame();
    }

    int fenceFd = -1;
    if (mReservedFlushFd != -1) {
        fdsan_close_with_tag(mReservedFlushFd, LOG_DOMAIN);
        mReservedFlushFd = -1;
    }
    auto queue = vkContext.GetQueue();
    auto err = RsVulkanContext::HookedVkQueueSignalReleaseImageOHOS(
        queue, 1, &semaphore, surface.image, &fenceFd);
    if (err != VK_SUCCESS) {
        if (err == VK_ERROR_DEVICE_LOST) {
            vkContext.DestroyAllSemaphoreFence();
        }
        RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsFromRS(callbackInfo);
        callbackInfo = nullptr;
        ROSEN_LOGE("RSSurfaceOhosVulkan QueueSignalReleaseImageOHOS failed %{public}d", err);
        return false;
    }
    callbackInfo->mFenceFd = ::dup(fenceFd);
    RsVulkanInterface::callbackSemaphoreInfoCnt_.fetch_add(+1, std::memory_order_relaxed);
    mReservedFlushFd = ::dup(fenceFd);
    fdsan_exchange_owner_tag(mReservedFlushFd, 0, LOG_DOMAIN);

    auto ret = NativeWindowFlushBuffer(surface.window, surface.nativeWindowBuffer, fenceFd, {});
    if (ret != OHOS::GSERROR_OK) {
        RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsFromRS(callbackInfo);
        callbackInfo = nullptr;
        ROSEN_LOGE("RSSurfaceOhosVulkan NativeWindowFlushBuffer failed");
        return false;
    }
    mSurfaceList.pop_front();
    RsVulkanInterface::CallbackSemaphoreInfo::DestroyCallbackRefsFromRS(callbackInfo);

    callbackInfo = nullptr;
    surface.fence.reset();
    surface.lastPresentedCount = mPresentCount;
    mPresentCount++;

#if defined(ROSEN_OHOS) && defined(ENABLE_HPAE_BLUR)
    if (submitWithFFTS) {
        SubmitGPGpuAndHpaeTask(preFrameId, curFrameId);
    }
#endif
    return true;
}

void RSSurfaceOhosVulkan::SetColorSpace(GraphicColorGamut colorSpace)
{
    if (colorSpace != colorSpace_) {
        colorSpace_ = colorSpace;
        for (auto &[key, val] : mSurfaceMap) {
            NativeWindowCancelBuffer(mNativeWindow, key);
        }
        mSurfaceMap.clear();
        mSurfaceList.clear();
    }
}

void RSSurfaceOhosVulkan::SetSurfaceBufferUsage(uint64_t usage)
{
    bufferUsage_ = usage;
}

void RSSurfaceOhosVulkan::SetSurfacePixelFormat(int32_t pixelFormat)
{
    if (pixelFormat != pixelFormat_) {
        for (auto &[key, val] : mSurfaceMap) {
            NativeWindowCancelBuffer(mNativeWindow, key);
        }
        mSurfaceMap.clear();
        mSurfaceList.clear();
    }
    pixelFormat_ = pixelFormat;
}

void RSSurfaceOhosVulkan::ClearBuffer()
{
    if (producer_ != nullptr) {
        ROSEN_LOGD("RSSurfaceOhosVulkan: Clear surface buffer!");
        producer_->GoBackground();
    }
}

void RSSurfaceOhosVulkan::ResetBufferAge()
{
    ROSEN_LOGD("RSSurfaceOhosVulkan: Reset Buffer Age!");
}

int RSSurfaceOhosVulkan::DupReservedFlushFd()
{
    if (mReservedFlushFd != -1) {
        return ::dup(mReservedFlushFd);
    }
    return -1;
}
} // namespace Rosen
} // namespace OHOS
