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

#include "ani_typographic_bounds_converter.h"

#include "ani_common.h"
#include "ani_text_utils.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;

ani_status AniTypographicBoundsConverter::ParseTypographicBoundsToAni(
    ani_env* env, ani_object& obj, double ascent, double descent, double leading, double width)
{
    obj = AniTextUtils::CreateAniObject(env, ANI_FIND_CLASS(env, ANI_CLASS_TYPOGRAPHIC_BOUNDS),
        ANI_CLASS_FIND_METHOD(env, ANI_CLASS_TYPOGRAPHIC_BOUNDS, "<ctor>", "dddd:"), ani_double(ascent),
        ani_double(descent), ani_double(leading), ani_double(width));
    return ANI_OK;
}
} // namespace OHOS::Text::ANI