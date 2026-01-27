/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_drawing_utils.h"
#include "typeface_ani/ani_typeface.h"
#include "rosen_text/font_collection.h"
#include "txt/platform.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

ani_status AniThrowError(ani_env* env, const std::string& message)
{
    ani_class errCls;
    const char* className = "escompat.Error";
    if (ANI_OK != env->FindClass(className, &errCls)) {
        ROSEN_LOGE("Not found %{public}s", className);
        return ANI_ERROR;
    }

    ani_method errCtor;
    if (ANI_OK != env->Class_FindMethod(errCls, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:", &errCtor)) {
        ROSEN_LOGE("get errCtor Failed %{public}s", className);
        return ANI_ERROR;
    }

    ani_object errObj;
    if (ANI_OK != env->Object_New(errCls, errCtor, &errObj, CreateAniString(env, message), CreateAniUndefined(env))) {
        ROSEN_LOGE("Create Object Failed %{public}s", className);
        return ANI_ERROR;
    }

    env->ThrowError(static_cast<ani_error>(errObj));
    return ANI_OK;
}

ani_status ThrowBusinessError(ani_env* env, DrawingErrorCode errorCode, const char* message)
{
    ani_object aniError;
    ani_status status = CreateBusinessError(env, static_cast<int32_t>(errorCode), message, aniError);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to create business, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    status = env->ThrowError(static_cast<ani_error>(aniError));
    if (status != ANI_OK) {
        ROSEN_LOGE("Fail to throw err, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    return ANI_OK;
}

ani_status CreateStdStringUtf16(ani_env* env, const ani_string& str, std::u16string& utf16Str)
{
    ani_size strSize;
    ani_status status = env->String_GetUTF16Size(str, &strSize);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to get utf16 str size, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }

    strSize++;
    std::vector<uint16_t> buffer(strSize);
    uint16_t* utf16Buffer = buffer.data();

    ani_size bytesWritten = 0;
    status = env->String_GetUTF16(str, utf16Buffer, strSize, &bytesWritten);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to get utf16 str, status: %{public}d", static_cast<int>(status));
        return status;
    }
    utf16Buffer[bytesWritten] = '\0';
    utf16Str = std::u16string(reinterpret_cast<const char16_t*>(utf16Buffer), strSize - 1);
    return ANI_OK;
}

ani_status CreateBusinessError(ani_env* env, int32_t error, const char* message, ani_object& err)
{
    ani_class aniClass;
    ani_status status = env->FindClass("@ohos.base.BusinessError", &aniClass);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find class, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    ani_method aniCtor;
    status = env->Class_FindMethod(aniClass, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:", &aniCtor);
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to find ctor, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    ani_string aniMsg = CreateAniString(env, message);
    status = env->Object_New(aniClass, aniCtor, &err, aniMsg, CreateAniUndefined(env));
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to new err, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    status = env->Object_SetPropertyByName_Int(err, "code", static_cast<ani_int>(error));
    if (status != ANI_OK) {
        ROSEN_LOGE("Failed to set code, status:%{public}d", static_cast<int32_t>(status));
        return status;
    }
    return ANI_OK;
}

bool GetColorQuadFromColorObj(ani_env* env, ani_object obj, Drawing::ColorQuad &color)
{
    ani_class colorCls = AniGlobalClass::GetInstance().colorInterface;
    if (colorCls == nullptr) {
        ROSEN_LOGE("GetColorQuadFromColorObj failed by cls is null");
        return false;
    }

    ani_method colorGetAlpha = AniGlobalMethod::GetInstance().colorGetAlpha;
    ani_method colorGetRed = AniGlobalMethod::GetInstance().colorGetRed;
    ani_method colorGetGreen = AniGlobalMethod::GetInstance().colorGetGreen;
    ani_method colorGetBlue = AniGlobalMethod::GetInstance().colorGetBlue;
    if (colorGetAlpha == nullptr || colorGetRed == nullptr || colorGetGreen == nullptr || colorGetBlue == nullptr) {
        ROSEN_LOGE("GetColorQuadFromColorObj failed by cls method is null");
        return false;
    }

    ani_boolean isColorClass;
    env->Object_InstanceOf(obj, colorCls, &isColorClass);

    if (!isColorClass) {
        return false;
    }
    
    ani_int alpha;
    ani_int red;
    ani_int green;
    ani_int blue;

    if ((env->Object_CallMethod_Int(obj, colorGetAlpha, &alpha) != ANI_OK) ||
        (env->Object_CallMethod_Int(obj, colorGetRed, &red) != ANI_OK) ||
        (env->Object_CallMethod_Int(obj, colorGetGreen, &green) != ANI_OK) ||
        (env->Object_CallMethod_Int(obj, colorGetBlue, &blue) != ANI_OK)) {
        ROSEN_LOGE("GetColorQuadFromColorObj failed by Color class");
        return false;
    }

    if (alpha < 0 || alpha > Color::RGB_MAX ||
        red < 0 || red > Color::RGB_MAX ||
        green < 0 || green > Color::RGB_MAX ||
        blue < 0 || blue > Color::RGB_MAX) {
        ROSEN_LOGE("GetColorQuadFromColorObj failed by Color class invaild value");
        return false;
    }

    color = Color::ColorQuadSetARGB(static_cast<uint32_t>(alpha), static_cast<uint32_t>(red),
        static_cast<uint32_t>(green), static_cast<uint32_t>(blue));
    return true;
}

bool GetColorQuadFromParam(ani_env* env, ani_object obj, Drawing::ColorQuad &color)
{
    ani_class intClass = AniGlobalClass::GetInstance().intCls;
    if (intClass == nullptr) {
        ROSEN_LOGE("GetColorQuadFromParam Failed to find class %{public}s", ANI_INT_STRING);
        return false;
    }
    ani_boolean isInt;
    env->Object_InstanceOf(obj, intClass, &isInt);
    if (isInt) {
        ani_int aniColor;
        if (ANI_OK != env->Object_CallMethod_Int(obj, AniGlobalMethod::GetInstance().intGet, &aniColor)) {
            ROSEN_LOGE("GetColorQuadFromParam failed by int value");
            return false;
        }
        color = static_cast<ColorQuad>(aniColor);
        return true;
    }

    return GetColorQuadFromColorObj(env, obj, color);
}

ani_status CreateColorObj(ani_env* env, const Drawing::Color& color, ani_object& obj)
{
    obj = CreateAniObject(env, AniGlobalClass::GetInstance().color, AniGlobalMethod::GetInstance().colorCtor,
        ani_int(color.GetAlpha()),
        ani_int(color.GetRed()),
        ani_int(color.GetGreen()),
        ani_int(color.GetBlue())
    );
    return ANI_OK;
}

bool GetRectFromAniRectObj(ani_env* env, ani_object obj, Drawing::Rect& rect)
{
    ani_class rectCls = AniGlobalClass::GetInstance().rectInterface;
    if (rectCls == nullptr) {
        ROSEN_LOGE("GetRectFromAniRectObj failed by cls is null");
        return false;
    }
    ani_method rectGetLeft = AniGlobalMethod::GetInstance().rectGetLeft;
    ani_method rectGetTop = AniGlobalMethod::GetInstance().rectGetTop;
    ani_method rectGetRight = AniGlobalMethod::GetInstance().rectGetRight;
    ani_method rectGetBottom = AniGlobalMethod::GetInstance().rectGetBottom;
    if (rectGetLeft == nullptr || rectGetTop == nullptr || rectGetRight == nullptr || rectGetBottom == nullptr) {
        ROSEN_LOGE("GetRectFromAniRectObj failed by cls method is null");
        return false;
    }

    ani_boolean isRectClass;
    env->Object_InstanceOf(obj, rectCls, &isRectClass);
    if (!isRectClass) {
        return false;
    }

    ani_double left = 0;
    ani_double top = 0;
    ani_double right = 0;
    ani_double bottom = 0;
    if ((env->Object_CallMethod_Double(obj, AniGlobalMethod::GetInstance().rectGetLeft, &left) != ANI_OK) ||
        (env->Object_CallMethod_Double(obj, AniGlobalMethod::GetInstance().rectGetTop, &top) != ANI_OK) ||
        (env->Object_CallMethod_Double(obj, AniGlobalMethod::GetInstance().rectGetRight, &right) != ANI_OK) ||
        (env->Object_CallMethod_Double(obj, AniGlobalMethod::GetInstance().rectGetBottom, &bottom) != ANI_OK)) {
        ROSEN_LOGE("GetRectFromAniRectObj failed");
        return false;
    }

    rect.SetLeft(left);
    rect.SetTop(top);
    rect.SetRight(right);
    rect.SetBottom(bottom);
    return true;
}

ani_status CreateRectObj(ani_env* env, const Drawing::Rect& rect, ani_object& obj)
{
    obj = CreateAniObject(env, AniGlobalClass::GetInstance().rect, AniGlobalMethod::GetInstance().rectCtor,
        ani_double(rect.left_),
        ani_double(rect.top_),
        ani_double(rect.right_),
        ani_double(rect.bottom_)
    );
    return ANI_OK;
}

ani_status GetPointFromPointObj(ani_env* env, ani_object obj, Drawing::Point& point)
{
    ani_class pointCls  = AniGlobalClass::GetInstance().pointInterface;
    if (pointCls == nullptr) {
        ROSEN_LOGE("GetPointFromPointObj Failed to find class %{public}s", ANI_INTERFACE_POINT_NAME);
        return ANI_NOT_FOUND;
    }
    ani_boolean isPoint;
    env->Object_InstanceOf(obj, pointCls, &isPoint);
    if (!isPoint) {
        return ANI_ERROR;
    }

    ani_method pointGetX = AniGlobalMethod::GetInstance().pointGetX;
    ani_method pointGetY = AniGlobalMethod::GetInstance().pointGetY;
    if (pointGetX == nullptr || pointGetY == nullptr) {
        ROSEN_LOGE("GetPointFromPointObj failed by cls method is null");
        return ANI_ERROR;
    }

    ani_double x = 0;
    ani_status ret = env->Object_CallMethod_Double(obj, pointGetX, &x);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Param x is invalid, ret %{public}d", ret);
        return ret;
    }

    ani_double y = 0;
    ret = env->Object_CallMethod_Double(obj, pointGetY, &y);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Param y is invalid, ret %{public}d", ret);
        return ret;
    }
    point.SetX(x);
    point.SetY(y);
    return ret;
}

ani_status CreatePointObj(ani_env* env, const Drawing::Point& point, ani_object& obj)
{
    obj = CreateAniObject(env, AniGlobalClass::GetInstance().point, AniGlobalMethod::GetInstance().pointCtor,
        ani_double(point.GetX()),
        ani_double(point.GetY())
    );
    return ANI_OK;
}

bool CreatePointObjAndCheck(ani_env* env, const Drawing::Point& point, ani_object& obj)
{
    obj = CreateAniObject(env, AniGlobalClass::GetInstance().point, AniGlobalMethod::GetInstance().pointCtor,
        ani_double(point.GetX()),
        ani_double(point.GetY())
    );
    ani_boolean isUndefined;
    env->Reference_IsUndefined(obj, &isUndefined);
    return !isUndefined;
}

bool ConvertFromAniPointsArray(ani_env* env, ani_array aniPointArray, Drawing::Point* points, uint32_t pointSize)
{
    for (uint32_t i = 0; i < pointSize; i++) {
        ani_ref pointRef;
        Drawing::Point point;
        if (ANI_OK != env->Array_Get(aniPointArray, static_cast<ani_size>(i), &pointRef)) {
            ROSEN_LOGE("aniPointArray get pointRef failed.");
            return false;
        }
        if (GetPointFromPointObj(env, static_cast<ani_object>(pointRef), point) != ANI_OK) {
            ROSEN_LOGE("pointRef is invalid");
            return false;
        }
        points[i] = point;
    }
    return true;
}

