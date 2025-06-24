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
#ifndef RENDER_SERVICE_BASE_CORE_COMMON_RS_SELF_DRAW_RECT_CHANGE_CALLBACK_FILTER_H
#define RENDER_SERVICE_BASE_CORE_COMMON_RS_SELF_DRAW_RECT_CHANGE_CALLBACK_FILTER_H

#include <unordered_set>

namespace OHOS {
namespace Rosen {

struct RectSize {
    int32_t width = 0;
    int32_t height = 0;
};

struct RectSizeRange {
    RectSize lowLimit = {};
    RectSize highLimit = {};
};

struct RectFilter {
    std::unordered_set<pid_t> pids = {};
    RectSizeRange range = {};
};

} // namespace Rosen
} // namespace OHOS
#endif //RENDER_SERVICE_BASE_CORE_COMMON_RS_SELF_DRAW_RECT_CHANGE_CALLBACK_FILTER_H