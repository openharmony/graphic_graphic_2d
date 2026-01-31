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

#include <atomic>
#include <cmath>
#include <functional>
#include <limits.h>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <unistd.h>
#include <utils/rect.h>
#include <vector>

#include "common/rs_macros.h"
#include "common/rs_anco_type.h"
#include "draw/color.h"
#include "../../../../utils/color_manager/export/color_space.h"

namespace OHOS {
class Surface;

namespace Rosen {
using AnimationId = uint64_t;
using NodeType = uint8_t;
using FrameRateLinkerId = uint64_t;
using InteractiveImplictAnimatorId = uint64_t;
using LeashPersistentId = uint64_t;
using ModifierId = uint64_t;
using NodeId = uint64_t;
using PropertyId = uint64_t;
using SurfaceId = uint64_t;

constexpr uint32_t UNI_MAIN_THREAD_INDEX = UINT32_MAX;
constexpr uint32_t UNI_RENDER_THREAD_INDEX = UNI_MAIN_THREAD_INDEX - 1;
constexpr uint64_t INVALID_NODEID = 0;
constexpr int32_t INSTANCE_ID_UNDEFINED = -1;
constexpr uint32_t RGBA_MAX = 255;
constexpr uint64_t INVALID_LEASH_PERSISTENTID = 0;
constexpr uint8_t TOP_OCCLUSION_SURFACES_NUM = 3;
constexpr uint8_t OCCLUSION_ENABLE_SCENE_NUM = 2;
constexpr int16_t DEFAULT_OCCLUSION_SURFACE_ORDER = -1;
constexpr int MAX_DIRTY_ALIGNMENT_SIZE = 128;
static const std::string CAPTURE_WINDOW_NAME = "CapsuleWindow";
constexpr uint32_t DEFAULT_DYNAMIC_RANGE_MODE_STANDARD = 2;
constexpr uint32_t DYNAMIC_RANGE_MODE_HIGH = 0;
constexpr uint32_t DYNAMIC_RANGE_MODE_CONSTRAINT = 1;
constexpr int32_t UI_PiPLINE_NUM_UNDEFINED = -1;

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

template<typename T>
inline constexpr bool ROSEN_NE(const T& x, const T& y)
{
    return !ROSEN_EQ(x, y);
}

inline bool ROSEN_LNE(float left, float right) // less not equal
{
    constexpr float epsilon = -0.001f;
    return (left - right) < epsilon;
}

inline bool ROSEN_GNE(float left, float right) // great not equal
{
    constexpr float epsilon = 0.001f;
    return (left - right) > epsilon;
}

inline bool ROSEN_GE(float left, float right) // great or equal
{
    constexpr float epsilon = -0.001f;
    return (left - right) > epsilon;
}

inline bool ROSEN_LE(float left, float right) // less or equal
{
    constexpr float epsilon = 0.001f;
    return (left - right) < epsilon;
}

/**
 * Bitmask enumeration for hierarchical type identification
 * Descendant types must include all ancestor bits following the rules:
 * childFlags = ParentFlags | AdditionalBits
 */
enum class RSUINodeType : uint32_t {
    UNKNOW               = 0x0000u,
    RS_NODE              = 0x0001u,
    DISPLAY_NODE         = 0x0011u,
    SURFACE_NODE         = 0x0021u,
    PROXY_NODE           = 0x0041u,
    CANVAS_NODE          = 0x0081u,
    EFFECT_NODE          = 0x0101u,
    WINDOW_KEYFRAME_NODE = 0x0801u,
    ROOT_NODE            = 0x1081u,
    CANVAS_DRAWING_NODE  = 0x2081u,
    UNION_NODE           = 0x4081u,
};

enum class FollowType : uint8_t {
    NONE,
    FOLLOW_TO_PARENT,
    FOLLOW_TO_SELF,
};

enum class RSSurfaceFrameType : uint8_t {
    NONE = 0,
    RS_SURFACE_FRAME = 1,
    RS_SURFACE_FRAME_DARWIN = 2,
    RS_SURFACE_FRAME_OHOS = 3,
    RS_SURFACE_FRAME_WINDOWS = 4,
    RS_SURFACE_FRAME_OHOS_GL = 5,
    RS_SURFACE_FRAME_OHOS_RASTER = 6,
    RS_SURFACE_FRAME_OHOS_VULKAN = 7,
};

#define LIKELY(exp) (__builtin_expect((exp) != 0, true))
#define UNLIKELY(exp) (__builtin_expect((exp) != 0, false))

#ifdef CM_FEATURE_ENABLE
#define CM_INLINE __attribute__((always_inline))
#else
#define CM_INLINE
#endif

/**
 * Bitmask enumeration for hierarchical type identification
 * Descendant types must include all ancestor bits following the rule:
 *   ChildFlags = ParentFlags | AdditionalBits
 */
enum class RSRenderNodeType : uint32_t {
    UNKNOW                 = 0x0000u,
    RS_NODE                = 0x0001u,
    SCREEN_NODE            = 0x0011u,
    SURFACE_NODE           = 0x0021u,
    PROXY_NODE             = 0x0041u,
    CANVAS_NODE            = 0x0081u,
    EFFECT_NODE            = 0x0101u,
    ROUND_CORNER_NODE      = 0x0201u,
    LOGICAL_DISPLAY_NODE   = 0x0401u,
    WINDOW_KEYFRAME_NODE   = 0x0801u,
    ROOT_NODE              = 0x1081u,
    CANVAS_DRAWING_NODE    = 0x2081u,
    UNION_NODE             = 0x4081u,
};

// types for Processor
enum class RSProcessorType : uint32_t {
    UNKNOW                          = 0x0000u,
    RS_PROCESSOR                    = 0x0001u,
    PHYSICAL_SCREEN_PROCESSOR       = 0x0011u,
    VIRTUAL_SCREEN_PROCESSOR        = 0x0021u,
    UNIRENDER_PROCESSOR             = 0x0041u,
    UNIRENDER_VIRTUAL_PROCESSOR     = 0x0081u,
};

enum class CompositeType : uint32_t {
    UNI_RENDER_COMPOSITE = 0,
    UNI_RENDER_MIRROR_COMPOSITE,
    UNI_RENDER_EXPAND_COMPOSITE,
    HARDWARE_COMPOSITE,
    SOFTWARE_COMPOSITE
};

enum RSRenderParamsDirtyType {
    NO_DIRTY = 0,
    MATRIX_DIRTY,
    LAYER_INFO_DIRTY,
    BUFFER_INFO_DIRTY,
    DRAWING_CACHE_TYPE_DIRTY,
    MAX_DIRTY_TYPE,
};

enum class NodeDirtyType : uint32_t {
    NOT_DIRTY           = 0x0000u,
    GEOMETRY            = 0x0001u,
    BACKGROUND          = 0x0002u,
    CONTENT             = 0x0004u,
    FOREGROUND          = 0x0008u,
    OVERLAY             = 0x0010u,
    APPEARANCE          = 0x0020u,
};

enum class CacheType : uint8_t {
    NONE = 0,
    CONTENT,
    ANIMATE_PROPERTY,
};

enum class DrawableCacheType : uint8_t {
    NONE = 0,
    CONTENT,
};

enum RSDrawingCacheType : uint8_t {
    DISABLED_CACHE = 0,
    FORCED_CACHE,           // must-to-do case
    TARGETED_CACHE,         // suggested case which could be disabled by optimized strategy
    FOREGROUND_FILTER_CACHE // using cache to draw foreground filter
};

enum class FilterCacheType : uint8_t {
    NONE              = 0,
    SNAPSHOT          = 1,
    FILTERED_SNAPSHOT = 2,
    BOTH              = SNAPSHOT | FILTERED_SNAPSHOT,
};

// opinc state
enum NodeCacheState : uint8_t {
    STATE_INIT = 0,
    STATE_CHANGE,
    STATE_UNCHANGE,
    STATE_DISABLE,
};

enum NodeChangeType : uint8_t {
    KEEP_UNCHANGE = 0,
    SELF_DIRTY,
};

// opinc cache state
enum NodeStrategyType : uint8_t {
    CACHE_NONE = 0,
    DDGR_OPINC_DYNAMIC,
    OPINC_AUTOCACHE,
    NODE_GROUP,
    CACHE_DISABLE,
};

enum NodeRecordState : uint8_t {
    RECORD_NONE = 0,
    RECORD_CALCULATE,
    RECORD_CACHING,
    RECORD_CACHED,
    RECORD_DISABLE,
};

enum DrawAreaEnableState : uint8_t {
    DRAW_AREA_INIT = 0,
    DRAW_AREA_ENABLE,
    DRAW_AREA_DISABLE,
};

// priority for node, higher number means lower priority
enum class NodePriorityType : uint8_t {
    MAIN_PRIORITY = 0, // node must render in main thread
    SUB_FOCUSNODE_PRIORITY, // node render in sub thread with the highest priority
    SUB_VIDEO_PRIORITY, // node render in sub thread with the second highest priority
    SUB_HIGH_PRIORITY, // node render in sub thread with the second priority
    SUB_LOW_PRIORITY, // node render in sub thread with low priority
};

enum class RSVisibleLevel : uint32_t {
    RS_ALL_VISIBLE = 0,
    RS_SEMI_NONDEFAULT_VISIBLE,
    RS_SEMI_DEFAULT_VISIBLE,
    RS_INVISIBLE,
    RS_SYSTEM_ANIMATE_SCENE,
    RS_UNKNOW_VISIBLE_LEVEL,
};

// status for sub thread node
enum class CacheProcessStatus : uint8_t {
    WAITING = 0, // waiting for process
    DOING, // processing
    DONE, // processed
    SKIPPED, // skip cur process and wait for next new data to process
    UNKNOWN,
};

// the type of surfaceCapture
enum class SurfaceCaptureType : uint8_t {
    DEFAULT_CAPTURE = 0,       // displayNode capture or window capture
    UICAPTURE = 1,             // UI capture
    SURFACE_CAPTURE_TYPE_BUTT, // a boundary for SurfaceTureCaptureType Security Check
};

#ifdef TP_FEATURE_ENABLE
// the type of TpFeatureConfig
enum class TpFeatureConfigType : uint8_t {
    DEFAULT_TP_FEATURE = 0,
    AFT_TP_FEATURE,
};
#endif

// types for RenderNodeDrawable
enum class RSRenderNodeDrawableType : uint32_t {
    UNKNOW = 0,
    RS_NODE_DRAWABLE,
    LOGICAL_DISPLAY_NODE_DRAWABLE,
    SCREEN_NODE_DRAWABLE,
    SURFACE_NODE_DRAWABLE,
    CANVAS_NODE_DRAWABLE,
    EFFECT_NODE_DRAWABLE,
    ROOT_NODE_DRAWABLE,
    CANVAS_DRAWING_NODE_DRAWABLE,
    UNION_NODE_DRAWABLE,
    WINDOW_KEYFRAME_NODE_DRAWABLE,
};

// zOrder of topLayer
enum class TopLayerZOrder : uint32_t {
    ROUNDED_CORNER_TOP = 9901,
    ROUNDED_CORNER_BOTTOM = 9900,
    POINTER_WINDOW = 9800,
    CHARGE_ACTION_TEXT = 9300,
    CHARGE_3D_MOTION = 9200,
    STYLUS = 9100,
    MINIMUM_VALUE = 9000,
};

struct FocusAppInfo {
    int32_t pid = -1;
    int32_t uid = -1;
    std::string bundleName = "";
    std::string abilityName = "";
    uint64_t focusNodeId = 0;
};

struct RSUICaptureInRangeParam {
    NodeId endNodeId = INVALID_NODEID;
    bool useBeginNodeSize = true;
};

enum class CaptureError : uint8_t {
    CAPTURE_OK = 0,
    CAPTURE_NO_PERMISSION,
    CAPTURE_NO_NODE,
    CAPTURE_CONFIG_WRONG,
    CAPTURE_PIXELMAP_NULL,
    CAPTURE_PIXELMAP_COPY_ERROR,
    CAPTURE_NULL_FAIL,
    HDR_SET_FAIL,
    CAPTURE_RENDER_FAIL,
    AUTO_NOT_SUPPORT,
    COLOR_SPACE_NOT_SUPPORT,
    DYNAMIC_RANGE_NOT_SUPPORT,
    CAPTURE_NO_SECURE_PERMISSION,
    // please add new enum before this comment
    CAPTURE_ERROR_BOUNDARY_BUTT,
};

struct RSSurfaceCaptureConfig {
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    bool useDma = false;
    bool useCurWindow = true;
    SurfaceCaptureType captureType = SurfaceCaptureType::DEFAULT_CAPTURE;
    bool isSync = false;
    Drawing::Rect mainScreenRect = {};
    std::vector<NodeId> blackList = {}; // exclude surfacenode in screenshot
    bool isSoloNodeUiCapture = false;
    bool isHdrCapture = false;
    bool needF16WindowCaptureForScRGB = false;
    bool needErrorCode = false;
    RSUICaptureInRangeParam uiCaptureInRangeParam = {};
    Drawing::Rect specifiedAreaRect = {};
    uint32_t backGroundColor = Drawing::Color::COLOR_TRANSPARENT;
    // {colorspace, isAutoAjust}
    std::pair<uint32_t, bool> colorSpace = {OHOS::ColorManager::ColorSpaceName::SRGB, false};
    // {dynamicRangeMode, isAutoAjust}
    std::pair<uint32_t, bool> dynamicRangeMode = {DEFAULT_DYNAMIC_RANGE_MODE_STANDARD, false};
    bool isSyncRender = false;
    bool isConfigTriggered = false;

