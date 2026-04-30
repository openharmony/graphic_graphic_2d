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

#include "ndkfontdesc_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS::Rosen::Drawing {
void OHDrawingTextFontDescriptorTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OH_Drawing_Array* descs =
        OH_Drawing_GetSystemFontFullNamesByType(static_cast<OH_Drawing_SystemFontType>(fdp.ConsumeIntegral<uint8_t>()));
    size_t length = OH_Drawing_GetDrawingArraySize(descs);
    for (size_t i = 0; i < length; i++) {
        const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(descs, i);
        OH_Drawing_FontDescriptor* desc = OH_Drawing_GetFontDescriptorByFullName(
            fullName, static_cast<OH_Drawing_SystemFontType>(fdp.ConsumeIntegral<uint8_t>()));
        size_t count = 0;
        OH_Drawing_FontDescriptor* matched = OH_Drawing_MatchFontDescriptors(desc, &count);
        OH_Drawing_DestroyFontDescriptors(matched, count);
        OH_Drawing_DestroyFontDescriptor(desc);
    }
    OH_Drawing_DestroySystemFontFullNames(descs);
}

void OHDrawingTextFontFullDescriptorTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);

    auto random = fdp.ConsumeRandomLengthString();
    auto streamArray = OH_Drawing_GetFontFullDescriptorsFromStream(random.data(), random.size());
    OH_Drawing_GetFontFullDescriptorsFromPath(random.data());
    auto pathArray = OH_Drawing_GetFontFullDescriptorsFromPath("/system/fonts/NotoSansCJK-Regular.ttc");

    auto streamDesc = OH_Drawing_GetFontFullDescriptorByIndex(streamArray, fdp.ConsumeIntegral<size_t>());
    auto pathDesc = OH_Drawing_GetFontFullDescriptorByIndex(pathArray, fdp.ConsumeIntegral<size_t>());

    OH_Drawing_String strAttr;
    auto id = static_cast<OH_Drawing_FontFullDescriptorAttributeId>(fdp.ConsumeIntegral<uint8_t>());
    OH_Drawing_GetFontFullDescriptorAttributeString(streamDesc, id, &strAttr);
    free(strAttr.strData);
    OH_Drawing_GetFontFullDescriptorAttributeString(pathDesc, id, &strAttr);
    free(strAttr.strData);

    int32_t intAttr;
    OH_Drawing_GetFontFullDescriptorAttributeInt(streamDesc, id, &intAttr);
    OH_Drawing_GetFontFullDescriptorAttributeInt(pathDesc, id, &intAttr);

    bool boolAttr;
    OH_Drawing_GetFontFullDescriptorAttributeBool(streamDesc, id, &boolAttr);
    OH_Drawing_GetFontFullDescriptorAttributeBool(pathDesc, id, &boolAttr);

    OH_Drawing_DestroyFontFullDescriptors(streamArray);
    OH_Drawing_DestroyFontFullDescriptors(pathArray);
}

void OHDrawingTextFontVariationAxisAttrsTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    auto pathArray = OH_Drawing_GetFontFullDescriptorsFromPath("/system/fonts/NotoSansCJK-Regular.ttc");
    auto desc = OH_Drawing_GetFontFullDescriptorByIndex(pathArray, fdp.ConsumeIntegral<size_t>());

    // Test variation axis attributes
    OH_Drawing_Array* axisArray =
        OH_Drawing_GetFontFullDescriptorAttributeArray(desc, FULL_DESCRIPTOR_ATTR_O_VARIATION_AXIS);
    size_t axisCount = OH_Drawing_GetDrawingArraySize(axisArray);
    for (size_t i = 0; i < axisCount; i++) {
        OH_Drawing_FontVariationAxis* axis = OH_Drawing_GetFontVariationAxisByIndex(axisArray, i);
        OH_Drawing_FontVariationAxisAttributeId axisId =
            static_cast<OH_Drawing_FontVariationAxisAttributeId>(fdp.ConsumeIntegral<uint8_t>());
        double doubleValue = 0.0;
        OH_Drawing_GetFontVariationAxisAttributeDouble(axis, axisId, &doubleValue);
        int intValue = 0;
        OH_Drawing_GetFontVariationAxisAttributeInt(axis, axisId, &intValue);
        OH_Drawing_String strValue = { nullptr, 0 };
        OH_Drawing_GetFontVariationAxisAttributeStr(axis, axisId, &strValue);
        free(strValue.strData);

        // Test with nullptr
        OH_Drawing_GetFontVariationAxisAttributeDouble(nullptr, axisId, &doubleValue);
        OH_Drawing_GetFontVariationAxisAttributeDouble(axis, axisId, nullptr);
        OH_Drawing_GetFontVariationAxisAttributeInt(nullptr, axisId, &intValue);
        OH_Drawing_GetFontVariationAxisAttributeInt(axis, axisId, nullptr);
        OH_Drawing_GetFontVariationAxisAttributeStr(nullptr, axisId, &strValue);
        OH_Drawing_GetFontVariationAxisAttributeStr(axis, axisId, nullptr);
    }
    OH_Drawing_DestroyFontVariationAxis(axisArray);

    // Test variation instance attributes
    OH_Drawing_Array* instanceArray =
        OH_Drawing_GetFontFullDescriptorAttributeArray(desc, FULL_DESCRIPTOR_ATTR_O_VARIATION_INSTANCE);
    size_t instanceCount = OH_Drawing_GetDrawingArraySize(instanceArray);
    for (size_t i = 0; i < instanceCount; i++) {
        OH_Drawing_FontVariationInstance* instance = OH_Drawing_GetFontVariationInstanceByIndex(instanceArray, i);
        OH_Drawing_FontVariationInstanceAttributeId instId =
            static_cast<OH_Drawing_FontVariationInstanceAttributeId>(fdp.ConsumeIntegral<uint8_t>());
        OH_Drawing_String strValue = { nullptr, 0 };
        OH_Drawing_GetFontVariationInstanceAttributeStr(instance, instId, &strValue);
        free(strValue.strData);

        // Test instance coordinate
        size_t coordLen = 0;
        OH_Drawing_FontVariationInstanceCoordinate* coord =
            OH_Drawing_GetFontVariationInstanceCoordinate(instance, &coordLen);

        // Test with nullptr
        OH_Drawing_GetFontVariationInstanceAttributeStr(nullptr, instId, &strValue);
        OH_Drawing_GetFontVariationInstanceAttributeStr(instance, instId, nullptr);
        free(coord);
    }
    OH_Drawing_DestroyFontVariationInstance(instanceArray);
    OH_Drawing_DestroyFontFullDescriptors(pathArray);
}

void OHDrawingTextFontPathTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);

    auto random = fdp.ConsumeIntegral<int32_t>();
    size_t num = 0;
    auto pathArray = OH_Drawing_GetFontPathsByType(static_cast<OH_Drawing_SystemFontType>(random), &num);

    for (size_t i = 0; i < num; i++) {
        OH_Drawing_String path = pathArray[i];
        free(path.strData);
    }
    free(pathArray);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHDrawingTextFontDescriptorTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingTextFontFullDescriptorTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingTextFontVariationAxisAttrsTest(data, size);
    OHOS::Rosen::Drawing::OHDrawingTextFontPathTest(data, size);
    return 0;
}
} // namespace OHOS::Rosen::Drawing