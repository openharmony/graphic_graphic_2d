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

#pragma once

#include <cstddef>
#include <cstdint>

#ifndef TRACE3D_API_VERSION
#define TRACE3D_API_VERSION(maj, min, patch) (uint32_t)((((maj)&255)<<16)|(((min)&255)<<8)|(((patch)&255)<<0))
#endif

#define TRACE3D_CAPTURE_LIB_NAME "libVkLayer_Trace3DCapture.so"

#define TRACE3D_LAYER_PARAM      "trace3d.layer"

namespace trace3d::api {

constexpr uint8_t PLATFORM_VER_MAJOR = 0;
constexpr uint8_t PLATFORM_VER_MINOR = 1;
constexpr uint8_t PLATFORM_VER_PATCH = 0;

#define TRACE3D_PLATFORM_API_VERSION TRACE3D_API_VERSION( \
    trace3d::api::PLATFORM_VER_MAJOR, trace3d::api::PLATFORM_VER_MINOR, trace3d::api::PLATFORM_VER_PATCH)

constexpr uint64_t PLATFORM_MAGIC =
    uint64_t(((uint64_t)'#' << 56) | ((uint64_t)'T' << 48) | ((uint64_t)'R' << 40) | ((uint64_t)'A' << 32) |
             ((uint64_t)'C' << 24) | ((uint64_t)'E' << 16) | ((uint64_t)'3' << 8) | ((uint64_t)'D' << 0));

enum RetCodePlatform : int32_t {
    RET_PLATFORM_SUCCESS = 0,
    RET_PLATFORM_ERR_UNKNOWN = -10001,
    RET_PLATFORM_ERR_INVALID_VERSION = -10002,
    RET_PLATFORM_ERR_UNSUPPORTED = -10003,
    RET_PLATFORM_ERR_INVALID_PARAM = -10004,
    RET_PLATFORM_ERR_ACCESS_DENIED = -10005,
    RET_PLATFORM_ERR_INIT = -10006,
    // Hrp Service errors
    RET_PLATFORM_ERR_HRP_SERVICE_INTERFACE_GET = -10101,
    RET_PLATFORM_ERR_HRP_SERVICE_INTERFACE_CALL = -10102,
};

enum class HrpServiceDir : uint32_t {
    UNKNOWN = 0,
    COMMON = 1,
    RS = 2,
    TRACE3D = 3,
};

struct HrpServiceDirInfo {
    HrpServiceDir baseDirType{ HrpServiceDir::UNKNOWN };
    const char* subDir;
    const char* subDir2;
};


enum HrpServiceFileFlag : uint32_t {
    HRP_SERVICE_FILE_FLAG_CREAT = (1<<1),
    HRP_SERVICE_FILE_FLAG_RDONLY = (1<<2),
    HRP_SERVICE_FILE_FLAG_WRONLY = (1<<3),
    HRP_SERVICE_FILE_FLAG_RDWR = (1<<4),
};

#pragma pack(push, 1)
struct TimeValue {
    uint64_t sec{0}, nsec{0};
    bool IsZero() const { return sec == 0 && nsec == 0; }
    bool operator == (const TimeValue &other) const { return !(sec == other.sec && nsec == other.nsec); }
    bool operator>(const TimeValue& other) const
    {
        return sec > other.sec || (sec == other.sec && nsec > other.nsec);
    }
};
#pragma pack(pop)
static_assert(sizeof(TimeValue)==(16), "Incorrect Trace3D proto struct size");

#pragma pack(push, 1)
struct FileInfo {
    uint32_t size{0};
    bool isDir{false};
    uint32_t accessBits{0};
    TimeValue accessTime{}, modifyTime{};
};
#pragma pack(pop)
static_assert(sizeof(FileInfo)==(41), "Incorrect Trace3D proto struct size");

} // namespace trace3d::api

#pragma pack(push, 1)
struct TRACE3D_PLATFORM_API_TABLE_HEADER {
    union {
        uint64_t magic{0};
        uint8_t magicBytes[8];
    };
    uint32_t version{0};
    uint32_t sizeBytes{0};
};
#pragma pack(pop)
static_assert(sizeof(TRACE3D_PLATFORM_API_TABLE_HEADER)==(16), "Incorrect Trace3D proto struct size"); //NOLINT

typedef trace3d::api::RetCodePlatform (*pTRACE3D_PlatformHrpServiceOpenFile)(
    const trace3d::api::HrpServiceDirInfo* dirInfo,
    const char* fileName, uint32_t flags, int* outFd, int32_t* outSvcRetCode);

typedef trace3d::api::RetCodePlatform (*pTRACE3D_PlatformHrpServicePopulateFiles)(
    const trace3d::api::HrpServiceDirInfo* dirInfo,
    void (*pfnPopulate)(const char* fname, const trace3d::api::FileInfo& fi, void* storage), void* storage,
    int32_t* outSvcRetCode);

typedef const char* (* pTRACE3D_PlatformHrpServiceGetBaseDir)(trace3d::api::HrpServiceDir baseDirType); //NOLINT

typedef bool (* pTRACE3D_PlatformProfilerIsSecureScreen)();

typedef trace3d::api::RetCodePlatform (*pTRACE3D_PlatformGetSystemParameter)(const char* paramName,
    const char* defaultValue, void (*pfnPopulate)(const char* value, void* storage), void* storage);

#pragma pack(push, 1)
struct TRACE3D_PLATFORM_API_TABLE {
    const TRACE3D_PLATFORM_API_TABLE_HEADER header{
        { trace3d::api::PLATFORM_MAGIC }, TRACE3D_PLATFORM_API_VERSION, sizeof(TRACE3D_PLATFORM_API_TABLE)
    };
    // NOLINTBEGIN
    pTRACE3D_PlatformHrpServiceOpenFile              HrpServiceOpenFile{nullptr};
    pTRACE3D_PlatformHrpServicePopulateFiles         HrpServicePopulateFiles{nullptr};
    pTRACE3D_PlatformHrpServiceGetBaseDir            HrpServiceGetBaseDir{nullptr};
    pTRACE3D_PlatformProfilerIsSecureScreen          ProfilerIsSecureScreen{nullptr};
    pTRACE3D_PlatformGetSystemParameter              GetSystemParameter{nullptr};
    // NOLINTEND
};
#pragma pack(pop)

typedef const TRACE3D_PLATFORM_API_TABLE_HEADER* (* pTRACE3D_PlatformGetAPI)();

typedef trace3d::api::RetCodePlatform (* pTRACE3D_CapturePlatformInit)(const pTRACE3D_PlatformGetAPI pfnGetPlatformAPI);
