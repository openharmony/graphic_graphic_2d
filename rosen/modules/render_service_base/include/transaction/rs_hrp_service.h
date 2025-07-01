/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_HRP_SERVICE_H
#define RS_HRP_SERVICE_H

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <string>

namespace OHOS::Rosen {

#define HRP_SERVICE_DIR "/data/local/shader_cache/local/system/RenderProfiler"

#define HRP_SERVICE_FILE_NAME_MAX_SIZE (255)
#define HRP_SERVICE_REQUEST_MAX_SIZE_BYTES (256*1024)

enum HrpServiceDir : uint32_t {
    HRP_SERVICE_DIR_UNKNOWN = 0,
    HRP_SERVICE_DIR_COMMON = 1,
    HRP_SERVICE_DIR_RS = 2,
    HRP_SERVICE_DIR_TRACE3D = 3,
};

enum RetCodeHrpService : int32_t {
    RET_HRP_SERVICE_MORE_DATA_AVAILABLE = 1,
    RET_HRP_SERVICE_SUCCESS = 0,
    RET_HRP_SERVICE_ERR_UNKNOWN = -1,
    RET_HRP_SERVICE_ERR_UNSUPPORTED = -2,
    RET_HRP_SERVICE_ERR_INVALID_PARAM = -3,
    RET_HRP_SERVICE_ERR_ACCESS_DENIED = -4,
    RET_HRP_SERVICE_ERR_INVALID_FILE_DESCRIPTOR = -5,
    // Proxy errors
    RET_HRP_SERVICE_ERR_PROXY_GET_REMOTE = -101,
    RET_HRP_SERVICE_ERR_PROXY_SEND_REQUEST = -102,
    RET_HRP_SERVICE_ERR_PROXY_INVALID_RESULT = -103,
    RET_HRP_SERVICE_ERR_PROXY_INVALID_RESULT_DATA = -104,
    // Stub errors
    RET_HRP_SERVICE_ERR_STUB_MAKE_DIR_ACCESS_DENIED = -201,
    RET_HRP_SERVICE_ERR_STUB_OPEN_FILE_ACCESS_DENIED = -202,
    RET_HRP_SERVICE_ERR_STUB_MAKE_DIR = -203,
    RET_HRP_SERVICE_ERR_STUB_OPEN_FILE = -204,
    RET_HRP_SERVICE_ERR_STUB_OPEN_DIR_ERROR = -205,
    RET_HRP_SERVICE_ERR_STUB_FILE_READ_ERROR_NOT_EXISTS = -206,
    RET_HRP_SERVICE_ERR_STUB_PATH_EXISTS_NOT_DIR = -207
};

struct HrpServiceDirInfo {
    HrpServiceDir baseDirType{ HRP_SERVICE_DIR_UNKNOWN };
    std::string subDir;
    std::string subDir2;
};

struct HrpServiceBaseFileInfo {
    struct TimeValue {
        uint64_t sec { 0 }, nsec { 0 };
    };
    uint32_t size { 0 };
    bool isDir { false };
    uint32_t accessBits { 0 };
    TimeValue accessTime {}, modifyTime {};
};

struct HrpServiceFileInfo : public HrpServiceBaseFileInfo {
    std::string name;
};

static inline bool HrpServiceValidDirOrFileName(const std::string& name)
{
    return (name.length() <= HRP_SERVICE_FILE_NAME_MAX_SIZE &&
            name.find("\\") == std::string::npos && name.find("/") == std::string::npos &&
            name != "." && name != "..");
}

static inline const char* HrpServiceGetDirFull(HrpServiceDir type)
{
    switch (type) {
        case HRP_SERVICE_DIR_COMMON:
            return HRP_SERVICE_DIR "/common";
        case HRP_SERVICE_DIR_RS:
            return HRP_SERVICE_DIR "/rs";
        case HRP_SERVICE_DIR_TRACE3D:
            return HRP_SERVICE_DIR "/trace3d";
        default:
            return ""; // empty string for convenient subfolder construction
    }
}

static inline HrpServiceDir HrpServiceGetDirType(uint32_t val)
{
    switch (val) {
        case HRP_SERVICE_DIR_COMMON:
            return HRP_SERVICE_DIR_COMMON;
        case HRP_SERVICE_DIR_RS:
            return HRP_SERVICE_DIR_RS;
        case HRP_SERVICE_DIR_TRACE3D:
            return HRP_SERVICE_DIR_TRACE3D;
        default:
            return HRP_SERVICE_DIR_UNKNOWN;
    }
}

} // namespace OHOS::Rosen

#endif // RS_HRP_SERVICE_H
