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
 * @tc.name: OH_Filter_Blur
 * @tc.desc: Create a  blur effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_Blur002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_Blur002 start";

    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    // -0.5 for test
    ASSERT_TRUE(OH_Filter_Blur(filter, -0.5f) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);

    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_Blur002 start";
}

/**
 * @tc.name: OH_Filter_Blur
 * @tc.desc: Create a blur effect filter with tile mode.
 * @tc.type: FUNC
 * @tc.require: IB3UJ4
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_BlurWithTileMode, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_BlurWithTileMode start";
    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwise can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    // 0.5 for test
    ASSERT_TRUE(OH_Filter_BlurWithTileMode(filter, 0.5f, CLAMP) == EFFECT_SUCCESS);
    // 0.5 for test
    ASSERT_TRUE(OH_Filter_BlurWithTileMode(nullptr, 0.5, CLAMP) == EFFECT_BAD_PARAMETER);
    // -1 for test
    ASSERT_TRUE(OH_Filter_BlurWithTileMode(filter, -1.0, CLAMP) == EFFECT_BAD_PARAMETER);
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
    ASSERT_TRUE(OH_Filter_GetEffectPixelMap(nullptr, &pixelMap1) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(pixelMap1);
}

/**
 * @tc.name: OH_Filter_GetEffectPixelMap
 * @tc.desc: Get a pixelmap created by the filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_GetEffectPixelMap002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GetEffectPixelMap002 start";

    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    OH_PixelmapNative*pixelMap1 = nullptr;
    ASSERT_TRUE(OH_Filter_GetEffectPixelMap(nullptr, &pixelMap1) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(pixelMap1);

    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GetEffectPixelMap002 start";
}

/**
 * @tc.name: OH_Filter_GetEffectPixelMap003
 * @tc.desc: Get a pixelmap created by the filter.
 * @tc.type: FUNC
 * @tc.require: I9CSQ0
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_GetEffectPixelMap003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GetEffectPixelMap003 start";
    ASSERT_TRUE(OH_Filter_GetEffectPixelMap(nullptr, nullptr) == EFFECT_BAD_PARAMETER);

    OH_PixelmapNative *pixelMap1 = nullptr;
    ASSERT_TRUE(OH_Filter_GetEffectPixelMap(nullptr, &pixelMap1) == EFFECT_BAD_PARAMETER);
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

/**
 * @tc.name: OH_Filter_BlurWithDirection
 * @tc.desc: Create a blur effect filter with direction.
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_BlurWithDirectionTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_BlurWithDirectionTest start";

    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    
    // test valid input
    float radius = 2.0f; // 2.0f is valid value
    float angle = 50.0f; // 50.0f is valid value
    EXPECT_EQ(OH_Filter_BlurWithDirection(filter, radius, angle, EffectTileMode::CLAMP), EFFECT_SUCCESS);

    // -0.5f test radius is invalid
    radius = -0.5f;
    EXPECT_EQ(OH_Filter_BlurWithDirection(filter, radius, angle, EffectTileMode::CLAMP), EFFECT_BAD_PARAMETER);

    // test filter is nullptr
    EXPECT_EQ(OH_Filter_BlurWithDirection(nullptr, radius, angle, EffectTileMode::CLAMP), EFFECT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_BlurWithDirectionTest end";
}

/**
 * @tc.name: OH_Filter_MapColorByBrightnessTest
 * @tc.desc: Create a OH_Filter_MapColorByBrightness effect filter with pixelmap.
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MapColorByBrightnessTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MapColorByBrightnessTest start";

    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // test only set colors
    OH_Filter_MapColorByBrightnessParams params;
    OH_Filter_Color color1 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    OH_Filter_Color color2 = {1.0f, 0.0f, 0.5f, 1.0f}; // color rgba
    OH_Filter_Color color3 = {1.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    OH_Filter_Color color4 = {0.0f, 0.5f, 0.5f, 1.0f}; // color rgba
    OH_Filter_Color colors[4] = {color1, color2, color3, color4};
    params.colors = colors;
    params.positions = nullptr;
    params.colorsNum = 0;
    EXPECT_EQ(OH_Filter_MapColorByBrightness(filter, &params), EFFECT_BAD_PARAMETER);

    // test the value for position width greater than 0 and less than 0
    float positions[4] = { 0.2f, -0.5f, 1.0f, -1.5f};
    params.positions = positions;
    params.colorsNum = 4;
    EXPECT_EQ(OH_Filter_MapColorByBrightness(filter, &params), EFFECT_SUCCESS);

    // test input is nullptr
    EXPECT_EQ(OH_Filter_MapColorByBrightness(nullptr, &params), EFFECT_BAD_PARAMETER);
    EXPECT_EQ(OH_Filter_MapColorByBrightness(filter, nullptr), EFFECT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MapColorByBrightnessTest end";
}

/**
 * @tc.name: OH_Filter_GammaCorrection
 * @tc.desc: Create a gamma correction effect filter with direction.
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_GammaCorrectionTest, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GammaCorrectionTest start";

    OH_PixelmapNative *pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    
    // test valid input
    float gamma = 0.5f;
    EXPECT_EQ(OH_Filter_GammaCorrection(filter, gamma), EFFECT_SUCCESS);

    // -0.5f test gamma is invalid
    gamma = -0.5f;
    EXPECT_EQ(OH_Filter_GammaCorrection(filter, gamma), EFFECT_BAD_PARAMETER);

    // test filter is nullptr
    EXPECT_EQ(OH_Filter_GammaCorrection(nullptr, gamma), EFFECT_BAD_PARAMETER);

    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_GammaCorrectionTest end";
}

/**
 * @tc.name: OH_Filter_MaskTransition001
 * @tc.desc: Test MaskTransition with null parameters.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MaskTransition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MaskTransition001 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // Test null filter
    OH_Filter_Vec2 fractionStops[] = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    OH_Filter_LinearGradientMask linearMask = {{0.0f, 0.0f}, {1.0f, 1.0f}, fractionStops, 2};
    EXPECT_TRUE(OH_Filter_MaskTransition(nullptr, *transPixMap, &linearMask,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    // Test null transition image
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, nullptr, &linearMask,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    // Test null mask
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, nullptr,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_MaskTransition002
 * @tc.desc: Test MaskTransition with linear gradient mask with invalid fractionStopsLength.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MaskTransition002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MaskTransition002 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // Test invalid fractionStopsLength (less than minimum)
    OH_Filter_Vec2 fractionStops1[] = {{0.0f, 0.0f}};
    OH_Filter_LinearGradientMask linearMask1 = {{0.0f, 0.0f}, {1.0f, 1.0f}, fractionStops1, 1};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &linearMask1,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    // Test invalid fractionStopsLength (more than maximum)
    OH_Filter_Vec2 fractionStops2[13];
    for (int i = 0; i < 13; i++) {
        fractionStops2[i] = {static_cast<float>(i) / 12.0f, static_cast<float>(i) / 12.0f};
    }
    OH_Filter_LinearGradientMask linearMask2 = {{0.0f, 0.0f}, {1.0f, 1.0f}, fractionStops2, 13};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &linearMask2,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_MaskTransition003
 * @tc.desc: Test MaskTransition with radial gradient mask with invalid fractionStopsLength.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MaskTransition003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MaskTransition003 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // Test invalid fractionStopsLength
    OH_Filter_Vec2 fractionStops1[] = {{0.0f, 0.0f}};
    OH_Filter_RadialGradientMask radialMask1 = {{0.5f, 0.5f}, 1.0f, 1.0f, fractionStops1, 1};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &radialMask1,
        EffectMaskType::RADIAL_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    // Test invalid fractionStopsLength (more than maximum)
    OH_Filter_Vec2 fractionStops2[13];
    for (int i = 0; i < 13; i++) {
        fractionStops2[i] = {static_cast<float>(i) / 12.0f, static_cast<float>(i) / 12.0f};
    }
    OH_Filter_RadialGradientMask radialMask2 = {{0.5f, 0.5f}, 1.0f, 1.0f, fractionStops2, 13};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &radialMask2,
        EffectMaskType::RADIAL_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_MaskTransition004
 * @tc.desc: Test MaskTransition with null fractionStops pointer.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MaskTransition004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MaskTransition004 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // Test null fractionStops for linear gradient
    OH_Filter_LinearGradientMask linearMask = {{0.0f, 0.0f}, {1.0f, 1.0f}, nullptr, 2};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &linearMask,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    // Test null fractionStops for radial gradient
    OH_Filter_RadialGradientMask radialMask = {{0.5f, 0.5f}, 1.0f, 1.0f, nullptr, 2};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &radialMask,
        EffectMaskType::RADIAL_GRADIENT_MASK, 0.5f, false) == EFFECT_BAD_PARAMETER);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_MaskTransition005
 * @tc.desc: Test MaskTransition with invalid mask type.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MaskTransition005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MaskTransition005 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    OH_Filter_Vec2 fractionStops[] = {{0.0f, 0.0f}, {1.0f, 1.0f}};
    OH_Filter_LinearGradientMask linearMask = {{0.0f, 0.0f}, {1.0f, 1.0f}, fractionStops, 2};
    // Test MAX (invalid) mask type
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &linearMask,
        EffectMaskType::MAX, 0.5f, false) == EFFECT_BAD_PARAMETER);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_MaskTransition006
 * @tc.desc: Test MaskTransition with valid parameters for linear gradient.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_MaskTransition006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_MaskTransition006 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // Test valid linear gradient mask with increasing y values
    OH_Filter_Vec2 fractionStops[] = {{0.0f, 0.0f}, {0.5f, 0.5f}, {1.0f, 1.0f}};
    OH_Filter_LinearGradientMask linearMask = {{0.0f, 0.0f}, {1.0f, 1.0f}, fractionStops, 3};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &linearMask,
        EffectMaskType::LINEAR_GRADIENT_MASK, 0.5f, false) == EFFECT_SUCCESS);

    // Test valid radial gradient mask with increasing y values
    OH_Filter_RadialGradientMask radialMask = {{0.5f, 0.5f}, 1.0f, 1.0f, fractionStops, 3};
    EXPECT_TRUE(OH_Filter_MaskTransition(filter, *transPixMap, &radialMask,
        EffectMaskType::RADIAL_GRADIENT_MASK, 0.5f, true) == EFFECT_SUCCESS);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_WaterDropletTransition001
 * @tc.desc: Test WaterDropletTransition with null parameters.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_WaterDropletTransition001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_WaterDropletTransition001 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    OH_Filter_WaterDropletParams params = {
        5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f
    };

    // Test null filter
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(nullptr, *transPixMap, &params, false)
        == EFFECT_BAD_PARAMETER);

    // Test null transition image
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, nullptr, &params, false)
        == EFFECT_BAD_PARAMETER);

    // Test null waterDropletParams
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, nullptr, false)
        == EFFECT_BAD_PARAMETER);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_WaterDropletTransition002
 * @tc.desc: Test WaterDropletTransition with parameter clamping (negative and out of range values).
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_WaterDropletTransition002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_WaterDropletTransition002 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    // Test with negative progress (should be clamped to 0.0f)
    OH_Filter_WaterDropletParams params1 = {
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f
    };
    // Should succeed due to clamping
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params1, false) == EFFECT_SUCCESS);

    // Test with out of range radius
    OH_Filter_WaterDropletParams params2 = {
        0.5f, 20.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f
    };
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params2, false) == EFFECT_SUCCESS);

    // Test with out of range transitionFadeWidth
    OH_Filter_WaterDropletParams params3 = {
        0.5f, 5.0f, 2.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f
    };
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params3, false) == EFFECT_SUCCESS);

    // Test with out of range distortionIntensity
    OH_Filter_WaterDropletParams params4 = {
        0.5f, 5.0f, 0.5f, 2.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f
    };
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params4, false) == EFFECT_SUCCESS);

    // Test with out of range noise
    OH_Filter_WaterDropletParams params5 = {
        0.5f, 5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 20.0f, 20.0f, 20.0f, 20.0f
    };
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params5, false) == EFFECT_SUCCESS);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: OH_Filter_WaterDropletTransition003
 * @tc.desc: Test WaterDropletTransition with inverse parameter.
 * @tc.type: FUNC
 * @tc.require: #22077
 * @tc.author: yangjh7799
 */
