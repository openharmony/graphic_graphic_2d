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
#include "pipeline/sk_resource_manager.h"

#include "rs_trace.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_task_dispatcher.h"

namespace OHOS::Rosen {
#ifdef ROSEN_OHOS
constexpr uint32_t MAX_CHECK_SIZE = 20; // empirical value
#endif
SKResourceManager& SKResourceManager::Instance()
{
    static SKResourceManager instance;
    return instance;
}

void SKResourceManager::HoldResource(const std::shared_ptr<Drawing::Image> &img)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return;
    }
    auto tid = gettid();
    if (!RSTaskDispatcher::GetInstance().HasRegisteredTask(tid)) {
        return;
    }
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (images_.find(tid) != images_.end()) {
        images_[tid]->HoldResource(img);
    } else {
        std::shared_ptr<Drawing::ResourceHolder> holder = std::make_shared<Drawing::ResourceHolder>();
        holder->HoldResource(img);
        images_.emplace(tid, holder);
    }
#endif
}

void SKResourceManager::HoldResource(std::shared_ptr<Drawing::Surface> surface)
{
#ifdef ROSEN_OHOS
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
        return;
    }
    auto tid = gettid();
    if (!RSTaskDispatcher::GetInstance().HasRegisteredTask(tid)) {
        return;
    }
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (std::any_of(skSurfaces_[tid].cbegin(), skSurfaces_[tid].cend(),
        [&surface](const std::shared_ptr<Drawing::Surface>& skSurface) {return skSurface.get() == surface.get(); })) {
        return;
    }
    skSurfaces_[tid].push_back(surface);
#endif
}

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
void SKResourceManager::DeleteSharedTextureContext(void* context)
{
    SharedTextureContext* cleanupHelper = static_cast<SharedTextureContext*>(context);
    if (cleanupHelper != nullptr) {
        delete cleanupHelper;
        cleanupHelper = nullptr;
    }
}
#endif

bool SKResourceManager::HaveReleaseableResourceCheck(const std::list<std::shared_ptr<Drawing::Surface>> &list)
{
#ifdef ROSEN_OHOS
    if (list.empty()) {
        return false;
    }
    if (list.size() > MAX_CHECK_SIZE) {
        /* to avoid this function taking too long, return true directly,
         * which means resources may need to be released
         */
        return true;
    }
    for (auto& surface : list) {
        if (surface.unique()) {
            return true;
        }
    }
    return false;
#else
    return false;
#endif
}


void SKResourceManager::ReleaseResource()
{
#ifdef ROSEN_OHOS
    RS_TRACE_FUNC();
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    for (auto& images : images_) {
        if (images.second->HaveReleaseableResourceCheck()) {
            RSTaskDispatcher::GetInstance().PostTask(images.first, [this]() {
                auto tid = gettid();
                std::scoped_lock<std::recursive_mutex> lock(mutex_);
                images_[tid]->ReleaseResource();
            });
        }
    }

    for (auto& skSurface : skSurfaces_) {
        if (HaveReleaseableResourceCheck(skSurface.second)) {
            RSTaskDispatcher::GetInstance().PostTask(skSurface.first, [this]() {
                auto tid = gettid();
                std::scoped_lock<std::recursive_mutex> lock(mutex_);
                size_t size = skSurfaces_[tid].size();
                while (size-- > 0) {
#ifdef RS_ENABLE_PREFETCH
                    size_t prefetchStep = 2;
                    if (size > prefetchStep) {
                        __builtin_prefetch(&*(++(++skSurfaces_[tid].begin())), 0, 1);
                    }
#endif
                    auto surface = skSurfaces_[tid].front();
                    skSurfaces_[tid].pop_front();
                    if (surface == nullptr) {
                        continue;
                    }
                    if (surface.unique()) {
                        surface = nullptr;
                    } else {
                        skSurfaces_[tid].push_back(surface);
                    }
                }
            });
        }
    }
#endif
}
} // OHOS::Rosen