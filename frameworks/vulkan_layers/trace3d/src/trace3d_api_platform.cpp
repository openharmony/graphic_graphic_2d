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

#if defined(TRACE_LOADER_OHOS)
#include <transaction/rs_render_service_client.h>
#endif

#include <trace3d/api_platform/trace3d_api_platform.h>
#include "trace3d_helper.h"


trace3d::api::RetCodePlatform PlatformHrpServiceOpenFile(
    const trace3d::api::HrpServiceDirInfo* dirInfo,
    const char *fileName, uint32_t flags, int *outFd, int32_t *outSvcRetCode)
{
    if (!dirInfo || !dirInfo->subDir || !dirInfo->subDir2 || !fileName || !outSvcRetCode || !outFd) {
        return trace3d::api::RET_PLATFORM_ERR_INVALID_PARAM;
    }
    uint32_t flagsFiltered = (flags&trace3d::api::HRP_SERVICE_FILE_FLAG_RDONLY) ? O_RDONLY : 0;
    flagsFiltered |= (flags&trace3d::api::HRP_SERVICE_FILE_FLAG_WRONLY) ? O_WRONLY : 0;
    flagsFiltered |= (flags&trace3d::api::HRP_SERVICE_FILE_FLAG_RDWR) ? O_RDWR : 0;
    flagsFiltered |= (flags&trace3d::api::HRP_SERVICE_FILE_FLAG_CREAT) ? O_CREAT : 0;
#if defined(TRACE_LOADER_OHOS)
    auto rsClient = std::static_pointer_cast<OHOS::Rosen::RSRenderServiceClient>(
        OHOS::Rosen::RSIRenderClient::CreateRenderServiceClient());
    if (rsClient == nullptr) {
        return trace3d::api::RET_PLATFORM_ERR_HRP_SERVICE_INTERFACE_GET;
    }
    int retFd = -1;
    OHOS::Rosen::HrpServiceDirInfo rosenDirInfo{OHOS::Rosen::HrpServiceGetDirType((uint32_t)dirInfo->baseDirType),
        dirInfo->subDir, dirInfo->subDir2};
    auto svcRetCode = rsClient->ProfilerServiceOpenFile(rosenDirInfo, fileName,
        static_cast<int32_t>(flagsFiltered), retFd);
    *outSvcRetCode = (int32_t)svcRetCode;
    if (svcRetCode < OHOS::Rosen::RET_HRP_SERVICE_SUCCESS || retFd == -1) {
        if (retFd != -1) {
            close(retFd);
        }
        return trace3d::api::RET_PLATFORM_ERR_HRP_SERVICE_INTERFACE_CALL;
    }
    *outFd = retFd;
    return trace3d::api::RET_PLATFORM_SUCCESS;
#else
    return trace3d::api::RET_PLATFORM_ERR_UNSUPPORTED;
#endif
}

