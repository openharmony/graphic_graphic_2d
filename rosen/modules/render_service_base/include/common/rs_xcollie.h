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
#ifndef RENDER_SERVICE_BASE_COMMON_RS_XCOLLIE_H
#define RENDER_SERVICE_BASE_COMMON_RS_XCOLLIE_H

#include <string>
#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSXCollie final {
public:
    /*
     * @brief monitor whether the execution time of the code segment exceeds expectations.
     * @param tag rs timer name.
     * @param timeoutSeconds Timeout duration(S), default 6s.
     * @param func callback function.
     * @param arg callback arg.
     * @param flag default(1) to generate log, if you need more, see xcollie_define.h.
     */
    RSXCollie(const std::string& tag, uint32_t timeoutSeconds = 6,
        std::function<void(void*)> func = nullptr, void* arg = nullptr, uint32_t flag = 1);
    ~RSXCollie();
private:
    void CancelRSXCollie(void);
    int32_t id_ = -1;
    bool isCanceled_ = false;
    std::string tag_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_BASE_COMMON_RS_XCOLLIE_H
