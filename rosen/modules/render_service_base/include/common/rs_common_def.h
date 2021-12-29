/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H

#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
using AnimationId = uint64_t;
using NodeId = uint64_t;

enum class RSRenderNodeType {
    BASE_NODE     = 0x0001,
    DISPLAY_NODE  = 0x0011,
    PROPERTY_NODE = 0x0021,
    SURFACE_NODE  = 0x0121,
    NODE          = 0x0221,
    ROOT_NODE     = 0x1221,
    TEXTURE_NODE  = 0x2221,
};

struct RSSurfaceRenderNodeConfig {
    NodeId id = 0;
};

struct RSDisplayNodeConfig {
    uint64_t screenId = 0;
};

#if defined(M_PI)
const float PI = M_PI;
#else
const float PI = std::atanf(1.0) * 4;
#endif

#define ROSEN_DEFINE_GETTER(type, funcName, memberName) \
public:                                                 \
    type Get##funcName() const                          \
    {                                                   \
        return memberName##_;                           \
    }                                                   \
                                                        \
protected:                                              \
    type memberName##_;

template<typename T>
inline const T ROSEN_EPSILON()
{
    return std::numeric_limits<T>::epsilon();
}

template<typename T>
inline bool ROSEN_EQ(T x, T y, T epsilon = ROSEN_EPSILON<T>())
{
    return (std::abs((x) - (y)) <= (epsilon));
}

class MemAllocater final {
    struct BlockHead {
        int size;
        char ptr[0];
    };
    using Cache = std::vector<char*>;

public:
    static MemAllocater& GetInstance();
    ~MemAllocater();

    void* Alloc(size_t size);
    void Free(void* ptr);

private:
    MemAllocater() = default;
    MemAllocater(const MemAllocater&) = delete;
    MemAllocater& operator=(const MemAllocater&) = delete;

    std::mutex mutex_;
    std::map<size_t, Cache> memCaches_;
    std::vector<char*> blocks_;
    static constexpr unsigned sizeStep_ = 64;
};

class MemObject {
public:
    explicit MemObject(size_t size) : size_(size) {}
    virtual ~MemObject() {}

    void* operator new(size_t size);
    void operator delete(void* ptr);

    void* operator new(std::size_t size, const std::nothrow_t&) noexcept;
    void operator delete(void* ptr, const std::nothrow_t&) noexcept;

protected:
    size_t size_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H
