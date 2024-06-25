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
#include <parameter.h>
#include <parameters.h>

#include "inner_event.h"
#include "gtest/gtest.h"

#include "render/rs_maskcolor_shader_filter.h"
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSMaskColorShaderFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskColorShaderFilterTest::SetUpTestCase() {}
void RSMaskColorShaderFilterTest::TearDownTestCase() {}
void RSMaskColorShaderFilterTest::SetUp() {}
void RSMaskColorShaderFilterTest::TearDown() {}

/**
 * @tc.name: InitColorModTest
 * @tc.desc: Verify function InitColorMod
 * @tc.type:FUNC
 * @tc.require: issuesI9PH4G
 */
HWTEST_F(RSMaskColorShaderFilterTest, InitColorModTest, TestSize.Level1)
{
    RSColor color;
    auto rsMaskColorShaderFilterOne = std::make_shared<RSMaskColorShaderFilter>(1, color);
    rsMaskColorShaderFilterOne->InitColorMod();
    auto rsMaskColorShaderFilterTwo = std::make_shared<RSMaskColorShaderFilter>(2, color);
    rsMaskColorShaderFilterTwo->InitColorMod();
    EXPECT_EQ(rsMaskColorShaderFilterTwo->colorPickerTask_, nullptr);
}

/**
 * @tc.name: CaclMaskColorTest
 * @tc.desc: Verify function CaclMaskColor
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSMaskColorShaderFilterTest, CaclMaskColorTest, TestSize.Level1)
{
    RSColor color;
    auto rsMaskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(1, color);
    auto rsMaskColorShaderFilterSecond = std::make_shared<RSMaskColorShaderFilter>(2, color);
    std::shared_ptr<Drawing::Image> image = nullptr;
    rsMaskColorShaderFilter->CaclMaskColor(image);
    rsMaskColorShaderFilterSecond->CaclMaskColor(image);
    image = std::make_shared<Drawing::Image>();
    rsMaskColorShaderFilterSecond->InitColorMod();
    rsMaskColorShaderFilter->CaclMaskColor(image);
    rsMaskColorShaderFilterSecond->CaclMaskColor(image);
    if (rsMaskColorShaderFilterSecond->colorPickerTask_ != nullptr) {
        rsMaskColorShaderFilterSecond->colorPickerTask_->waitRelease_->store(true);
        rsMaskColorShaderFilterSecond->CaclMaskColor(image);
        rsMaskColorShaderFilterSecond->colorPickerTask_->valid_ = true;
        rsMaskColorShaderFilterSecond->CaclMaskColor(image);
        rsMaskColorShaderFilterSecond->colorPickerTask_->firstGetColorFinished_ = true;
        rsMaskColorShaderFilterSecond->CaclMaskColor(image);
    }
    EXPECT_EQ(rsMaskColorShaderFilter->colorMode_, 1);
}

/**
 * @tc.name: PostProcessTest
 * @tc.desc: Verify function PostProcess
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSMaskColorShaderFilterTest, PostProcessTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSColor color;
    auto rsMaskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(1, color);
    rsMaskColorShaderFilter->PostProcess(canvas);
    EXPECT_EQ(rsMaskColorShaderFilter->colorPickerTask_, nullptr);
}

/**
 * @tc.name: ReleaseColorPickerFilterTest
 * @tc.desc: Verify function ReleaseColorPickerFilter
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSMaskColorShaderFilterTest, ReleaseColorPickerFilterTest, TestSize.Level1)
{
    RSColor color;
    auto rsMaskColorShaderFilter = std::make_shared<RSMaskColorShaderFilter>(1, color);
    rsMaskColorShaderFilter->ReleaseColorPickerFilter();
    EXPECT_EQ(rsMaskColorShaderFilter->colorPickerTask_, nullptr);
    rsMaskColorShaderFilter->colorMode_ = 2;
    rsMaskColorShaderFilter->InitColorMod();
    rsMaskColorShaderFilter->ReleaseColorPickerFilter();
    EXPECT_EQ(rsMaskColorShaderFilter->colorMode_, 2);
}
} // namespace Rosen
} // namespace OHOS