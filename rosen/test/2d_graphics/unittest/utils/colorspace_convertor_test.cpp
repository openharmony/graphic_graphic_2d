/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "utils/colorspace_convertor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorspaceConvertorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

static void CheckSkColorSpaceEqual(sk_sp<SkColorSpace> cs0, sk_sp<SkColorSpace> cs1)
{
    skcms_TransferFunction f0;
    cs0->transferFn(&f0);
    skcms_TransferFunction f1;
    cs1->transferFn(&f1);
    ASSERT_TRUE(ColorManager::FloatEqual(f0.g, f1.g));
    ASSERT_TRUE(ColorManager::FloatEqual(f0.a, f1.a));
    ASSERT_TRUE(ColorManager::FloatEqual(f0.b, f1.b));
    ASSERT_TRUE(ColorManager::FloatEqual(f0.c, f1.c));
    ASSERT_TRUE(ColorManager::FloatEqual(f0.d, f1.d));
    ASSERT_TRUE(ColorManager::FloatEqual(f0.e, f1.e));
    ASSERT_TRUE(ColorManager::FloatEqual(f0.f, f1.f));

    skcms_Matrix3x3 m0;
    cs0->toXYZD50(&m0);
    skcms_Matrix3x3 m1;
    cs1->toXYZD50(&m1);
    for (uint32_t i = 0; i < MATRIX3_SIZE; ++i) {
        for (uint32_t j = 0; j < MATRIX3_SIZE; ++j) {
            ASSERT_TRUE(ColorManager::FloatEqual(m0.vals[i][j], m1.vals[i][j]));
        }
    }
}
void ColorspaceConvertorTest::SetUpTestCase() {}
void ColorspaceConvertorTest::TearDownTestCase() {}
void ColorspaceConvertorTest::SetUp() {}
void ColorspaceConvertorTest::TearDown() {}

/**
 * @tc.name: ColorspaceConvertToDrawingColorspace001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:ICB15J
 * @tc.author:
 */
HWTEST_F(ColorspaceConvertorTest, ColorspaceConvertToDrawingColorspace001, TestSize.Level1)
{
    auto drawingColorSpace = Drawing::ColorSpaceConvertor::
        ColorSpaceConvertToDrawingColorSpace(nullptr);
    ASSERT_TRUE(drawingColorSpace == nullptr);
     
     // other value will crash when check.
    for (int i = ColorManager::NONE; i <= ColorManager::LINEAR_BT2020; i++) {
        auto colorspace = std::make_shared<ColorManager::ColorSpace>(static_cast<ColorManager::ColorSpaceName>(i));
        drawingColorSpace = Drawing::ColorSpaceConvertor::
                ColorSpaceConvertToDrawingColorSpace(colorspace);
        if (i != ColorManager::NONE && i != ColorManager::CUSTOM) {
            // None and Custom will crash when check.
            CheckSkColorSpaceEqual(drawingColorSpace->GetSkColorSpace(),
            colorspace->ToSkColorSpace());
        }
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS