/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
#define RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H

#include <set>
#include <string>

namespace OHOS {
namespace Rosen {

enum class DirtyRegionDebugType {
    DISABLED = 0,
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    CURRENT_SUB_AND_WHOLE,
    CURRENT_WHOLE_AND_MULTI_HISTORY
};

class RSSystemProperties final {
public:
    ~RSSystemProperties() = default;

    // used by clients
    static bool GetUniRenderEnabled();
    static void InitUniRenderEnabled(const std::string &bundleName);
    static DirtyRegionDebugType GetDirtyRegionDebugType();
    static bool GetOcclusionEnabled();
    static std::string GetRSEventProperty(const std::string &paraName);

private:
    RSSystemProperties() = default;

    static inline bool isUniRenderEnabled_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_COMMON_RS_COMMON_DEF_H
