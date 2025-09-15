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

    if (index < NUMBER_ZREO || index >= MATRIX_SIZE) {
        ThrowBusinessError(env, DrawingErrorCode::ERROR_INVALID_PARAM, "Invalid param index out of range.");
        return 0;
    }

    return aniMatrix->GetMatrix()->Get(index);
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