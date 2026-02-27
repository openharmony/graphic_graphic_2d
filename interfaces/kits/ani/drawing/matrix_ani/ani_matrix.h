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

#ifndef OHOS_ROSEN_ANI_MATRIX_H
#define OHOS_ROSEN_ANI_MATRIX_H

#include "ani_drawing_utils.h"
#include "utils/matrix.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniMatrix final {
public:
    explicit AniMatrix(std::shared_ptr<Matrix> matrix = nullptr) : matrix_(matrix) {}
    ~AniMatrix();

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithMatrix(ani_env* env, ani_object obj, ani_object aniMatrixObj);
    static void SetTranslation(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static ani_double GetValue(ani_env* env, ani_object obj, ani_int index);
    static void Reset(ani_env* env, ani_object obj);
    static void preConcat(ani_env* env, ani_object obj, ani_object aniMatrixObj);
    static ani_boolean RectStaysRect(ani_env* env, ani_object obj);
    static ani_boolean IsAffine(ani_env* env, ani_object obj);
    static void PreRotate(ani_env* env, ani_object obj, ani_double degree, ani_double px, ani_double py);
    static void PreScale(ani_env* env, ani_object obj, ani_double sx, ani_double sy, ani_double px, ani_double py);
    static ani_boolean SetRectToRect(ani_env* env, ani_object obj, ani_object aniSrcRectObj,
        ani_object aniDstRectObj, ani_enum_item aniScaleToFit);
    static void PostScale(ani_env* env, ani_object obj, ani_double sx, ani_double sy, ani_double px, ani_double py);
    static void PostTranslate(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static ani_object GetAll(ani_env* env, ani_object obj);
    static void SetMatrix(ani_env* env, ani_object obj, ani_array aniValueArrayObj);
    static void SetMatrixWithObject(ani_env* env, ani_object obj, ani_object matrixArryaObj);
    static ani_boolean MapRect(ani_env* env, ani_object obj, ani_object aniDstRectObj, ani_object aniSrcRectObj);
    static void PostRotate(ani_env* env, ani_object obj, ani_double degree, ani_double px, ani_double py);
    static ani_boolean Invert(ani_env* env, ani_object obj, ani_object aniMatrixObj);
    static ani_boolean IsEqual(ani_env* env, ani_object obj, ani_object aniMatrixObj);
    static ani_boolean SetPolyToPoly(ani_env* env, ani_object obj, ani_array aniSrcPointArray,
        ani_array aniDstPointArray, ani_int count);
    static void PreConcat(ani_env* env, ani_object obj, ani_object aniMatrixObj);
    static ani_boolean IsIdentity(ani_env* env, ani_object obj);
    static ani_object MapPoints(ani_env* env, ani_object obj, ani_array aniSrcPointArray);
    static void SetRotation(ani_env* env, ani_object obj, ani_double degree, ani_double px, ani_double py);
    static void PreTranslate(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static void SetScale(ani_env* env, ani_object obj, ani_double sx, ani_double sy, ani_double px, ani_double py);
    static void PreSkew(ani_env* env, ani_object obj, ani_double kx, ani_double ky, ani_double px, ani_double py);
    static void PostSkew(ani_env* env, ani_object obj, ani_double kx, ani_double ky, ani_double px, ani_double py);
    static void SetSkew(ani_env* env, ani_object obj, ani_double kx, ani_double ky, ani_double px, ani_double py);
    static void SetSinCos(ani_env* env, ani_object obj, ani_double sinValue, ani_double cosValue, ani_double px,
        ani_double py);
    static void SetConcat(ani_env* env, ani_object obj, ani_object matrixAobj, ani_object matrixBobj);
    static void PostConcat(ani_env* env, ani_object obj, ani_object matrixobj);
    static ani_double MapRadius(ani_env* env, ani_object obj, ani_double radius);

    std::shared_ptr<Matrix> GetMatrix();

private:
    static ani_object MatrixTransferStatic(
        ani_env* env, [[maybe_unused]]ani_object obj, ani_object output, ani_object input);
    static ani_long GetMatrixAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    void OnSetMatrix(ani_env* env, ani_object obj, ani_object matrixArryaObj);
    std::shared_ptr<Matrix>* GetMatrixPtrAddr();
    std::shared_ptr<Matrix> matrix_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_MATRIX_H