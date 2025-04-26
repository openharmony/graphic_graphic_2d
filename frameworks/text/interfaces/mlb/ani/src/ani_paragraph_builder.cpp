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

#include "ani.h"
#include "ani_common.h"
#include "ani_font_collection.h"
#include "ani_paragraph.h"
#include "ani_paragraph_builder.h"
#include "ani_text_utils.h"
#include "utils/text_log.h"
#include "font_collection.h"
#include "text_style.h"

namespace OHOS::Text::NAI {

AniParagraphBuilder::AniParagraphBuilder()
{
}

void AniParagraphBuilder::Constructor(ani_env* env, ani_object object, ani_object paragraphStyle,
                                      ani_object fontCollection)
{
    AniParagraphBuilder* aniParagraphBuilder = new AniParagraphBuilder();
    if (ANI_OK
        != env->Object_SetFieldByName_Long(object, NATIVE_OBJ, reinterpret_cast<ani_long>(aniParagraphBuilder))) {
        TEXT_LOGE("Failed to create ani ParagraphBuilder obj");
        return;
    }

    std::unique_ptr<TypographyStyle> typographyStyleNative = AniCommon::ParseParagraphStyle(env, paragraphStyle);
    if (typographyStyleNative == nullptr) {
        TEXT_LOGE("Failed to parse typographyStyle");
        return;
    }
    AniFontCollection* aniFontCollection = AniTextUtils::GetNativeFromObj<AniFontCollection>(env, fontCollection);
    if (aniFontCollection == nullptr || aniFontCollection->GetFontCollection() == nullptr) {
        TEXT_LOGE("FontCollection is null");
        return;
    }
    std::shared_ptr<FontCollection> fontCollectionNative = aniFontCollection->GetFontCollection();
    std::unique_ptr<TypographyCreate> typographyCreateNative =
        TypographyCreate::Create(*typographyStyleNative, fontCollectionNative);
    if (!typographyCreateNative) {
        TEXT_LOGE("Failed to create typography creator");
        return;
    }
    aniParagraphBuilder->typographyCreate_ = std::move(typographyCreateNative);
}

ani_status AniParagraphBuilder::AniInit(ani_vm* vm, uint32_t* result)
{
    ani_env* env;
    ani_status ret;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraphBuilder null env:%{public}d", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    ret = env->FindClass(ANI_CLASS_PARAGRAPH_BUILDER, &cls);
    if (ret != ANI_OK) {
        TEXT_LOGE("[ANI] AniParagraphBuilder can't find class:%{public}d", ret);
        return ANI_NOT_FOUND;
    }

    std::string ctorSignature = std::string(ANI_CLASS_PARAGRAPH_STYLE) + std::string(ANI_CLASS_FONT_COLLECTION) + ":V";
    std::string pushStyleSignature = std::string(ANI_CLASS_TEXT_STYLE) + ":V";
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
        TEXT_LOGE("[ANI] AniParagraphBuilder bind methods fail:%{public}d", ret);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

void AniParagraphBuilder::PushStyle(ani_env* env, ani_object object, ani_object textStyle)
{
    AniParagraphBuilder* aniParagraphBuilder = AniTextUtils::GetNativeFromObj<AniParagraphBuilder>(env, object);
    if (aniParagraphBuilder == nullptr || aniParagraphBuilder->typographyCreate_ == nullptr) {
        TEXT_LOGE("paragraphBuilder is null");
        return;
    }
}

void AniParagraphBuilder::PopStyle(ani_env* env, ani_object object)
{
    AniParagraphBuilder* aniParagraphBuilder = AniTextUtils::GetNativeFromObj<AniParagraphBuilder>(env, object);
    if (aniParagraphBuilder == nullptr || aniParagraphBuilder->typographyCreate_ == nullptr) {
        TEXT_LOGE("paragraphBuilder is null");
        return;
    }

    aniParagraphBuilder->typographyCreate_->PopStyle();
}

void AniParagraphBuilder::AddText(ani_env* env, ani_object object, ani_string text)
{
    std::u16string textStr = AniTextUtils::AniToStdStringUtf16(env, text);
    AniParagraphBuilder* aniParagraphBuilder = AniTextUtils::GetNativeFromObj<AniParagraphBuilder>(env, object);
    if (aniParagraphBuilder == nullptr || aniParagraphBuilder->typographyCreate_ == nullptr) {
        TEXT_LOGE("paragraphBuilder is null");
        return;
    }
    aniParagraphBuilder->typographyCreate_->AppendText(textStr);
}

ani_object AniParagraphBuilder::Build(ani_env* env, ani_object object)
{
    AniParagraphBuilder* aniParagraphBuilder = AniTextUtils::GetNativeFromObj<AniParagraphBuilder>(env, object);
    if (aniParagraphBuilder == nullptr || aniParagraphBuilder->typographyCreate_ == nullptr) {
        TEXT_LOGE("paragraphBuilder is null");
        return AniTextUtils::CreateAniUndefined(env);
    }
    std::unique_ptr<OHOS::Rosen::Typography> typography = aniParagraphBuilder->typographyCreate_->CreateTypography();
    ani_object pargraphObj = AniTextUtils::CreateAniObject(env, ANI_CLASS_PARAGRAPH, ":V");

    AniParagraph* aniParagraph = AniTextUtils::GetNativeFromObj<AniParagraph>(env, pargraphObj);
    if (aniParagraph != nullptr) {
        aniParagraph->setTypography(typography);
    }
    return pargraphObj;
}
} // namespace OHOS::Text::NAI