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

#include "ani_text_blob.h"
#include "font_ani/ani_font.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_TEXT_BLOB_NAME = "@ohos.graphics.drawing.drawing.TextBlob";
const char* ANI_CLASS_TEXT_BLOB_RUN_BUFFER_NAME = "@ohos.graphics.drawing.drawing.TextBlobRunBuffer";
static constexpr size_t CHAR16_SIZE = 2;

ani_status AniTextBlob::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_TEXT_BLOB_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_TEXT_BLOB_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "bounds", ":C{@ohos.graphics.common2D.common2D.Rect}",
            reinterpret_cast<void*>(Bounds) },
        ani_native_function { "uniqueID", ":l", reinterpret_cast<void*>(UniqueID) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_TEXT_BLOB_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "makeFromPosText", "C{std.core.String}iC{std.core.Array}"
            "C{@ohos.graphics.drawing.drawing.Font}:C{@ohos.graphics.drawing.drawing.TextBlob}",
            reinterpret_cast<void*>(MakeFromPosText) },
        ani_native_function { "makeFromString", "C{std.core.String}C{@ohos.graphics.drawing.drawing.Font}"
            "E{@ohos.graphics.drawing.drawing.TextEncoding}:C{@ohos.graphics.drawing.drawing.TextBlob}",
            reinterpret_cast<void*>(MakeFromString) },
        ani_native_function { "makeFromRunBuffer", "C{std.core.Array}C{@ohos.graphics.drawing.drawing.Font}"
            "C{@ohos.graphics.common2D.common2D.Rect}:C{@ohos.graphics.drawing.drawing.TextBlob}",
            reinterpret_cast<void*>(MakeFromRunBuffer) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_TEXT_BLOB_NAME);
        return ANI_NOT_FOUND;
    }
    return ANI_OK;
}

ani_object AniTextBlob::Bounds(ani_env* env, ani_object obj, ani_object aniRectObj)
{
    auto aniTextBlob = GetNativeFromObj<AniTextBlob>(env, obj);
    if (aniTextBlob == nullptr || aniTextBlob->GetTextBlob() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniTextBlob::Bounds textBlob is nullptr.");
        return CreateAniUndefined(env);
    }
    
    std::shared_ptr<Rect> rect = aniTextBlob->GetTextBlob()->Bounds();
    if (rect == nullptr) {
        ROSEN_LOGE("AniTextBlob::Bounds rect is nullptr");
        return CreateAniUndefined(env);
    }
    ani_object rectObj = nullptr;
    if (CreateRectObj(env, *rect, rectObj) != ANI_OK) {
        ROSEN_LOGE("AniTextBlob::Bounds CreateRectObj failed");
        return CreateAniUndefined(env);
    }
    return rectObj;
}

ani_long AniTextBlob::UniqueID(ani_env* env, ani_object obj)
{
    auto aniTextBlob = GetNativeFromObj<AniTextBlob>(env, obj);
    if (aniTextBlob == nullptr || aniTextBlob->GetTextBlob() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniTextBlob::UniqueID textBlob is nullptr.");
        return -1;
    }
    return aniTextBlob->GetTextBlob()->UniqueID();
}

std::shared_ptr<Font> AniTextBlob::GetValidFont(const std::shared_ptr<Font>& font)
{
    if (font == nullptr) {
        ROSEN_LOGE("GetValidFont: font is nullptr");
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = GetThemeFont(font);
    return themeFont ? themeFont : font;
}

ani_object AniTextBlob::MakeFromPosText(ani_env* env, ani_object obj, ani_string aniText, ani_int len,
    ani_object pointArray, ani_object aniFontObj)
{
    std::string textStr = CreateStdString(env, aniText);
    ani_int aniLength;
    if (ANI_OK != env->Object_GetPropertyByName_Int(pointArray, "length", &aniLength)) {
        ROSEN_LOGE("AniTextBlob::MakeFromPosText points is invalid");
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect Argv[2].");
        return CreateAniUndefined(env);
    }
    if (aniLength == 0 || textStr.size() == 0) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Argv[0] is empty.");
        return CreateAniUndefined(env);
    }
    if (len != aniLength) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM,
            "string length does not match points array length.");
        return CreateAniUndefined(env);
    }

    auto aniFont = GetNativeFromObj<AniFont>(env, aniFontObj);
    if (aniFont == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect Argv[3] type.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Font> font = GetValidFont(aniFont->GetFont());
    if (font == nullptr) {
        ROSEN_LOGE("AniTextBlob::MakeFromPosText font is null");
        return CreateAniUndefined(env);
    }

    uint32_t pointsSize = static_cast<uint32_t>(aniLength);
    std::unique_ptr<Point[]> points = std::make_unique<Point[]>(pointsSize);
    if (!MakePoints(env, points.get(), pointsSize, static_cast<ani_array>(pointArray))) {
        ROSEN_LOGE("AniTextBlob::MakeFromPosText: Argv[2] is invalid");
        return CreateAniUndefined(env);
    }
    ani_object aniObj = GetTextBlobObj(env, textStr.c_str(), textStr.size(), points.get(), font);
    return aniObj;
}

