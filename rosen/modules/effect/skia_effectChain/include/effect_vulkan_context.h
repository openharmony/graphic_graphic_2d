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

#ifndef EFFECT_VULKAN_CONTEXT_H
#define EFFECT_VULKAN_CONTEXT_H

#include "platform/ohos/backend/rs_vulkan_context.h"

namespace OHOS::Rosen {
class EffectVulkanContext {
public:
    class DrawContextHolder {
    public:
        explicit DrawContextHolder(std::function<void()> callback) : destructCallback_(std::move(callback)) {}

        ~DrawContextHolder()
        {
            destructCallback_();
        }
    private:
        std::function<void()> destructCallback_;
    };
    static EffectVulkanContext& GetSingleton(const std::string& cacheDir = "");
    explicit EffectVulkanContext(std::string cacheDir = "");
    ~EffectVulkanContext();

    EffectVulkanContext(const EffectVulkanContext&) = delete;
    EffectVulkanContext &operator=(const EffectVulkanContext&) = delete;
    EffectVulkanContext(const EffectVulkanContext&&) = delete;
    EffectVulkanContext &operator=(const EffectVulkanContext&&) = delete;

    std::shared_ptr<Drawing::GPUContext> CreateDrawingContext();
    static void ReleaseDrawingContextForThread(int tid);

    static void SaveNewDrawingContext(int tid, std::shared_ptr<Drawing::GPUContext> drawingContext);
private:
    std::shared_ptr<RsVulkanInterface> vulkanInterface_;
    static std::map<int, std::shared_ptr<Drawing::GPUContext>> drawingContextMap_;
    static std::mutex drawingContextMutex_;
};
}

#endif