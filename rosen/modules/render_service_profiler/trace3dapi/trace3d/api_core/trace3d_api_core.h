/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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


#ifndef __TRACE3D_API_CORE_H__
#define __TRACE3D_API_CORE_H__

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>

#if _MSC_VER
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
#elif defined(__GNUC__)
#endif

#define TRACE3D_API_VERSION(maj, min, patch) \
    (uint32_t)((((maj)&255)<<16)|(((min)&255)<<8)|(((patch)&255)<<0))

#define TRACE3D_API_VERSION_MAJOR(__ver) (((__ver)>>16)&255)
#define TRACE3D_API_VERSION_MINOR(__ver) (((__ver)>>8)&255)
#define TRACE3D_API_VERSION_PATCH(__ver) ((__ver)&255)

#pragma pack(push, 1)
struct TRACE3D_API_TABLE_HEADER {
    union {
        uint64_t magic{0};
        uint8_t magicBytes[8];
    };
    uint32_t version{0};
    uint32_t sizeBytes{0};
};
#pragma pack(pop)
static_assert(sizeof(TRACE3D_API_TABLE_HEADER)==(16), "Incorrect Trace3D proto struct size");

#define GPU_ZONE_VK_LOG_PREFIX "GPU_ZONE_VK: "
#define GPU_ZONE_GL_LOG_PREFIX "GPU_ZONE_GL: "

namespace trace3d::api {

constexpr uint8_t CORE_VER_MAJOR = 0;
constexpr uint8_t CORE_VER_MINOR = 1;
constexpr uint8_t CORE_VER_PATCH = 9;

#define TRACE3D_CORE_API_VERSION \
    TRACE3D_API_VERSION(trace3d::api::CORE_VER_MAJOR, trace3d::api::CORE_VER_MINOR, trace3d::api::CORE_VER_PATCH)

constexpr uint64_t MAGIC = uint64_t(
    ((uint64_t)'#' << 56) | ((uint64_t)'T' << 48) | ((uint64_t)'R' << 40) |
    ((uint64_t)'A' << 32) | ((uint64_t)'C' << 24) | ((uint64_t)'E' << 16) |
    ((uint64_t)'3' << 8) | ((uint64_t)'D' << 0));

enum class ParamValueType : uint8_t {
    UNKNOWN = 0,
    UINT64,
    UINT32,
    UINT16,
    UINT8,
    INT64,
    INT32,
    INT16,
    INT8,
    DOUBLE,
    FLOAT,
    BOOL,
};

[[maybe_unused]] static ParamValueType ValidateParamValueType(uint8_t val) {
    if (val == (uint8_t)ParamValueType::UINT64) { return ParamValueType::UINT64; }
    else if (val == (uint8_t)ParamValueType::UINT32) { return ParamValueType::UINT32; }
    else if (val == (uint8_t)ParamValueType::UINT16) { return ParamValueType::UINT16; }
    else if (val == (uint8_t)ParamValueType::UINT8) { return ParamValueType::UINT16; }
    else if (val == (uint8_t)ParamValueType::INT64) { return ParamValueType::INT64; }
    else if (val == (uint8_t)ParamValueType::INT32) { return ParamValueType::INT32; }
    else if (val == (uint8_t)ParamValueType::INT16) { return ParamValueType::INT16; }
    else if (val == (uint8_t)ParamValueType::INT8) { return ParamValueType::INT8; }
    else if (val == (uint8_t)ParamValueType::DOUBLE) { return ParamValueType::DOUBLE; }
    else if (val == (uint8_t)ParamValueType::FLOAT) { return ParamValueType::FLOAT; }
    else if (val == (uint8_t)ParamValueType::BOOL) { return ParamValueType::BOOL; }
    return ParamValueType::UNKNOWN;
};

union ParamValue {
    uint64_t uint64{0};
    uint32_t uint32;
    uint16_t uint16;
    uint8_t uint8;
    int64_t int64;
    int32_t int32;
    int16_t int16;
    int8_t int8;
    double dbl;
    float flt;
    bool bl;
};

enum RetCode : int32_t {
    RET_SUCCESS_DEBUG_GROUP_ALREADY_EMPTY = 1,
    RET_SUCCESS = 0,
    RET_ERR_UNKNOWN = -1,
    RET_ERR_INVALID_PARAM = -2,
    RET_ERR_UNSUPPORTED_COMMAND = -3,
    RET_ERR_WRITE_FILE = -4,
    RET_ERR_INVALID_DISPATCH_TABLE = -5,
    RET_ERR_GLES_LAYER_INIT = -6,
    RET_ERR_GLES_CAPTURER_INIT = -7,
    RET_ERR_LOG_DISABLED = -8,
    RET_ERR_INVALID_DEBUG_POOL = -9,
    RET_ERR_INVALID_DATA = -10,
    RET_ERR_INVALID_DATA_FORMAT = -11,
    RET_ERR_INVALID_DATA_TYPE_SIZE = -12,
    RET_ERR_DEBUG_POOL_ALLOC = -13,
    RET_ERR_PULL_DATA_FAILED_TO_MAP_MEMORY = -14,
    RET_ERR_PULL_DATA_FAILED_TO_CREATE_FILE = -15,
    RET_ERR_EMPTY_DEBUG_POOL = -16,
    RET_ERR_INVALID_GLES_CONTEXT = -17,
    RET_ERR_INVALID_VK_DEVICE = -18,
    RET_ERR_INVALID_GLES_CONTEXT_PARAM = -19,
    RET_ERR_INVALID_VK_DEVICE_PARAM = -20,
    RET_ERR_INVALID_CALL = -21,
    RET_ERR_NOT_READY = -22,
    RET_ERR_INCORRECT_USAGE = -23,
    RET_ERR_GPU_COUNTERS_DISABLED = -24,
    RET_ERR_DEBUG_POOL_BIND = -25,

    // GPU Request VK
    RET_ERR_CORE_GPU_REQUEST_VK_INIT = -100,
    RET_ERR_CORE_GPU_REQUEST_VK_INIT_STAGING = -101,
    RET_ERR_CORE_GPU_REQUEST_VK_IMAGE_DIRECT = -102,
    RET_ERR_CORE_GPU_REQUEST_VK_BUFFER_DIRECT = -103,
    RET_ERR_CORE_GPU_REQUEST_VK_IMAGE_TRANSFER = -104,
    RET_ERR_CORE_GPU_REQUEST_VK_BUFFER_TRANSFER = -105,