bool AniTextBlob::MakePoints(ani_env* env, Point* points, uint32_t size, ani_array array)
{
    for (uint32_t i = 0; i < size; i++) {
        Drawing::Point point;
        ani_ref pointRef;
        if (ANI_OK != env->Array_Get(array, (ani_int)i, &pointRef)) {
            return false;
        }
        if (GetPointFromPointObj(env, static_cast<ani_object>(pointRef), point) != ANI_OK) {
            return false;
        }
        points[i] = point;
    }
    return true;
}

ani_object AniTextBlob::MakeFromString(ani_env* env, ani_object obj, ani_string aniText, ani_object aniFontObj,
    ani_enum_item aniEncoding)
{
    auto aniFont = GetNativeFromObj<AniFont>(env, aniFontObj);
    if (aniFont == nullptr || aniFont->GetFont() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "AniTextBlob::MakeFromString font is nullptr.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Font> font = GetValidFont(aniFont->GetFont());
    if (font == nullptr) {
        ROSEN_LOGE("AniTextBlob::MakeFromString font is nullptr");
        return CreateAniUndefined(env);
    }

    std::u16string textStr;
    ani_status status = CreateStdStringUtf16(env, aniText, textStr);
    if (status != ANI_OK) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Fail get utf16.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<TextBlob> textBlob =
        TextBlob::MakeFromText(textStr.c_str(), CHAR16_SIZE * textStr.size(), *font, TextEncoding::UTF16);
    if (textBlob == nullptr) {
        ROSEN_LOGE("AniTextBlob::MakeFromString textBlob is nullptr");
        return CreateAniUndefined(env);
    }
    return CreateTextBlobObj(env, textBlob);
}

ani_object AniTextBlob::MakeFromRunBuffer(ani_env* env, ani_object obj, ani_object posArray, ani_object aniFontObj,
    ani_object aniRectObj)
{
    ani_int aniLength;
    if (ANI_OK != env->Object_GetPropertyByName_Int(posArray, "length", &aniLength)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
        return CreateAniUndefined(env);
    }
    if (aniLength == 0) {
        ROSEN_LOGE("AniTextBlob::MakeFromRunBuffer failed. param is invalid");
        return CreateAniUndefined(env);
    }
    uint32_t size = static_cast<uint32_t>(aniLength);
    auto aniFont = GetNativeFromObj<AniFont>(env, aniFontObj);
    if (aniFont == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter1 type.");
        return CreateAniUndefined(env);
    }

    std::shared_ptr<Font> font = GetValidFont(aniFont->GetFont());
    if (font == nullptr) {
        ROSEN_LOGE("AniTextBlob::MakeFromRunBuffer font is null");
        return CreateAniUndefined(env);
    }
    TextBlobBuilder::RunBuffer runBuffer;
    std::shared_ptr<TextBlobBuilder> textBlobBuilder = std::make_shared<TextBlobBuilder>();
    ani_boolean isRectUndefined = ANI_TRUE;
    env->Reference_IsUndefined(aniRectObj, &isRectUndefined);
    if (isRectUndefined) {
        runBuffer = textBlobBuilder->AllocRunPos(*font, size);
    } else {
        ani_boolean isRectNullptr = ANI_TRUE;
        env->Reference_IsNull(aniRectObj, &isRectNullptr);
        Drawing::Rect drawingRect;
        if (!isRectNullptr && !GetRectFromAniRectObj(env, aniRectObj, drawingRect)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter2 type.");
            return CreateAniUndefined(env);
        }
        runBuffer = textBlobBuilder->AllocRunPos(*font, size, isRectNullptr ? nullptr : &drawingRect);
    }

    if (!MakeRunBuffer(env, runBuffer, size, posArray)) {
        ROSEN_LOGE("AniTextBlob::MakeFromRunBuffer Argv[0] is invalid");
        return CreateAniUndefined(env);
    }
    std::shared_ptr<TextBlob> textBlob = textBlobBuilder->Make();
    if (textBlob == nullptr) {
        ROSEN_LOGE("AniTextBlob::MakeFromRunBuffer textBlob is invalid");
        return CreateAniUndefined(env);
    }
    return CreateTextBlobObj(env, textBlob);
}

ani_object AniTextBlob::GetTextBlobObj(ani_env* env, const char* buffer, size_t bufferLen, const Point points[],
    const std::shared_ptr<Font>& font)
{
    std::shared_ptr<TextBlob> textBlob =
        TextBlob::MakeFromPosText(buffer, bufferLen, points, *font, TextEncoding::UTF8);
    if (textBlob == nullptr) {
        ROSEN_LOGE("GetTextBlobObj: textBlob is nullptr");
        return CreateAniUndefined(env);
    }
    return CreateTextBlobObj(env, textBlob);
}

ani_object AniTextBlob::CreateTextBlobObj(ani_env* env, const std::shared_ptr<TextBlob> textBlob)
{
    AniTextBlob* aniTextBlob = new AniTextBlob(textBlob);
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_TEXT_BLOB_NAME, aniTextBlob);
    ani_boolean isUndefined;
    env->Reference_IsUndefined(aniObj, &isUndefined);
    if (isUndefined) {
        delete aniTextBlob;
        ROSEN_LOGE("AniTextBlob::CreateTextBlobObj failed cause aniObj is undefined");
    }
    return aniObj;
}

