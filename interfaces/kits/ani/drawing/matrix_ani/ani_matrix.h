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
    AniMatrix() = default;
    explicit AniMatrix(const Matrix& matrix) : matrix_(matrix) {}
    ~AniMatrix() = default;

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithMatrix(ani_env* env, ani_object obj, ani_object aniMatrixObj);
    static void SetTranslation(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static ani_double GetValue(ani_env* env, ani_object obj, ani_int index);
    static void Reset(ani_env* env, ani_object obj);

    Matrix& GetMatrix();

private:
    Matrix matrix_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_MATRIX_H