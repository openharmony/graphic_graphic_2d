/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include "mock_dlsym.h"

#include "feature/hwc/rs_uni_hwc_prevalidate_util.h"

namespace OHOS {
namespace Rosen {

bool g_handleEventFuncNull = false;
bool g_preValidateFuncNull = false;

int32_t MockHandleEventFunc(uint32_t devId, uint32_t eventId, const std::vector<int32_t>& eventData)
{
    return 0;
}

int32_t MockPreValidateFunc(uint32_t,
    const std::vector<RequestLayerInfo>&, std::map<uint64_t, RequestCompositionType>&)
{
    return 0;
}

extern "C" void *dlsym(void *handle, const char *symbol)
{
    std::string strSymbol(symbol);
    if (strSymbol == "HandleHWCEvent") {
        if (g_handleEventFuncNull) {
            return nullptr;
        }
        return reinterpret_cast<void *>(MockHandleEventFunc);
    }
    if (strSymbol == "RequestLayerStrategy") {
        if (g_preValidateFuncNull) {
            return nullptr;
        }
        return reinterpret_cast<void *>(MockHandleEventFunc);
    }
    return nullptr;
}
}  // namespace Rosen
}  // namespace OHOS