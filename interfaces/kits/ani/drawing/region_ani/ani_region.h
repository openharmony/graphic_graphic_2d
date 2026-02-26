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

#ifndef OHOS_ROSEN_ANI_REGION_H
#define OHOS_ROSEN_ANI_REGION_H

#include "ani_drawing_utils.h"
#include "utils/region.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniRegion final {
public:
    explicit AniRegion(std::shared_ptr<Region> region = nullptr) : region_(region) {}
    ~AniRegion();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithRegion(ani_env* env, ani_object obj, ani_object aniRegionObj);
    static void ConstructorWithRect(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
        ani_int bottom);
    static void SetEmpty(ani_env* env, ani_object obj);
    static ani_boolean QuickReject(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
        ani_int bottom);
    static ani_boolean SetRect(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
        ani_int bottom);
    static ani_boolean IsRegionContained(ani_env* env, ani_object obj, ani_object aniRegionObj);
    static ani_boolean QuickRejectRegion(ani_env* env, ani_object obj, ani_object aniRegionobj);
    static ani_boolean IsComplex(ani_env* env, ani_object obj);
    static ani_object GetBoundaryPath(ani_env* env, ani_object obj);
    static void SetRegion(ani_env* env, ani_object obj, ani_object aniRegionobj);
    static ani_boolean IsEqual(ani_env* env, ani_object obj, ani_object aniOtherobj);
    static ani_object GetBounds(ani_env* env, ani_object obj);
    static ani_boolean IsEmpty(ani_env* env, ani_object obj);
    static ani_boolean Op(ani_env* env, ani_object obj, ani_object aniRegionObj, ani_enum_item aniRegionOp);
    static ani_boolean SetPath(ani_env* env, ani_object obj, ani_object aniPathObj, ani_enum_item aniClipRegion);
    static ani_boolean IsPointContained(ani_env* env, ani_object obj, ani_int dx, ani_int dy);
    static ani_boolean IsRect(ani_env* env, ani_object obj);
    static ani_boolean QuickContains(ani_env* env, ani_object obj, ani_int left, ani_int top, ani_int right,
        ani_int bottom);
    static void Offset(ani_env* env, ani_object obj, ani_int dx, ani_int dy);

    std::shared_ptr<Region> GetRegion();

private:
    static ani_object RegionTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetRegionAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    std::shared_ptr<Region>* GetRegionPtrAddr();
    std::shared_ptr<Region> region_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_REGION_H