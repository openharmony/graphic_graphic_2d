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
const std::string G_SYSTEM_FONT_DIR = "/system/fonts";

ani_status AniTypeface::AniInit(ani_env *env)
{
    ani_class cls = AniGlobalClass::GetInstance().typeface;
    if (cls == nullptr) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_TYPEFACE_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getFamilyName", ":C{std.core.String}",
            reinterpret_cast<void*>(GetFamilyName) },
        ani_native_function { "isBold", nullptr, reinterpret_cast<void*>(IsBold) },
        ani_native_function { "isItalic", nullptr, reinterpret_cast<void*>(IsItalic) },
        ani_native_function { "makeFromCurrent", nullptr, reinterpret_cast<void*>(MakeFromCurrent) },
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
        ani_native_function { "makeFromRawFileWithArguments", nullptr,
            reinterpret_cast<void*>(MakeFromRawFileWithArguments) },
    };

    ani_status ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
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
    if (ANI_OK != env->Object_SetField_Long(
        obj, AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniTypeface::Constructor failed create aniTypeface");
        delete aniTypeface;
        return;
    }
}

ani_string AniTypeface::GetFamilyName(ani_env* env, ani_object obj)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, obj, AniGlobalField::GetInstance().typefaceNativeObj);
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
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().typeface,
        AniGlobalMethod::GetInstance().typefaceCtor);
    if (ANI_OK != env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniTypeface))) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::CreateAniTypeface failed create aniTypeface");
        return CreateAniUndefined(env);
    }
    return aniObj;
}

bool IsTypefaceRegistered(const std::string& filePath, const std::shared_ptr<Typeface>& typeface)
{
    if (filePath.substr(0, G_SYSTEM_FONT_DIR.length()) == G_SYSTEM_FONT_DIR) {
        return true;
    }
    auto callback = Drawing::Typeface::GetTypefaceRegisterCallBack();
    if (callback == nullptr) {
        return true;
    }
    return callback(typeface);
}

ani_object AniTypeface::MakeFromFile(ani_env* env, ani_object obj, ani_string aniFilePath)
{
    std::string filePath = CreateStdString(env, aniFilePath);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(filePath.c_str());
    if (typeface == nullptr) {
        ROSEN_LOGE("AniTypeface::MakeFromFile create typeface failed!");
        return CreateAniUndefined(env);
    }
    if (!IsTypefaceRegistered(filePath, typeface)) {
        ROSEN_LOGE("AniTypeface::MakeFromFile MakeRegister Typeface failed!");
        return CreateAniUndefined(env);
    }
    return CreateAniTypeface(env, typeface);
}

ani_object AniTypeface::MakeFromFileWithArguments(ani_env* env, ani_object obj, ani_string aniFilePath,
    ani_object argumentsObj)
{
    std::string filePath = CreateStdString(env, aniFilePath);
    auto aniTypefaceArguments = GetNativeFromObj<AniTypefaceArguments>(env, argumentsObj,
        AniGlobalField::GetInstance().typefaceArgumentsNativeObj);
    if (aniTypefaceArguments == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }
    FontArguments fontArguments;
    AniTypefaceArguments::ConvertToFontArguments(aniTypefaceArguments->GetTypefaceArgumentsHelper(), fontArguments);
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(filePath.c_str(), fontArguments);
    if (typeface == nullptr) {
        ROSEN_LOGE("AniTypeface::MakeFromFileWithArguments create typeface failed!");
        return CreateAniUndefined(env);
    }
    if (!IsTypefaceRegistered(filePath, typeface)) {
        ROSEN_LOGE("AniTypeface::MakeFromFileWithArguments MakeRegister Typeface failed!");
        return CreateAniUndefined(env);
    }
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
    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().typeface,
        AniGlobalMethod::GetInstance().typefaceCtor);
    if (ANI_OK != env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniTypeface))) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::MakeFromRawFile failed create aniTypeface");
        return CreateAniUndefined(env);
    }
    return aniObj;
#else
    return CreateAniUndefined(env);
#endif
}