bool GetPoint3FromPoint3dObj(ani_env* env, ani_object obj, Drawing::Point3& point3d)
{
    ani_class point3dClass = AniGlobalClass::GetInstance().point3dInterface;
    if (point3dClass == nullptr) {
        ROSEN_LOGE("[ANI] can't find class %{public}s", ANI_INTERFACE_POINT3D_NAME);
        return false;
    }
    ani_boolean isPoint3dClass;
    env->Object_InstanceOf(obj, point3dClass, &isPoint3dClass);
    if (!isPoint3dClass) {
        ROSEN_LOGE("Get point3d object failed");
        return false;
    }
    ani_method point3dGetX = AniGlobalMethod::GetInstance().point3dGetX;
    ani_method point3dGetY = AniGlobalMethod::GetInstance().point3dGetY;
    ani_method point3dGetZ = AniGlobalMethod::GetInstance().point3dGetZ;
    if (point3dGetX == nullptr || point3dGetY == nullptr || point3dGetZ == nullptr) {
        ROSEN_LOGE("GetPoint3FromPoint3dObj failed by cls method is null");
        return false;
    }

    ani_double x = 0;
    ani_status ret = env->Object_CallMethod_Double(obj, point3dGetX, &x);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Param x is invalid, ret %{public}d", ret);
        return false;
    }

    ani_double y = 0;
    ret = env->Object_CallMethod_Double(obj, point3dGetY, &y);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Param y is invalid, ret %{public}d", ret);
        return false;
    }

    ani_double z = 0;
    ret = env->Object_CallMethod_Double(obj, point3dGetZ, &z);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Param z is invalid, ret %{public}d", ret);
        return false;
    }

    point3d = Point3(x, y, z);
    return true;
}