    // When adding new members, please ensure to add the corresponding comparison logic in the operator== and
    // serialization/deserialization logic in the OnSurfaceCapture method.
    bool operator==(const RSSurfaceCaptureConfig& config) const
    {
        return ROSEN_EQ(scaleX, config.scaleX) && ROSEN_EQ(scaleY, config.scaleY) &&
               (useDma == config.useDma) && (useCurWindow == config.useCurWindow) &&
               (captureType == config.captureType) && (isSync == config.isSync) &&
               (mainScreenRect == config.mainScreenRect) && (blackList == config.blackList) &&
               (isSoloNodeUiCapture == config.isSoloNodeUiCapture) &&
               (isHdrCapture == config.isHdrCapture) &&
               (needF16WindowCaptureForScRGB == config.needF16WindowCaptureForScRGB) &&
               (needErrorCode == config.needErrorCode) &&
               (uiCaptureInRangeParam.endNodeId == config.uiCaptureInRangeParam.endNodeId) &&
               (uiCaptureInRangeParam.useBeginNodeSize == config.uiCaptureInRangeParam.useBeginNodeSize) &&
               (specifiedAreaRect == config.specifiedAreaRect) &&
               (backGroundColor == config.backGroundColor) &&
               (colorSpace == config.colorSpace) &&
               (dynamicRangeMode == config.dynamicRangeMode);
    }
};

struct RSSurfaceCaptureBlurParam {
    bool isNeedBlur = false;
    float blurRadius = 1E-6;
};

struct RSSurfaceCaptureParam {
    NodeId id = 0;
    RSSurfaceCaptureConfig config = {};
    bool isSystemCalling = false;
    bool isSelfCapture = false;
    bool isFreeze = false;
    RSSurfaceCaptureBlurParam blurParam = {};
    bool needCaptureSpecialLayer = false;
    bool hasDirtyContentInSurfaceCapture = false;
};

struct RSSurfaceCapturePermissions {
    bool screenCapturePermission = false;
    bool isSystemCalling = false;
    bool selfCapture = false;
};

#define CHECK_FALSE_RETURN(var)      \
    do {                             \
        if (!(var)) {                \
            return;                  \
        }                            \
    } while (0)                      \

#define CHECK_FALSE_RETURN_VALUE(var, value)     \
    do {                                         \
        if (!(var)) {                            \
            return value;                        \
        }                                        \
    } while (0)

#define IS_SCB_WINDOW_TYPE(windowType)                                                                                 \
    (windowType == SurfaceWindowType::SYSTEM_SCB_WINDOW || windowType == SurfaceWindowType::SCB_DESKTOP ||             \
        windowType == SurfaceWindowType::SCB_WALLPAPER || windowType == SurfaceWindowType::SCB_SCREEN_LOCK ||          \
        windowType == SurfaceWindowType::SCB_NEGATIVE_SCREEN || windowType == SurfaceWindowType::SCB_DROPDOWN_PANEL || \
        windowType == SurfaceWindowType::SCB_VOLUME_PANEL ||                                                           \
        windowType == SurfaceWindowType::SCB_BANNER_NOTIFICATION || windowType == SurfaceWindowType::SCB_GESTURE_BACK)

enum class DeviceType : uint8_t {
    PHONE,
    PC,
    TABLET,
    OTHERS,
};

enum GrallocBufferAttr : uint32_t {
    // used in set roi region to codec, must be the same as private key in codec
    GRALLOC_BUFFER_ATTR_BUFFER_ROI_INFO = 2054,
};

// types for PC SystemAnimatedScenes
enum class SystemAnimatedScenes : uint32_t {
    ENTER_MISSION_CENTER, // Enter the mission center
    EXIT_MISSION_CENTER, // Exit the mission center
    ENTER_TFS_WINDOW, // Three-finger sliding window recovery
    EXIT_TFU_WINDOW, // The three-finger up window disappears
    ENTER_WINDOW_FULL_SCREEN, // Enter the window full screen
    EXIT_WINDOW_FULL_SCREEN, // Exit the window full screen
    ENTER_MAX_WINDOW, // Enter the window maximization state
    EXIT_MAX_WINDOW, // Exit the window maximization state
    ENTER_SPLIT_SCREEN, // Enter the split screen
    EXIT_SPLIT_SCREEN, // Exit the split screen
    ENTER_APP_CENTER, // Enter the app center
    EXIT_APP_CENTER, // Exit the app center
    APPEAR_MISSION_CENTER, // A special case scenario that displays the mission center
    ENTER_WIND_CLEAR, // Enter win+D in clear screen mode
    ENTER_WIND_RECOVER, // Enter win+D in recover mode
    ENTER_RECENTS, // Enter recents only for phone, end with EXIT_RECENTS instead of OTHERS
    EXIT_RECENTS, // Exit recents only for phone
    LOCKSCREEN_TO_LAUNCHER, // Enter unlock screen for pc scene
    ENTER_MIN_WINDOW, // Enter the window minimization state
    RECOVER_MIN_WINDOW, // Recover minimized window
    SNAPSHOT_ROTATION,  //  Enter tablet's snapshot rotation scene
    DRAG_WINDOW, // Enter scale window scene
    OTHERS, // 1.Default state 2.The state in which the animation ends
};

// types for RSSurfaceRenderNode
enum class RSSurfaceNodeType : uint8_t {
    DEFAULT,
    APP_WINDOW_NODE,          // surfacenode created as app main window
    STARTING_WINDOW_NODE,     // starting window, surfacenode created by wms
    SELF_DRAWING_WINDOW_NODE, // create by wms, such as bootanimation
    LEASH_WINDOW_NODE,        // leashwindow
    ABILITY_COMPONENT_NODE,   // surfacenode created as ability component
    SELF_DRAWING_NODE,        // surfacenode created by arkui component (except ability component)
    SURFACE_TEXTURE_NODE,      // create by video
    FOREGROUND_SURFACE,
    SCB_SCREEN_NODE,          // surfacenode created as sceneboard
    UI_EXTENSION_COMMON_NODE, // uiextension node
    UI_EXTENSION_SECURE_NODE, // uiextension node that requires info callback
    CURSOR_NODE,              // cursor node created by MMI
    ABILITY_MAGNIFICATION_NODE, // local magnification
    NODE_MAX,
};

enum class MultiThreadCacheType : uint8_t {
    NONE = 0,
    LEASH_WINDOW,
    ARKTS_CARD,
    NONFOCUS_WINDOW,
};

enum class UiFirstModeType : uint8_t {
    SINGLE_WINDOW_MODE,
    MULTI_WINDOW_MODE,
};

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore

enum class UiFirstCcmType : uint8_t {
    SINGLE = 1,
    MULTI = 2,
    HYBRID = 3,
};

enum class RSUIFirstSwitch {
    NONE = 0,               // follow RS rules
    MODAL_WINDOW_CLOSE = 1, // open app with modal window animation, close uifirst
    FORCE_DISABLE = 2,      // force close uifirst
    FORCE_ENABLE = 3,       // force open uifirst
    FORCE_ENABLE_LIMIT = 4, // force open uifirst, but for limited
    FORCE_DISABLE_NONFOCUS = 5, // force close uifirst when only in nonfocus window
    FORCE_DISABLE_CARD = 6, // force close uifirst on card
};

enum class SelfDrawingNodeType : uint8_t {
    DEFAULT,
    VIDEO,
    XCOM,
};

enum class SurfaceWindowType : uint8_t {
    DEFAULT_WINDOW = 0,
    SYSTEM_SCB_WINDOW = 1,
    SCB_DESKTOP = 2,
    SCB_WALLPAPER = 3,
    SCB_SCREEN_LOCK = 4,
    SCB_NEGATIVE_SCREEN = 5,
    SCB_DROPDOWN_PANEL = 6,
    SCB_VOLUME_PANEL = 7,
    SCB_BANNER_NOTIFICATION = 8,
    SCB_GESTURE_BACK = 9,
};

enum class SurfaceHwcNodeType : uint8_t {
    DEFAULT_HWC_TYPE = 0,
    DEFAULT_HWC_VIDEO = 1,
    DEFAULT_HWC_ROSENWEB = 2,
};

struct RSSurfaceRenderNodeConfig {
    NodeId id = 0;
    std::string name = "SurfaceNode";
    RSSurfaceNodeType nodeType = RSSurfaceNodeType::DEFAULT;
    void* additionalData = nullptr;
    bool isTextureExportNode = false;
    bool isSync = false;
    enum SurfaceWindowType surfaceWindowType = SurfaceWindowType::DEFAULT_WINDOW;
    std::string bundleName = "";
};

struct RSAdvancedDirtyConfig {
    // a threshold, if the number of rectangles is larger than it, we will merge all rectangles to one
    static const int RECT_NUM_MERGING_ALL = 35;
    // a threshold, if the number of rectangles is larger than it, we will merge all rectangles by level
    static const int RECT_NUM_MERGING_BY_LEVEL = 20;
    // maximal number of dirty rectangles in one surface/display node when advancedDirty is opened
    static const int MAX_RECT_NUM_EACH_NODE = 10;
    // number of dirty rectangles in one surface/display node when advancedDirty is closed
    static const int DISABLED_RECT_NUM_EACH_NODE = 1;
    // expected number of rectangles after merging
    static const int EXPECTED_OUTPUT_NUM = 3;
    // maximal tolerable cost in merging
    // if the merging cost of two rectangles is larger than it, we will not merge
    // later it could be set to a quantity related to screen area
    static const int MAX_TOLERABLE_COST = INT_MAX;
};

static RSAdvancedDirtyConfig advancedDirtyConfig;

// codes for arkui-x start
// types for RSSurfaceExt
enum class RSSurfaceExtType : uint8_t {
    NONE,
    SURFACE_TEXTURE,
    SURFACE_PLATFORM_TEXTURE,
};

struct RSSurfaceExtConfig {
    RSSurfaceExtType type = RSSurfaceExtType::NONE;
    void* additionalData = nullptr;
};
using RSSurfaceTextureConfig = RSSurfaceExtConfig;
using RSSurfaceTextureAttachCallBack = std::function<void(int64_t textureId, bool attach)>;
using RSSurfaceTextureUpdateCallBack = std::function<void(std::vector<float>&)>;
using RSSurfaceTextureInitTypeCallBack = std::function<void(int32_t&)>;
// codes for arkui-x end

struct RSDisplayNodeConfig {
    uint64_t screenId = 0;
    bool isMirrored = false;
    NodeId mirrorNodeId = 0;
    bool isSync = false;
    uint32_t mirrorSourceRotation = 4; // default INVALID_SCREEN_ROTATION
};

// ability state of surface node
enum class RSSurfaceNodeAbilityState : uint8_t {
    BACKGROUND,
    FOREGROUND,
};

struct SubSurfaceCntUpdateInfo {
    int updateCnt_ = 0;
    NodeId preParentId_ = INVALID_NODEID;
    NodeId curParentId_ = INVALID_NODEID;
};

constexpr int64_t NS_TO_S = 1000000000;
constexpr int64_t NS_PER_MS = 1000000;
constexpr uint32_t SIZE_UPPER_LIMIT = 1000;
constexpr uint32_t PARTICLE_EMMITER_UPPER_LIMIT = 2000;
constexpr uint32_t PARTICLE_UPPER_LIMIT = 1000000;

#if defined(M_PI)
constexpr float PI = M_PI;
#else
static const float PI = std::atanf(1.0) * 4;
#endif

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

inline constexpr int32_t ExtractTid(uint64_t token)
{
    // extract high 32 bits of token as tid
    return static_cast<int32_t>(token >> 32);
}

inline constexpr uint64_t MakeNodeId(pid_t pid, uint32_t uid)
{
    // combine pid and uid to nodeId
    return (static_cast<uint64_t>(pid) << 32) | uid;
}

inline uint64_t GenerateUniqueNodeIdForRS()
{
    static std::atomic<uint32_t> uid { 0 };
    return MakeNodeId(getpid(), uid.fetch_add(1, std::memory_order_relaxed));
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

enum class RSInterfaceErrorCode : uint32_t {
#undef NO_ERROR
    NO_ERROR = 0,
    NONSYSTEM_CALLING,
    NOT_SELF_CALLING,
    WRITE_PARCEL_ERROR,
    UNKNOWN_ERROR,
    NULLPTR_ERROR,
};

struct VSyncConnParam {
    uint64_t id = 0;
    NodeId windowNodeId = 0;
    bool fromXcomponent = false;
};

enum DrawNodeType : uint32_t {
    PureContainerType = 0,
    MergeableType,
    DrawPropertyType,
    GeometryPropertyType
};

enum class ComponentEnableSwitch : uint8_t {
    TEXTBLOB = 0,
    SVG,
    HMSYMBOL,
    CANVAS,
    MAX_VALUE,
};
typedef enum : uint32_t {
    SA_WATER_MARK_DEFAULT_SIZE = 0, // 512KB
    SA_WATER_MARK_MIDDLE_SIZE = 1, // 6M
    SA_WATER_MARK_BOTTOM = 2,
} SaSurfaceWatermarkMaxSize;

typedef enum : uint32_t {
    WATER_MARK_SUCCESS = 0,
    WATER_MARK_NAME_ERROR = (1U << 1),
    WATER_MARK_RS_CONNECTION_ERROR = (1U << 2),
    WATER_MARK_IMG_ASTC_ERROR = (1U << 3),
    WATER_MARK_NOT_SUPPORT_ERROR = (1U << 4),
    WATER_MARK_RENDER_SERVICE_NULL = (1U << 5),
    WATER_MARK_WRITE_PARCEL_ERR = (1U << 6),
    WATER_MARK_IPC_ERROR = (1U << 7),
    WATER_MARK_READ_PARCEL_ERR = (1U << 8),
    WATER_MARK_RS_NOT_FIND_NODE = (1U << 9),
    WATER_MARK_PERMISSION_ERROR = (1U << 10),
    WATER_MARK_IMG_SIZE_ERROR = (1U << 11),
    WATER_MARK_NODE_NOT_SCREEN = (1U << 12),
    WATER_MARK_PIXELMAP_INVALID = (1U << 13),
    WATER_MARK_NOT_SURFACE_NODE_ERROR = (1U << 14),
    WATER_MARK_INVALID_WATERMARK_TYPE = (1U << 15),
} SurfaceWatermarkStatusCode;

typedef enum : uint8_t {
    CUSTOM_WATER_MARK = 0,
    SYSTEM_WATER_MARK = 1,
    INVALID_WATER_MARK = 2,
} SurfaceWatermarkType;
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H
