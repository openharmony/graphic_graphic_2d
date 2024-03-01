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

#include "memory_handler.h"
#include "render_context_log.h"
#include <set>
#include <cmath>

namespace OHOS {
namespace Rosen {
void MemoryHandler::ConfigureContext(Drawing::GPUContextOptions* context, const char* identity,
    const size_t size, const std::string& cacheFilePath, bool isUni)
{
    context->SetAllowPathMaskCaching(true);
    auto& cache = ShaderCache::Instance();
    cache.SetFilePath(cacheFilePath);
    cache.InitShaderCache(identity, size, isUni);
    context->SetPersistentCache(&cache);
}

void MemoryHandler::ClearRedundantResources(Drawing::GPUContext* gpuContext)
{
    if (gpuContext != nullptr) {
        LOGD("gpuContext clear redundant resources");
        gpuContext->Flush();
        // GPU resources that haven't been used in the past 10 seconds
        gpuContext->PerformDeferredCleanup(std::chrono::seconds(10));
    }
}

std::string MemoryHandler::QuerryShader()
{
    const auto& cache = ShaderCache::Instance();
    if (!cache.IfInitialized()) {
        LOGD("ShaderCache is not intialized.");
    }
    size_t shaderRam = cache.QuerryShaderSize();
    size_t shaderNum = cache.QuerryShaderNum();
    std::string ramString = "ShaderCache RAM used:" + std::to_string(shaderRam / 1024) + " kB\n";
    ramString += "ShaderCache num is: " + std::to_string(shaderNum);
    return ramString;
}

std::string MemoryHandler::ClearShader()
{
    const auto& cache = ShaderCache::Instance();
    LOGD("All shaders are cleaned");
    cache.CleanAllShaders();
    return "All shaders are cleaned, RAM freed: 0";
}
}   // namespace Rosen
}   // namespace OHOS