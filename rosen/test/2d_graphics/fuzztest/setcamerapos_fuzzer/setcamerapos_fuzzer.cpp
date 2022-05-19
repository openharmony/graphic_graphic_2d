/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "setcamerapos_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "utils/camera3d.h"
#include "utils/matrix.h"
#include "utils/scalar.h"

const int CONSTANTS_NUMBER = 5;

namespace OHOS {
namespace Rosen {
namespace Drawing {
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (data == nullptr || objectSize > size) {
        return 0;
    }
    auto ret = memcpy_s(&object, objectSize, data, objectSize);
    if (ret != EOK) {
        return 0;
    }
    return objectSize;
}

bool SetCameraPosFuzzTest(const uint8_t* data, size_t size)
{
    scalar positionX;
    scalar positionY;
    if (data == nullptr || size < sizeof(scalar) + sizeof(scalar)) {
        return false;
    }

    size_t startPos = 0;
    startPos += GetObject<scalar>(positionX, data + startPos, size - startPos);
    startPos += GetObject<scalar>(positionY, data + startPos, size - startPos);
    Camera3D camera3d;
    Matrix matrix;
    camera3d.ApplyToMatrix(matrix);
    camera3d.SetCameraPos(positionX, positionY, CONSTANTS_NUMBER);
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::SetCameraPosFuzzTest(data, size);
    return 0;
}