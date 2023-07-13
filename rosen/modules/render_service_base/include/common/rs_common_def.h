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
#include <string>
#include <unordered_map>
#include <unistd.h>

#include "common/rs_macros.h"

namespace OHOS {
class Surface;

namespace Rosen {
using AnimationId = uint64_t;
using NodeId = uint64_t;
using PropertyId = uint64_t;
constexpr uint32_t UNI_MAIN_THREAD_INDEX = UINT32_MAX;
constexpr uint64_t INVALID_NODEID = UINT64_MAX;

// types in the same layer should be 0/1/2/4/8
// types for UINode
enum class RSUINodeType : uint32_t {
    BASE_NODE    = 0x000010u,
    RS_NODE      = 0x000110u,   // formerly RSPropertyNode
    DISPLAY_NODE = 0x001110u,
    SURFACE_NODE = 0x002110u,
    PROXY_NODE   = 0x004110u,
    CANVAS_NODE  = 0x008110u,   // formerly RSNode
    EFFECT_NODE  = 0x010110u,
    ROOT_NODE    = 0x108110u,
    CANVAS_DRAWING_NODE = 0x208110u,
};

enum class FollowType : uint8_t {
    NONE,
    FOLLOW_TO_PARENT,
    FOLLOW_TO_SELF,
};

static inline const std::unordered_map<RSUINodeType, std::string> RSUINodeTypeStrs = {
    {RSUINodeType::BASE_NODE,           "BaseNode"},
    {RSUINodeType::DISPLAY_NODE,        "DisplayNode"},
    {RSUINodeType::RS_NODE,             "RsNode"},
    {RSUINodeType::SURFACE_NODE,        "SurfaceNode"},
    {RSUINodeType::PROXY_NODE,          "ProxyNode"},
    {RSUINodeType::CANVAS_NODE,         "CanvasNode"},
    {RSUINodeType::ROOT_NODE,           "RootNode"},
    {RSUINodeType::EFFECT_NODE,         "EffectNode"},
    {RSUINodeType::CANVAS_DRAWING_NODE, "CanvasDrawingNode"},
};

// types for RenderNode
enum class RSRenderNodeType : uint32_t {
    BASE_NODE    = 0x000011u,
    RS_NODE      = 0x000111u,   // formerly RSPropertyRenderNode
    DISPLAY_NODE = 0x001111u,
    SURFACE_NODE = 0x002111u,
    PROXY_NODE   = 0x004111u,
    CANVAS_NODE  = 0x008111u,   // formerly RSRenderNode
    EFFECT_NODE  = 0x010111u,
    ROOT_NODE    = 0x108111u,
    CANVAS_DRAWING_NODE = 0x208111u,
};

enum class CacheType : uint8_t {
    NONE = 0,
    CONTENT,
    ANIMATE_PROPERTY,
};

enum RSDrawingCacheType : uint16_t {
    DISABLED_CACHE = 0,
    FORCED_CACHE,    // must-to-do case
    TARGETED_CACHE   // suggested case which could be disabled by optimized strategy
};

// priority for node, higher number means lower priority
enum class NodePriorityType : uint32_t {
    MAIN_PRIORITY = 0, // node must render in main thread
    SUB_HIGH_PRIORITY, // node render in sub thread with high priority
    SUB_LOW_PRIORITY, // node render in sub thread with low priority
};

// status for sub thread node
enum class CacheProcessStatus : uint32_t {
    WAITING = 0, // waiting for process
    DOING, // processing
    DONE, // processed
};

// types for RSSurfaceRenderNode
enum class RSSurfaceNodeType : uint8_t {
    DEFAULT,
    APP_WINDOW_NODE,          // surfacenode created as app main window
    ABILITY_COMPONENT_NODE,   // surfacenode created as ability component
    SELF_DRAWING_NODE,        // surfacenode created by arkui component (except ability component)
    STARTING_WINDOW_NODE,     // starting window, surfacenode created by wms
    LEASH_WINDOW_NODE,        // leashwindow
    SELF_DRAWING_WINDOW_NODE, // create by wms, such as pointer window and bootanimation
};

struct RSSurfaceRenderNodeConfig {
    NodeId id = 0;
    std::string name = "SurfaceNode";
    std::string bundleName = "";
    RSSurfaceNodeType nodeType = RSSurfaceNodeType::DEFAULT;
    void* additionalData = nullptr;
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
inline constexpr bool ROSEN_EQ(const T& x, const T& y)
{
    if constexpr (std::is_floating_point<T>::value) {
        return (std::abs((x) - (y)) <= (std::numeric_limits<T>::epsilon()));
    } else {
        return x == y;
    }
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

inline bool ROSEN_LNE(float left, float right) //less not equal
{
    constexpr float epsilon = -0.001f;
    return (left - right) < epsilon;
}

inline bool ROSEN_GE(float left, float right) //great or equal
{
    constexpr float epsilon = -0.001f;
    return (left - right) > epsilon;
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