    // GPU Request GL
    RET_ERR_CORE_GPU_REQUEST_GL_INIT = -200,
    RET_ERR_CORE_GPU_REQUEST_GL_INIT_STAGING = -201,
    RET_ERR_CORE_GPU_REQUEST_GL_IMAGE = -202,
    RET_ERR_CORE_GPU_REQUEST_GL_BUFFER = -203,
};

enum class GPUCounterAPIType : uint8_t {
    Vk = 0,
    GL,
    COUNT
};

// only for vulkan counters available
// simple copy of vulkan enum
enum GPUPipelineStageBits : uint64_t {
    GPU_VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT                     = 0x00000001,
    GPU_VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT                   = 0x00000002,
    GPU_VK_PIPELINE_STAGE_VERTEX_INPUT_BIT                    = 0x00000004,
    GPU_VK_PIPELINE_STAGE_VERTEX_SHADER_BIT                   = 0x00000008,
    GPU_VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT     = 0x00000010,
    GPU_VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT  = 0x00000020,
    GPU_VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT                 = 0x00000040,
    GPU_VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT                 = 0x00000080,
    GPU_VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT            = 0x00000100,
    GPU_VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT             = 0x00000200,
    GPU_VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT         = 0x00000400,
    GPU_VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT                  = 0x00000800,
    GPU_VK_PIPELINE_STAGE_TRANSFER_BIT                        = 0x00001000,
    GPU_VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT                  = 0x00002000,
    GPU_VK_PIPELINE_STAGE_MAX
};

using GPUPipelineStageFlags = uint64_t;

} // namespace trace3d::api

// Trace3D Core API definition

// O.1.0
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceLogMsg)(const char *format, ...);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceLogMsgParam)(const char *message,
    trace3d::api::ParamValueType type, trace3d::api::ParamValue value);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceGroupPush)(const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceGroupPop)();
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceGlobalGroupPush)(const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceGlobalGroupPop)();

constexpr uint32_t TRACE3D_DEBUG_TAG_POOL_DEFAULT = 0;
constexpr uint32_t TRACE3D_DEBUG_TAG_POOL_GPU_ZONE = 1;
constexpr uint32_t TRACE3D_DEBUG_TAG_POOL_MAX_NUMBER = 8;

constexpr char TRACE3D_DEBUG_TAG_SEPARATOR[] = "|";

typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolLogMsg)(uint32_t tagPool, const char *format, ...);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolLogMsgParam)(uint32_t tagPool, const char *message,
    trace3d::api::ParamValueType type, trace3d::api::ParamValue value);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolGroupPush)(uint32_t tagPool, const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolGroupPop)(uint32_t tagPool);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolGlobalGroupPush)(
    uint32_t tagPool, const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolGlobalGroupPop)(uint32_t tagPool);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolAlloc)(uint32_t *outTagPool);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolFree)(uint32_t tagPool);

typedef const char* (* pTRACE3D_CoreDebugTraceGroupGet)();
typedef const char* (* pTRACE3D_CoreDebugTraceGlobalGroupGet)();
typedef const char* (* pTRACE3D_CoreDebugTracePoolGroupGet)(uint32_t tagPool);
typedef const char* (* pTRACE3D_CoreDebugTracePoolGlobalGroupGet)(uint32_t tagPool);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceGroupSet)(const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceGlobalGroupSet)(const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolGroupSet)(uint32_t tagPool, const char *groupTagName);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolGlobalGroupSet)(uint32_t tagPool, const char *groupTagName);

// O.1.1
enum Trace3DCoreDebugTraceLogLevel : uint8_t {
    TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_QUIET = 0,
    TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_ERR = 1,
    TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_WARN = 2,
    TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_INFO = 3,
    TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_DEBUG = 4,
};

enum Trace3DCoreDebugTraceMessageFlag : uint64_t {
    TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE = (1<<0),
};

typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceLogLevelMsg)(Trace3DCoreDebugTraceLogLevel logLevel,
    uint64_t flags, const char *format, ...);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTraceLogLevelMsgParam)(Trace3DCoreDebugTraceLogLevel logLevel,
    uint64_t flags, const char *message, trace3d::api::ParamValueType type, trace3d::api::ParamValue value);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolLogLevelMsg)(
    uint32_t tagPool, Trace3DCoreDebugTraceLogLevel logLevel, uint64_t flags,
    const char *format, ...);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolLogLevelMsgParam)(
    uint32_t tagPool, Trace3DCoreDebugTraceLogLevel logLevel, uint64_t flags,
    const char *message, trace3d::api::ParamValueType type,
    trace3d::api::ParamValue value);

// O.1.2 (GPU Time Trace API)
typedef trace3d::api::RetCode (*pTRACE3D_CoreGPUTimeTraceGroupPush)(
    void *cbHandle, const char *name, const uint32_t color,
    const trace3d::api::GPUCounterAPIType type, const char *format, ...);
typedef trace3d::api::RetCode (*pTRACE3D_CoreGPUTimeTraceGroupAddTimeStamp)(
    const char *name, trace3d::api::GPUPipelineStageFlags psf,
    const trace3d::api::GPUCounterAPIType type, const char *format, ...);
typedef trace3d::api::RetCode (* pTRACE3D_CoreGPUTimeTraceGroupPop)(
    const trace3d::api::GPUCounterAPIType type, const char *format, ...);

// O.1.3
enum Trace3DCoreDeviceContextParamType : uint8_t {
    TRACE3D_CORE_DEVICE_CONTEXT_PARAM_UNKNOWN = 0,
    TRACE3D_CORE_DEVICE_CONTEXT_PARAM_STAT_CMD_SUBMIT_DISABLED = 1,
};

#pragma pack(push, 1)
struct Trace3DCoreDeviceContextParamValue {
    Trace3DCoreDeviceContextParamType type{TRACE3D_CORE_DEVICE_CONTEXT_PARAM_UNKNOWN};
    trace3d::api::ParamValueType valueType{};
    trace3d::api::ParamValue value[4];
};
#pragma pack(pop)
static_assert(sizeof(Trace3DCoreDeviceContextParamValue)==34, "Incorrect Trace3D proto struct size");

typedef trace3d::api::RetCode (*pTRACE3D_CoreGetContextParamGLES)(
    void *context, Trace3DCoreDeviceContextParamType type,
    Trace3DCoreDeviceContextParamValue *paramValue);
typedef trace3d::api::RetCode (* pTRACE3D_CoreUpdateContextParamGLES)(void *context,
    const Trace3DCoreDeviceContextParamValue *paramValue, Trace3DCoreDeviceContextParamValue *coreParamValuePrev);
typedef trace3d::api::RetCode (* pTRACE3D_CoreGetDeviceParamVk)(void *device, Trace3DCoreDeviceContextParamType type,
    Trace3DCoreDeviceContextParamValue *paramValue);
typedef trace3d::api::RetCode (* pTRACE3D_CoreUpdateDeviceParamVk)(void *device,
    const Trace3DCoreDeviceContextParamValue *paramValue, Trace3DCoreDeviceContextParamValue *coreParamValuePrev);

// 0.1.4
typedef trace3d::api::RetCode (* pTRACE3D_CoreGPUTimeTraceGroupPopWithName)(const char *name,
    const trace3d::api::GPUCounterAPIType type, const char *format, ...);
typedef trace3d::api::RetCode (* pTRACE3D_CoreGPUTimeTraceGroupSwitchThreads)(void *cbHandle,
    const trace3d::api::GPUCounterAPIType type, uint16_t fromThreadId, uint16_t toThreadId);

enum Trace3DCoreGPUTimeTraceMode : uint8_t {
    TRACE3D_CORE_GPU_TIME_TRACE_MODE_AUTO = 0,
    TRACE3D_CORE_GPU_TIME_TRACE_MODE_MANUAL = 1,
};
typedef void (* pTRACE3D_CoreGPUTimeTraceSetMode)(Trace3DCoreGPUTimeTraceMode mode);
typedef Trace3DCoreGPUTimeTraceMode (* pTRACE3D_CoreGPUTimeTraceGetMode)();

