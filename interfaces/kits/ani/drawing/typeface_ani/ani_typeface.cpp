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

#include "ani_typeface.h"
#include "typeface_arguments_ani/ani_typeface_arguments.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/font_napi/js_typeface.h"
#include "tool_ani/ani_tool.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_TYPEFACE_NAME = "@ohos.graphics.drawing.drawing.Typeface";

ani_status AniTypeface::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_TYPEFACE_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_TYPEFACE_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getFamilyName", ":C{std.core.String}",
            reinterpret_cast<void*>(GetFamilyName) },
    };

    std::array statitMethods = {
        ani_native_function { "makeFromFile", "C{std.core.String}:C{@ohos.graphics.drawing.drawing.Typeface}",
            reinterpret_cast<void*>(MakeFromFile) },
        ani_native_function { "makeFromFileWithArguments", "C{std.core.String}"
            "C{@ohos.graphics.drawing.drawing.TypefaceArguments}:C{@ohos.graphics.drawing.drawing.Typeface}",
            reinterpret_cast<void*>(MakeFromFileWithArguments) },
        ani_native_function { "makeFromRawFile", nullptr,
            reinterpret_cast<void*>(MakeFromRawFile) },
        ani_native_function { "typefaceTransferStaticNative", nullptr,
            reinterpret_cast<void*>(TypefaceTransferStatic) },
        ani_native_function { "getTypefaceAddr", nullptr, reinterpret_cast<void*>(GetTypefaceAddr) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s ret: %{public}d", ANI_CLASS_TYPEFACE_NAME, ret);
        return ANI_NOT_FOUND;
    }

    ret = env->Class_BindStaticNativeMethods(cls, statitMethods.data(), statitMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s ret: %{public}d", ANI_CLASS_TYPEFACE_NAME, ret);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniTypeface::Constructor(ani_env* env, ani_object obj)
{
    AniTypeface* aniTypeface = new AniTypeface(GetZhCnTypeface());
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniTypeface::Constructor failed create aniTypeface");
        delete aniTypeface;
        return;
    }
}

ani_string AniTypeface::GetFamilyName(ani_env* env, ani_object obj)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, obj);
    if (aniTypeface == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return ani_string{};
    }

    std::shared_ptr<Typeface> typeface = aniTypeface->GetTypeface();
    if (typeface == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return ani_string{};
    }
    return CreateAniString(env, typeface->GetFamilyName());
}

ani_object AniTypeface::CreateAniTypeface(ani_env* env, std::shared_ptr<Typeface> typeface)
{
    AniTypeface* aniTypeface = new AniTypeface(typeface);
    ani_object aniObj = CreateAniObject(env, ANI_CLASS_TYPEFACE_NAME, nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniObj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypeface))) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::CreateAniTypeface failed create aniTypeface");
        return CreateAniUndefined(env);
    }
    return aniObj;
}

ani_object AniTypeface::MakeFromFile(ani_env* env, ani_object obj, ani_string aniFilePath)
{
    std::string filePath = CreateStdString(env, aniFilePath);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(filePath.c_str());
    if (typeface == nullptr) {
        ROSEN_LOGE("AniTypeface::MakeFromFile create typeface failed!");
        return nullptr;
    }
    return CreateAniTypeface(env, typeface);
}

ani_object AniTypeface::MakeFromFileWithArguments(ani_env* env, ani_object obj, ani_string aniFilePath,
    ani_object argumentsObj)
{
    std::string filePath = CreateStdString(env, aniFilePath);
    auto aniTypefaceArguments = GetNativeFromObj<AniTypefaceArguments>(env, argumentsObj);
    if (aniTypefaceArguments == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }
    FontArguments fontArguments;
    AniTypefaceArguments::ConvertToFontArguments(aniTypefaceArguments->GetTypefaceArgumentsHelper(), fontArguments);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(filePath.c_str(), fontArguments);
    return CreateAniTypeface(env, typeface);
}

std::shared_ptr<Typeface> LoadZhCnTypeface()
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(AniTypeface::ZH_CN_TTF);
    if (typeface == nullptr) {
        typeface = Typeface::MakeDefault();
    }
    return typeface;
}

std::shared_ptr<Typeface> AniTypeface::GetZhCnTypeface()
{
    static std::shared_ptr<Typeface> zhCnTypeface = LoadZhCnTypeface();
    return zhCnTypeface;
}

ani_object AniTypeface::MakeFromRawFile(ani_env* env, ani_object obj, ani_object aniResourceObj)
{
#ifdef ROSEN_OHOS
    std::unique_ptr<uint8_t[]> rawFileArrayBuffer;
    size_t rawFileArrayBufferSize = 0;

    ResourceInfo resourceInfo;
    if (!AniTool::GetResourceInfo(env, aniResourceObj, resourceInfo) ||
        !AniTool::GetResourceRawFileDataBuffer(std::move(rawFileArrayBuffer), rawFileArrayBufferSize, resourceInfo)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniTypeface::MakeFromRawFile get rawfilebuffer failed.");
        return CreateAniUndefined(env);
    }
    auto memoryStream = std::make_unique<MemoryStream>((rawFileArrayBuffer.get()), rawFileArrayBufferSize, true);
    auto rawTypeface = Typeface::MakeFromStream(std::move(memoryStream));
    if (rawTypeface == nullptr) {
        ROSEN_LOGE("AniTypeface::MakeFromRawFile Create rawTypeface failed!");
        return CreateAniUndefined(env);
    }
    AniTypeface* aniTypeface = new AniTypeface(rawTypeface);
    if (Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr) {
        bool ret = Drawing::Typeface::GetTypefaceRegisterCallBack()(rawTypeface);
        if (!ret) {
            delete aniTypeface;
            ROSEN_LOGE("AniTypeface::MakeFromRawFile MakeRegister Typeface failed!");
            return CreateAniUndefined(env);
        }
    }
    ani_object aniObj = CreateAniObject(env, ANI_CLASS_TYPEFACE_NAME, nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniObj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypeface))) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::MakeFromRawFile failed create aniTypeface");
        return CreateAniUndefined(env);
    }
    return aniObj;
#else
    return CreateAniUndefined(env);
#endif
}

ani_object AniTypeface::TypefaceTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsTypeface = reinterpret_cast<JsTypeface*>(unwrapResult);
    if (jsTypeface->GetTypeface() == nullptr) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic jsTypeface is null");
        return nullptr;
    }

    auto aniTypeface = new AniTypeface(jsTypeface->GetTypeface());
    ani_object aniTypefaceObj = CreateAniObject(env, ANI_CLASS_TYPEFACE_NAME, nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniTypefaceObj,
        NATIVE_OBJ, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic failed create aniTypeface");
        delete aniTypeface;
        return nullptr;
    }
    return aniTypefaceObj;
}

ani_long AniTypeface::GetTypefaceAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, input);
    if (aniTypeface == nullptr || aniTypeface->GetTypeface() == nullptr) {
        ROSEN_LOGE("AniTypeface::GetTypefaceAddr aniTypeface is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniTypeface->GetTypefacePtrAddr());
}

std::shared_ptr<Typeface>* AniTypeface::GetTypefacePtrAddr()
{
    return &typeface_;
}

std::shared_ptr<Typeface> AniTypeface::GetTypeface()
{
    return typeface_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
