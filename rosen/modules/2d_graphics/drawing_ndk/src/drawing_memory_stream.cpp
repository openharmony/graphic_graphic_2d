/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "drawing_memory_stream.h"
#include <cstddef>
#include <unordered_map>
#include "utils/memory_stream.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static MemoryStream* CastToMemoryStream(OH_Drawing_MemoryStream* cCanvas)
{
    return reinterpret_cast<MemoryStream*>(cCanvas);
}

OH_Drawing_MemoryStream* OH_Drawing_MemoryStreamCreate(const void* data, size_t length, bool copyData)
{
    if (data == nullptr || length == 0) {
        return nullptr;
    }
    return (OH_Drawing_MemoryStream*)new MemoryStream(data, length, copyData);
}

void OH_Drawing_MemoryStreamDestroy(OH_Drawing_MemoryStream* cMemoryStream)
{
    if (cMemoryStream == nullptr) {
        return;
    }
    delete CastToMemoryStream(cMemoryStream);
}
