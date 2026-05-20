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

#include <trace3d/api_platform/trace3d_api_platform.h>
#include "trace3d_helper.h"

extern "C" {
const TRACE3D_PLATFORM_API_TABLE_HEADER* TRACE3D_PlatformGetAPI();

} // extern "C"

namespace trace3d {

struct CaptureLib {
    void *handle{nullptr};
    bool initialized{false};
};

static SafeObject< CaptureLib > g_CaptureLib;

size_t GetFileSize(const char *fileName)
{
    size_t size = 0;
    FILE *f = fopen(fileName, "rb");
    if (f) {
        fseek(f, 0L, SEEK_END);
        size = static_cast<size_t>(ftell(f));
        fclose(f);
    }
    return size;
}

void *DlopenSharedLibrary(const char *libFullName)
{
    void *handle = nullptr;
    handle = dlopen(libFullName, RTLD_LAZY);
    return handle;
}

void *DlopenBundledSharedLibrary(const char *libName)
{
    std::string fileName = std::string(TRACE3D_APP_LIB_URI) + libName;
    void *libHandle = DlopenSharedLibrary(fileName.c_str());
    if (!libHandle) {
        fileName = std::string(TRACE3D_SYSTEM_LIB_URI) + libName;
        libHandle = DlopenSharedLibrary(fileName.c_str());
    }
    return libHandle;
}

size_t TestBundledSharedLibrary(const char *libName)
{
    std::string fileName = std::string(TRACE3D_APP_LIB_URI) + libName;
    size_t sizeBytes = GetFileSize(fileName.c_str());
    if (!sizeBytes) {
        fileName = std::string(TRACE3D_SYSTEM_LIB_URI) + libName;
        sizeBytes = GetFileSize(fileName.c_str());
    }
    return sizeBytes;
}

void* CaptureInit()
{
    auto lockLib = g_CaptureLib.LockWrite();

    auto &lib = lockLib.second;

    if (lib.initialized) {
        return lib.handle;
    }

    if (OHOS::system::GetParameter(TRACE3D_LAYER_PARAM, "0") != "1") {
        lib.initialized = true;
        return nullptr;
    }

    size_t foundSize = TestBundledSharedLibrary(TRACE3D_CAPTURE_LIB_NAME);
    if (foundSize > 0) {
        TRACE3D_LOGI("%s: found:'%s', size:%zu\n", __FUNCTION__, TRACE3D_CAPTURE_LIB_NAME, foundSize);

        lib.handle = DlopenBundledSharedLibrary(TRACE3D_CAPTURE_LIB_NAME);
        if (lib.handle) {
            trace3d::api::RetCodePlatform retCode = trace3d::api::RET_PLATFORM_ERR_INIT;

            auto pfnCapturePlatformInit =
                (pTRACE3D_CapturePlatformInit)dlsym(lib.handle, "TRACE3D_CapturePlatformInit");
            if (pfnCapturePlatformInit) {
                retCode = pfnCapturePlatformInit(TRACE3D_PlatformGetAPI);
            }
            TRACE3D_LOGI("%s: init platform retCode:%d\n", __FUNCTION__, retCode);
        } else {
            TRACE3D_LOGE("%s: failed open:'%s', error:%d ('%s')\n", __FUNCTION__, TRACE3D_CAPTURE_LIB_NAME, errno,
                strerror(errno));
        }
    }

    lib.initialized = true;
    return lib.handle;
}

} // namespace trace3d
