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

#include "effect_filter_unittest.h"

#include "effect_filter.h"
#include "image/pixelmap_native.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

static void CreatePixelMap(OH_PixelmapNative*** pixelmap)
{
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    OH_PixelmapInitializationOptions_Create(&ops);
    // 2 means alphaType
    OH_PixelmapInitializationOptions_SetAlphaType(ops, 2);
    // 4 means height
    OH_PixelmapInitializationOptions_SetHeight(ops, 4);
    // 4 means width
    OH_PixelmapInitializationOptions_SetWidth(ops, 4);
    // 4 means pixelFormat
    OH_PixelmapInitializationOptions_SetPixelFormat(ops, 4);
    // 255 means rgba data
    uint8_t data[] = {
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255
    };
    // 16 means data length
    size_t dataLength = 16;
    OH_PixelmapNative_CreatePixelmap(data, dataLength, ops, *pixelmap);
}

/**
 * @tc.name: OH_Filter_CreateEffect
 * @tc.desc: Create a effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_CreateEffect001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_CreateEffect001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(nullptr, &filter) != EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, nullptr) != EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: OH_Filter_Invert
 * @tc.desc: Create a invert effect filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_Invert001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_Invert001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Invert(filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Invert(nullptr) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: OH_Filter_Blur
 * @tc.desc: Create a  blur effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_Blur001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_Blur001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    // 0.5 for test
    ASSERT_TRUE(OH_Filter_Blur(filter, 0.5f) == EFFECT_SUCCESS);
    // 0.5 for test
    ASSERT_TRUE(OH_Filter_Blur(nullptr, 0.5) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: OH_Filter_GrayScale
 * @tc.desc: Create a gray scale effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_GrayScale001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GrayScale001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_GrayScale(filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_GrayScale(nullptr) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: OH_Filter_Brighten
 * @tc.desc: Create a brighten effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_Brighten001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_Brighten001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    // 0.5 for test
    ASSERT_TRUE(OH_Filter_Brighten(filter, 0.5) == EFFECT_SUCCESS);
    // 0.5 for test
    ASSERT_TRUE(OH_Filter_Brighten(nullptr, 0.5) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: OH_Filter_SetColorMatrix
 * @tc.desc: Create a effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_SetColorMatrix001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_SetColorMatrix001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    // -1 means color matrix value for test
    OH_Filter_ColorMatrix matrix {
        -1.0, 0, 0, 0, 1,
        0, -1.0, 0, 0, 1,
        0, 0, -1.0, 0, 1,
        0, 0, 0, 1, 0
    };
    ASSERT_TRUE(OH_Filter_SetColorMatrix(filter, &matrix) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_SetColorMatrix(nullptr, &matrix) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: OH_Filter_GetEffectPixelMap
 * @tc.desc: Get a pixelmap created by the filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_GetEffectPixelMap001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GetEffectPixelMap001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Invert(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative*pixelMap1 = nullptr;
    ASSERT_TRUE(OH_Filter_GetEffectPixelMap(filter, &pixelMap1) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(pixelMap1);
}

/**
 * @tc.name: OH_Filter_Release
 * @tc.desc: Get a pixelmap created by the filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_Release001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_Release001 start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Invert(filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Release(nullptr) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}
} // namespace Rosen
} // namespace OHOS
