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