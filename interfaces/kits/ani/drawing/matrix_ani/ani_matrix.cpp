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

namespace OHOS::Rosen {
namespace Drawing {
const char* ANI_CLASS_MATRIX_NAME = "L@ohos/graphics/drawing/drawing/Matrix;";
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
        ani_native_function { "constructorNative", ":V", reinterpret_cast<void*>(Constructor) },
        ani_native_function { "constructorNative", "L@ohos/graphics/drawing/drawing/Matrix;:V",
            reinterpret_cast<void*>(ConstructorWithMatrix) },
        ani_native_function { "getValue", "D:D", reinterpret_cast<void*>(GetValue) },
        ani_native_function { "reset", ":V", reinterpret_cast<void*>(Reset) },
        ani_native_function { "setTranslation", "DD:V", reinterpret_cast<void*>(SetTranslation) },
    };

    ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size());
    if (ret != ANI_OK) {
        ROSEN_LOGE("[ANI] bind methods fail: %{public}s", ANI_CLASS_MATRIX_NAME);
        return ANI_NOT_FOUND;
    }

    return ANI_OK;
}

void AniMatrix::Constructor(ani_env* env, ani_object obj)
{
    AniMatrix* aniMatrix = new AniMatrix();
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(aniMatrix))) {
        ROSEN_LOGE("AniMatrix::Constructor failed create AniMatrix");
        delete aniMatrix;
        return;
    }
}

void AniMatrix::ConstructorWithMatrix(ani_env* env, ani_object obj, ani_object aniMatrixObj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, aniMatrixObj);
    if (aniMatrix == nullptr) {
        AniThrowError(env, "Invalid params. "); // message length must be a multiple of 4, for example 16, 20, etc
        return;
    }

    AniMatrix* newAniMatrix = new AniMatrix(aniMatrix->GetMatrix());
    if (ANI_OK != env->Object_SetFieldByName_Long(obj, NATIVE_OBJ, reinterpret_cast<ani_long>(newAniMatrix))) {
        ROSEN_LOGE("AniMatrix::Constructor failed create AniMatrix");
        delete newAniMatrix;
        return;
    }
}

void AniMatrix::Reset(ani_env* env, ani_object obj)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr) {
        AniThrowError(env, "Invalid params. ");
        return;
    }

    aniMatrix->GetMatrix().Reset();
}

void AniMatrix::SetTranslation(ani_env* env, ani_object obj, ani_double dx, ani_double dy)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr) {
        AniThrowError(env, "Invalid params. ");
        return;
    }
    aniMatrix->GetMatrix().Translate(dx, dy);
}

ani_double AniMatrix::GetValue(ani_env* env, ani_object obj, ani_double index)
{
    auto aniMatrix = GetNativeFromObj<AniMatrix>(env, obj);
    if (aniMatrix == nullptr) {
        AniThrowError(env, "Invalid params. ");
        return 0;
    }

    if (CheckInt32OutOfRange(static_cast<ani_int>(index), NUMBER_ZREO, MATRIX_SIZE)) {
        AniThrowError(env, "matrix index out of range. ");
        return 0;
    }

    return aniMatrix->GetMatrix().Get(index);
}


Matrix& AniMatrix::GetMatrix()
{
    return matrix_;
}
} // namespace Drawing
} // namespace OHOS::Rosen