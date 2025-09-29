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

#include "ani_matrix.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "interop_js/hybridgref_ani.h"
#include "interop_js/hybridgref_napi.h"
#include "drawing/matrix_napi/js_matrix.h"

namespace OHOS::Rosen {
namespace Drawing {
constexpr int32_t NUMBER_ZREO = 0;
constexpr int32_t MATRIX_SIZE = 9;
constexpr uint32_t POLY_POINT_COUNT_MAX = 4;

ani_status AniMatrix::AniInit(ani_env *env)
{
    ani_class cls = nullptr;
    ani_status ret = env->FindClass(ANI_CLASS_MATRIX_NAME, &cls);
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] can't find class: %{public}s", ANI_CLASS_MATRIX_NAME);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function { "constructorNative", ":", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "C{@ohos.graphics.drawing.drawing.Matrix}:",
            reinterpret_cast<void*>(ConstructorWithMatrix) },
        ani_native_function { "getValue", "i:d", reinterpret_cast<void*>(GetValue) },
        ani_native_function { "reset", ":", reinterpret_cast<void*>(Reset) },
        ani_native_function { "setTranslation", "dd:", reinterpret_cast<void*>(SetTranslation) },
        ani_native_function { "preRotate", nullptr, reinterpret_cast<void*>(PreRotate) },
        ani_native_function { "preScale", nullptr, reinterpret_cast<void*>(PreScale) },
        ani_native_function { "setRectToRect", nullptr, reinterpret_cast<void*>(SetRectToRect) },
        ani_native_function { "postScale", nullptr, reinterpret_cast<void*>(PostScale) },
        ani_native_function { "postTranslate", nullptr, reinterpret_cast<void*>(PostTranslate) },
        ani_native_function { "getAll", nullptr, reinterpret_cast<void*>(GetAll) },
        ani_native_function { "setMatrix", nullptr, reinterpret_cast<void*>(SetMatrix) },
        ani_native_function { "mapRect", nullptr, reinterpret_cast<void*>(MapRect) },
        ani_native_function { "postRotate", nullptr, reinterpret_cast<void*>(PostRotate) },
        ani_native_function { "invert", nullptr, reinterpret_cast<void*>(Invert) },
        ani_native_function { "isEqual", nullptr, reinterpret_cast<void*>(IsEqual) },
        ani_native_function { "setPolyToPoly", nullptr, reinterpret_cast<void*>(SetPolyToPoly) },
        ani_native_function { "preConcat", nullptr, reinterpret_cast<void*>(PreConcat) },
        ani_native_function { "isIdentity", nullptr, reinterpret_cast<void*>(IsIdentity) },
        ani_native_function { "mapPoints", nullptr, reinterpret_cast<void*>(MapPoints) },
        ani_native_function { "setRotation", nullptr, reinterpret_cast<void*>(SetRotation) },
        ani_native_function { "preTranslate", nullptr, reinterpret_cast<void*>(PreTranslate) },
        ani_native_function { "setScale", nullptr, reinterpret_cast<void*>(SetScale) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: ret %{public}d %{public}s", ret, ANI_CLASS_MATRIX_NAME);
        return ANI_NOT_FOUND;
    }

    std::array staticMethods = {
        ani_native_function { "matrixTransferStaticNative", nullptr, reinterpret_cast<void*>(MatrixTransferStatic) },
        ani_native_function { "getMatrixAddr", nullptr, reinterpret_cast<void*>(GetMatrixAddr) },
    };

    ret = env->Class_BindStaticNativeMethods(cls, staticMethods.data(), staticMethods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind static methods fail: %{public}s", ANI_CLASS_MATRIX_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniMatrix::Constructor(ani_env* env, ani_object obj)
{
    std::shared_ptr<Matrix> matrix = std::make_shared<Matrix>();
    AniMatrix* aniMatrix = new AniMatrix(matrix);
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniMatrix))) {
        ROSEN_LOGE("AniMatrix::Constructor failed create AniMatrix");
        delete aniMatrix;
        return;
    }
}

void AniMatrix::ConstructorWithMatrix(ani_env* env, ani_object obj, ani_object aniMatrixObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    std::shared_ptr<Matrix> other = aniMatrix->GetMatrix();
    std::shared_ptr<Matrix> matrix = other == nullptr ? std::make_shared<Matrix>() : std::make_shared<Matrix>(*other);
    AniMatrix* newAniMatrix = new AniMatrix(matrix);
    ani_status ret = env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniMatrix));
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniMatrix::Constructor failed create AniMatrix. ret: %{public}d", ret);
        delete newAniMatrix;
        return;
    }
}

void AniMatrix::Reset(ani_env* env, ani_object obj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }

    aniMatrix->GetMatrix()->Reset();
}

void AniMatrix::SetTranslation(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniMatrix->GetMatrix()->Translate(dx, dy);
}

ani_double AniMatrix::GetValue(ani_env* env, ani_object obj, ani_int index)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return 0;
    }

    if (CheckInt32OutOfRange(static_cast<ani_int>(index), NUMBER_ZREO, MATRIX_SIZE)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param index out of range.");
        return 0;
    }

    return aniMatrix->GetMatrix()->Get(index);
}

ani_boolean AniMatrix::RectStaysRect(ani_env* env, ani_object obj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    return aniMatrix->GetMatrix()->RectStaysRect();
}

ani_boolean AniMatrix::IsAffine(ani_env* env, ani_object obj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    return aniMatrix->GetMatrix()->IsAffine();
}

void AniMatrix::PreRotate(ani_env* env, ani_object obj, ani_double degree, ani_double px, ani_double py)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniMatrix->GetMatrix()->PreRotate(degree, px, py);
}

void AniMatrix::PreScale(ani_env* env, ani_object obj, ani_double sx, ani_double sy, ani_double px, ani_double py)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniMatrix->GetMatrix()->PreScale(sx, sy, px, py);
}

ani_boolean AniMatrix::SetRectToRect(ani_env* env, ani_object obj, ani_object aniSrcRectObj,
    ani_object aniDstRectObj, ani_enum_item aniScaleToFit)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }
    Drawing::Rect src;
    if (!GetRectFromAniRectObj(env, aniSrcRectObj, src)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param src.");
        return false;
    }

    Drawing::Rect dst;
    if (!GetRectFromAniRectObj(env, aniDstRectObj, dst)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param dst.");
        return false;
    }

    ani_int scaleToFit = 0;
    if (ANI_OK != env->EnumItem_GetValue_Int(aniScaleToFit, &scaleToFit)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param scaleToFit.");
        return false;
    }
    return aniMatrix->GetMatrix()->SetRectToRect(src, dst, static_cast<Drawing::ScaleToFit>(scaleToFit));
}
void AniMatrix::PostScale(ani_env* env, ani_object obj, ani_double sx, ani_double sy, ani_double px, ani_double py)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniMatrix->GetMatrix()->PostScale(sx, sy, px, py);
}

void AniMatrix::PostTranslate(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return;
    }
    aniMatrix->GetMatrix()->PostTranslate(dx, dy);
}

ani_object DoubleToObject(ani_env *env, double value)
{
    ani_object aniObject = nullptr;
    ani_double doubleValue = static_cast<ani_double>(value);
    ani_class aniClass;
    static const char* className = "std.core.Double";
    ani_status ret = env->FindClass(className, &aniClass);
    if (ret != ANI_OK) {
        ROSEN_LOGE("Not found '%{public}s' ret: %{public}d.", className, ret);
        return aniObject;
    }
    ani_method ctorMethod;
    ret = env->Class_FindMethod(aniClass, "<ctor>", "d:", &ctorMethod);
    if (ANI_OK != env->Class_FindMethod(aniClass, "<ctor>", "d:", &ctorMethod)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Class_FindMethod <ctor> Failed.");
        return aniObject;
    }

    if (ANI_OK != env->Object_New(aniClass, ctorMethod, &aniObject, doubleValue)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Object_New Double Faild.");
        return aniObject;
    }
    return aniObject;
}

ani_object AniMatrix::GetAll(ani_env* env, ani_object obj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return CreateAniUndefined(env);
    }
    Drawing::Matrix::Buffer buffer;
    aniMatrix->GetMatrix()->GetAll(buffer);

    ani_class arrayCls = nullptr;
    if (ANI_OK != env->FindClass("escompat.Array", &arrayCls)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Find class failed.");
        return CreateAniUndefined(env);
    }
    ani_method arrayCtor;
    if (ANI_OK != env->Class_FindMethod(arrayCls, "<ctor>", "i:", &arrayCtor)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Class_FindMethod <ctor> Failed");
        return CreateAniUndefined(env);
    }

    ani_object arrayObj;
    if (ANI_OK != env->Object_New(arrayCls, arrayCtor, &arrayObj, Drawing::Matrix::MATRIX_SIZE)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Object_New Array Faild");
        return CreateAniUndefined(env);
    }

    ani_size index = 0;
    for (auto item : buffer) {
        ani_object aniValue = DoubleToObject(env, item);
        if (ANI_OK != env->Object_CallMethodByName_Void(arrayObj, "$_set", "iC{std.core.Object}:", index, aniValue)) {
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Object_CallMethodByName_Void Array Faild");
            return CreateAniUndefined(env);
        }
        index++;
    }
    return arrayObj;
}

void AniMatrix::SetMatrix(ani_env* env, ani_object obj, ani_object aniValueArrayObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return;
    }
    std::vector<float> matrixVector;
    ani_int aniLength;
    ani_status ret = env->Object_GetPropertyByName_Int(aniValueArrayObj, "length", &aniLength);
    if (ret != ANI_OK) {
        ROSEN_LOGE("AniMatrix::SetMatrix aniValueArrayObj are invalid ret: %{public}d", ret);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix.");
        return;
    }
    uint32_t matrixSize = static_cast<uint32_t>(aniLength);
    if (matrixSize != ARGC_NINE) {
        ROSEN_LOGE("AniMatrix::SetMatrix aniValueArrayObj size{%{public}d} invalid.", matrixSize);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix array length.");
        return;
    }
    matrixVector.reserve(matrixSize);
    for (uint32_t i = 0; i < matrixSize; i++) {
        ani_ref matrixRef;
        ani_double matrixValue;
        ret = env->Object_CallMethodByName_Ref(
            aniValueArrayObj, "$_get", "i:C{std.core.Object}", &matrixRef, static_cast<ani_int>(i));
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniMatrix::SetMatrix aniValueArrayObj get pointRef failed. ret: %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix array element.");
            return;
        }
        ret = env->Object_CallMethodByName_Double(static_cast<ani_object>(matrixRef), "toDouble", ":d", &matrixValue);
        if (ret != ANI_OK) {
            ROSEN_LOGE("AniMatrix::SetMatrix matrixRef is invalid. ret: %{public}d", ret);
            ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix array element.");
            return;
        }
        matrixVector.push_back(matrixValue);
    }
    aniMatrix->GetMatrix()->SetMatrix(matrixVector[ARGC_ZERO], matrixVector[ARGC_ONE], matrixVector[ARGC_TWO],
        matrixVector[ARGC_THREE], matrixVector[ARGC_FOUR], matrixVector[ARGC_FIVE], matrixVector[ARGC_SIX],
        matrixVector[ARGC_SEVEN], matrixVector[ARGC_EIGHT]);
}

ani_boolean AniMatrix::MapRect(ani_env* env, ani_object obj, ani_object aniDstRectObj, ani_object aniSrcRectObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return false;
    }
    Drawing::Rect dst;
    if (!GetRectFromAniRectObj(env, aniDstRectObj, dst)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param dst.");
        return false;
    }

    Drawing::Rect src;
    if (!GetRectFromAniRectObj(env, aniSrcRectObj, src)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param src.");
        return false;
    }
    auto result = aniMatrix->GetMatrix()->MapRect(dst, src);
    if ((env->Object_SetPropertyByName_Double(aniDstRectObj, "left", ani_double(dst.GetLeft())) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(aniDstRectObj, "top", ani_double(dst.GetTop())) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(aniDstRectObj, "right", ani_double(dst.GetRight())) != ANI_OK) ||
        (env->Object_SetPropertyByName_Double(aniDstRectObj, "bottom", ani_double(dst.GetBottom())) != ANI_OK)) {
        ROSEN_LOGE("Set dstRectObject from rect failed.");
        return false;
    }
    return result;
}

