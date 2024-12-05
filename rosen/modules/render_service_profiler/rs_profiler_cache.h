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

#ifndef RS_PROFILER_CACHE_H
#define RS_PROFILER_CACHE_H

#include <map>
#include <mutex>
#include <sstream>

#ifdef RENDER_PROFILER_APPLICATION
#include <vector>
#else
#include "common/rs_macros.h"
#endif

namespace OHOS::Rosen {

class Archive;

struct Image final {
public:
    static constexpr size_t maxSize = 600 * 1024 * 1024; // NOLINT

public:
    explicit Image() = default;

    size_t Size() const;
    void Serialize(Archive& archive);
    bool IsValid() const;

public:
    std::vector<uint8_t> data;
    uint64_t parcelSkipBytes = 0u;
    int32_t dmaWidth = 0;
    int32_t dmaHeight = 0;
    int32_t dmaStride = 0;
    int32_t dmaFormat = 0;
    uint64_t dmaUsage = 0u;
    size_t dmaSize = 0u;
};

class RSB_EXPORT ImageCache final {
public:
    static uint64_t New();
    static bool Add(uint64_t id, Image&& image);
    static bool Exists(uint64_t id);
    static Image* Get(uint64_t id);
    static size_t Size();
    static size_t Consumption();
    static void Reset();

    static std::string Dump();

    static void Serialize(Archive& archive);
    static void Deserialize(Archive& archive);

    // deprecated
    static void Serialize(FILE* file);
    static void Deserialize(FILE* file);
    static void Serialize(std::stringstream& stream);
    static void Deserialize(std::stringstream& stream);

private:
    static std::atomic_uint64_t id_;
    static std::mutex mutex_;
    static std::map<uint64_t, Image> cache_;
    static std::atomic_size_t consumption_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_CACHE_H