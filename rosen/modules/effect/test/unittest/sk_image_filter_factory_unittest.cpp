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

#include "sk_image_filter_factory_unittest.h"

#include "sk_image_filter_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: BlurTest001
 * @tc.desc: create a blur filter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, BlurTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest BlurTest001 start";
    // 0.5 blur radius for test
    auto filter = Rosen::SKImageFilterFactory::Blur(0.5);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: BlurTest002
 * @tc.desc: create a blur filter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, BlurTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest BlurTest002 start";
    // 0 blur radius for test
    auto filter = Rosen::SKImageFilterFactory::Blur(0);
    EXPECT_TRUE(filter == nullptr);
}

/**
 * @tc.name: BrightnessTest001
 * @tc.desc: create a brightness filter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, BrightnessTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest BrightnessTest001 start";
    // 0.5 brightness for test
    auto filter = Rosen::SKImageFilterFactory::Brightness(0.5);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: BrightnessTest002
 * @tc.desc: create a brightness filter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, BrightnessTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest BrightnessTest002 start";
    // -1 brightness for test
    auto filter = Rosen::SKImageFilterFactory::Brightness(-1);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: GrayscaleTest001
 * @tc.desc: create a grayscale filter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, GrayscaleTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest GrayscaleTest001 start";
    auto filter = Rosen::SKImageFilterFactory::Grayscale();
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: InvertTest001
 * @tc.desc: create a invert filter
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, InvertTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest InvertTest001 start";
    auto filter = Rosen::SKImageFilterFactory::Invert();
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest001
 * @tc.desc: create a custom filter with a default matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest001 start";
    struct PixelColorMatrix matrix;
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(matrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest002
 * @tc.desc: create a filter with a grayscale matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest002 start";
    struct PixelColorMatrix colorMatrix;
    // grayscale matrix
    // [0.2126, 0.7152, 0.0722, 0, 0, 0.2126, 0.7152, 0.0722, 0, 0, 0.2126, 0.7152, 0.0722, 0, 0, 0, 0, 0, 1, 0]
    // for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        0.2126, 0.7152, 0.0722, 0, 0,
        0.2126, 0.7152, 0.0722, 0, 0,
        0.2126, 0.7152, 0.0722, 0, 0,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest003
 * @tc.desc: create a filter with a brightness matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest003 start";
    // 0.5 for test
    float degree = 0.5;
    struct PixelColorMatrix colorMatrix;
    // brightness matrix for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        1, 0, 0, 0, degree,
        0, 1, 0, 0, degree,
        0, 0, 1, 0, degree,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest004
 * @tc.desc: create a filter with a brightness matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest004 start";
    // -1.0 for test
    float degree = -1.0;
    struct PixelColorMatrix colorMatrix;
    // brightness matrix for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        1, 0, 0, 0, degree,
        0, 1, 0, 0, degree,
        0, 0, 1, 0, degree,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest005
 * @tc.desc: create a filter with a brightness matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest005, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest005 start";
    // 1.0 for test
    float degree = 1.0;
    struct PixelColorMatrix colorMatrix;
    // brightness matrix for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        1, 0, 0, 0, degree,
        0, 1, 0, 0, degree,
        0, 0, 1, 0, degree,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest006
 * @tc.desc: create a filter with a invert matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest006, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest006 start";
    struct PixelColorMatrix colorMatrix;
    // invert matrix
    // [-1.0, 0, 0, 0, 1, 0, -1.0, 0, 0, 1, 0, 0, -1.0, 0, 1, 0, 0, 0, 1, 0]
    // for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        -1.0, 0, 0, 0, 1,
        0, -1.0, 0, 0, 1,
        0, 0, -1.0, 0, 1,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest007
 * @tc.desc: create a filter with a increase matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest007, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest007 start";
    struct PixelColorMatrix colorMatrix;
    // increase matrix
    // [1.5, 0, 0, 0, -0.25, 0, 1.5, 0, 0, -0.25, 0, 0, 1.5, 0, -0.25, 0, 0, 0, 1, 0]
    // for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        1.5, 0, 0, 0, -0.25,
        0, 1.5, 0, 0, -0.25,
        0, 0, 1.5, 0, -0.25,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: ApplyColorMatrixTest008
 * @tc.desc: create a filter with a decrease matrix
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SKImageFilterFactoryUnittest, ApplyColorMatrixTest008, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SKImageFilterFactoryUnittest ApplyColorMatrixTest008 start";
    struct PixelColorMatrix colorMatrix;
    // decrease matrix
    // [0.6065, 0.3575, 0.036, 0, 0, 0.1065, 0.8575, 0.036, 0, 0, 0.1065, 0.3575, 0.536, 0, 0, 0, 0, 0, 1, 0]
    // for test
    float matrix[colorMatrix.MATRIX_SIZE] = {
        0.6065, 0.3575, 0.036, 0, 0,
        0.1065, 0.8575, 0.036, 0, 0,
        0.1065, 0.3575, 0.536, 0, 0,
        0, 0, 0, 1, 0
    };
    for (int i = 0; i < colorMatrix.MATRIX_SIZE; ++i) {
        colorMatrix.val[i] = matrix[i];
    }
    auto filter = Rosen::SKImageFilterFactory::ApplyColorMatrix(colorMatrix);
    EXPECT_TRUE(filter != nullptr);
}
} // namespace Rosen
} // namespace OHOS