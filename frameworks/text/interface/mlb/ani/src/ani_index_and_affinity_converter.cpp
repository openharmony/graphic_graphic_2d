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

#include "ani.h"
#include "ani_common.h"
#include "ani_index_and_affinity_converter.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

ani_status AniIndexAndAffinityConverter::ParseIndexAndAffinityToAni(
    ani_env* env, const OHOS::Rosen::IndexAndAffinity indexAndAffinity, ani_object& aniObj)
{
    aniObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_POSITION_WITH_AFFINITY, ":V");
    env->Object_SetPropertyByName_Double(aniObj, "position", indexAndAffinity.index);
    env->Object_SetPropertyByName_Ref(aniObj, "affinity",
        AniTextUtils::CreateAniEnum(env, ANI_ENUM_AFFINITY, static_cast<int>(indexAndAffinity.affinity)));
    return ANI_OK;
}
} // namespace OHOS::Text::ANI