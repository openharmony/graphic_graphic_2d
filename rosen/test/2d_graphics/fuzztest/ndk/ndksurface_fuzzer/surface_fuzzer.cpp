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

#include "surface_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

#include "drawing_canvas.h"
#include "drawing_gpu_context.h"
#include "drawing_surface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

void SurfaceFuzzTest000(const uint8_t* data, size_t size)
{
    g_data = data;
    g_size = size;
    g_pos = 0;

    // Due to conflicts between libfuzzer and dlclose, effective use cases cannot be provided.
    OH_Drawing_SurfaceGetCanvas(nullptr);
    OH_Drawing_SurfaceDestroy(nullptr);

    return;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::SurfaceFuzzTest000(data, size);
    return 0;
}