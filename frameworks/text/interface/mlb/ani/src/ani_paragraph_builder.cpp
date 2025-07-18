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

#include <algorithm>
#include <codecvt>
#include <cstdint>

#include "ani_common.h"
#include "ani_font_collection.h"
#include "ani_paragraph.h"
#include "ani_paragraph_builder.h"
#include "ani_paragraph_style_converter.h"
#include "ani_text_style_converter.h"
#include "ani_text_utils.h"
#include "font_collection.h"
#include "text_style.h"
#include "utils/text_log.h"

namespace OHOS::Text::ANI {
using namespace OHOS::Rosen;
void AniParagraphBuilder::Constructor(
    ani_env* env, ani_object object, ani_object paragraphStyle, ani_object fontCollection)
{
    std::unique_ptr<TypographyStyle> typographyStyleNative =
        AniParagraphStyleConverter::ParseParagraphStyleToNative(env, paragraphStyle);
    if (typographyStyleNative == nullptr) {
        TEXT_LOGE("Failed to parse typographyStyle");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    AniFontCollection* aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(env, fontCollection);
    if (aniFontCollection == nullptr) {
        TEXT_LOGE("FontCollection is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    std::shared_ptr<FontCollection> fontCollectionNative = aniFontCollection->GetFontCollection();
    if (fontCollectionNative == nullptr) {
        TEXT_LOGE("Failed to get font collection");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    std::unique_ptr<TypographyCreate> typographyCreateNative =
        TypographyCreate::Create(*typographyStyleNative, fontCollectionNative);
    if (typographyCreateNative == nullptr) {
        TEXT_LOGE("Failed to create typography creator");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "TypographyCreate create error.");
        return;
    }

    TypographyCreate* typographyCreate = typographyCreateNative.release();
    ani_status ret = env->Object_SetFieldByName_Long(object, NATIVE_OBJ, reinterpret_cast<ani_long>(typographyCreate));
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to create ani ParagraphBuilder obj");
        delete typographyCreate;
        typographyCreate = nullptr;
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "AniParagraphBuilder create error.");
        return;
    }
}

ani_status AniParagraphBuilder::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env{nullptr};
    ani_status ret = vm->GetEnv(ANI_VERSION_1, &env);
    if (ret != ANI_OK || env == nullptr) {
        TEXT_LOGE("Failed to get env, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH_BUILDER, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to  find class, ret %{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string ctorSignature =
        std::string(ANI_INTERFACE_PARAGRAPH_STYLE) + std::string(ANI_CLASS_FONT_COLLECTION) + ":V";
    std::string pushStyleSignature = std::string(ANI_INTERFACE_TEXT_STYLE) + ":V";
    std::string buildStyleSignature = ":" + std::string(ANI_CLASS_PARAGRAPH);
    std::array methods = {
        ani_native_function{"constructorNative", ctorSignature.c_str(), reinterpret_cast<void*>(Constructor)},
        ani_native_function{"pushStyle", pushStyleSignature.c_str(), reinterpret_cast<void*>(PushStyle)},
        ani_native_function{"popStyle", ":V", reinterpret_cast<void*>(PopStyle)},
        ani_native_function{"addText", "Lstd/core/String;:V", reinterpret_cast<void*>(AddText)},
        ani_native_function{"build", buildStyleSignature.c_str(), reinterpret_cast<void*>(Build)},
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        TEXT_LOGE("Failed to bind methods, ret %{public}d", ret);
        return ANI_ERROR;
    }
    return ANI_OK;
}

void AniParagraphBuilder::PushStyle(ani_env* env, ani_object object, ani_object textStyle)
{
    TypographyCreate* typographyCreate = AniTextUtils::GetNativeFromObj<TypographyCreate>(env, object);
    if (typographyCreate == nullptr) {
        TEXT_LOGE("ParagraphBuilder is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    TextStyle textStyleNative;
    ani_status status = AniTextStyleConverter::ParseTextStyleToNative(env, textStyle, textStyleNative);
    if (status == ANI_OK) {
        typographyCreate->PushStyle(textStyleNative);
    }
}

void AniParagraphBuilder::PopStyle(ani_env* env, ani_object object)
{
    TypographyCreate* typographyCreate = AniTextUtils::GetNativeFromObj<TypographyCreate>(env, object);
    if (typographyCreate == nullptr) {
        TEXT_LOGE("ParagraphBuilder is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    typographyCreate->PopStyle();
}

void AniParagraphBuilder::AddText(ani_env* env, ani_object object, ani_string text)
{
    std::u16string textStr;
    ani_status status = AniTextUtils::AniToStdStringUtf16(env, text, textStr);
    if (status != ANI_OK) {
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Fail get utf16.");
        return;
    }
    TypographyCreate* typographyCreate = AniTextUtils::GetNativeFromObj<TypographyCreate>(env, object);
    if (typographyCreate == nullptr) {
        TEXT_LOGE("ParagraphBuilder is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    typographyCreate->AppendText(textStr);
}

ani_object AniParagraphBuilder::Build(ani_env* env, ani_object object)
{
    TypographyCreate* typographyCreate = AniTextUtils::GetNativeFromObj<TypographyCreate>(env, object);
    if (typographyCreate == nullptr) {
        TEXT_LOGE("ParagraphBuilder is null");
        AniTextUtils::ThrowBusinessError(env, TextErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return AniTextUtils::CreateAniUndefined(env);
    }
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    OHOS::Rosen::Typography* typographPtr = typography.release();
    ani_object typographObj = AniParagraph::SetTypography(env, typographPtr);
    if (AniTextUtils::IsUndefined(env, typographObj)) {
        TEXT_LOGE("Failed to create typograph obj");
        delete typographPtr;
        typographPtr = nullptr;
    }
    return typographObj;
}
} // namespace OHOS::Text::ANI