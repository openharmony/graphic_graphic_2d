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
            ASSERT_TRUE(drawingColorSpace != nullptr);
        }
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS