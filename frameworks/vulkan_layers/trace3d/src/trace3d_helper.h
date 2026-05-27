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

#include <cstdlib>
#include <string>
#include <map>
#include <cstdarg>
#include <shared_mutex>

#include <dirent.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <securec.h>
#include <parameters.h>
#include <hilog/log.h>

#define LOADER_TAG "TRACE3D_LOADER"

#undef LOG_DOMAIN
// The "0xD001405" is the domain ID for graphic module that alloted by the OS.
#define LOG_DOMAIN 0xD001405

#ifdef TRACE_LOADER_OHOS
#define TRACE3D_LOG_PRINT HiLogPrint
#else
#define TRACE3D_LOG_PRINT OH_LOG_Print
#endif

#define TRACE3D_LOGI(__format, ...) TRACE3D_LOG_PRINT(LOG_APP, LOG_INFO, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define TRACE3D_LOGW(__format, ...) TRACE3D_LOG_PRINT(LOG_APP, LOG_WARN, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define TRACE3D_LOGE(__format, ...) TRACE3D_LOG_PRINT(LOG_APP, LOG_ERROR, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)
#define TRACE3D_LOGD(__format, ...) TRACE3D_LOG_PRINT(LOG_APP, LOG_DEBUG, LOG_DOMAIN, LOADER_TAG, __format, __VA_ARGS__)

#define TRACE3D_LOADER_API __attribute__((visibility("default"), used))

#define TRACE3D_APP_LIB_URI      "/data/storage/el1/bundle/libs/arm64/"
#define TRACE3D_SYSTEM_LIB_URI   "/system/lib64/"

namespace trace3d {

template<class TObject>
struct SafeObject {
    [[nodiscard]] std::pair<std::shared_lock<std::shared_mutex>, const TObject&> LockRead() const
    {
        return { std::shared_lock<std::shared_mutex>(mtx_), object_ };
    }
    [[nodiscard]] std::pair<std::unique_lock<std::shared_mutex>, TObject&> LockWrite()
    {
        return { std::unique_lock<std::shared_mutex>(mtx_), object_ };
    }

private:
    TObject object_;
    mutable std::shared_mutex mtx_;
};

} // namespace trace3d
