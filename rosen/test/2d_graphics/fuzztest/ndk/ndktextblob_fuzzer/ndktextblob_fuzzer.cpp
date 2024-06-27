/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "ndktextblob_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

#include "drawing_font.h"
#include "drawing_rect.h"
#include "drawing_text_blob.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
} // namespace

namespace Drawing {

void NativeDrawingTextBlobTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_TextBlobBuilder* textBlobBuilder = OH_Drawing_TextBlobBuilderCreate();
    OH_Drawing_TextBlob* textBlob = OH_Drawing_TextBlobBuilderMake(textBlobBuilder);
    OH_Drawing_TextBlob* textBlobTwo = OH_Drawing_TextBlobBuilderMake(nullptr);

    float left = GetObject<float>();
    float top = GetObject<float>();
    float right = GetObject<float>();
    float bottom = GetObject<float>();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(left, top, right, bottom);
    OH_Drawing_TextBlobGetBounds(nullptr, rect);
    OH_Drawing_TextBlobGetBounds(textBlob, nullptr);
    OH_Drawing_TextBlobGetBounds(textBlob, rect);

    OH_Drawing_TextBlobUniqueID(nullptr);
    OH_Drawing_TextBlobUniqueID(textBlob);

    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_TextBlobDestroy(textBlobTwo);
    OH_Drawing_TextBlobBuilderDestroy(textBlobBuilder);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::NativeDrawingTextBlobTest001(data, size);
    return 0;
}
