/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ndkstyleattrs_fuzzer.h"

#include <cstddef>
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS::Rosen::Drawing {
namespace {
constexpr size_t ATTR_DOUBLE_COUNT = 2;
constexpr size_t ATTR_INT_COUNT = 3;
constexpr size_t ARRAY_LEN_MAX = 8;
} // namespace

void OHDrawingTextStyleAttrsDoubleTest(OH_Drawing_TextStyle* textStyle, FuzzedDataProvider& fdp)
{
    for (size_t i = 0; i < ATTR_DOUBLE_COUNT; i++) {
        OH_Drawing_TextStyleAttributeId id =
            static_cast<OH_Drawing_TextStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
        double setValue = fdp.ConsumeFloatingPoint<double>();
        OH_Drawing_SetTextStyleAttributeDouble(textStyle, id, setValue);
        double getValue = 0.0;
        OH_Drawing_GetTextStyleAttributeDouble(textStyle, id, &getValue);
    }
}

void OHDrawingTextStyleAttrsIntTest(OH_Drawing_TextStyle* textStyle, FuzzedDataProvider& fdp)
{
    for (size_t i = 0; i < ATTR_INT_COUNT; i++) {
        OH_Drawing_TextStyleAttributeId id =
            static_cast<OH_Drawing_TextStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
        int setValue = fdp.ConsumeIntegral<int>();
        OH_Drawing_SetTextStyleAttributeInt(textStyle, id, setValue);
        int getValue = 0;
        OH_Drawing_GetTextStyleAttributeInt(textStyle, id, &getValue);
    }
}

void OHDrawingTextStyleAttrsNullptrTest(FuzzedDataProvider& fdp)
{
    OH_Drawing_TextStyleAttributeId id = static_cast<OH_Drawing_TextStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    double doubleValue = fdp.ConsumeFloatingPoint<double>();
    int intValue = fdp.ConsumeIntegral<int>();

    OH_Drawing_SetTextStyleAttributeDouble(nullptr, id, doubleValue);
    OH_Drawing_GetTextStyleAttributeDouble(nullptr, id, &doubleValue);
    OH_Drawing_GetTextStyleAttributeDouble(nullptr, id, nullptr);

    OH_Drawing_SetTextStyleAttributeInt(nullptr, id, intValue);
    OH_Drawing_GetTextStyleAttributeInt(nullptr, id, &intValue);
    OH_Drawing_GetTextStyleAttributeInt(nullptr, id, nullptr);
}

void OHDrawingTypographyStyleAttrsDoubleTest(OH_Drawing_TypographyStyle* typoStyle, FuzzedDataProvider& fdp)
{
    OH_Drawing_TypographyStyleAttributeId id =
        static_cast<OH_Drawing_TypographyStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    double setValue = fdp.ConsumeFloatingPoint<double>();
    OH_Drawing_SetTypographyStyleAttributeDouble(typoStyle, id, setValue);

    double getValue = 0.0;
    OH_Drawing_GetTypographyStyleAttributeDouble(typoStyle, id, &getValue);
}

void OHDrawingTypographyStyleAttrsIntTest(OH_Drawing_TypographyStyle* typoStyle, FuzzedDataProvider& fdp)
{
    OH_Drawing_TypographyStyleAttributeId id =
        static_cast<OH_Drawing_TypographyStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    int setValue = fdp.ConsumeIntegral<int>();
    OH_Drawing_SetTypographyStyleAttributeInt(typoStyle, id, setValue);

    int getValue = 0;
    OH_Drawing_GetTypographyStyleAttributeInt(typoStyle, id, &getValue);
}

void OHDrawingTypographyStyleAttrsBoolTest(OH_Drawing_TypographyStyle* typoStyle, FuzzedDataProvider& fdp)
{
    OH_Drawing_TypographyStyleAttributeId id =
        static_cast<OH_Drawing_TypographyStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    bool setValue = fdp.ConsumeBool();
    OH_Drawing_SetTypographyStyleAttributeBool(typoStyle, id, setValue);

    bool getValue = false;
    OH_Drawing_GetTypographyStyleAttributeBool(typoStyle, id, &getValue);
}

void OHDrawingTypographyStyleAttrsDoubleArrayTest(OH_Drawing_TypographyStyle* typoStyle, FuzzedDataProvider& fdp)
{
    OH_Drawing_TypographyStyleAttributeId id =
        static_cast<OH_Drawing_TypographyStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    size_t arrayLen = fdp.ConsumeIntegralInRange<size_t>(1, ARRAY_LEN_MAX);
    double arr[ARRAY_LEN_MAX];
    for (size_t i = 0; i < arrayLen; i++) {
        arr[i] = fdp.ConsumeFloatingPoint<double>();
    }
    OH_Drawing_SetTypographyStyleAttributeDoubleArray(typoStyle, id, arr, arrayLen);

    double* outArr = nullptr;
    size_t outLen = 0;
    OH_Drawing_GetTypographyStyleAttributeDoubleArray(typoStyle, id, &outArr, &outLen);
}

void OHDrawingTypographyStyleAttrsNullptrTest(FuzzedDataProvider& fdp)
{
    OH_Drawing_TypographyStyleAttributeId id =
        static_cast<OH_Drawing_TypographyStyleAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    double doubleValue = fdp.ConsumeFloatingPoint<double>();
    int intValue = fdp.ConsumeIntegral<int>();
    bool boolValue = fdp.ConsumeBool();

    OH_Drawing_SetTypographyStyleAttributeDouble(nullptr, id, doubleValue);
    OH_Drawing_GetTypographyStyleAttributeDouble(nullptr, id, &doubleValue);
    OH_Drawing_GetTypographyStyleAttributeDouble(nullptr, id, nullptr);

    OH_Drawing_SetTypographyStyleAttributeInt(nullptr, id, intValue);
    OH_Drawing_GetTypographyStyleAttributeInt(nullptr, id, &intValue);
    OH_Drawing_GetTypographyStyleAttributeInt(nullptr, id, nullptr);

    OH_Drawing_SetTypographyStyleAttributeBool(nullptr, id, boolValue);
    OH_Drawing_GetTypographyStyleAttributeBool(nullptr, id, &boolValue);
    OH_Drawing_GetTypographyStyleAttributeBool(nullptr, id, nullptr);

    OH_Drawing_SetTypographyStyleAttributeDoubleArray(nullptr, id, &doubleValue, 1);
    double* outArr = nullptr;
    OH_Drawing_GetTypographyStyleAttributeDoubleArray(nullptr, id, &outArr, nullptr);
}

void OHDrawingStyleAttrsTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    OHDrawingTextStyleAttrsDoubleTest(textStyle, fdp);
    OHDrawingTextStyleAttrsIntTest(textStyle, fdp);
    OHDrawingTextStyleAttrsNullptrTest(fdp);
    OH_Drawing_DestroyTextStyle(textStyle);

    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OHDrawingTypographyStyleAttrsDoubleTest(typoStyle, fdp);
    OHDrawingTypographyStyleAttrsIntTest(typoStyle, fdp);
    OHDrawingTypographyStyleAttrsBoolTest(typoStyle, fdp);
    OHDrawingTypographyStyleAttrsDoubleArrayTest(typoStyle, fdp);
    OHDrawingTypographyStyleAttrsNullptrTest(fdp);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS::Rosen::Drawing

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::Rosen::Drawing::OHDrawingStyleAttrsTest(data, size);
    return 0;
}