trace3d::api::RetCodePlatform PlatformHrpServicePopulateFiles(const trace3d::api::HrpServiceDirInfo* dirInfo,
    void (*pfnPopulate)(const char* fname, const trace3d::api::FileInfo& fi, void* storage), void* storage,
    int32_t* outSvcRetCode)
{
    if (!dirInfo || !dirInfo->subDir || !dirInfo->subDir2 || !pfnPopulate || !storage || !outSvcRetCode) {
        return trace3d::api::RET_PLATFORM_ERR_INVALID_PARAM;
    }
#if defined(TRACE_LOADER_OHOS)
    auto rsClient = std::static_pointer_cast<OHOS::Rosen::RSRenderServiceClient>(
        OHOS::Rosen::RSIRenderClient::CreateRenderServiceClient());
    if (rsClient == nullptr) {
        return trace3d::api::RET_PLATFORM_ERR_HRP_SERVICE_INTERFACE_GET;
    }
    std::vector<OHOS::Rosen::HrpServiceFileInfo> files;
    OHOS::Rosen::HrpServiceDirInfo rosenDirInfo{OHOS::Rosen::HrpServiceGetDirType((uint32_t)dirInfo->baseDirType),
        dirInfo->subDir, dirInfo->subDir2};
    uint32_t firstFileIndex = 0;
    auto svcRetCode = OHOS::Rosen::RET_HRP_SERVICE_MORE_DATA_AVAILABLE;
    while (svcRetCode == OHOS::Rosen::RET_HRP_SERVICE_MORE_DATA_AVAILABLE) {
        svcRetCode = rsClient->ProfilerServicePopulateFiles(rosenDirInfo, firstFileIndex, files);
        *outSvcRetCode = (int32_t)svcRetCode;
        if (svcRetCode < OHOS::Rosen::RET_HRP_SERVICE_SUCCESS) {
            return trace3d::api::RET_PLATFORM_ERR_HRP_SERVICE_INTERFACE_CALL;
        }
        for (const auto &f : files) {
            trace3d::api::FileInfo fi{};
            fi.size = f.size;
            fi.isDir = f.isDir;
            fi.accessBits = f.accessBits;
            fi.accessTime.sec = f.accessTime.sec;
            fi.accessTime.nsec = f.accessTime.nsec;
            fi.modifyTime.sec = f.modifyTime.sec;
            fi.modifyTime.nsec = f.modifyTime.nsec;
            pfnPopulate(f.name.c_str(), fi, storage);
        }
        firstFileIndex += files.size();
        files.clear();
    }

    return trace3d::api::RET_PLATFORM_SUCCESS;
#else
    return trace3d::api::RET_PLATFORM_ERR_UNSUPPORTED;
#endif
}

const char* PlatformHrpServiceGetBaseDir(trace3d::api::HrpServiceDir baseDirType)
{
#if defined(TRACE_LOADER_OHOS)
    return OHOS::Rosen::HrpServiceGetDirFull(OHOS::Rosen::HrpServiceGetDirType((uint32_t)baseDirType));
#else
    return "";
#endif
}

bool PlatformProfilerIsSecureScreen()
{
    bool retValue = false;
#if defined(TRACE_LOADER_OHOS)
    auto rsClient = std::static_pointer_cast<OHOS::Rosen::RSRenderServiceClient>(
        OHOS::Rosen::RSIRenderClient::CreateRenderServiceClient());
    if (rsClient == nullptr) {
        return retValue;
    }
    retValue = rsClient->ProfilerIsSecureScreen();
#endif
    return retValue;
}

trace3d::api::RetCodePlatform PlatformGetSystemParameter(const char *paramName, const char *defaultValue,
    void (* pfnPopulate)(const char *value, void *storage), void *storage)
{
    if (!paramName || !defaultValue || !pfnPopulate) {
        return trace3d::api::RET_PLATFORM_ERR_INVALID_PARAM;
    }
#if defined(TRACE_LOADER_OHOS)
    std::string paramValue = OHOS::system::GetParameter(paramName, defaultValue);

    pfnPopulate(paramValue.c_str(), storage);
    return trace3d::api::RET_PLATFORM_SUCCESS;
#else
    return trace3d::api::RET_PLATFORM_ERR_UNSUPPORTED;
#endif
}

static const TRACE3D_PLATFORM_API_TABLE g_Trace3DPlatformAPITable = {
    { { trace3d::api::PLATFORM_MAGIC }, TRACE3D_PLATFORM_API_VERSION, (uint32_t)sizeof(TRACE3D_PLATFORM_API_TABLE) },
    PlatformHrpServiceOpenFile,
    PlatformHrpServicePopulateFiles,
    PlatformHrpServiceGetBaseDir,
    PlatformProfilerIsSecureScreen,
    PlatformGetSystemParameter,
};

extern "C" {
TRACE3D_LOADER_API
const TRACE3D_PLATFORM_API_TABLE_HEADER* TRACE3D_PlatformGetAPI() { return &g_Trace3DPlatformAPITable.header; }

} // extern "C"