ani_object CreateAniUndefined(ani_env* env)
{
    ani_ref aniRef;
    env->GetUndefined(&aniRef);
    return static_cast<ani_object>(aniRef);
}

ani_object CreateAniObject(ani_env* env, const ani_class cls, const ani_method ctor, ...)
{
    if (cls == nullptr || ctor == nullptr) {
        ROSEN_LOGE("[Drawing] CreateAniObject cls or ctor is null");
        return CreateAniUndefined(env);
    }
    ani_object aniObject;
    va_list args;
    va_start(args, ctor);
    if (env->Object_New_V(cls, ctor, &aniObject, args) != ANI_OK) {
        ROSEN_LOGE("[Drawing] CreateAniObject Object_New failed");
        va_end(args);
        return CreateAniUndefined(env);
    }
    va_end(args);

    return aniObject;
}

ani_object CreateAniNull(ani_env* env)
{
    ani_ref aniRef;
    env->GetNull(&aniRef);
    return static_cast<ani_object>(aniRef);
}

bool CreateAniEnumByEnumIndex(ani_env* env, const ani_enum enumType, ani_size index, ani_enum_item& enumItem)
{
    if (enumType == nullptr) {
        ROSEN_LOGE("CreateAniEnumByEnumIndex enumType is null.");
        return false;
    }
    ani_status ret = env->Enum_GetEnumItemByIndex(enumType, index, &enumItem);
    if (ret != ANI_OK) {
        ROSEN_LOGE("CreateAniEnumByEnumIndex get enum by index failed. ret: %{public}d.", ret);
        return false;
    }

    return true;
}

bool CreateAniEnumByEnumName(ani_env* env, const ani_enum enumType, const std::string itemName, ani_enum_item& enumItem)
{
    if (enumType == nullptr) {
        ROSEN_LOGE("CreateAniEnumByEnumName enumType is null.");
        return false;
    }
    ani_status ret = env->Enum_GetEnumItemByName(enumType, itemName.c_str(), &enumItem);
    if (ret != ANI_OK) {
        ROSEN_LOGE("CreateAniEnumByEnumName get enum by name failed. ret: %{public}d.", ret);
        return false;
    }

    return true;
}

bool SetPointToAniPointArrayWithIndex(ani_env* env, Drawing::Point& point, ani_array& pointArray, uint32_t index)
{
    ani_object pointObj;
    if (!CreatePointObjAndCheck(env, point, pointObj)) {
        ROSEN_LOGE("AniPathIterator::Next Set pointObj from point failed.");
        return false;
    }
    ani_status ret = env->Array_Set(pointArray, static_cast<ani_size>(index), pointObj);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniPathIterator::Next Array_Set Faild. ret: %{public}d", ret);
        return false;
    }
    return true;
}

