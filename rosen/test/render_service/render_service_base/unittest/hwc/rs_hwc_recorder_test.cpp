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

#include "gtest/gtest.h"

#include "params/rs_render_params.h"
#include "hwc/rs_hwc_recorder.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

#include "platform/common/rs_log.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
const RectI DEFAULT_RECT = {0, 0, 100, 100};
} // namespace

class RSHwcRecorderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHwcRecorderTest::SetUpTestCase() {}
void RSHwcRecorderTest::TearDownTestCase() {}
void RSHwcRecorderTest::SetUp() {}
void RSHwcRecorderTest::TearDown() {}

/**
 * @tc.name: SetBlendWithBackground_001
 * @tc.desc: test SetBlendWithBackground
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcRecorderTest, SetBlendWithBackground_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetHwcRecorder().SetBlendWithBackground(true);
    ASSERT_TRUE(node.GetHwcRecorder().isBlendWithBackground_);
}

/**
 * @tc.name: IsBlendWithBackground_001
 * @tc.desc: test IsBlendWithBackground
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcRecorderTest, IsBlendWithBackground_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetHwcRecorder().SetBlendWithBackground(true);
    ASSERT_TRUE(node.GetHwcRecorder().IsBlendWithBackground());
}

/**
 * @tc.name: SetForegroundColorValid_001
 * @tc.desc: test SetForegroundColorValid
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcRecorderTest, SetForegroundColorValid_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetHwcRecorder().SetForegroundColorValid(true);
    ASSERT_TRUE(node.GetHwcRecorder().isForegroundColorValid_);
}

/**
 * @tc.name: IsForegroundColorValid_001
 * @tc.desc: test IsForegroundColorValid
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcRecorderTest, IsForegroundColorValid_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.GetHwcRecorder().SetForegroundColorValid(true);
    ASSERT_TRUE(node.GetHwcRecorder().IsForegroundColorValid());
}

/**
 * @tc.name: SetIntersectWithPreviousFilter_001
 * @tc.desc: test SetIntersectWithPreviousFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcRecorderTest, SetIntersectWithPreviousFilter_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
    ASSERT_TRUE(node.HwcSurfaceRecorder().isIntersectWithPreviousFilter_);
}

/**
 * @tc.name: IsIntersectWithPreviousFilter_001
 * @tc.desc: test IsIntersectWithPreviousFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcRecorderTest, IsIntersectWithPreviousFilter_001, TestSize.Level1)
{
    NodeId id = 0;
    RSSurfaceRenderNode node(id);
    node.HwcSurfaceRecorder().SetIntersectWithPreviousFilter(true);
    ASSERT_TRUE(node.HwcSurfaceRecorder().IsIntersectWithPreviousFilter());
}

} //namespace OHOS::Rosen
