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
#include "pipeline/rs_task_dispatcher.h"

namespace OHOS::Rosen {
SKResourceManager& SKResourceManager::Instance()
{
    static SKResourceManager instance;
    return instance;
}

#ifndef USE_ROSEN_DRAWING
void SKResourceManager::HoldResource(sk_sp<SkImage> img)
{
#ifdef ROSEN_OHOS
    auto tid = gettid();
    if (!RSTaskDispatcher::GetInstance().HasRegisteredTask(tid)) {
        return;
    }
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (std::any_of(skImages_[tid].cbegin(), skImages_[tid].cend(),
        [&img](const sk_sp<SkImage>& skImage) {return skImage.get() == img.get(); })) {
        return ;
    }
    skImages_[tid].push_back(img);
#endif
}
#else
void SKResourceManager::HoldResource(const std::shared_ptr<Drawing::Image> &img)
{
#ifdef ROSEN_OHOS
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
#endif

void SKResourceManager::ReleaseResource()
{
#ifdef ROSEN_OHOS
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
#ifndef USE_ROSEN_DRAWING
    for (auto& skImages : skImages_) {
        if (skImages.second.size() > 0) {
            RSTaskDispatcher::GetInstance().PostTask(skImages.first, [this]() {
            auto tid = gettid();
            std::scoped_lock<std::recursive_mutex> lock(mutex_);
            size_t size = skImages_[tid].size();
            while (size-- > 0) {
                auto image = skImages_[tid].front();
                skImages_[tid].pop_front();
                if (image == nullptr) {
                    continue;
                }
                if (image->unique()) {
                    image = nullptr;
                } else {
                    skImages_[tid].push_back(image);
                }
            }
            });
        }
    }
#else
    for (auto& images : images_) {
        if (!images.second->IsEmpty()) {
            RSTaskDispatcher::GetInstance().PostTask(images.first, [this]() {
                auto tid = gettid();
                std::scoped_lock<std::recursive_mutex> lock(mutex_);
                images_[tid]->ReleaseResource();
            });
        }
    }
#endif
#endif
}
} // OHOS::Rosen