bool DrawingPointConvertToAniPoint(ani_env* env, ani_object obj, const Drawing::Point& point)
{
    ani_method pointSetX = AniGlobalMethod::GetInstance().pointSetX;
    ani_method pointSetY = AniGlobalMethod::GetInstance().pointSetY;
    if (pointSetX == nullptr || pointSetY == nullptr) {
        ROSEN_LOGE("DrawingPointConvertToAniPoint failed by method is null");
        return false;
    }
    if (env->Object_CallMethod_Void(obj, pointSetX, point.GetX()) != ANI_OK ||
        env->Object_CallMethod_Void(obj, pointSetY, point.GetY()) != ANI_OK) {
        ROSEN_LOGE("DrawingPointConvertToAniPoint failed by Object_SetProperty_Double");
        return false;
    }
    return true;
}

bool DrawingRectConvertToAniRect(ani_env* env, ani_object obj, const Drawing::Rect& rect)
{
    ani_method rectSetLeft = AniGlobalMethod::GetInstance().rectSetLeft;
    ani_method rectSetTop = AniGlobalMethod::GetInstance().rectSetTop;
    ani_method rectSetRight = AniGlobalMethod::GetInstance().rectSetRight;
    ani_method rectSetBottom = AniGlobalMethod::GetInstance().rectSetBottom;
    if (rectSetLeft == nullptr || rectSetTop == nullptr || rectSetRight == nullptr || rectSetBottom == nullptr) {
        ROSEN_LOGE("DrawingRectConvertToAniRect failed by method is null");
        return false;
    }
    if (env->Object_CallMethod_Void(obj, rectSetLeft, rect.GetLeft()) != ANI_OK ||
        env->Object_CallMethod_Void(obj, rectSetTop, rect.GetTop()) != ANI_OK ||
        env->Object_CallMethod_Void(obj, rectSetRight, rect.GetRight()) != ANI_OK ||
        env->Object_CallMethod_Void(obj, rectSetBottom, rect.GetBottom()) != ANI_OK) {
        ROSEN_LOGE("DrawingRectConvertToAniRect failed by Object_SetProperty_Double");
        return false;
    }
    return true;
}

std::shared_ptr<Font> GetThemeFont(std::shared_ptr<Font> font)
{
    std::shared_ptr<FontMgr> fontMgr = GetFontMgr(font);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Typeface> themeTypeface =
        std::shared_ptr<Typeface>(fontMgr->MatchFamilyStyle(SPText::OHOS_THEME_FONT, FontStyle()));
    if (themeTypeface == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Font> themeFont = std::make_shared<Font>(*font);
    themeFont->SetTypeface(themeTypeface);
    return themeFont;
}

std::shared_ptr<Font> MatchThemeFont(std::shared_ptr<Font> font, int32_t unicode)
{
    std::shared_ptr<FontMgr> fontMgr = GetFontMgr(font);
    if (fontMgr == nullptr) {
        return nullptr;
    }
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    std::shared_ptr<Drawing::Font> themeFont = std::make_shared<Drawing::Font>(*font);
    for (const auto& family : themeFamilies) {
        std::shared_ptr<Drawing::Typeface> themeTypeface =
            std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(family.c_str(), FontStyle()));
        themeFont->SetTypeface(themeTypeface);
        if (themeFont->UnicharToGlyph(unicode)) {
            return themeFont;
        }
    }
    return nullptr;
}

std::shared_ptr<FontMgr> GetFontMgr(std::shared_ptr<Font> font)
{
    if (!font->IsThemeFontFollowed() || font->GetTypeface() != AniTypeface::GetZhCnTypeface()) {
        return nullptr;
    }

    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    if (fontCollection == nullptr) {
        return nullptr;
    }
    std::shared_ptr<FontMgr> fontMgr = fontCollection->GetFontMgr();
    if (fontMgr == nullptr) {
        return nullptr;
    }
    return fontMgr;
}

ani_array CreateAniArrayWithSize(ani_env* env, size_t size)
{
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        ROSEN_LOGE("GetUndefined Failed.");
        return nullptr;
    }
    ani_array resultArray;
    if (ANI_OK != env->Array_New(size, undefinedRef, &resultArray)) {
        ROSEN_LOGE("Array_New Failed.");
        return nullptr;
    }
    return resultArray;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS