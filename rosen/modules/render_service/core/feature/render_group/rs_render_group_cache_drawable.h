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

#ifndef RENDER_GROUP_CACHE_DRAWABLE_H
#define RENDER_GROUP_CACHE_DRAWABLE_H

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
class RSRenderGroupCacheDrawable {
public:
    RSRenderGroupCacheDrawable() = default;
    ~RSRenderGroupCacheDrawable() = default;

    RSRenderGroupCacheDrawable(const RSRenderGroupCacheDrawable& other) = delete;
    RSRenderGroupCacheDrawable(RSRenderGroupCacheDrawable&& other) = delete;
    RSRenderGroupCacheDrawable& operator=(const RSRenderGroupCacheDrawable& other) = delete;
    RSRenderGroupCacheDrawable& operator=(RSRenderGroupCacheDrawable&& other) = delete;

    static void SetDrawBlurForCache(bool value);
    static bool IsDrawingBlurForCache();

    static void SetDrawExcludedSubTreeForCache(bool value);
    static bool IsDrawingExcludedSubTreeForCache();

private:
    static thread_local bool drawBlurForCache_;
    static thread_local bool drawExcludedSubTreeForCache_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_GROUP_CACHE_DRAWABLE_H