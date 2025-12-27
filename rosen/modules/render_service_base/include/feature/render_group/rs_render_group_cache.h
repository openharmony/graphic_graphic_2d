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

#ifndef RS_RENDER_GROUP_CACHE_H
#define RS_RENDER_GROUP_CACHE_H

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSRenderGroupCache {
public:
    RSRenderGroupCache() = default;
    ~RSRenderGroupCache() = default;

    RSRenderGroupCache(const RSRenderGroupCache& other) = default;
    RSRenderGroupCache(RSRenderGroupCache&& other) = default;
    RSRenderGroupCache& operator=(const RSRenderGroupCache& other) = default;
    RSRenderGroupCache& operator=(RSRenderGroupCache&& other) = default;

    bool ExcludedFromNodeGroup(bool isExcluded);
    bool IsExcludedFromNodeGroup() const
    {
        return excludedFromNodeGroup_;
    }

    bool SetHasChildExcludedFromNodeGroup(bool isExcluded);
    bool HasChildExcludedFromNodeGroup() const
    {
        return hasChildExcludedFromNodeGroup_;
    }

private:
    bool excludedFromNodeGroup_ = false;
    bool hasChildExcludedFromNodeGroup_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_RENDER_GROUP_CACHE_H