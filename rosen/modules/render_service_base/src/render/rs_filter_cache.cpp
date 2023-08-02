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

#include "render/rs_filter_cache.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {

RSFilterCache& RSFilterCache::Instance()
{
    static RSFilterCache instance;
    return instance;
}

std::shared_ptr<RSMaterialFilter> RSFilterCache::GetMaterialFilterFromCache(const MaterialParam& materialParam,
    BLUR_COLOR_MODE colorMode)
{
    RSFilter::FilterType type = RSFilter::FilterType::MATERIAL;
    std::uint64_t radius = uint64_t(materialParam.radius);
    std::uint64_t saturation = uint64_t(materialParam.saturation * PRECISION_FACTOR);
    std::uint64_t brightness = uint64_t(materialParam.brightness * PRECISION_FACTOR);

    uint32_t hash = SkOpts::hash(&type, sizeof(type), 0);
    hash = SkOpts::hash(&radius, sizeof(radius), hash);
    hash = SkOpts::hash(&saturation, sizeof(saturation), hash);
    hash = SkOpts::hash(&brightness, sizeof(brightness), hash);
    hash = SkOpts::hash(&materialParam.maskColor, sizeof(materialParam.maskColor), hash);
    hash = SkOpts::hash(&colorMode, sizeof(colorMode), hash);

    auto iter = materialFilterCache_.find(hash);
    if (iter != materialFilterCache_.end()) {
        return iter->second;
    }

    std::shared_ptr<RSMaterialFilter> filter = std::make_shared<RSMaterialFilter>(materialParam, colorMode);
    if (filter) {
        if (materialFilterCache_.size() > MAX_LIMIT_SIZE) {
            materialFilterCache_.clear();
        }
        materialFilterCache_.emplace(hash, filter);
    }
    return filter;
}

} // namespace Rosen
} // namespace OHOS