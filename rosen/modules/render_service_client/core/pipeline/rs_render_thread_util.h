/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_UTIL
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_UTIL

#include "utils/rect.h"

namespace OHOS {
namespace Rosen {

#ifdef ROSEN_OHOS
namespace {
constexpr uint32_t API_VERSION_MOD = 1000;
constexpr uint32_t INVALID_API_COMPATIBLE_VERSION = 0;
} // namespace
#endif

class RSRenderThreadUtil {
public:
    static void SrcRectScaleDown(Drawing::Rect& srcRect, const Drawing::Rect& localBounds);
    static void SrcRectScaleFit(const Drawing::Rect& srcRect, Drawing::Rect& dstRect, const Drawing::Rect& localBounds);
#ifdef ROSEN_OHOS
    static uint32_t GetApiCompatibleVersion();
#endif
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_RENDER_THREAD_UTIL