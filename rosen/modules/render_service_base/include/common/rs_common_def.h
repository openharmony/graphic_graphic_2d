/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <unistd.h>

#include "common/rs_macros.h"

namespace OHOS {
class Surface;

namespace Rosen {
using AnimationId = uint64_t;
using NodeId = uint64_t;
using PropertyId = uint64_t;

// types in the same layer should be 0/1/2/4/8
// types for UINode
enum class RSUINodeType : uint32_t {
    BASE_NODE    = 0x00010u,
    RS_NODE      = 0x00110u,   // formerly RSPropertyNode
    DISPLAY_NODE = 0x01110u,
    SURFACE_NODE = 0x02110u,
    PROXY_NODE   = 0x04110u,
    CANVAS_NODE  = 0x08110u,   // formerly RSNode
    ROOT_NODE    = 0x18110u,
};

enum class FollowType : uint8_t {
    NONE,
    FOLLOW_TO_PARENT,
    FOLLOW_TO_SELF,
    FOLLOW_VISITOR,
};

static inline const std::unordered_map<RSUINodeType, std::string> RSUINodeTypeStrs = {
    {RSUINodeType::BASE_NODE,    "BaseNode"},
    {RSUINodeType::DISPLAY_NODE, "DisplayNode"},
    {RSUINodeType::RS_NODE,      "RsNode"},
    {RSUINodeType::SURFACE_NODE, "SurfaceNode"},
    {RSUINodeType::PROXY_NODE,   "ProxyNode"},
    {RSUINodeType::CANVAS_NODE,  "CanvasNode"},
    {RSUINodeType::ROOT_NODE,    "RootNode"},
};

// types for RenderNode
enum class RSRenderNodeType : uint32_t {
    BASE_NODE    = 0x00011u,
    RS_NODE      = 0x00111u,   // formerly RSPropertyRenderNode
    DISPLAY_NODE = 0x01111u,
    SURFACE_NODE = 0x02111u,
    PROXY_NODE   = 0x04111u,
    CANVAS_NODE  = 0x08111u,   // formerly RSRenderNode
    ROOT_NODE    = 0x18111u,
};

// types for RSSurfaceRenderNode
enum class RSSurfaceNodeType : uint8_t {
    DEFAULT,
    APP_WINDOW_NODE,   // surfacenode created as app main window
    ABILITY_COMPONENT_NODE, // surfacenode created as ability component
    SELF_DRAWING_NODE, // surfacenode created by arkui component (except ability component)
    STARTING_WINDOW_NODE, //  starting window, surfacenode created by wms
    LEASH_WINDOW_NODE, // leashwindow
    SELF_DRAWING_WINDOW_NODE, // create by wms
};

using UseSurfaceToRenderFunc = bool (*)(const void*, const size_t, const int32_t, const int32_t);
struct RSSurfaceRenderNodeConfig {
    NodeId id = 0;
    std::string name = "SurfaceNode";
    RSSurfaceNodeType nodeType = RSSurfaceNodeType::DEFAULT;
    UseSurfaceToRenderFunc onRender = nullptr;
};

struct RSDisplayNodeConfig {
    uint64_t screenId = 0;
    bool isMirrored = false;
    NodeId mirrorNodeId = 0;
};

#if defined(M_PI)
constexpr float PI = M_PI;
#else
static const float PI = std::atanf(1.0) * 4;
#endif

template<typename T>
inline bool ROSEN_EQ(const T& x, const T& y)
{
    return x == y;
}

template<>
inline bool ROSEN_EQ(const float& x, const float& y)
{
    return (std::abs((x) - (y)) <= (std::numeric_limits<float>::epsilon()));
}

template<>
inline bool ROSEN_EQ(const double& x, const double& y)
{
    return (std::abs((x) - (y)) <= (std::numeric_limits<double>::epsilon()));
}

template<>
inline bool ROSEN_EQ(const long double& x, const long double& y)
{
    return (std::abs((x) - (y)) <= (std::numeric_limits<long double>::epsilon()));
}

template<typename T>
inline bool ROSEN_EQ(T x, T y, T epsilon)
{
    return (std::abs((x) - (y)) <= (epsilon));
}

template<typename T>
inline bool ROSEN_EQ(const std::weak_ptr<T>& x, const std::weak_ptr<T>& y)
{
    return !(x.owner_before(y) || y.owner_before(x));
}

class MemObject {
public:
    explicit MemObject(size_t size) : size_(size) {}
    virtual ~MemObject() = default;

    void* operator new(size_t size);
    void operator delete(void* ptr);

    void* operator new(std::size_t size, const std::nothrow_t&) noexcept;
    void operator delete(void* ptr, const std::nothrow_t&) noexcept;

protected:
    size_t size_;
};

inline constexpr pid_t ExtractPid(uint64_t id)
{
    // extract high 32 bits of nodeid/animationId/propertyId as pid
    return static_cast<pid_t>(id >> 32);
}

template<class Container, class Predicate>
inline typename Container::size_type EraseIf(Container& container, Predicate pred)
{
    // erase from container if pred returns true, backport of c++20 std::remove_if
    typename Container::size_type oldSize = container.size();
    const typename Container::iterator end = container.end();
    for (typename Container::iterator iter = container.begin(); iter != end;) {
        if (pred(*iter)) {
            iter = container.erase(iter);
        } else {
            ++iter;
        }
    }
    return oldSize - container.size();
}

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H