// 0.1.5
typedef const char* (* pTRACE3D_CoreDebugTraceAllGroupsGet)();
typedef const char* (* pTRACE3D_CoreDebugTraceGlobalAllGroupsGet)();
typedef const char* (* pTRACE3D_CoreDebugTracePoolAllGroupsGet)(uint32_t tagPool);
typedef const char* (* pTRACE3D_CoreDebugTracePoolGlobalAllGroupsGet)(uint32_t tagPool);
typedef trace3d::api::RetCode (* pTRACE3D_CoreDebugTracePoolBind)(uint32_t tagPool);

// 0.1.6
typedef trace3d::api::RetCode (* pTRACE3D_CoreGPUTimeTraceGroupPopWithNameCb)(void *cbHandle, const char *name,
    const trace3d::api::GPUCounterAPIType type, const char *format, ...);
typedef trace3d::api::RetCode (* pTRACE3D_CoreGPUTimeTraceGroupAddTimeStampCb)(void *cbHandle, const char *name,
    trace3d::api::GPUPipelineStageFlags psf, const trace3d::api::GPUCounterAPIType type, const char *format, ...);

// 0.1.7
enum Trace3DCoreDebugTagParamType : uint8_t {
    TRACE3D_CORE_DEBUG_TAG_PARAM_UNKNOWN = 0,
    TRACE3D_CORE_DEBUG_TAG_PARAM_REPLAY_THREAD = 1,
    TRACE3D_CORE_DEBUG_TAG_PARAM_WM_NODE = 2,
    TRACE3D_CORE_DEBUG_TAG_PARAM_WM_FRAME = 3,

    TRACE3D_CORE_DEBUG_TAG_PARAM_COUNT
};

#pragma pack(push, 1)
struct Trace3DCoreDebugTagParamValue {
    Trace3DCoreDebugTagParamValue() {
        unknown.value[0] = 0, unknown.value[1] = 0, unknown.value[2] = 0, unknown.value[3] = 0;
    }
    Trace3DCoreDebugTagParamType type{TRACE3D_CORE_DEBUG_TAG_PARAM_UNKNOWN};
    union {
        struct {
            uint64_t value[4] = {};
        } unknown;
        struct {
            uint16_t id;
        } replayThread;
        struct {
            uint64_t id;
        } wmNode;
        struct {
            uint64_t number;
        } wmFrame;
    };
};
#pragma pack(pop)
static_assert(sizeof(Trace3DCoreDebugTagParamValue)==33, "Incorrect Trace3D proto struct size");

typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTraceGroupPushWithParam)(
    const char *groupTagName, const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (
    *pTRACE3D_CoreDebugTraceGlobalGroupPushWithParam)(const char *groupTagName,
        const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolGroupPushWithParam)(uint32_t tagPool,
    const char *groupTagName, const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolGlobalGroupPushWithParam)(uint32_t tagPool,
    const char *groupTagName, const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTraceGroupSetWithParam)(const char *groupTagName,
    const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTraceGlobalGroupSetWithParam)(
    const char *groupTagName, const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolGroupSetWithParam)(uint32_t tagPool,
    const char *groupTagName, const Trace3DCoreDebugTagParamValue *param);
typedef trace3d::api::RetCode (*pTRACE3D_CoreDebugTracePoolGlobalGroupSetWithParam)(uint32_t tagPool,
    const char *groupTagName, const Trace3DCoreDebugTagParamValue *param);
typedef const char *(*pTRACE3D_CoreDebugTraceGroupGetWithParam)(Trace3DCoreDebugTagParamType paramType,
    Trace3DCoreDebugTagParamValue *outParam);
typedef const char *(*pTRACE3D_CoreDebugTraceGlobalGroupGetWithParam)(Trace3DCoreDebugTagParamType paramType,
    Trace3DCoreDebugTagParamValue *outParam);
typedef const char *(*pTRACE3D_CoreDebugTracePoolGroupGetWithParam)(uint32_t tagPool,
    Trace3DCoreDebugTagParamType paramType, Trace3DCoreDebugTagParamValue *outParam);
typedef const char *(*pTRACE3D_CoreDebugTracePoolGlobalGroupGetWithParam)(uint32_t tagPool,
    Trace3DCoreDebugTagParamType paramType, Trace3DCoreDebugTagParamValue *outParam);

// 0.1.8
#pragma pack(push, 1)
struct Trace3DOffset2D {
    int32_t x{ 0 };
    int32_t y{ 0 };
};
#pragma pack(pop)
static_assert(sizeof(Trace3DOffset2D)==8, "Incorrect Trace3D proto struct size");

#pragma pack(push, 1)
struct Trace3DExtent2D {
    uint32_t width{ 0 };
    uint32_t height{ 0 };
};
#pragma pack(pop)
static_assert(sizeof(Trace3DExtent2D)==8, "Incorrect Trace3D proto struct size");

#pragma pack(push, 1)
struct Trace3DRect2D {
    Trace3DOffset2D offset;
    Trace3DExtent2D extent;
};
#pragma pack(pop)
static_assert(sizeof(Trace3DRect2D)==16, "Incorrect Trace3D proto struct size");

#pragma pack(push, 1)
struct Trace3DGPUImageLevelInfo {
    uint32_t format{ 0 }; // VkFormat / GLenum internalFormat
    Trace3DExtent2D extent;
    uint64_t imageMemoryOffset{ 0 };
    uint32_t imageMemorySizeBytes{ 0 };
    uint64_t nativeBuffer{ 0 };
    uint8_t reserved[32] = {};
};
#pragma pack(pop)
static_assert(sizeof(Trace3DGPUImageLevelInfo)==64, "Incorrect Trace3D proto struct size");

typedef trace3d::api::RetCode (*pTRACE3D_CoreGPURequestVkImage)(
    void *device, void *image, const Trace3DRect2D *pImageRect,
    uint32_t *pLevelCountOut, uint32_t level,
    Trace3DGPUImageLevelInfo *pLevelInfoOut, size_t maxDataBytes,
    size_t *pDataBytesOut, void *pDataOut);

typedef trace3d::api::RetCode (*pTRACE3D_CoreGPURequestGLTexture)(
    void *context, uint32_t texture, const Trace3DRect2D *pTexRect,
    uint32_t *pLevelCountOut, uint32_t level,
    Trace3DGPUImageLevelInfo *pLevelInfoOut, size_t maxDataBytes,
    size_t *pDataBytesOut, void *pDataOut);

#pragma pack(push, 1)
struct Trace3DDataViewInfo {
    uint64_t offset{ 0 };
    uint32_t sizeBytes{ 0 };
    uint32_t reserved0{ 0 };
};
#pragma pack(pop)
static_assert(sizeof(Trace3DDataViewInfo)==16, "Incorrect Trace3D proto struct size");

#pragma pack(push, 1)
struct Trace3DGPUBufferInfo {
    uint64_t memoryOffset{ 0 };
    uint32_t memorySizeBytes{ 0 };
    uint64_t nativeBuffer{ 0 };
    uint32_t reserved0{ 0 };
    uint64_t reserved1{ 0 };
};
#pragma pack(pop)
static_assert(sizeof(Trace3DGPUBufferInfo)==32, "Incorrect Trace3D proto struct size");

typedef trace3d::api::RetCode (*pTRACE3D_CoreGPURequestVkBuffer)(
    void *device, void *buffer, const Trace3DDataViewInfo *pBufferView,
    Trace3DGPUBufferInfo *pInfoOut, size_t maxDataBytes, size_t *pDataBytesOut,
    void *pDataOut);

typedef trace3d::api::RetCode (*pTRACE3D_CoreGPURequestGLBuffer)(
    void *context, uint32_t buffer, const Trace3DDataViewInfo *pBufferView,
    Trace3DGPUBufferInfo *pInfoOut, size_t maxDataBytes, size_t *pDataBytesOut,
    void *pDataOut);

// 0.1.9
enum Trace3DGPUResourceType : uint8_t {
    TRACE3D_GPU_RESOURCE_UNKNOWN = 0,
    TRACE3D_GPU_RESOURCE_VK_IMAGE = 1,
    TRACE3D_GPU_RESOURCE_VK_BUFFER = 2,
};

#pragma pack(push, 1)
union TRace3DGPUResourceAccess {
    struct {
        bool read : 1;
        bool write : 1;
    } bits;
    uint8_t allBits{ 0 };
};
#pragma pack(pop)
static_assert(sizeof(TRace3DGPUResourceAccess)==1, "Incorrect Trace3D proto struct size");

#pragma pack(push, 1)
struct Trace3DGPUResourceInfo {
    Trace3DGPUResourceType type{ TRACE3D_GPU_RESOURCE_UNKNOWN };
    union {
        void *handle;
        uint64_t handleUInt64{ 0 };
    };
    union {
        void *memory;
        uint64_t memoryUInt64{ 0 };
    };
    uint64_t memoryOffset{ 0 };
    uint32_t memorySizeBytes{ 0 };
    uint64_t nativeBuffer{ 0 };
    TRace3DGPUResourceAccess access;

    uint8_t reserved[26] = {};
};
#pragma pack(pop)
static_assert(sizeof(Trace3DGPUResourceInfo)==64, "Incorrect Trace3D proto struct size");

enum Trace3DGPUResourcePopulateFlagBits : uint32_t {
    TRACE3D_GPU_RESOURCE_POPULATE_FLAG_READ_ACCESS = (1<<0),
    TRACE3D_GPU_RESOURCE_POPULATE_FLAG_WRITE_ACCESS = (1<<1),
    TRACE3D_GPU_RESOURCE_POPULATE_FLAG_NATIVE_BUFFER_ONLY = (1<<2),
};

using Trace3DGPUResourcePopulateFlags = uint32_t;

typedef trace3d::api::RetCode (* pTRACE3D_CoreGPURequestVkCommandBufferResources)(void *commandBuffer,
    Trace3DGPUResourcePopulateFlags populateFlags, void (* populateResources)(
        size_t count, const Trace3DGPUResourceInfo *pResorces, void *pStorageOut), void *pStorageOut);

typedef trace3d::api::RetCode (* pTRACE3D_CoreResetStatisticTime)();

enum Trace3DCoreParamType : uint8_t {
    TRACE3D_CORE_PARAM_UNKNOWN = 0,

    // Common params
    TRACE3D_CORE_PARAM_STATISTIC_ORIGIN_TIME_MICROSECONDS = 1, // UINT64
    TRACE3D_CORE_PARAM_STATISTIC_TIME_MICROSECONDS = 2, // UINT64
    TRACE3D_CORE_PARAM_TIME_MICROSECONDS = 3, // UINT64

    // Mode
    TRACE3D_CORE_PARAM_MODE = 10, // UINT32

    // Flags
    TRACE3D_CORE_PARAM_STAT_FLAGS = 21, // UINT64
    TRACE3D_CORE_PARAM_LOG_FLAGS = 22, // UINT64
    TRACE3D_CORE_PARAM_METRIC_FLAGS = 23, // UINT64
};

#pragma pack(push, 1)
struct Trace3DCoreParamValue {
    Trace3DCoreParamType type{TRACE3D_CORE_PARAM_UNKNOWN};
    trace3d::api::ParamValueType valueType{};
    trace3d::api::ParamValue value[4];
};
#pragma pack(pop)
static_assert(sizeof(Trace3DCoreParamValue)==34, "Incorrect Trace3D proto struct size");

typedef trace3d::api::RetCode (* pTRACE3D_CoreGetParam)(Trace3DCoreParamType type, Trace3DCoreParamValue *paramValue);
typedef trace3d::api::RetCode (* pTRACE3D_CoreUpdateParam)(const Trace3DCoreParamValue *paramValue);

// Core API Table

#if !defined(TRACE3D_CORE_API_NO_NAMESPACE) && !defined(TRACE3D_CORE_API_NAMESPACE)
#error "Must define TRACE3D_CORE_API_NAMESPACE"
#endif

#if !defined(TRACE3D_CORE_API_NO_NAMESPACE)
#define TRACE3D_CORE_API_TABLE TRACE3D_CORE_API_NAMESPACE::TRACE3D_CORE_API_TABLE_IMPL
#endif

#if !defined(TRACE3D_CORE_API_NO_NAMESPACE)
namespace TRACE3D_CORE_API_NAMESPACE {
#endif

#pragma pack(push, 1)
#if !defined(TRACE3D_CORE_API_NO_NAMESPACE)
struct TRACE3D_CORE_API_TABLE_IMPL {
#else
struct TRACE3D_CORE_API_TABLE {
#endif
    const TRACE3D_API_TABLE_HEADER header{
        { trace3d::api::MAGIC },
        TRACE3D_CORE_API_VERSION,
        sizeof(TRACE3D_CORE_API_TABLE)
    };

    // O.1.0
    pTRACE3D_CoreDebugTraceLogMsg              DebugTraceLogMsg{nullptr};
    pTRACE3D_CoreDebugTraceLogMsgParam         DebugTraceLogMsgParam{nullptr};
    pTRACE3D_CoreDebugTraceGroupPush           DebugTraceGroupPush{nullptr};
    pTRACE3D_CoreDebugTraceGroupPop            DebugTraceGroupPop{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupPush     DebugTraceGlobalGroupPush{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupPop      DebugTraceGlobalGroupPop{nullptr};
    pTRACE3D_CoreDebugTracePoolLogMsg          DebugTracePoolLogMsg{nullptr};
    pTRACE3D_CoreDebugTracePoolLogMsgParam     DebugTracePoolLogMsgParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupPush       DebugTracePoolGroupPush{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupPop        DebugTracePoolGroupPop{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupPush DebugTracePoolGlobalGroupPush{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupPop  DebugTracePoolGlobalGroupPop{nullptr};
    pTRACE3D_CoreDebugTracePoolAlloc           DebugTracePoolAlloc{nullptr};
    pTRACE3D_CoreDebugTracePoolFree            DebugTracePoolFree{nullptr};

    pTRACE3D_CoreDebugTraceGroupGet            DebugTraceGroupGet{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupGet      DebugTraceGlobalGroupGet{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupGet        DebugTracePoolGroupGet{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupGet  DebugTracePoolGlobalGroupGet{nullptr};
    pTRACE3D_CoreDebugTraceGroupSet            DebugTraceGroupSet{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupSet      DebugTraceGlobalGroupSet{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupSet        DebugTracePoolGroupSet{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupSet  DebugTracePoolGlobalGroupSet{nullptr};

    // O.1.1
    pTRACE3D_CoreDebugTraceLogLevelMsg           DebugTraceLogLevelMsg{nullptr};
    pTRACE3D_CoreDebugTraceLogLevelMsgParam      DebugTraceLogLevelMsgParam{nullptr};
    pTRACE3D_CoreDebugTracePoolLogLevelMsg       DebugTracePoolLogLevelMsg{nullptr};
    pTRACE3D_CoreDebugTracePoolLogLevelMsgParam  DebugTracePoolLogLevelMsgParam{nullptr};

    // O.1.2
    pTRACE3D_CoreGPUTimeTraceGroupPush           GPUTimeTraceGroupPush{nullptr};
    pTRACE3D_CoreGPUTimeTraceGroupAddTimeStamp   GPUTimeTraceGroupAddTimeStamp{nullptr};
    pTRACE3D_CoreGPUTimeTraceGroupPop            GPUTimeTraceGroupPop{nullptr};

    // O.1.3
    pTRACE3D_CoreGetContextParamGLES             GetContextParamGLES{nullptr};
    pTRACE3D_CoreUpdateContextParamGLES          UpdateContextParamGLES{nullptr};
    pTRACE3D_CoreGetDeviceParamVk                GetDeviceParamVk{nullptr};
    pTRACE3D_CoreUpdateDeviceParamVk             UpdateDeviceParamVk{nullptr};

    // 0.1.4
    pTRACE3D_CoreGPUTimeTraceGroupPopWithName        GPUTimeTraceGroupPopWithName{nullptr};
    pTRACE3D_CoreGPUTimeTraceGroupSwitchThreads      GPUTimeTraceGroupSwitchThreads{nullptr};
    pTRACE3D_CoreGPUTimeTraceSetMode                 GPUTimeTraceSetMode{nullptr};
    pTRACE3D_CoreGPUTimeTraceGetMode                 GPUTimeTraceGetMode{nullptr};

    // 0.1.5
    pTRACE3D_CoreDebugTraceAllGroupsGet              DebugTraceAllGroupsGet{nullptr};
    pTRACE3D_CoreDebugTraceGlobalAllGroupsGet        DebugTraceGlobalAllGroupsGet{nullptr};
    pTRACE3D_CoreDebugTracePoolAllGroupsGet          DebugTracePoolAllGroupsGet{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalAllGroupsGet    DebugTracePoolGlobalAllGroupsGet{nullptr};
    pTRACE3D_CoreDebugTracePoolBind                  DebugTracePoolBind{nullptr};

    // 0.1.6
    pTRACE3D_CoreGPUTimeTraceGroupPopWithNameCb      GPUTimeTraceGroupPopWithNameCb{nullptr};
    pTRACE3D_CoreGPUTimeTraceGroupAddTimeStampCb     GPUTimeTraceGroupAddTimeStampCb{nullptr};

    // 0.1.7
    pTRACE3D_CoreDebugTraceGroupPushWithParam               DebugTraceGroupPushWithParam{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupPushWithParam         DebugTraceGlobalGroupPushWithParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupPushWithParam           DebugTracePoolGroupPushWithParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupPushWithParam     DebugTracePoolGlobalGroupPushWithParam{nullptr};
    pTRACE3D_CoreDebugTraceGroupSetWithParam                DebugTraceGroupSetWithParam{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupSetWithParam          DebugTraceGlobalGroupSetWithParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupSetWithParam            DebugTracePoolGroupSetWithParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupSetWithParam      DebugTracePoolGlobalGroupSetWithParam{nullptr};
    pTRACE3D_CoreDebugTraceGroupGetWithParam                DebugTraceGroupGetWithParam{nullptr};
    pTRACE3D_CoreDebugTraceGlobalGroupGetWithParam          DebugTraceGlobalGroupGetWithParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGroupGetWithParam            DebugTracePoolGroupGetWithParam{nullptr};
    pTRACE3D_CoreDebugTracePoolGlobalGroupGetWithParam      DebugTracePoolGlobalGroupGetWithParam{nullptr};

    // 0.1.8
    pTRACE3D_CoreGPURequestVkImage                          GPURequestVkImage{nullptr};
    pTRACE3D_CoreGPURequestGLTexture                        GPURequestGLTexture{nullptr};
    pTRACE3D_CoreGPURequestVkBuffer                         GPURequestVkBuffer{nullptr};
    pTRACE3D_CoreGPURequestGLBuffer                         GPURequestGLBuffer{nullptr};

    // 0.1.9
    pTRACE3D_CoreGPURequestVkCommandBufferResources         GPURequestVkCommandBufferResources{nullptr};
    pTRACE3D_CoreResetStatisticTime                         ResetStatisticTime{nullptr};
    pTRACE3D_CoreGetParam                                   GetParam{nullptr};
    pTRACE3D_CoreUpdateParam                                UpdateParam{nullptr};
};
#pragma pack(pop)

#if !defined(TRACE3D_CORE_API_NO_NAMESPACE)
} // TRACE3D_CORE_API_NAMESPACE
#endif

typedef uint32_t (* pTRACE3D_CoreGetAPI)(struct TRACE3D_CORE_API_TABLE *apiTable);

#define TRACE3D_CORE_GET_API_ENTRY_NAME "TRACE3D_CoreGetAPI"

// Base for converting string to uint64_t
constexpr uint32_t STR_TO_UINT_BASE = 10;

static inline const TRACE3D_CORE_API_TABLE* TRACE3D_CoreInitImpl()
{
    static bool initDone = false;
    static TRACE3D_CORE_API_TABLE *apiTablePtr = nullptr;
    if (initDone) {
        return apiTablePtr;
    }
    uint64_t procAddrUInt = 0;
    const char *env = getenv(TRACE3D_CORE_GET_API_ENTRY_NAME);

    if (env) {
        char *end = nullptr;
        procAddrUInt = std::strtoull(env, &end, STR_TO_UINT_BASE);
    }
    pTRACE3D_CoreGetAPI coreGetAPI = (pTRACE3D_CoreGetAPI)(uintptr_t)procAddrUInt;

    if (coreGetAPI) {
        TRACE3D_CORE_API_TABLE apiTableTmp;
        const uint32_t apiTableSize = coreGetAPI(&apiTableTmp);

        if (apiTableSize == sizeof(TRACE3D_CORE_API_TABLE)) {
            static TRACE3D_CORE_API_TABLE apiTable{};

            memcpy_s((void *)&apiTable, apiTableSize, (const void *)&apiTableTmp, apiTableSize);
            apiTablePtr = &apiTable;
        }
    }
    initDone = true;
    return apiTablePtr;
}

namespace trace3d::api {

enum class CustomStringParamType : uint32_t {
    DEBUG_TAG = 0,
    CODE_CONTEXT,
    COUNT
};

static inline const char *CustomStringParamTypeNames[(size_t)CustomStringParamType::COUNT] = {
    "debugTag",
    "codeContext",
};

class DebugScope final {
public:
    DebugScope(const TRACE3D_CORE_API_TABLE *coreAPI, uint32_t tagPool = TRACE3D_DEBUG_TAG_POOL_DEFAULT)
        : coreAPI_(nullptr),
          tagPool_(TRACE3D_DEBUG_TAG_POOL_DEFAULT),
          pushed_(false) {
        if (tagPool < TRACE3D_DEBUG_TAG_POOL_MAX_NUMBER
                && coreAPI
                && coreAPI->DebugTracePoolGroupPushWithParam
                && coreAPI->DebugTracePoolGroupPop
                && coreAPI->DebugTracePoolLogLevelMsg
                && coreAPI->DebugTracePoolGroupGet
                && coreAPI->DebugTracePoolGroupGetWithParam
                && coreAPI->DebugTracePoolGroupSetWithParam
                && coreAPI->DebugTracePoolAllGroupsGet) {
            coreAPI_ = coreAPI;
            tagPool_ = tagPool;
        }
    }
    DebugScope(const TRACE3D_CORE_API_TABLE *coreAPI, const std::string_view tagName,
            uint32_t tagPool = TRACE3D_DEBUG_TAG_POOL_DEFAULT, const Trace3DCoreDebugTagParamValue tagParam = {})
        : DebugScope(coreAPI, tagPool) {
        if (coreAPI_) {
            pushed_ = coreAPI_->DebugTracePoolGroupPushWithParam(
                tagPool_, tagName.data(), &tagParam) >=
                trace3d::api::RET_SUCCESS;
        }
    }
    DebugScope(DebugScope &other) { this->operator = (other); }
    DebugScope(DebugScope &&other) { this->operator = (other); }
    ~DebugScope() {
        if (pushed_) {
            coreAPI_->DebugTracePoolGroupPop(tagPool_);
        }
    }

    DebugScope& operator = (DebugScope &other) {
        coreAPI_ = other.coreAPI_;
        tagPool_ = other.tagPool_;
        pushed_ = other.pushed_;
        other.coreAPI_ = nullptr;
        other.tagPool_ = TRACE3D_DEBUG_TAG_POOL_DEFAULT;
        other.pushed_ = false;
        return *this;
    }

    inline bool IsActive() const { return coreAPI_ != nullptr; }

    template<typename ...TArgs>
    void Log(Trace3DCoreDebugTraceLogLevel logLevel, const char *format, TArgs... args) const {
        if (coreAPI_) {
            coreAPI_->DebugTracePoolLogLevelMsg(tagPool_, logLevel, 0, format, args...);
        }
    }

    template<typename ...TArgs>
    void LogBt(Trace3DCoreDebugTraceLogLevel logLevel, const char *format, TArgs... args) const {
        if (coreAPI_) {
            coreAPI_->DebugTracePoolLogLevelMsg(
                tagPool_, logLevel,
                TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE, format,
                args...);
        }
    }

    const char* GetTag() const {
        return coreAPI_ ? coreAPI_->DebugTracePoolGroupGet(tagPool_) : "";
    }

    const char* GetTagWithParam(Trace3DCoreDebugTagParamType paramType,
        Trace3DCoreDebugTagParamValue &tagParam) const {
        return coreAPI_ ? coreAPI_->DebugTracePoolGroupGetWithParam(tagPool_, paramType, &tagParam) : "";
    }

    bool SetTagParam(const Trace3DCoreDebugTagParamValue &tagParam) const {
        if (coreAPI_) {
            return coreAPI_->DebugTracePoolGroupSetWithParam(tagPool_, nullptr, &tagParam) >= trace3d::api::RET_SUCCESS;
        }
        return false;
    }

    const char* GetCompositeTag() const {
        return coreAPI_ ? coreAPI_->DebugTracePoolAllGroupsGet(tagPool_) : "";
    }

private:
    const TRACE3D_CORE_API_TABLE *coreAPI_{nullptr};
    uint32_t tagPool_{ TRACE3D_DEBUG_TAG_POOL_DEFAULT };
    bool pushed_{false};
};

struct GPUZoneVkTypeInfo {
    static inline constexpr char LOG_PREFIX[] = GPU_ZONE_VK_LOG_PREFIX;
    static inline constexpr GPUCounterAPIType API_TYPE = GPUCounterAPIType::Vk;
};

struct GPUZoneGLTypeInfo {
    static inline constexpr char LOG_PREFIX[] = GPU_ZONE_GL_LOG_PREFIX;
    static inline constexpr GPUCounterAPIType API_TYPE = GPUCounterAPIType::GL;
};

template<typename TInfo>
class GPUZone final {
public:
    GPUZone() : coreAPI_(nullptr), cbHandle_(nullptr) {}
    GPUZone(const TRACE3D_CORE_API_TABLE *coreAPI, void *cbHandle, const std::string_view zoneName, bool tagged,
            const char *codeContext = nullptr, const uint32_t color = 0)
        : coreAPI_(nullptr), cbHandle_(nullptr) { Begin(coreAPI, cbHandle, zoneName, tagged, codeContext, color); }
    GPUZone(GPUZone &other) { (*this) = other; }
    GPUZone(GPUZone &&other) { (*this) = other; }
    ~GPUZone() { End(); }

    GPUZone& operator = (GPUZone &other) {
        coreAPI_ = other.coreAPI_;
        cbHandle_ = other.cbHandle_;
        other.coreAPI_ = nullptr;
        other.cbHandle_ = nullptr;
        return *this;
    }

    inline bool Begin(const TRACE3D_CORE_API_TABLE *coreAPI, void *cbHandle,
                      const std::string_view zoneName, bool tagged,
                      const char *codeContext = nullptr,
                      const uint32_t color = 0) {
        End(nullptr, codeContext);
        if (coreAPI
                && coreAPI->GPUTimeTraceGroupPush
                && coreAPI->GPUTimeTraceGroupAddTimeStampCb
                && coreAPI->GPUTimeTraceGroupPopWithNameCb
                && coreAPI->DebugTraceLogLevelMsg) {
            trace3d::api::RetCode retCode = trace3d::api::RET_ERR_UNKNOWN;
            if (tagged) {
                DebugScope dbgScope = DebugScope(coreAPI, TRACE3D_DEBUG_TAG_POOL_GPU_ZONE);
                retCode = coreAPI->GPUTimeTraceGroupPush(
                    cbHandle, zoneName.data(), color, TInfo::API_TYPE,
                    "\"%s\":\"%s\",\"%s\":\"%s\"",
                    CustomStringParamTypeNames[(
                        size_t)CustomStringParamType::DEBUG_TAG],
                    dbgScope.GetTag(),
                    CustomStringParamTypeNames[(
                        size_t)CustomStringParamType::CODE_CONTEXT],
                    (codeContext ? codeContext : ""));
            } else {
                if (codeContext) {
                    retCode = coreAPI->GPUTimeTraceGroupPush(
                        cbHandle, zoneName.data(), color, TInfo::API_TYPE,
                        "\"%s\":\"%s\"",
                        CustomStringParamTypeNames[(
                            size_t)CustomStringParamType::CODE_CONTEXT],
                        codeContext);
                } else {
                    retCode = coreAPI->GPUTimeTraceGroupPush(
                        cbHandle, zoneName.data(), color, TInfo::API_TYPE,
                        nullptr);
                }
            }
            if (retCode >= trace3d::api::RET_SUCCESS) {
                coreAPI_ = coreAPI;
                cbHandle_ = cbHandle;
            } else if (retCode == trace3d::api::RET_ERR_GPU_COUNTERS_DISABLED) {
                retCode = trace3d::api::RET_SUCCESS;
            } else if (retCode < trace3d::api::RET_SUCCESS) {
                coreAPI->DebugTraceLogLevelMsg(
                    TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_ERR,
                    TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE,
                    "%sERROR: Begin zoneName:'%s' (retCode:%d).",
                    TInfo::LOG_PREFIX, zoneName.data(), retCode);
            }
            return (retCode >= trace3d::api::RET_SUCCESS);
        }
        return true;
    }

    inline bool End(const char *zoneName = nullptr, const char *codeContext = nullptr) {
        if (coreAPI_) {
            auto retCode = trace3d::api::RET_ERR_UNKNOWN;
            if (codeContext) {
                retCode = coreAPI_->GPUTimeTraceGroupPopWithNameCb(cbHandle_, zoneName, TInfo::API_TYPE,
                    "\"%s\":\"%s\"", CustomStringParamTypeNames[(size_t)CustomStringParamType::CODE_CONTEXT],
                    codeContext);
            } else {
                retCode = coreAPI_->GPUTimeTraceGroupPopWithNameCb(
                    cbHandle_, zoneName, TInfo::API_TYPE, nullptr);
            }
            if (retCode == trace3d::api::RET_ERR_GPU_COUNTERS_DISABLED) {
                retCode = trace3d::api::RET_SUCCESS;
            } else if (retCode < trace3d::api::RET_SUCCESS) {
                coreAPI_->DebugTraceLogLevelMsg(TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_ERR,
                    TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE, "%sERROR: End (retCode:%d).",
                    TInfo::LOG_PREFIX, retCode);
            }
            coreAPI_ = nullptr;
            cbHandle_ = nullptr;
            return retCode >= trace3d::api::RET_SUCCESS;
        }
        return true;
    }

    inline bool Add(const std::string_view stampName, const char *codeContext = nullptr,
            GPUPipelineStageFlags psf = GPU_VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT) {
        if (coreAPI_) {
            auto retCode = trace3d::api::RET_ERR_UNKNOWN;
            if (codeContext) {
                retCode = coreAPI_->GPUTimeTraceGroupAddTimeStampCb(cbHandle_, stampName.data(), psf, TInfo::API_TYPE,
                    "\"%s\":\"%s\"", CustomStringParamTypeNames[(size_t)CustomStringParamType::CODE_CONTEXT],
                    codeContext);
            } else {
                retCode = coreAPI_->GPUTimeTraceGroupAddTimeStampCb(cbHandle_, stampName.data(), psf, TInfo::API_TYPE,
                    nullptr);
            }
            if (retCode == trace3d::api::RET_ERR_GPU_COUNTERS_DISABLED) {
                retCode = trace3d::api::RET_SUCCESS;
            } else if (retCode < trace3d::api::RET_SUCCESS) {
                coreAPI_->DebugTraceLogLevelMsg(TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_ERR,
                    TRACE3D_CORE_DEBUG_TRACE_MESSAGE_FLAG_BACKTRACE, "%sERROR: Add stampName:'%s' (retCode:%d).",
                    TInfo::LOG_PREFIX, stampName.data(), retCode);
            }
            return retCode >= trace3d::api::RET_SUCCESS;
        }
        return false;
    }

protected:
    const TRACE3D_CORE_API_TABLE *coreAPI_{nullptr};
    void *cbHandle_{nullptr};
};

typedef GPUZone<GPUZoneVkTypeInfo> GPUZoneVk;
typedef GPUZone<GPUZoneGLTypeInfo> GPUZoneGL;

} // namespace trace3d::api

#if defined(TRACE3D_CORE_API_INIT_DEFAULT) && !defined(TRACE3D_CORE_API_INIT)
    #define TRACE3D_CORE_API_INIT() TRACE3D_CoreInitImpl()
#endif

#define TRACE3D_DbgScope__(__tagPool)                                          \
  ::trace3d::api::DebugScope(TRACE3D_CORE_API_INIT(), (__tagPool))
#define TRACE3D_DbgScopeNamed__(__name, __tagPool)                             \
  ::trace3d::api::DebugScope(TRACE3D_CORE_API_INIT(), (__name), (__tagPool))
#define TRACE3D_DbgScopeNamedWithParam__(__name, __tagPool, __param)           \
  ::trace3d::api::DebugScope(TRACE3D_CORE_API_INIT(), (__name), (__tagPool),   \
                             (__param))
#define TRACE3D_GPUZoneVk__(__cb, __name, __tag, __col)                        \
  ::trace3d::api::GPUZoneVk(TRACE3D_CORE_API_INIT(), (__cb), (__name),         \
                            (__tag), TRACE3D_GPUZONE_CODE_CONTEXT, (__col))
#define TRACE3D_GPUZoneGL__(__name, __tag, __col)                              \
  ::trace3d::api::GPUZoneGL(TRACE3D_CORE_API_INIT(), nullptr, (__name),        \
                            (__tag), TRACE3D_GPUZONE_CODE_CONTEXT, (__col))

//----------------------------------------------------------------------------------------------------------------------
//                                       Trace3D Debug API (for end user)
//----------------------------------------------------------------------------------------------------------------------

// DebugScope constructors
#define TRACE3D_DbgScopeDefault()                                                                       \
  TRACE3D_DbgScope__(TRACE3D_DEBUG_TAG_POOL_DEFAULT)
#define TRACE3D_DbgScopeGPUZone()                                                                       \
  TRACE3D_DbgScope__(TRACE3D_DEBUG_TAG_POOL_GPU_ZONE)
#define TRACE3D_DbgScopeNamedDefault(__name)                                                            \
  TRACE3D_DbgScopeNamed__((__name), TRACE3D_DEBUG_TAG_POOL_DEFAULT)
#define TRACE3D_DbgScopeNamedDefaultWithParam(__name, __param)                                          \
  TRACE3D_DbgScopeNamedWithParam__((__name), TRACE3D_DEBUG_TAG_POOL_DEFAULT, (__param))
#define TRACE3D_DbgScopeDefaultWithParam(__param)                                                       \
  TRACE3D_DbgScopeNamedWithParam__("", TRACE3D_DEBUG_TAG_POOL_DEFAULT, (__param))
#define TRACE3D_DbgScopeNamedGPUZone(__name)                                                            \
  TRACE3D_DbgScopeNamed__((__name), TRACE3D_DEBUG_TAG_POOL_GPU_ZONE)
#define TRACE3D_DbgScopeNamedGPUZoneWithParam(__name, __param)                                          \
  TRACE3D_DbgScopeNamedWithParam__((__name), TRACE3D_DEBUG_TAG_POOL_GPU_ZONE, (__param))
#define TRACE3D_DbgScopeGPUZoneWithParam(__param)                                                       \
  TRACE3D_DbgScopeNamedWithParam__("", TRACE3D_DEBUG_TAG_POOL_GPU_ZONE, (__param))
// DebugScope methods
#define TRACE3D_DbgScope_Log(__scope, __ll, __fmt, ...) (__scope).Log((__ll), (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogErr(__scope, __fmt, ...)                                                    \
  TRACE3D_DbgScope_Log((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_ERR, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogWarn(__scope, __fmt, ...)                                                   \
  TRACE3D_DbgScope_Log((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_WARN, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogInfo(__scope, __fmt, ...)                                                   \
  TRACE3D_DbgScope_Log((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_INFO, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogDebug(__scope, __fmt, ...)                                                  \
  TRACE3D_DbgScope_Log((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_DEBUG, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogBt(__scope, __ll, __fmt, ...) (__scope).LogBt((__ll), (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogErrBt(__scope, __fmt, ...)                                                  \
  TRACE3D_DbgScope_LogBt((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_ERR, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogWarnBt(__scope, __fmt, ...)                                                 \
  TRACE3D_DbgScope_LogBt((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_WARN, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogInfoBt(__scope, __fmt, ...)                                                 \
  TRACE3D_DbgScope_LogBt((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_INFO, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_LogDebugBt(__scope, __fmt, ...)                                                \
  TRACE3D_DbgScope_LogBt((__scope), TRACE3D_CORE_DEBUG_TRACE_LOG_LEVEL_DEBUG, (__fmt), __VA_ARGS__)
#define TRACE3D_DbgScope_GetTag(__scope) (__scope).GetTag()
#define TRACE3D_DbgScope_GetTagWithParam(__scope, __paramType, __outParam)                              \
    (__scope).GetTagWithParam((__paramType), (__outParam))
#define TRACE3D_DbgScope_SetTagParam(__scope, __param) (__scope).SetTagParam((__param))
#define TRACE3D_DbgScope_GetCompositeTag(__scope) (__scope).GetCompositeTag()
#define TRACE3D_DbgScope_IsActive(__scope) (__scope).IsActive()

//----------------------------------------------------------------------------------------------------------------------
//                                      Trace3D GPUZone API (for end user)
//----------------------------------------------------------------------------------------------------------------------

#if defined(TRACE3D_GPUZONE_CODE_CONTEXT_USE_DEFAULT) && !defined(TRACE3D_GPUZONE_CODE_CONTEXT)
    #define TRACE3D_GPUZONE_CODE_CONTEXT (std::string(__FUNCTION__) + ":" + std::to_string(__LINE__)).c_str()
#elif !defined(TRACE3D_GPUZONE_CODE_CONTEXT)
    #define TRACE3D_GPUZONE_CODE_CONTEXT nullptr
#endif

// GPUZoneVk constructors
#define TRACE3D_GPUZoneVk ::trace3d::api::GPUZoneVk
#define TRACE3D_GPUZoneVkNamed(__cb, __name) TRACE3D_GPUZoneVk__((__cb), (__name), false, 0)
#define TRACE3D_GPUZoneVkNamedColoured(__cb, __name, __col) TRACE3D_GPUZoneVk__((__cb), (__name), false, (__col))
#define TRACE3D_GPUZoneVkNamedTagged(__cb, __name) TRACE3D_GPUZoneVk__((__cb), (__name), true, 0)
#define TRACE3D_GPUZoneVkNamedTaggedColoured(__cb, __name, __col) TRACE3D_GPUZoneVk__((__cb), (__name), true, (__col))

// GPUZoneVk methods
#define TRACE3D_GPUZoneVk_Begin(__zone, __cb, __name)                                                       \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), (__cb), (__name), false, TRACE3D_GPUZONE_CODE_CONTEXT, 0)
#define TRACE3D_GPUZoneVk_BeginColoured(__zone, __cb, __name, __col)                                        \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), (__cb), (__name), false, TRACE3D_GPUZONE_CODE_CONTEXT, (__col))
#define TRACE3D_GPUZoneVk_BeginTagged(__zone, __cb, __name)                                                 \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), (__cb), (__name), true, TRACE3D_GPUZONE_CODE_CONTEXT, 0)
#define TRACE3D_GPUZoneVk_BeginTaggedColoured(__zone, __cb, __name, __col)                                  \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), (__cb), (__name), true, TRACE3D_GPUZONE_CODE_CONTEXT, (__col))
#define TRACE3D_GPUZoneVk_End(__zone) (__zone).End(nullptr, TRACE3D_GPUZONE_CODE_CONTEXT)
#define TRACE3D_GPUZoneVk_EndWithName(__zone, __name) (__zone).End((__name), TRACE3D_GPUZONE_CODE_CONTEXT)
#define TRACE3D_GPUZoneVk_Add(__zone, __name) (__zone).Add((__name), TRACE3D_GPUZONE_CODE_CONTEXT)

// GPUZoneGL constructors
#define TRACE3D_GPUZoneGL ::trace3d::api::GPUZoneGL
#define TRACE3D_GPUZoneGLNamed(__name) TRACE3D_GPUZoneGL__((__name), false, 0)
#define TRACE3D_GPUZoneGLNamedColoured(__name, __col) TRACE3D_GPUZoneGL__((__name), false, (__col))
#define TRACE3D_GPUZoneGLNamedTagged(__name) TRACE3D_GPUZoneGL__((__name), true, 0)
#define TRACE3D_GPUZoneGLNamedTaggedColoured(__name, __col) TRACE3D_GPUZoneGL__((__name), true, (__col))

// GPUZoneGL methods
#define TRACE3D_GPUZoneGL_Begin(__zone, __name) (__zone).Begin(TRACE3D_CORE_API_INIT(), nullptr, (__name), false, \
                 TRACE3D_GPUZONE_CODE_CONTEXT, 0)
#define TRACE3D_GPUZoneGL_BeginColoured(__zone, __name, __col)                                                    \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), nullptr, (__name), false, TRACE3D_GPUZONE_CODE_CONTEXT, (__col))
#define TRACE3D_GPUZoneGL_BeginTagged(__zone, __name)                                                             \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), nullptr, (__name), true, TRACE3D_GPUZONE_CODE_CONTEXT, 0)
#define TRACE3D_GPUZoneGL_BeginTaggedColoured(__zone, __name, __col)                                              \
  (__zone).Begin(TRACE3D_CORE_API_INIT(), nullptr, (__name), true, TRACE3D_GPUZONE_CODE_CONTEXT, (__col))
#define TRACE3D_GPUZoneGL_End(__zone) (__zone).End(nullptr, TRACE3D_GPUZONE_CODE_CONTEXT)
#define TRACE3D_GPUZoneGL_EndWithName(__zone, __name) (__zone).End((__name), TRACE3D_GPUZONE_CODE_CONTEXT)
#define TRACE3D_GPUZoneGL_Add(__zone, __name) (__zone).Add((__name), TRACE3D_GPUZONE_CODE_CONTEXT)

#if _MSC_VER
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#endif

#endif // __TRACE3D_API_CORE_H__
