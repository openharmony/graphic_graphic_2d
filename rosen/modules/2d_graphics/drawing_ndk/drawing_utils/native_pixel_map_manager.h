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

#ifndef DRAWING_NATIVE_PIXEL_MAP_MANAGER_H
#define DRAWING_NATIVE_PIXEL_MAP_MANAGER_H
#include <mutex>
#include <unordered_map>

enum class NativePixelMapType : uint8_t {
    OBJECT_UNKNOWN,
    OBJECT_FROM_JS,
    OBJECT_FROM_C
};

class NativePixelMapManager {
public:
    static NativePixelMapManager& GetInstance();
    ~NativePixelMapManager() = default;

    void Register(void* handle, NativePixelMapType type);
    void Unregister(void* handle);
    NativePixelMapType GetNativePixelMapType(void* handle);

    NativePixelMapManager(const NativePixelMapManager&) = delete;
    NativePixelMapManager(const NativePixelMapManager&&) = delete;
    NativePixelMapManager& operator=(const NativePixelMapManager&) = delete;
    NativePixelMapManager& operator=(const NativePixelMapManager&&) = delete;
private:
    NativePixelMapManager() = default;
    std::unordered_map<void*, NativePixelMapType> mapper_;
    std::mutex mutex_;
};
#endif // DRAWING_NATIVE_PIXEL_MAP_MANAGER_H