void AniMatrix::PostRotate(ani_env* env, ani_object obj, ani_double degree, ani_double px, ani_double py)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return;
    }
    aniMatrix->GetMatrix()->PostRotate(degree, px, py);
}

ani_boolean AniMatrix::Invert(ani_env* env, ani_object obj, ani_object aniMatrixObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
    }

    auto aniNewMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj);
    if (aniNewMatrix == nullptr || aniNewMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix.");
        return false;
    }
    std::shared_ptr<Matrix> matrix = aniNewMatrix->GetMatrix();
    auto result = aniMatrix->GetMatrix()->Invert(*matrix);
    if (ANI_OK != env->Object_SetFieldByName_Long(aniMatrixObj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniNewMatrix))) {
        ROSEN_LOGE("AniMatrix::Constructor failed create AniMatrix");
        return false;
    }
    return result;
}

ani_boolean AniMatrix::IsEqual(ani_env* env, ani_object obj, ani_object aniMatrixObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return false;
    }
    auto aniNewMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj);
    if (aniNewMatrix == nullptr || aniNewMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix.");
        return false;
    }
    return aniMatrix->GetMatrix()->operator == (*aniNewMatrix->GetMatrix());
}

bool ConvertFromAniPointsArray(
    ani_env* env, ani_object aniPointArray, Drawing::Point* points, uint32_t pointSize)
{
    for (uint32_t i = 0; i < pointSize; i++) {
        ani_ref pointRef;
        Drawing::Point point;
        if (ANI_OK != env->Object_CallMethodByName_Ref(
            aniPointArray, "$_get", "i:C{std.core.Object}", &pointRef, (ani_int)i)) {
            ROSEN_LOGE("aniPointArray get pointRef failed.");
            return false;
        }
        if (!GetPointFromAniPointObj(env, static_cast<ani_object>(pointRef), point)) {
            ROSEN_LOGE("pointRef is invalid");
            return false;
        }
        points[i] = point;
    }
    return true;
}

ani_boolean AniMatrix::SetPolyToPoly(ani_env* env, ani_object obj, ani_object aniSrcPointArray,
    ani_object aniDstPointArray, ani_int count)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return false;
    }

    uint32_t inputSize = static_cast<uint32_t>(count);
    if (inputSize > POLY_POINT_COUNT_MAX) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param 'count' > 4");
        return false;
    }

    Drawing::Point srcPoints[POLY_POINT_COUNT_MAX];
    if (!ConvertFromAniPointsArray(env, aniSrcPointArray, srcPoints, inputSize)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid 'src' Array<Point> type.");
        return false;
    }
    Drawing::Point dstPoints[POLY_POINT_COUNT_MAX];
    if (!ConvertFromAniPointsArray(env, aniDstPointArray, dstPoints, inputSize)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid 'dst' Array<Point> type.");
        return false;
    }
    return aniMatrix->GetMatrix()->SetPolyToPoly(srcPoints, dstPoints, count);
}

void AniMatrix::PreConcat(ani_env* env, ani_object obj, ani_object aniMatrixObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return;
    }
    auto aniNewMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj);
    if (aniNewMatrix == nullptr || aniNewMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid param matrix.");
        return;
    }
    std::shared_ptr<Matrix> matrix = aniNewMatrix->GetMatrix();
    aniMatrix->GetMatrix()->PreConcat(*matrix);
}

ani_boolean AniMatrix::IsIdentity(ani_env* env, ani_object obj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "invalid params.");
        return false;
    }
    return aniMatrix->GetMatrix()->IsIdentity();
}

ani_object GetPointArray(ani_env* env, const std::vector<Drawing::Point>& points, uint32_t count)
{
    ani_class arrayCls = nullptr;
    if (ANI_OK != env->FindClass("escompat.Array", &arrayCls)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "FindClass escompat.Array Failed");
        return CreateAniUndefined(env);
    }
    ani_method arrayCtor;
    if (ANI_OK != env->Class_FindMethod(arrayCls, "<ctor>", "i:", &arrayCtor)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Class_FindMethod <ctor> Failed");
        return CreateAniUndefined(env);
    }

    ani_object arrayObj;
    if (ANI_OK != env->Object_New(arrayCls, arrayCtor, &arrayObj, count)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Object_New Array Faild");
        return CreateAniUndefined(env);
    }

    ani_size index = 0;
    for (const auto& item : points) {
        ani_object aniPointObj;
        if (!CreatePointObjAndCheck(env, item, aniPointObj)) {
            return aniPointObj;
        }

        if (ANI_OK != env->Object_CallMethodByName_Void(
            arrayObj, "$_set", "iC{std.core.Object}:", index, aniPointObj)) {
            ROSEN_LOGE("SObject_CallMethodByName_Void  $_set Faild ");
            return CreateAniUndefined(env);
        }
        index++;
    }
    return arrayObj;
}

