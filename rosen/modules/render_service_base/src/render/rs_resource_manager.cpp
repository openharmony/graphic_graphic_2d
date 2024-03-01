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
#include "render/rs_resource_manager.h"

namespace OHOS::Rosen {
RSResourceManager& RSResourceManager::Instance()
{
    static RSResourceManager sInstance;
    return sInstance;
}

void RSResourceManager::UploadTexture(bool paraUpload, const std::shared_ptr<Drawing::Image>& image,
    const std::shared_ptr<Media::PixelMap>& pixelMap, uint64_t uniqueId)
{
    if (hookFunction_ != nullptr) {
        hookFunction_(paraUpload, image, pixelMap, uniqueId);
    } else {
        RS_LOGE("hookFunction_ is nullptr");
    }
}

void RSResourceManager::SetUploadTextureFunction(TextureUpload hook) {
    if (hook != nullptr) {
        hookFunction_ = hook;
    } else {
        RS_LOGE("SetUploadTextureFunction is nullptr");
    }
}
}
