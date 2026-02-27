/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_TEXT_ANI_TEXT_LAYOUT_RESULT_CONVERTER_H
#define OHOS_TEXT_ANI_TEXT_LAYOUT_RESULT_CONVERTER_H

#include <ani.h>
#include "typography_types.h"

namespace OHOS::Text::ANI {
class AniTextLayoutResultConverter {
public:
    static ani_status ParseTextRectSizeToNative(
        ani_env* env, ani_object object, OHOS::Rosen::TextRectSize& rectSize);

    static ani_status ParseTextLayoutResultToAni(
        ani_env* env, const OHOS::Rosen::TextLayoutResult& result, ani_object& obj);
};
} // namespace OHOS::Text::ANI
#endif // OHOS_TEXT_ANI_TEXT_LAYOUT_RESULT_CONVERTER_H
