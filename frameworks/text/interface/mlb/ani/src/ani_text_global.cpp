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

#include "ani_text_global.h"

#include "ani_common.h"
#include "ani_text_utils.h"
#include "text_global_config.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
namespace {
    const std::string SET_TEXT_HIGH_CONTRAST_SIGN = std::string(ANI_ENUM_TEXT_HIGH_CONTRAST) + ":V";
    const std::string SET_TEXT_UNDEFINED_GLYPH_DISPLAY_SIGN =
        std::string(ANI_ENUM_TEXT_UNDEFINED_GLYPH_DISPLAY) + ":V";
} // namespace

std::vector<ani_native_function> AniTextGlobal::InitMethods(ani_env* env)
{
    std::vector<ani_native_function> methods = {
        ani_native_function{"setTextHighContrast",
            SET_TEXT_HIGH_CONTRAST_SIGN.c_str(), reinterpret_cast<void*>(SetTextHighContrast)},
        ani_native_function{"setTextUndefinedGlyphDisplay",
            SET_TEXT_UNDEFINED_GLYPH_DISPLAY_SIGN.c_str(), reinterpret_cast<void*>(SetTextUndefinedGlyphDisplay)},
    };
    return methods;
}

ani_status AniTextGlobal::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env = nullptr;
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }
    ani_namespace ns = nullptr;
    ret = env->FindNamespace(ANI_NAMESPACE_TEXT, &ns);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to find namespace %{public}s, ret %{public}d", ANI_NAMESPACE_TEXT, ret);
        return ANI_NOT_FOUND;
    }

    std::vector<ani_native_function> methods = InitMethods(env);
    ret = env->Namespace_BindNativeFunctions(ns, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods for text global, ret %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}

void AniTextGlobal::SetTextHighContrast(ani_env* env, ani_enum_item action)
{
    uint32_t textHighContrast = 0;

    ani_status status = env->EnumItem_GetIndex(action, &textHighContrast);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to get text high contrast index, ret %{public}d", status);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argv convert failed");
        return;
    }

    auto result = TextGlobalConfig::SetTextHighContrast(textHighContrast);
    if (result != TEXT_SUCCESS) {
        TEXT_LOGE("Failed to set text high contrast, high contrast value: %{public}u", textHighContrast);
    }
}
void AniTextGlobal::SetTextUndefinedGlyphDisplay(ani_env* env, ani_enum_item noGlyphShow)
{
    uint32_t textUndefinedGlyphDisplay = 0;
    ani_status status = env->EnumItem_GetIndex(noGlyphShow, &textUndefinedGlyphDisplay);
    if (status != ANI_OK) {
        TEXT_LOGE("Failed to get text undefined glyph display index, ret %{public}d", status);
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Argv convert failed");
        return;
    }

    auto result = TextGlobalConfig::SetTextUndefinedGlyphDisplay(textUndefinedGlyphDisplay);
    if (result != TEXT_SUCCESS) {
        TEXT_LOGE("Failed to set text undefined glyph display, undefined glyph display: %{public}u",
            textUndefinedGlyphDisplay);
    }
}
} // namespace OHOS::Text::ANI