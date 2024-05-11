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

#include "drawing_pixel_map.h"
#include "native_pixel_map_manager.h"

OH_Drawing_PixelMap* OH_Drawing_PixelMapGetFromNativePixelMap(NativePixelMap_* nativePixelMap)
{
    NativePixelMapManager::GetInstance().Register(nativePixelMap, NativePixelMapType::OBJECT_FROM_JS);
    return reinterpret_cast<OH_Drawing_PixelMap*>(nativePixelMap);
}

OH_Drawing_PixelMap* OH_Drawing_PixelMapGetFromOhPixelMapNative(OH_PixelmapNative* ohPixelMapNative)
{
    NativePixelMapManager::GetInstance().Register(ohPixelMapNative, NativePixelMapType::OBJECT_FROM_C);
    return reinterpret_cast<OH_Drawing_PixelMap*>(ohPixelMapNative);
}

void OH_Drawing_PixelMapDissolve(OH_Drawing_PixelMap* pixelMap)
{
    NativePixelMapManager::GetInstance().Unregister(pixelMap);
}