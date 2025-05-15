/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "rs_vk_image_manager.h"

#include <parameter.h>
#include <parameters.h>
#include "include/core/SkColorSpace.h"
#include "native_buffer_inner.h"
#include "platform/common/rs_log.h"
#include "include/gpu/GrBackendSurface.h"
#include "pipeline/hardware_thread/rs_hardware_thread.h"
#include "pipeline/rs_task_dispatcher.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"
#include "params/rs_surface_render_params.h"

namespace OHOS {
namespace Rosen {
namespace {
void WaitAcquireFence(const sptr<SyncFence>& acquireFence)
{
    if (acquireFence == nullptr) {
        return;
    }
    acquireFence->Wait(3000); // 3000ms
}

constexpr size_t MAX_CACHE_SIZE = 16;
constexpr size_t MAX_CACHE_SIZE_FOR_REUSE = 40;
static const bool ENABLE_VKIMAGE_DFX = system::GetBoolParameter("persist.graphic.enable_vkimage_dfx", false);

#define DFX_LOG(enableDfx, format, ...) \
    ((enableDfx) ? (void) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__) : (void) 0)
#define DFX_LOGD(enableDfx, format, ...) \
    ((enableDfx) ? (void) HILOG_ERROR(LOG_CORE, format, ##__VA_ARGS__) : \
                   (void) HILOG_DEBUG(LOG_CORE, format, ##__VA_ARGS__))
}

NativeVkImageRes::~NativeVkImageRes()
{
    NativeBufferUtils::DeleteVkImage(mVulkanCleanupHelper);
    DestroyNativeWindowBuffer(mNativeWindowBuffer);
}

std::shared_ptr<NativeVkImageRes> NativeVkImageRes::Create(sptr<OHOS::SurfaceBuffer> buffer)
{
    if (buffer == nullptr) {
        ROSEN_LOGE("NativeVkImageRes::Create buffer is nullptr");
        return nullptr;
    }
    auto width = buffer->GetSurfaceBufferWidth();
    auto height = buffer->GetSurfaceBufferHeight();
    NativeWindowBuffer* nativeWindowBuffer = CreateNativeWindowBufferFromSurfaceBuffer(&buffer);
    bool isProtected = (buffer->GetUsage() & BUFFER_USAGE_PROTECTED) != 0;
    auto backendTexture = NativeBufferUtils::MakeBackendTextureFromNativeBuffer(nativeWindowBuffer,
        width, height, isProtected);
    if (!backendTexture.IsValid() || !backendTexture.GetTextureInfo().GetVKTextureInfo()) {
        DestroyNativeWindowBuffer(nativeWindowBuffer);
        return nullptr;
    }
    return std::make_unique<NativeVkImageRes>(
        nativeWindowBuffer,
        backendTexture,
        new NativeBufferUtils::VulkanCleanupHelper(RsVulkanContext::GetSingleton(),
            backendTexture.GetTextureInfo().GetVKTextureInfo()->vkImage,
            backendTexture.GetTextureInfo().GetVKTextureInfo()->vkAlloc.memory));
}

std::shared_ptr<NativeVkImageRes> RSVkImageManager::MapVkImageFromSurfaceBuffer(
    const sptr<OHOS::SurfaceBuffer>& buffer,
    const sptr<SyncFence>& acquireFence,
    pid_t threadIndex)
{
    if (buffer == nullptr) {
        ROSEN_LOGE("RSVkImageManager::MapVkImageFromSurfaceBuffer buffer is nullptr");
        return nullptr;
    }
    WaitAcquireFence(acquireFence);
    std::lock_guard<std::mutex> lock(opMutex_);
    bool isProtectedCondition = (buffer->GetUsage() & BUFFER_USAGE_PROTECTED) ||
        RsVulkanContext::GetSingleton().GetIsProtected();
    auto bufferId = buffer->GetSeqNum();
    if (isProtectedCondition || imageCacheSeqs_.find(bufferId) == imageCacheSeqs_.end()) {
        RS_TRACE_NAME_FMT("create vkImage, bufferId=%u", bufferId);
        return NewImageCacheFromBuffer(buffer, threadIndex, isProtectedCondition);
    } else {
        RS_TRACE_NAME_FMT("find cache vkImage, bufferId=%u", bufferId);
        return imageCacheSeqs_[bufferId];
    }
}

std::shared_ptr<NativeVkImageRes> RSVkImageManager::CreateImageCacheFromBuffer(sptr<OHOS::SurfaceBuffer> buffer,
    const sptr<SyncFence>& acquireFence)
{
    WaitAcquireFence(acquireFence);
    auto bufferId = buffer->GetSeqNum();
    auto imageCache = NativeVkImageRes::Create(buffer);
    if (imageCache == nullptr) {
        ROSEN_LOGE(
            "RSVkImageManager::CreateImageCacheFromBuffer: failed to create ImageCache for buffer id %{public}d.",
            bufferId);
        return nullptr;
    }
    return imageCache;
}

std::shared_ptr<NativeVkImageRes> RSVkImageManager::NewImageCacheFromBuffer(
    const sptr<OHOS::SurfaceBuffer>& buffer, pid_t threadIndex, bool isProtectedCondition)
{
    auto bufferId = buffer->GetSeqNum();
    auto deleteFlag = buffer->GetBufferDeleteFromCacheFlag();
    auto imageCache = NativeVkImageRes::Create(buffer);
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSVkImageManager::NewImageCacheFromBuffer: failed to create ImageCache for buffer id %{public}d.",
            bufferId);
        return {};
    }

    size_t imageCacheSeqSize = imageCacheSeqs_.size();
    DFX_LOGD(ENABLE_VKIMAGE_DFX, "RSVkImageManagerDfx: create image, bufferId=%{public}u, threadIndex=%{public}d, "
        "deleteFlag=%{public}d, isProtected=%{public}d,cacheSeq=%{public}lu",
        bufferId, threadIndex, deleteFlag, isProtectedCondition, imageCacheSeqSize);
    RS_TRACE_NAME_FMT("RSVkImageManagerDfx: create image, bufferId=%u, "
        "deleteFlag=%d, isProtected=%d, cacheSeq=%lu",
        bufferId, deleteFlag, isProtectedCondition, imageCacheSeqSize);
    imageCache->SetThreadIndex(threadIndex);
    imageCache->SetBufferDeleteFromCacheFlag(deleteFlag);
    if (isProtectedCondition) {
        return imageCache;
    }

    if (imageCacheSeqSize <= MAX_CACHE_SIZE_FOR_REUSE) {
        imageCacheSeqs_.emplace(bufferId, imageCache);
    }
    cacheQueue_.push(bufferId);

    return imageCache;
}

void RSVkImageManager::ShrinkCachesIfNeeded()
{
    while (cacheQueue_.size() > MAX_CACHE_SIZE) {
        const uint32_t id = cacheQueue_.front();
        UnMapVkImageFromSurfaceBuffer(id);
        cacheQueue_.pop();
    }
}

void RSVkImageManager::UnMapVkImageFromSurfaceBuffer(uint32_t seqNum)
{
    DFX_LOG(ENABLE_VKIMAGE_DFX, "RSVkImageManagerDfx: tryUnmapImage, bufferId=%{public}u", seqNum);
    pid_t threadIndex = UNI_RENDER_THREAD_INDEX;
    {
        std::lock_guard<std::mutex> lock(opMutex_);
        if (imageCacheSeqs_.count(seqNum) == 0) {
            return;
        }
        threadIndex = imageCacheSeqs_[seqNum]->GetThreadIndex();
    }
    auto func = [this, seqNum]() {
        std::lock_guard<std::mutex> lock(opMutex_);
        auto iter = imageCacheSeqs_.find(seqNum);
        if (iter == imageCacheSeqs_.end()) {
            return;
        }
        RS_TRACE_NAME_FMT("RSVkImageManagerDfx: unmap image, bufferId=%u", seqNum);
        imageCacheSeqs_.erase(iter);
        DFX_LOGD(ENABLE_VKIMAGE_DFX, "RSVkImageManagerDfx: UnmapImage, bufferId=%{public}u, "
            "cacheSeq=[%{public}lu]",
            seqNum, imageCacheSeqs_.size());
    };
    DFX_LOGD(ENABLE_VKIMAGE_DFX, "RSVkImageManagerDfx: findImageToUnmap, bufferId=%{public}u", seqNum);
    RSTaskDispatcher::GetInstance().PostTask(threadIndex, func);
}

void RSVkImageManager::DumpVkImageInfo(std::string &dumpString)
{
    std::lock_guard<std::mutex> lock(opMutex_);
    dumpString.append("\n---------RSVkImageManager-DumpVkImageInfo-Begin----------\n");
    dumpString.append("imageCacheSeqs size: " + std::to_string(imageCacheSeqs_.size()) + "\n");
    for (auto iter = imageCacheSeqs_.begin(); iter != imageCacheSeqs_.end(); ++iter) {
        dumpString.append("vkimageinfo: bufferId=" + std::to_string(iter->first) +
            ", threadIndex=" + std::to_string(iter->second->GetThreadIndex()) +
            ", deleteFlag=" + std::to_string(iter->second->GetBufferDeleteFromCacheFlag()) + "\n");
    }
    dumpString.append("\n---------RSVkImageManager-DumpVkImageInfo-End----------\n");
}

} // namespace Rosen
} // namespace OHOS