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

#include <unordered_map>
#include "image/image.h"

#include "memory/rs_dfx_string.h"
#include "memory/rs_memory_track.h"

namespace OHOS {
namespace Media {
class PixelMap;
}

namespace Rosen {
class RSB_EXPORT RSImageCache {
public:
    static RSImageCache& Instance();

    void CacheDrawingImage(uint64_t uniqueId, std::shared_ptr<Drawing::Image> img);
    std::shared_ptr<Drawing::Image> GetDrawingImageCache(uint64_t uniqueId) const;
    void IncreaseDrawingImageCacheRefCount(uint64_t uniqueId);
    void ReleaseDrawingImageCache(uint64_t uniqueId);

    void CachePixelMap(uint64_t uniqueId, std::shared_ptr<Media::PixelMap> pixelMap);
    std::shared_ptr<Media::PixelMap> GetPixelMapCache(uint64_t uniqueId) const;
    void IncreasePixelMapCacheRefCount(uint64_t uniqueId);
    void ReleasePixelMapCache(uint64_t uniqueId);

    void CacheRenderDrawingImageByPixelMapId(uint64_t uniqueId, std::shared_ptr<Drawing::Image> img, pid_t tid = -1);
    std::shared_ptr<Drawing::Image> GetRenderDrawingImageCacheByPixelMapId(uint64_t uniqueId, pid_t tid = -1) const;

    RSImageCache() = default;
    ~RSImageCache() = default;

private:
    RSImageCache(const RSImageCache&) = delete;
    RSImageCache(const RSImageCache&&) = delete;
    RSImageCache& operator=(const RSImageCache&) = delete;
    RSImageCache& operator=(const RSImageCache&&) = delete;
    void ReleaseDrawingImageCacheByPixelMapId(uint64_t uniqueId);

    mutable std::mutex mutex_;
    // the second element of pair indicates ref count of skImage/pixelMap by RSImage
    // ref count +1 in RSImage Unmarshalling func and -1 in RSImage destruction func
    // skImage/pixelMap will be removed from cache if ref count decreases to 0
    std::unordered_map<uint64_t, std::pair<std::shared_ptr<Drawing::Image>, uint64_t>> drawingImageCache_;
    std::unordered_map<uint64_t, std::pair<std::shared_ptr<Media::PixelMap>, uint64_t>> pixelMapCache_;
    mutable std::mutex mapMutex_;
    std::unordered_map<uint64_t, std::unordered_map<pid_t, std::shared_ptr<Drawing::Image>>>
        pixelMapIdRelatedDrawingImageCache_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_CACHE_H
