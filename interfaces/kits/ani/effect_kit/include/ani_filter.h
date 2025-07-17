/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ANI_FILTER_H
#define OHOS_ANI_FILTER_H

#include <memory>
#include <mutex>
#include <vector>

#include "ani.h"
#include "include/core/SkImageFilter.h"
#include "pixel_map_ani.h"

namespace OHOS {
namespace Rosen {
enum class DrawError;
class AniFilter {
public:
    static ani_object Blur(ani_env* env, ani_object obj, ani_double param);
    static ani_object GetEffectPixelMap(ani_env* env, ani_object obj);
    static ani_object CreateEffect(ani_env* env, ani_object para);
    static ani_status Init(ani_env* env);
    std::shared_ptr<Media::PixelMap> GetDstPixelMap();
    std::shared_ptr<Media::PixelMap> GetSrcPixelMap();

private:
    void AddNextFilter(sk_sp<SkImageFilter> filter);
    DrawError Render(bool forceCPU);
    std::vector<sk_sp<SkImageFilter>> skFilters_;
    std::shared_ptr<Media::PixelMap> srcPixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> dstPixelMap_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ANI_FILTER_H