ani_object AniTypeface::MakeFromRawFileWithArguments(ani_env* env, ani_object obj, ani_object resource,
    ani_object argumentsObj)
{
#ifdef ROSEN_OHOS
    std::unique_ptr<uint8_t[]> rawFileArrayBuffer;
    size_t rawFileArrayBufferSize = 0;
    ResourceInfo resourceInfo;
    if (!AniTool::GetResourceInfo(env, resource, resourceInfo) ||
        !AniTool::GetResourceRawFileDataBuffer(std::move(rawFileArrayBuffer), rawFileArrayBufferSize, resourceInfo)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "AniTypeface::makeFromRawFileWithArguments get rawfilebuffer failed.");
        return CreateAniUndefined(env);
    }
    
    auto memory_stream = std::make_unique<MemoryStream> ((rawFileArrayBuffer.get()), rawFileArrayBufferSize, true);
    auto aniTypefaceArguments = GetNativeFromObj<AniTypefaceArguments>(env, argumentsObj,
        AniGlobalField::GetInstance().typefaceArgumentsNativeObj);
    if (aniTypefaceArguments == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }
    FontArguments fontArguments;
    AniTypefaceArguments::ConvertToFontArguments(aniTypefaceArguments->GetTypefaceArgumentsHelper(), fontArguments);

    auto rawTypeface = Typeface::MakeFromStream(std::move(memory_stream), fontArguments);
    if (rawTypeface == nullptr) {
        ROSEN_LOGE("AniTypeface::makeFromRawFileWithArguments Create rawTypeface failed.");
        return CreateAniUndefined(env);
    }

    AniTypeface* aniTypeface = new AniTypeface(rawTypeface);
    if (Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr) {
        bool ret = Drawing::Typeface::GetTypefaceRegisterCallBack()(rawTypeface);
        if (!ret) {
            delete aniTypeface;
            ROSEN_LOGE("AniTypeface::makeFromRawFileWithArguments MakeRegister Typeface failed!");
            return CreateAniUndefined(env);
        }
    }

    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().typeface,
        AniGlobalMethod::GetInstance().typefaceCtor);
    if (ANI_OK != env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniTypeface))) {
        delete aniTypeface;
        ROSEN_LOGE("AniTypeface::makeFromRawFileWithArguments failed create aniTypeface");
        return CreateAniUndefined(env);
    }
    return aniObj;
#else
    return CreateAniUndefined(env);
#endif
}

ani_object AniTypeface::MakeFromCurrent(ani_env* env, ani_object obj, ani_object argumentsObj)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, obj, AniGlobalField::GetInstance().typefaceNativeObj);
    if (aniTypeface == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Typeface> typeface = aniTypeface->GetTypeface();
    if (typeface == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }

    auto aniTypefaceArguments = GetNativeFromObj<AniTypefaceArguments>(env, argumentsObj,
        AniGlobalField::GetInstance().typefaceArgumentsNativeObj);
    if (aniTypefaceArguments == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params. ");
        return CreateAniUndefined(env);
    }
    FontArguments fontArguments;
    AniTypefaceArguments::ConvertToFontArguments(aniTypefaceArguments->GetTypefaceArgumentsHelper(), fontArguments);

    auto rawTypeface = typeface->MakeClone(fontArguments);
    if (rawTypeface == nullptr) {
        ROSEN_LOGE("AniTypeface::MakeFromCurrent create rawTypeface failed.");
        return nullptr;
    }

    AniTypeface* aniRawTypeface = new AniTypeface(rawTypeface);
    if (rawTypeface->IsCustomTypeface() && Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr &&
        !Drawing::Typeface::GetTypefaceRegisterCallBack()(rawTypeface)) {
        delete aniRawTypeface;
        ROSEN_LOGE("AniTypeface::MakeFromCurrent MakeRegister Typeface failed.");
        return nullptr;
    }

    ani_object aniObj = CreateAniObject(env, AniGlobalClass::GetInstance().typeface,
        AniGlobalMethod::GetInstance().typefaceCtor);
    if (ANI_OK != env->Object_SetField_Long(
        aniObj, AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniRawTypeface))) {
        delete aniRawTypeface;
        ROSEN_LOGE("AniTypeface::MakeFromCurrent failed create aniTypeface");
        return CreateAniUndefined(env);
    }
    return aniObj;
}

ani_object AniTypeface::TypefaceTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic failed to unwrap");
        return CreateAniUndefined(env);
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic unwrapResult is null");
        return CreateAniUndefined(env);
    }
    auto jsTypeface = reinterpret_cast<JsTypeface*>(unwrapResult);
    if (jsTypeface->GetTypeface() == nullptr) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic jsTypeface is null");
        return CreateAniUndefined(env);
    }

    auto aniTypeface = new AniTypeface(jsTypeface->GetTypeface());
    ani_object aniTypefaceObj = CreateAniObject(env, AniGlobalClass::GetInstance().typeface,
        AniGlobalMethod::GetInstance().typefaceCtor);
    if (ANI_OK != env->Object_SetField_Long(aniTypefaceObj,
        AniGlobalField::GetInstance().typefaceNativeObj, reinterpret_cast<ani_long>(aniTypeface))) {
        ROSEN_LOGE("AniTypeface::TypefaceTransferStatic failed create aniTypeface");
        delete aniTypeface;
        return CreateAniUndefined(env);
    }
    return aniTypefaceObj;
}

ani_boolean AniTypeface::IsBold(ani_env* env, ani_object obj)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, obj, AniGlobalField::GetInstance().typefaceNativeObj);
    if (aniTypeface == nullptr || aniTypeface->GetTypeface() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    return aniTypeface->GetTypeface()->GetBold();
}

ani_boolean AniTypeface::IsItalic(ani_env* env, ani_object obj)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, obj, AniGlobalField::GetInstance().typefaceNativeObj);
    if (aniTypeface == nullptr || aniTypeface->GetTypeface() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    return aniTypeface->GetTypeface()->GetItalic();
}

ani_long AniTypeface::GetTypefaceAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniTypeface = GetNativeFromObj<AniTypeface>(env, input, AniGlobalField::GetInstance().typefaceNativeObj);
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