HWTEST_F(EffectFilterUnittest, OH_Filter_WaterDropletTransition003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "EffectFilterUnittest OH_Filter_WaterDropletTransition003 start";
    OH_PixelmapNative *pixmap = nullptr;
    OH_PixelmapNative ** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);

    OH_PixelmapNative *transitionPixmap = nullptr;
    OH_PixelmapNative ** transPixMap = &transitionPixmap;
    CreatePixelMap(&transPixMap);
    ASSERT_TRUE(*transPixMap != nullptr);

    OH_Filter *filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    OH_Filter_WaterDropletParams params = {
        5.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f
    };

    // Test with inverse = true
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params, true) == EFFECT_SUCCESS);

    // Test with inverse = false
    EXPECT_TRUE(OH_Filter_WaterDropletTransition(filter, *transPixMap, &params, false) == EFFECT_SUCCESS);

    EXPECT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
    OH_PixelmapNative_Release(*transPixMap);
}

/**
 * @tc.name: WaterGlassNullParameters
 * @tc.desc: Create a water glass effect filter with null parameters.
 * @tc.type: FUNC
 */
HWTEST_F(EffectFilterUnittest, WaterGlassNullParameters, TestSize.Level1)
{
    OH_PixelmapNative* pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter* filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_WaterGlass(nullptr, nullptr) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_WaterGlass(filter, nullptr) == EFFECT_BAD_PARAMETER);
    OH_Filter_WaterGlassDataParams params;
    ASSERT_TRUE(OH_Filter_WaterGlass(nullptr, &params) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: WaterGlassValidParameters
 * @tc.desc: Create a water glass effect filter with valid parameters.
 * @tc.type: FUNC
 */
HWTEST_F(EffectFilterUnittest, WaterGlassValidParameters, TestSize.Level1)
{
    OH_PixelmapNative* pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter* filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);
    OH_Filter_WaterGlassDataParams params;
    ASSERT_TRUE(OH_Filter_WaterGlass(filter, &params) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: ReededGlassNullParameters
 * @tc.desc: Create a reeded glass effect filter with null parameters.
 * @tc.type: FUNC
 */
HWTEST_F(EffectFilterUnittest, ReededGlassNullParameters, TestSize.Level1)
{
    OH_PixelmapNative* pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter* filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    ASSERT_TRUE(OH_Filter_ReededGlass(nullptr, nullptr) == EFFECT_BAD_PARAMETER);
    ASSERT_TRUE(OH_Filter_ReededGlass(filter, nullptr) == EFFECT_BAD_PARAMETER);
    OH_Filter_ReededGlassDataParams params;
    ASSERT_TRUE(OH_Filter_ReededGlass(nullptr, &params) == EFFECT_BAD_PARAMETER);

    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}

/**
 * @tc.name: ReededGlassValidParameters
 * @tc.desc: Create a reeded glass effect filter with valid parameters.
 * @tc.type: FUNC
 */
HWTEST_F(EffectFilterUnittest, ReededGlassValidParameters, TestSize.Level1)
{
    OH_PixelmapNative* pixmap = nullptr;
    /** pixmap is necessary, otherwize can not create pixelmap*/
    OH_PixelmapNative** pixMap = &pixmap;
    CreatePixelMap(&pixMap);
    ASSERT_TRUE(*pixMap != nullptr);
    OH_Filter* filter = nullptr;
    ASSERT_TRUE(OH_Filter_CreateEffect(*pixMap, &filter) == EFFECT_SUCCESS);

    OH_Filter_ReededGlassDataParams params;
    ASSERT_TRUE(OH_Filter_ReededGlass(filter, &params) == EFFECT_SUCCESS);
    ASSERT_TRUE(OH_Filter_Release(filter) == EFFECT_SUCCESS);
    OH_PixelmapNative_Release(*pixMap);
}
} // namespace Rosen
} // namespace OHOS
