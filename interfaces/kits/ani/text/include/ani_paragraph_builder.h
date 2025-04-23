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

#ifndef OHOS_ROSEN_ANI_PARAGRAPH_BUILDER_H
#define OHOS_ROSEN_ANI_PARAGRAPH_BUILDER_H

#include <ani.h>

#include "typography_create.h"

namespace OHOS::Rosen {
class AniParagraphBuilder final {
public:
    AniParagraphBuilder();
    static ani_status AniInit(ani_vm* vm, uint32_t* result);

private:
    static void Constructor(ani_env* env, ani_object object, ani_object paragraphStyle, ani_object fontCollection);
    static void PushStyle(ani_env* env, ani_object object, ani_object textStyle);
    static void PopStyle(ani_env* env, ani_object object);
    static void AddText(ani_env* env, ani_object object, ani_string text);
    static ani_object Build(ani_env* env, ani_object object);

    std::unique_ptr<TypographyCreate> typographyCreate_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PARAGRAPH_BUILDER_H