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

#ifndef OHOS_TEXT_ANI_RUN_H
#define OHOS_TEXT_ANI_RUN_H

#include <ani.h>
#include <memory>

#include "run.h"
#include "typography.h"

namespace OHOS::Text::ANI {
class AniRun final {
public:
    static ani_status AniInit(ani_vm* vm, uint32_t* result);
    static ani_object CreateRun(ani_env* env, Rosen::Run* run);

private:
    static ani_int GetGlyphCount(ani_env* env, ani_object object);
    static ani_object GetGlyphs(ani_env* env, ani_object object);
    static ani_object GetGlyphsByRange(ani_env* env, ani_object object, ani_object range);
    static ani_object GetPositions(ani_env* env, ani_object object);
    static ani_object GetPositionsByRange(ani_env* env, ani_object object, ani_object range);
    static ani_object GetOffsets(ani_env* env, ani_object object);
    static ani_object GetFont(ani_env* env, ani_object object);
    static void Paint(ani_env* env, ani_object object, ani_object canvas, ani_double x, ani_double y);
    static ani_object GetStringIndices(ani_env* env, ani_object object, ani_object range);
    static ani_object GetStringRange(ani_env* env, ani_object object);
    static ani_object GetTypographicBounds(ani_env* env, ani_object object);
    static ani_object GetImageBounds(ani_env* env, ani_object object);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_RUN_H