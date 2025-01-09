/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "blender_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "effect/blender.h"
#include "include/core/SkBlender.h"
#include "include/core/SkBlendMode.h"
namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t BLENDMODE_SIZE = 29;
} // namespace
namespace Drawing {
bool BlenderFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Blender blender;
    uint32_t blendMode = GetObject<uint32_t>();
    std::shared_ptr<Blender> blenderOne = Blender::CreateWithBlendMode(
        static_cast<BlendMode>(blendMode % BLENDMODE_SIZE));
    blenderOne->GetDrawingType();
    uint32_t skBlendMode = GetObject<uint32_t>();
    sk_sp<SkBlender> skBlender = SkBlender::Mode(static_cast<SkBlendMode>(skBlendMode % BLENDMODE_SIZE));
    return true;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::BlenderFuzzTest001(data, size);
    return 0;
}
