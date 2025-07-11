/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "effect_vulkan_context.h"

namespace OHOS::Rosen {
std::map<int, std::shared_ptr<Drawing::GPUContext>> EffectVulkanContext::drawingContextMap_;
std::mutex EffectVulkanContext::drawingContextMutex_;

EffectVulkanContext::EffectVulkanContext(std::string cacheDir)
{
    vulkanInterface_ = std::make_shared<RsVulkanInterface>();
    vulkanInterface_->Init(VulkanInterfaceType::BASIC_RENDER, false);
}

EffectVulkanContext::~EffectVulkanContext()
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    drawingContextMap_.clear();
}

EffectVulkanContext& EffectVulkanContext::GetSingleton(const std::string& cacheDir)
{
    static EffectVulkanContext singleton = EffectVulkanContext(cacheDir);
    return singleton;
}

void EffectVulkanContext::ReleaseDrawingContextForThread(int tid)
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    drawingContextMap_.erase(tid);
}

void EffectVulkanContext::SaveNewDrawingContext(int tid, std::shared_ptr<Drawing::GPUContext> drawingContext)
{
    std::lock_guard<std::mutex> lock(drawingContextMutex_);
    static thread_local auto func = [tid]() {
        EffectVulkanContext::ReleaseDrawingContextForThread(tid);
    };
    static thread_local auto drawContextHolder = std::make_shared<DrawContextHolder>(func);
    drawingContextMap_[tid] = drawingContext;
}

std::shared_ptr<Drawing::GPUContext> EffectVulkanContext::CreateDrawingContext()
{
    static thread_local int tid = gettid();
    {
        std::lock_guard<std::mutex> lock(drawingContextMutex_);
        auto iter = drawingContextMap_.find(tid);
        if (iter != drawingContextMap_.end() && iter->second != nullptr) {
            return iter->second; // has created before
        }
    }
    auto context = vulkanInterface_->DoCreateDrawingContext();
    SaveNewDrawingContext(tid, context);

    return context;
}
}