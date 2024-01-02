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

#ifndef OHOS_MEMORY_HANDLER_H
#define OHOS_MEMORY_HANDLER_H

#ifndef USE_ROSEN_DRAWING
#include <include/core/SkSurface.h>
#if defined(NEW_SKIA)
#include <include/gpu/GrDirectContext.h>
#else
#include <include/gpu/GrContext.h>
#endif
#else
#include "image/gpu_context.h"
#endif
#include "cache_data.h"
#include "shader_cache.h"

namespace OHOS {
namespace Rosen {
class MemoryHandler {
public:
#ifndef USE_ROSEN_DRAWING
    static void ConfigureContext(GrContextOptions* context, const char* identity, const size_t size,
        const std::string& cacheFilePath = mUniRenderCacheDir, bool isUni = true);
#else
    static void ConfigureContext(Drawing::GPUContextOptions* context, const char* identity, const size_t size,
        const std::string& cacheFilePath = mUniRenderCacheDir, bool isUni = true);
#endif
    MemoryHandler() = default;
#ifndef USE_ROSEN_DRAWING
#if defined(NEW_SKIA)
    static void ClearRedundantResources(GrDirectContext* grContext);
#else
    static void ClearRedundantResources(GrContext* grContext);
#endif
#else
    static void ClearRedundantResources(Drawing::GPUContext* gpuContext);
#endif
    static std::string QuerryShader();
    static std::string ClearShader();
private:
    inline static const std::string mUniRenderCacheDir = "/data/service/el0/render_service";
};
}   // namespace Rosen
}   // namespace OHOS
#endif // OHOS_MEMORY_HANDLER_H
