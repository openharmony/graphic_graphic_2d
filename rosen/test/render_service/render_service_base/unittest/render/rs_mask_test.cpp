/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "include/render/rs_mask.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaskTest::SetUpTestCase() {}
void RSMaskTest::TearDownTestCase() {}
void RSMaskTest::SetUp() {}
void RSMaskTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    SkPaint paint;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(paint);
    ASSERT_TRUE(mask != nullptr);

    ASSERT_TRUE(mask->IsGradientMask());
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    SkPaint paint;
    SkPath path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, paint);
    ASSERT_TRUE(mask != nullptr);

    ASSERT_TRUE(mask->IsPathMask());
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by SVG
     */
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(mask != nullptr);

    ASSERT_TRUE(mask->IsSvgMask());
}

/**
 * @tc.name: LifeCycle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    SkPaint paint;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(paint);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    ASSERT_TRUE(mask->GetMaskPaint() == paint);
}

/**
 * @tc.name: LifeCycle005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    SkPaint paint;
    SkPath path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, paint);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    ASSERT_TRUE(mask->GetMaskPath() == path);
}

/**
 * @tc.name: LifeCycle006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by SVG
     */
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    ASSERT_EQ(mask->GetSvgX(), x);
    ASSERT_EQ(mask->GetSvgY(), y);
    ASSERT_EQ(mask->GetScaleX(), scaleX);
    ASSERT_EQ(mask->GetScaleY(), scaleY);
}

/**
 * @tc.name: LifeCycle007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by SVG
     */
    double x = 0.0f;
    double y = 0.0f;
    double scaleX = 1.0f;
    double scaleY = 1.0f;
    sk_sp<SkSVGDOM> svgDom = nullptr;
    std::shared_ptr<RSMask> mask = RSMask::CreateSVGMask(x, y, scaleX, scaleY, svgDom);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    mask->SetSvgX(1.0f);
    ASSERT_EQ(mask->GetSvgX(), 1.0f);

    mask->SetSvgY(1.0f);
    ASSERT_EQ(mask->GetSvgY(), 1.0f);

    mask->SetScaleX(1.0f);
    ASSERT_EQ(mask->GetScaleX(), 1.0f);

    mask->SetScaleY(1.0f);
    ASSERT_EQ(mask->GetScaleY(), 1.0f);
}

/**
 * @tc.name: LifeCycle008
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Path
     */
    SkPaint paint;
    SkPath path;
    std::shared_ptr<RSMask> mask = RSMask::CreatePathMask(path, paint);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    SkPath path2;
    mask->SetMaskPath(path2);
    ASSERT_TRUE(mask->GetMaskPath() == path2);
}

/**
 * @tc.name: LifeCycle009
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaskTest, LifeCycle009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    SkPaint paint;
    std::shared_ptr<RSMask> mask = RSMask::CreateGradientMask(paint);
    ASSERT_TRUE(mask != nullptr);

    /**
     * @tc.steps: step2. check set value
     */
    SkPaint paint2;
    mask->SetMaskPaint(paint2);
    ASSERT_TRUE(mask->GetMaskPaint() == paint2);
}
} // namespace OHOS::Rosen