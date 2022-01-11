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

#include <surface_utils.h>
#include <cinttypes>
#include "buffer_log.h"

namespace OHOS {
using namespace HiviewDFX;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0, "SurfaceUtils" };
}

SurfaceUtils* SurfaceUtils::GetInstance()
{
    static SurfaceUtils instance;
    return &instance;
}

SurfaceUtils::~SurfaceUtils()
{
    surfaceCache_.clear();
}

sptr<Surface> SurfaceUtils::GetSurface(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (surfaceCache_.count(uniqueId) == 0) {
        BLOGE("Cannot find surface by uniqueId %" PRIu64 ".", uniqueId);
        return nullptr;
    }
    return surfaceCache_[uniqueId];
}

SurfaceError SurfaceUtils::Add(uint64_t uniqueId, const sptr<Surface> &surface)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (surface == nullptr) {
        BLOGE(" surface is nullptr.");
        return GSERROR_INVALID_ARGUMENTS;
    }
    if (surfaceCache_.count(uniqueId) == 0) {
        surfaceCache_[uniqueId] = surface;
        return GSERROR_OK;
    }
    BLOGW("the surface by uniqueId %" PRIu64 " already existed", uniqueId);
    return GSERROR_OK;
}

SurfaceError SurfaceUtils::Remove(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> lockGuard(mutex_);
    if (surfaceCache_.count(uniqueId) == 0) {
        BLOGE("Delete failed without surface by uniqueId %" PRIu64 "", uniqueId);
        return GSERROR_INVALID_OPERATING;
    }
    surfaceCache_.erase(uniqueId);
    return GSERROR_OK;
}
} // namespace OHOS
