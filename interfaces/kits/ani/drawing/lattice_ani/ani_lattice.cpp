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

#include "ani_lattice.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/lattice_napi/js_lattice.h"

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_ANI_LATTICE_NAME = "@ohos.graphics.drawing.drawing.Lattice";

bool GetLatticeDividers(ani_env* env, ani_object dividersArray, uint32_t count, std::vector<int>& dividers)
{
    ani_int aniLength;
    if (ANI_OK != env->Object_GetPropertyByName_Int(dividersArray, "length", &aniLength)) {
        ROSEN_LOGE("AniLattice::CreateImageLattice dividers are invalid");
        return false;
    }
    uint32_t dividersSize = aniLength;

    if (dividersSize != count || dividersSize > 5) { // 5 is max value, the api limit max size
        ROSEN_LOGE("AniLattice::CreateImageLattice dividers are invalid");
        return false;
    }
    if (dividersSize != 0) {
        dividers.reserve(dividersSize);
        for (uint32_t i = 0; i < dividersSize; i++) {
            ani_int divider;
            ani_ref dividerRef;
            int ret = 0;
            if ((ret = env->Object_CallMethodByName_Ref(dividersArray,
                "$_get", "i:C{std.core.Object}", &dividerRef, (ani_int)i)) != ANI_OK) {
                ROSEN_LOGE("AniLattice::CreateImageLattice  get divider ref failed. ret: %{public}d", ret);
                return false;
            }
            if ((ret = env->Object_CallMethodByName_Int(
                static_cast<ani_object>(dividerRef), "unboxed", ":i", &divider)) != ANI_OK) {
                ROSEN_LOGE("AniLattice::CreateImageLattice Get divider failed. ret: %{public}d", ret);
                return false;
            }
            dividers.push_back(divider);
        }
    }
    return true;
}

bool GetLatticeRectTypes(ani_env* env, ani_object rectTypesArray, uint32_t count,
    std::vector<Lattice::RectType>& latticeRectTypes)
{
    ani_int aniLength;
    if (ANI_OK != env->Object_GetPropertyByName_Int(rectTypesArray, "length", &aniLength)) {
        ROSEN_LOGE("AniLattice::CreateImageLattice rectTypes are invalid");
        return false;
    }
    uint32_t rectTypesSize = aniLength;

    if ((rectTypesSize != 0 && rectTypesSize != count) || rectTypesSize > 36) { // 36: max value, limit max size
        ROSEN_LOGE("AniLattice::CreateImageLattice rectTypes are invalid");
        return false;
    }
    if (rectTypesSize != 0) {
        latticeRectTypes.reserve(rectTypesSize);
        for (uint32_t i = 0; i < rectTypesSize; i++) {
            ani_int rectType;
            ani_ref rectTypeRef;
            if (ANI_OK != env->Object_CallMethodByName_Ref(
                rectTypesArray, "$_get", "i:C{std.core.Object}", &rectTypeRef, (ani_int)i) ||
                ANI_OK != env->EnumItem_GetValue_Int((ani_enum_item)rectTypeRef, &rectType)) {
                ROSEN_LOGE("AniLattice::CreateImageLattice Incorrect parameter dividers type.");
                return false;
            }
            if (rectType < 0 || rectType > 2) { // 2: FIXEDCOLOR
                ROSEN_LOGE("AniLattice::CreateImageLattice rectType is invalid");
                return false;
            }
            latticeRectTypes.push_back(static_cast<Lattice::RectType>(rectType));
        }
    }
    return true;
}

bool GetLatticeColors(ani_env* env, ani_object colorsArray, uint32_t count, std::vector<Color>& latticeColors)
{
    ani_int aniLength;
    if (ANI_OK != env->Object_GetPropertyByName_Int(colorsArray, "length", &aniLength)) {
        ROSEN_LOGE("AniLattice::CreateImageLattice colors are invalid");
        return false;
    }
    uint32_t colorsSize = aniLength;

    if ((colorsSize != 0 && colorsSize != count) || colorsSize > 36) { // 36: max value
        ROSEN_LOGE("AniLattice::CreateImageLattice colors are invalid");
        return false;
    }

    ani_boolean isColorClass;
    if (colorsSize != 0) {
        latticeColors.reserve(colorsSize);
        for (uint32_t i = 0; i < colorsSize; i++) {
            ani_ref colorRef;
            ani_int aniColor;
            Drawing::ColorQuad colorQuad;
            if (ANI_OK != env->Object_CallMethodByName_Ref(
                colorsArray, "$_get", "i:C{std.core.Object}", &colorRef, (ani_int)i)) {
                ROSEN_LOGE("AniLattice::CreateImageLattice colors is invalid");
                return false;
            }
            if (i == 0) {
                ani_class colorClass;
                env->FindClass("@ohos.graphics.common2D.common2D.Color", &colorClass);
                env->Object_InstanceOf(static_cast<ani_object>(colorRef), colorClass, &isColorClass);
            }

            if ((isColorClass && !GetColorQuadFromColorObj(env, static_cast<ani_object>(colorRef), colorQuad)) ||
                (!isColorClass && ANI_OK != env->Object_CallMethodByName_Int(
                    static_cast<ani_object>(colorRef), "unboxed", ":d", &aniColor))) {
                ROSEN_LOGE("AniLattice::CreateImageLattice colors is invalid");
                return false;
            }
            colorQuad = isColorClass ? colorQuad : static_cast<Drawing::ColorQuad>(aniColor);
            Drawing::Color drawingColor;
            drawingColor.SetColorQuad(colorQuad);
            latticeColors.push_back(drawingColor);
        }
    }
    return true;
}

