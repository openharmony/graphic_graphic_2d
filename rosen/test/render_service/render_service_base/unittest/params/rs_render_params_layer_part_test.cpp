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

/**
 * @tc.name: RSRenderParamsLayerPartBranchTest
 * @tc.desc: Branch coverage tests for layer part render params (commit 266e787e55)
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */

#include "gtest/gtest.h"
#include "params/rs_render_params.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSRenderParamsLayerPartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderParamsLayerPartTest::SetUpTestCase() {}
void RSRenderParamsLayerPartTest::TearDownTestCase() {}
void RSRenderParamsLayerPartTest::SetUp() {}
void RSRenderParamsLayerPartTest::TearDown() {}

/**
 * @tc.name: SetLayerPartRenderEnabledNoChangeBranch
 * @tc.desc: Test SetLayerPartRenderEnabled when value is same (branch coverage)
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSRenderParamsLayerPartTest, SetLayerPartRenderEnabledNoChangeBranch, TestSize.Level1)
{
    constexpr NodeId id = 1;
    RSRenderParams params(id);

    // Initial state
    params.needSync_ = false;
    params.isLayerPartRenderEnable_ = false;

    // Test branch: value is same, should not set needSync
    params.SetLayerPartRenderEnabled(false);
    ASSERT_FALSE(params.needSync_);

    // Now set to true (should change)
    params.needSync_ = false;
    params.SetLayerPartRenderEnabled(true);
    ASSERT_TRUE(params.needSync_);
    ASSERT_TRUE(params.isLayerPartRenderEnable_);

    // Test branch: value is same again
    params.needSync_ = false;
    params.SetLayerPartRenderEnabled(true);
    ASSERT_FALSE(params.needSync_);

    // Change to false again
    params.needSync_ = false;
    params.SetLayerPartRenderEnabled(false);
    ASSERT_TRUE(params.needSync_);
    ASSERT_FALSE(params.isLayerPartRenderEnable_);
}

/**
 * @tc.name: SetLayerPartRenderCurrentFrameDirtyRegionNoChangeBranch
 * @tc.desc: Test SetLayerPartRenderCurrentFrameDirtyRegion when value is same (branch coverage)
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSRenderParamsLayerPartTest, SetLayerPartRenderCurrentFrameDirtyRegionNoChangeBranch, TestSize.Level1)
{
    constexpr NodeId id = 1;
    RSRenderParams params(id);

    RectI testRect1 = {10, 10, 100, 100};
    RectI testRect2 = {20, 20, 200, 200};

    // Initial state
    params.needSync_ = false;
    params.layerPartRenderCurrentFrameDirtyRegion_ = testRect1;

    // Test branch: value is same, should not set needSync
    params.SetLayerPartRenderCurrentFrameDirtyRegion(testRect1);
    ASSERT_FALSE(params.needSync_);

    // Now set to different value (should change)
    params.needSync_ = false;
    params.SetLayerPartRenderCurrentFrameDirtyRegion(testRect2);
    ASSERT_TRUE(params.needSync_);
    ASSERT_EQ(params.layerPartRenderCurrentFrameDirtyRegion_, testRect2);

    // Test branch: value is same again
    params.needSync_ = false;
    params.SetLayerPartRenderCurrentFrameDirtyRegion(testRect2);
    ASSERT_FALSE(params.needSync_);

    // Change to different value again
    params.needSync_ = false;
    params.SetLayerPartRenderCurrentFrameDirtyRegion(testRect1);
    ASSERT_TRUE(params.needSync_);
    ASSERT_EQ(params.layerPartRenderCurrentFrameDirtyRegion_, testRect1);
}

/**
 * @tc.name: SetLayerPartRenderEnabledWithEmptyDirtyRegion
 * @tc.desc: Test interaction between enable flag and dirty region
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSRenderParamsLayerPartTest, SetLayerPartRenderEnabledWithEmptyDirtyRegion, TestSize.Level1)
{
    constexpr NodeId id = 1;
    RSRenderParams params(id);

    // Enable with empty dirty region
    params.SetLayerPartRenderEnabled(true);
    ASSERT_TRUE(params.GetLayerPartRenderEnabled());

    RectI emptyRect = params.GetLayerPartRenderCurrentFrameDirtyRegion();
    ASSERT_TRUE(emptyRect.IsEmpty());

    // Set dirty region
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);
    ASSERT_EQ(params.GetLayerPartRenderCurrentFrameDirtyRegion(), dirtyRect);

    // Disable feature but dirty region should remain
    params.SetLayerPartRenderEnabled(false);
    ASSERT_FALSE(params.GetLayerPartRenderEnabled());
    ASSERT_EQ(params.GetLayerPartRenderCurrentFrameDirtyRegion(), dirtyRect);
}

/**
 * @tc.name: LayerPartRenderParamsSyncTest
 * @tc.desc: Test sync behavior of layer part render params
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSRenderParamsLayerPartTest, LayerPartRenderParamsSyncTest, TestSize.Level1)
{
    constexpr NodeId id = 1;
    RSRenderParams params(id);
    auto target = std::make_unique<RSRenderParams>(id);
    auto renderParams = static_cast<RSRenderParams*>(target.get());

    // Set up source params
    params.SetLayerPartRenderEnabled(true);
    RectI dirtyRect = {10, 10, 100, 100};
    params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);

    // Sync to target
    params.OnSync(target);

    // Verify target has the same values
    ASSERT_TRUE(renderParams->isLayerPartRenderEnable_);
    ASSERT_EQ(renderParams->layerPartRenderCurrentFrameDirtyRegion_, dirtyRect);
}

/**
 * @tc.name: LayerPartRenderParamsMultipleChanges
 * @tc.desc: Test multiple changes to layer part render params
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSRenderParamsLayerPartTest, LayerPartRenderParamsMultipleChanges, TestSize.Level1)
{
    constexpr NodeId id = 1;
    RSRenderParams params(id);

    // Multiple enable/disable cycles
    for (int i = 0; i < 3; ++i) {
        params.needSync_ = false;

        // Enable
        params.SetLayerPartRenderEnabled(true);
        ASSERT_TRUE(params.needSync_);

        // Set dirty region
        params.needSync_ = false;
        RectI dirtyRect = {i * 10, i * 10, 100, 100};
        params.SetLayerPartRenderCurrentFrameDirtyRegion(dirtyRect);
        ASSERT_TRUE(params.needSync_);

        // Disable
        params.needSync_ = false;
        params.SetLayerPartRenderEnabled(false);
        ASSERT_TRUE(params.needSync_);
    }
}

} // namespace OHOS::Rosen
