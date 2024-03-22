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
<<<<<<< HEAD
#include "utils/resource_holder.h"
#include "draw/surface.h"
=======

#include "utils/resource_holder.h"

>>>>>>> zhangpeng/master
#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSB_EXPORT SKResourceManager final {
public:
    static SKResourceManager& Instance();
    void HoldResource(const std::shared_ptr<Drawing::Image> &img);
<<<<<<< HEAD
    void HoldResource(std::shared_ptr<Drawing::Surface> surface);
=======
>>>>>>> zhangpeng/master
    void ReleaseResource();
private:
    SKResourceManager() = default;
    ~SKResourceManager() = default;

    std::recursive_mutex mutex_;
    std::map<pid_t, std::shared_ptr<Drawing::ResourceHolder>> images_;
<<<<<<< HEAD
    std::map<pid_t, std::list<std::shared_ptr<Drawing::Surface>>> skSurfaces_;
=======
>>>>>>> zhangpeng/master
};
} // OHOS::Rosen
#endif // SK_RESOURCE_MANAGER_H