bool AniTextBlob::MakeRunBuffer(ani_env* env, TextBlobBuilder::RunBuffer& runBuffer, uint32_t size, ani_object posArray)
{
    if (size > MAX_ELEMENTSIZE) {
        ROSEN_LOGE("AniTextBlob::MakeRunBuffer failed. size exceeds the upper limit.");
        return false;
    }
    ani_class runBufferClass;
    if (ANI_OK != env->FindClass(ANI_CLASS_TEXT_BLOB_RUN_BUFFER_NAME, &runBufferClass)) {
        return false;
    }
    for (uint32_t i = 0; i < size; i++) {
        Drawing::Point point;
        ani_ref runBufferRef;
        if (ANI_OK != env->Object_CallMethodByName_Ref(
            posArray, "$_get", "i:Y", &runBufferRef, (ani_int)i)) {
            ROSEN_LOGE("AniTextBlob::MakeRunBuffer Object_CallMethodByName_Ref failed");
            return false;
        }
        ani_object aniRunBuffer =  static_cast<ani_object>(runBufferRef);
        ani_boolean isRunBufferClass;
        env->Object_InstanceOf(aniRunBuffer, runBufferClass, &isRunBufferClass);
        if (!isRunBufferClass) {
            return false;
        }

        ani_int glyph;
        ani_double positionX;
        ani_double positionY;
        if ((env->Object_GetPropertyByName_Int(aniRunBuffer, "glyph", &glyph) != ANI_OK) ||
            (env->Object_GetPropertyByName_Double(aniRunBuffer, "positionX", &positionX) != ANI_OK) ||
            (env->Object_GetPropertyByName_Double(aniRunBuffer, "positionY", &positionY) != ANI_OK)) {
            ROSEN_LOGE("AniTextBlob::MakeRunBuffer Object_GetPropertyByName failed");
            return false;
        }
        
        runBuffer.glyphs[i] = (uint16_t)glyph;
        runBuffer.pos[2 * i] = positionX; // 2: double
        runBuffer.pos[2 * i + 1] = positionY; // 2: double
    }
    return true;
}

std::shared_ptr<TextBlob> AniTextBlob::GetTextBlob()
{
    return textBlob_;
}
} // namespace Drawing
} // namespace OHOS::Rosen