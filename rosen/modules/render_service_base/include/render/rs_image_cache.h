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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H

#include <mutex>
#include <unordered_map>
#include "include/core/SkImage.h"

namespace OHOS {
namespace Rosen {
class RSImageCache {
public:
    static RSImageCache& Instance();

    void CacheSkiaImage(uint64_t uniqueId, sk_sp<SkImage> img);
    sk_sp<SkImage> GetSkiaImageCache(uint64_t uniqueId) const;
    void ReleaseSkiaImageCache(uint64_t uniqueId);

    RSImageCache() = default;
    ~RSImageCache() = default;

private:
    RSImageCache(const RSImageCache&) = delete;
    RSImageCache(const RSImageCache&&) = delete;
    RSImageCache& operator=(const RSImageCache&) = delete;
    RSImageCache& operator=(const RSImageCache&&) = delete;

    mutable std::mutex mutex_;
    std::unordered_map<uint64_t, sk_sp<SkImage>> skiaImageCache_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H
