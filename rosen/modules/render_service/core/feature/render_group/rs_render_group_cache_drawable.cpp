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

#include "rs_render_group_cache_drawable.h"

namespace OHOS::Rosen {
namespace DrawableV2 {

thread_local bool RSRenderGroupCacheDrawable::drawBlurForCache_ = false;
thread_local bool RSRenderGroupCacheDrawable::drawExcludedSubTreeForCache_ = false;

void RSRenderGroupCacheDrawable::SetDrawBlurForCache(bool value)
{
    drawBlurForCache_ = value;
}
bool RSRenderGroupCacheDrawable::IsDrawingBlurForCache()
{
    return drawBlurForCache_;
}

void RSRenderGroupCacheDrawable::SetDrawExcludedSubTreeForCache(bool value)
{
    drawExcludedSubTreeForCache_ = value;
}
bool RSRenderGroupCacheDrawable::IsDrawingExcludedSubTreeForCache()
{
    return drawExcludedSubTreeForCache_;
}

} // namespace DrawableV2
} // namespace OHOS::Rosen