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
#include <set>
#include <cmath>

namespace OHOS {
namespace Rosen {
void MemoryHandler::ConfigureContext(GrContextOptions* context, const char* identity,
    const size_t size, const std::string& cacheFilePath, bool isUni)
{
    context->fAllowPathMaskCaching = true;
    auto &cache = ShaderCache::Instance();
    cache.SetFilePath(cacheFilePath);
    cache.InitShaderCache(identity, size, isUni);
    context->fPersistentCache = &cache;
}
}   // namespace Rosen
}   // namespace OHOS