ani_status AniLattice::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_ANI_LATTICE_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_ANI_LATTICE_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "createImageLattice", nullptr,
            reinterpret_cast<void*>(CreateImageLattice) },
        ani_native_function { "latticeTransferStaticNative", nullptr,
            reinterpret_cast<void*>(LatticeTransferStatic) },
        ani_native_function { "getLatticeAddr", nullptr, reinterpret_cast<void*>(GetLatticeAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s ret: %{public}d", ANI_CLASS_ANI_LATTICE_NAME, ret);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

AniLattice::~AniLattice()
{
    lattice_ = nullptr;
}

ani_object AniLattice::CreateImageLattice(ani_env* env,
    ani_object obj, ani_object xDivs, ani_object yDivs, ani_int fXCount,
    ani_int fYCount, ani_object fBounds, ani_object fRectTypes, ani_object fColors)
{
    uint32_t xCount = static_cast<uint32_t>(fXCount);
    uint32_t yCount = static_cast<uint32_t>(fYCount);

    Lattice lat;
    if (!GetLatticeDividers(env, xDivs, xCount, lat.fXDivs)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter0 type.");
        return CreateAniUndefined(env);
    }
    if (!GetLatticeDividers(env, yDivs, yCount, lat.fYDivs)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter1 type.");
        return CreateAniUndefined(env);
    }
    lat.fXCount = static_cast<int>(xCount);
    lat.fYCount = static_cast<int>(yCount);

    ani_boolean isUndefined = ANI_TRUE;
    ani_boolean isNull = ANI_TRUE;
    env->Reference_IsUndefined(fBounds, &isUndefined);
    env->Reference_IsNull(fBounds, &isNull);
    Drawing::Rect drawingRect;
    if (!isUndefined && !isNull && GetRectFromAniRectObj(env, fBounds, drawingRect)) {
        lat.fBounds.push_back(Drawing::RectI(
            drawingRect.left_, drawingRect.top_, drawingRect.right_, drawingRect.bottom_));
    }

    env->Reference_IsUndefined(fRectTypes, &isUndefined);
    env->Reference_IsNull(fRectTypes, &isNull);
    uint32_t count = (xCount + 1) * (yCount + 1); // 1: grid size need + 1
    if (!isUndefined && !isNull) {
        if (!GetLatticeRectTypes(env, fRectTypes, count, lat.fRectTypes)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter6 type.");
            return CreateAniUndefined(env);
        }
    }

    env->Reference_IsUndefined(fColors, &isUndefined);
    env->Reference_IsNull(fColors, &isNull);
    if (!isUndefined && !isNull) {
        if (!GetLatticeColors(env, fColors, count, lat.fColors)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Incorrect parameter7 type.");
            return CreateAniUndefined(env);
        }
    }

    AniLattice* aniLattice = new AniLattice(std::make_shared<Lattice>(lat));
    ani_object aniObj = CreateAniObjectStatic(env, ANI_CLASS_ANI_LATTICE_NAME, aniLattice);
    ani_boolean objIsUndefined;
    env->Reference_IsUndefined(aniObj, &objIsUndefined);
    if (objIsUndefined) {
        delete aniLattice;
        ROSEN_LOGE("AniLattice::CreateImageLattice failed cause aniObj is undefined");
    }
    return aniObj;
}

ani_object AniLattice::LatticeTransferStatic(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniLattice::LatticeTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniLattice::LatticeTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsLattice = reinterpret_cast<JsLattice*>(unwrapResult);
    if (jsLattice->GetLatticePtr() == nullptr) {
        ROSEN_LOGE("AniLattice::LatticeTransferStatic jsLattice is null");
        return nullptr;
    }

    auto aniLattice = new AniLattice(jsLattice->GetLatticePtr());

    ani_object aniLatticeObj = CreateAniObject(env, ANI_CLASS_ANI_LATTICE_NAME, nullptr);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniLatticeObj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniLattice))) {
        ROSEN_LOGE("AniLattice::LatticeTransferStatic failed create aniLattice");
        delete aniLattice;
        return nullptr;
    }
    return aniLatticeObj;
}

ani_long AniLattice::GetLatticeAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniLattice = GetNativeFromObj<AniLattice>(env, input);
    if (aniLattice == nullptr || aniLattice->GetLattice() == nullptr) {
        ROSEN_LOGE("AniLattice::GetLatticeAddr aniLattice is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniLattice->GetLatticePtrAddr());
}

std::shared_ptr<Lattice>* AniLattice::GetLatticePtrAddr()
{
    return &lattice_;
}

std::shared_ptr<Lattice> AniLattice::GetLattice()
{
    return lattice_;
}
} // namespace Drawing
} // namespace OHOS::Rosen
