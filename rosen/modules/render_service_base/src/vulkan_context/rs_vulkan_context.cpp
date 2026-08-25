/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

/**
 * RsVulkanContext实现（重构版）
 *
 * 从RsVulkanInterface拆分到独立文件
 * 提供CreateOrGet静态方法实现三种类型单例
 */

#include "vulkan_context/rs_vulkan_context.h"

#include <memory>

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"

#include "vulkan/vulkan.h"

namespace OHOS {
namespace Rosen {

/**
 * Get - 根据类型获取单例
 *
 * 实现三种类型的单例管理：
 * - 0 - BASIC_RENDER: 启用HTS，非保护
 * - 1 - PROTECTED_REDRAW: 不启用HTS，保护模式(DRM)
 * - 2 - UNPROTECTED_REDRAW: 不启用HTS，非保护
 */
RsVulkanContext& RsVulkanContext::Get(RenderEngineType type)
{
    switch (type) {
        case RenderEngineType::UNPROTECTED_REDRAW: {
            static RsVulkanContext unprotectedRedrawSingleton = RsVulkanContext(false, false, type);
            return unprotectedRedrawSingleton;
        }
        case RenderEngineType::PROTECTED_REDRAW: {
            static RsVulkanContext protectedRedrawSingleton = RsVulkanContext(true, false, type);
            return protectedRedrawSingleton;
        }
        case RenderEngineType::BASIC_RENDER:
        default:
        {
            static RsVulkanContext basicRenderSingleton = RsVulkanContext(false, true, RenderEngineType::BASIC_RENDER);
            return basicRenderSingleton;
        }
    }
}

RsVulkanContext::RsVulkanContext(bool isProtected, bool isHtsEnable, RenderEngineType type)
{
    RS_TRACE_NAME("Init VulkanContext");
    type_ = type;
    vulkanInterface_ = std::make_shared<RsVulkanInterface>(isProtected, isHtsEnable, type_);

    if (IsValid()) {
        ROSEN_LOGI("RsVulkanContext::Init success for type %{public}u", static_cast<uint32_t>(type_));
    } else {
        ROSEN_LOGE("RsVulkanContext::Init failed for type %{public}u", static_cast<uint32_t>(type_));
    }
}

/**
 * 析构函数
 */
RsVulkanContext::~RsVulkanContext()
{
    type_ = RenderEngineType::BASIC_RENDER;
}

bool RsVulkanContext::QueryMaxGpuBufferSize(uint32_t& maxWidth, uint32_t& maxHeight)
{
    if (!vulkanInterface_) {
        return false;
    }
    return vulkanInterface_->QueryMaxGpuBufferSize(maxWidth, maxHeight);
}

std::shared_ptr<Drawing::GPUContext> RsVulkanContext::CreateDrawingGPUContext(const std::string& cacheDir)
{
    auto drawingContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    std::string version = std::to_string(VK_API_VERSION_1_2);
    MemoryHandler::ConfigureContext(&options, version.c_str(), version.size(), cacheDir);
    drawingContext->BuildFromVK(GetGrVkBackendContext(), options);

    int maxResources = 0;
    size_t maxResourcesSize = 0;
    drawingContext->GetResourceCacheLimits(&maxResources, &maxResourcesSize);
    if (maxResourcesSize > 0) {
        drawingContext->SetResourceCacheLimits(CACHE_LIMITS_TIMES * maxResources,
            CACHE_LIMITS_TIMES * std::fmin(maxResourcesSize, GR_CACHE_MAX_BYTE_SIZE));
    } else {
        drawingContext->SetResourceCacheLimits(GR_CACHE_MAX_COUNT, GR_CACHE_MAX_BYTE_SIZE);
    }
    return drawingContext;
}

void RsVulkanContext::ReleaseDrawingGPUContext(std::shared_ptr<Drawing::GPUContext>& gpuContext)
{
    if (gpuContext) {
        if (IsValid()) {
            gpuContext->FlushAndSubmit(true);
            gpuContext->PurgeUnlockAndSafeCacheGpuResources();
        }
        gpuContext.reset();
    }
}
} // namespace Rosen
} // namespace OHOS
