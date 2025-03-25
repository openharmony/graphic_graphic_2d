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

#include "customized/random_matrix.h"

#include "random/random_data.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
Drawing::Matrix RandomDrawingMatrix::GetRandomDrawingMatrix()
{
    Drawing::Matrix matrix;
    Drawing::Matrix::Buffer buffer;
    for (size_t i = 0; i < Drawing::Matrix::MATRIX_SIZE; i++) {
        buffer[i] = RandomData::GetRandomFloat();
    }
    matrix.SetAll(buffer);
    return matrix;
}
} // namespace Rosen
} // namespace OHOS