ani_object AniMatrix::MapPoints(ani_env* env, ani_object obj, ani_object aniSrcPointArray)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid params.");
        return CreateAniUndefined(env);
    }
    
    ani_int aniLength;
    ani_status ret = env->Object_GetPropertyByName_Int(aniSrcPointArray, "length", &aniLength);
    if (ret != ANI_OK || aniLength == 0) {
        ROSEN_LOGE("AniMatrix::MapPoints error getting aniSrcPointArray length. ret: %{public}d, length: %{public}d",
            ret, aniLength);
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array size.");
        return CreateAniUndefined(env);
    }
    uint32_t pointSize = static_cast<uint32_t>(aniLength);
    std::vector<Drawing::Point> srcPoints(pointSize);
    std::vector<Drawing::Point> dstPoints(pointSize);
    if (!ConvertFromAniPointsArray(env, aniSrcPointArray, srcPoints.data(), pointSize)) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array data.");
        return CreateAniUndefined(env);
    }
    aniMatrix->GetMatrix()->MapPoints(dstPoints, srcPoints, pointSize);
    return GetPointArray(env, dstPoints, pointSize);
}

void AniMatrix::SetRotation(ani_env* env, ani_object obj, ani_double degree, ani_double px, ani_double py)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array size.");
        return;
    }

    aniMatrix->GetMatrix()->Rotate(degree, px, py);
}

void AniMatrix::PreTranslate(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array size.");
        return;
    }
    aniMatrix->GetMatrix()->PreTranslate(dx, dy);
}

void AniMatrix::SetScale(ani_env* env, ani_object obj, ani_double sx, ani_double sy, ani_double px, ani_double py)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid src array size.");
        return;
    }
    aniMatrix->GetMatrix()->Scale(sx, sy, px, py);
}
ani_object AniMatrix::MatrixTransferStatic(
    ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input)
{
    void* unwrapResult = nullptr;
    bool success = arkts_esvalue_unwrap(env, input, &unwrapResult);
    if (!success) {
        ROSEN_LOGE("AniMatrix::MatrixTransferStatic failed to unwrap");
        return nullptr;
    }
    if (unwrapResult == nullptr) {
        ROSEN_LOGE("AniMatrix::MatrixTransferStatic unwrapResult is null");
        return nullptr;
    }
    auto jsMatrix = reinterpret_cast<JsMatrix*>(unwrapResult);
    if (jsMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("AniMatrix::MatrixTransferStatic jsMatrix is null");
        return nullptr;
    }

    auto aniMatrix = new AniMatrix(jsMatrix->GetMatrix());
    if (ANI_OK != env->Object_SetFieldByName_Long(output, NATIVE_OBJ, reinterpret_cast<ani_long>(aniMatrix))) {
        ROSEN_LOGE("AniMatrix::MatrixTransferStatic failed create aniMatrix");
        delete aniMatrix;
        return nullptr;
    }
    return output;
}

ani_long AniMatrix::GetMatrixAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, input);
    if (aniMatrix == nullptr || aniMatrix->GetMatrix() == nullptr) {
        ROSEN_LOGE("AniMatrix::GetMatrixAddr aniMatrix is null");
        return 0;
    }
    return reinterpret_cast<ani_long>(aniMatrix->GetMatrixPtrAddr());
}

std::shared_ptr<Matrix>* AniMatrix::GetMatrixPtrAddr()
{
    return &matrix_;
}

std::shared_ptr<Matrix> AniMatrix::GetMatrix()
{
    return matrix_;
}

AniMatrix::~AniMatrix()
{
    matrix_ = nullptr;
}
} // namespace Drawing
} // namespace OHOS::Rosen