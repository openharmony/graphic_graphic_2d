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

#include <cstddef>
#include "ani.h"
#include "ani_common.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"

namespace OHOS::Rosen {
std::unique_ptr<TypographyStyle> AniCommon::ParseParagraphStyle(ani_env* env, ani_object obj)
{
    ani_class cls;
    ani_status ret;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH_STYLE, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] can't find class:%{public}d", ret);
        return nullptr;
    }
    ani_boolean isObj = false;
    ret = env->Object_InstanceOf(obj, cls, &isObj);
    if (!isObj) {
        TEXT_LOGE("[ANI] Object mismatch:%{public}d", ret);
        return nullptr;
    }
    std::unique_ptr<TypographyStyle> paragraphStyle = std::make_unique<TypographyStyle>();
    ani_ref maxLinesRef = nullptr;
    if (AniTextUtils::ReadOptionalField(env, obj, "maxLines", maxLinesRef) == ANI_OK && maxLinesRef != nullptr) {
        ani_double maxLines;
        env->Object_CallMethodByName_Double(static_cast<ani_object>(maxLinesRef), "maxLines", nullptr, &maxLines);
        paragraphStyle->maxLines = static_cast<size_t>(maxLines);
    }

    return paragraphStyle;
}
} // namespace OHOS::Rosen