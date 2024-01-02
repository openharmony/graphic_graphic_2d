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

#ifndef RS_RESOURCE_MANAGER
#define RS_RESOURCE_MANAGER

#include "platform/common/rs_log.h"

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkImage.h"
#else
#include "image/image.h"
#endif
#include "render/rs_pixel_map_util.h"

#include "draw/surface.h"
#include "draw/canvas.h"
#include "image/gpu_context.h"
#include "image/image.h"

namespace OHOS::Rosen {

class RSB_EXPORT RSResourceManager final {
public:
#ifndef USE_ROSEN_DRAWING
    typedef void (*TextureUpload)(bool, const sk_sp<SkImage>&,
    const std::shared_ptr<Media::PixelMap>&, uint64_t);
#else
    typedef void (*TextureUpload)(bool, const std::shared_ptr<Drawing::Image>&,
    const std::shared_ptr<Media::PixelMap>&, uint64_t);
#endif

    static RSResourceManager& Instance();

#ifndef USE_ROSEN_DRAWING
    void UploadTexture(bool paraUpload, const sk_sp<SkImage>&,
    const std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId);
#else
    void UploadTexture(bool paraUpload, const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId);
#endif

private:
    friend class RSUploadResourceThread;

    void SetUploadTextureFunction(TextureUpload hook);

private:
    RSResourceManager() = default;
    ~RSResourceManager() = default;
    RSResourceManager(const RSResourceManager&) = delete;
    RSResourceManager(const RSResourceManager&&)= delete;
    RSResourceManager& operator=(const RSResourceManager&) = delete;
    RSResourceManager& operator=(const RSResourceManager&&) = delete;

    TextureUpload hookFunction_ = nullptr;
};

}
#endif