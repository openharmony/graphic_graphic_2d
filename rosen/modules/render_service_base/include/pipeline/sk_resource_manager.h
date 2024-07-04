/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef SK_RESOURCE_MANAGER_H
#define SK_RESOURCE_MANAGER_H
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <unistd.h>
#include "utils/resource_holder.h"
#include "draw/surface.h"
#include "common/rs_macros.h"

namespace OHOS::Rosen {

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
struct SharedSurfaceContext {
    SharedSurfaceContext(std::shared_ptr<Drawing::Surface> sharedSurface)
        : sharedSurface_(std::move(sharedSurface)) {}

private:
    std::shared_ptr<Drawing::Surface> sharedSurface_;
};

struct SharedTextureContext {
    SharedTextureContext(std::shared_ptr<Drawing::Image> sharedImage)
        : sharedImage_(std::move(sharedImage)) {}

private:
    std::shared_ptr<Drawing::Image> sharedImage_;
};
#endif

class RSB_EXPORT SKResourceManager final {
public:
    static SKResourceManager& Instance();
    void HoldResource(const std::shared_ptr<Drawing::Image> &img);
    void HoldResource(std::shared_ptr<Drawing::Surface> surface);
    void ReleaseResource();
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    static void DeleteSharedTextureContext(void* context);
#endif
private:
    SKResourceManager() = default;
    ~SKResourceManager() = default;

    std::recursive_mutex mutex_;
    std::map<pid_t, std::shared_ptr<Drawing::ResourceHolder>> images_;
    std::map<pid_t, std::list<std::shared_ptr<Drawing::Surface>>> skSurfaces_;
};
} // OHOS::Rosen
#endif // SK_RESOURCE_MANAGER_H