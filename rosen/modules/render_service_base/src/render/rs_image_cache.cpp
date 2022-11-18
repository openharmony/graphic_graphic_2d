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

#include "render/rs_image_cache.h"

namespace OHOS {
namespace Rosen {
// modify the RSImageCache instance as global to extend life cycle, fix destructor crash
static RSImageCache gRSImageCacheInstance;

RSImageCache& RSImageCache::Instance()
{
    return gRSImageCacheInstance;
}

void RSImageCache::CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img)
{
    if (img) {
        std::lock_guard<std::mutex> lock(mutex_);
        skiaImageCache_.emplace(uniqueId, img);
    }
}

sk_sp<SkImage> RSImageCache::GetSkiaImageCache(uint64_t uniqueId) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end()) {
        return it->second;
    }
    return nullptr;
}

void RSImageCache::ReleaseSkiaImageCache(uint64_t uniqueId)
{
    // release the skimage if nobody else holds it
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = skiaImageCache_.find(uniqueId);
    if (it != skiaImageCache_.end() && (!it->second || it->second->unique())) {
        skiaImageCache_.erase(it);
    }
}
} // namespace Rosen
} // namespace OHOS
