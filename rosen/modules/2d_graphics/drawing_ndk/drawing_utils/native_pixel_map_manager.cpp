/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "native_pixel_map_manager.h"

#ifdef OHOS_PLATFORM
#include "pixelmap_native_impl.h"
#include "native_pixel_map.h"
#endif

NativePixelMapManager& NativePixelMapManager::GetInstance()
{
    static NativePixelMapManager instance;
    return instance;
}

void NativePixelMapManager::Register(void* handle, NativePixelMapType type)
{
    std::lock_guard<std::mutex> lck(mutex_);
    mapper_.emplace(handle, type);
}

void NativePixelMapManager::Unregister(void* handle)
{
    std::lock_guard<std::mutex> lck(mutex_);
    mapper_.erase(handle);
}

NativePixelMapType NativePixelMapManager::GetNativePixelMapType(void* handle)
{
    std::lock_guard<std::mutex> lck(mutex_);
    auto iter = mapper_.find(handle);
    if (iter == mapper_.end()) {
        return NativePixelMapType::OBJECT_UNKNOWN;
    }
    return iter->second;
}

namespace OHOS {
namespace Rosen {
#ifdef OHOS_PLATFORM
std::shared_ptr<OHOS::Media::PixelMap> GetPixelMapFromNativePixelMap(OH_Drawing_PixelMap* pixelMap)
{
    if (pixelMap == nullptr) {
        return nullptr;
    }
    
    std::shared_ptr<OHOS::Media::PixelMap> pixelMapPtr = nullptr;
    switch (NativePixelMapManager::GetInstance().GetNativePixelMapType(pixelMap)) {
        case NativePixelMapType::OBJECT_FROM_C:
            if (pixelMap) {
                pixelMapPtr = reinterpret_cast<OH_PixelmapNative*>(pixelMap)->GetInnerPixelmap();
            }
            break;
        case NativePixelMapType::OBJECT_FROM_JS:
            pixelMapPtr = OHOS::Media::PixelMapNative_GetPixelMap(reinterpret_cast<NativePixelMap_*>(pixelMap));
            break;
        default:
            break;
    }
    
    return pixelMapPtr;
}
#endif
} // namespace Rosen
} // namespace OHOS