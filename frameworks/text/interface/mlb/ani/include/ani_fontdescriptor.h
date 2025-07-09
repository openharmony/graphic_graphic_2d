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

#ifndef OHOS_TEXT_ANI_FONT_DESCRIPTOR_H
#define OHOS_TEXT_ANI_FONT_DESCRIPTOR_H

#include <ani.h>
#include <memory>

#include "font_parser.h"
#include "run.h"

namespace OHOS::Text::ANI {
class AniFontDescriptor final {
public:
    static ani_status AniInit(ani_vm* vm, uint32_t* result);

private:
    static ani_object GetSystemFontFullNamesByType(ani_env* env, ani_enum_item fontType);
    static ani_object GetFontDescriptorByFullName(ani_env* env, ani_string fullName, ani_enum_item fontType);
    static ani_object MatchFontDescriptors(ani_env* env, ani_object desc);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_FONT_DESCRIPTOR_H