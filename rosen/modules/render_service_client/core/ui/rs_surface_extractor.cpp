/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui/rs_surface_extractor.h"

#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {

std::shared_ptr<RSSurface> RSSurfaceExtractor::ExtractRSSurface(std::shared_ptr<RSSurfaceNode> node)
{
    if (node == nullptr) {
        ROSEN_LOGE("RSSurfaceExtractor::ExtractRSSurface, node is nullptr");
        return nullptr;
    }
    return node->surface_;
}

} // namespace Rosen
} // namespace OHOS