/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_opinc_cache.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSOpincCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOpincCacheTest::SetUpTestCase() {}
void RSOpincCacheTest::TearDownTestCase() {}
void RSOpincCacheTest::SetUp() {}
void RSOpincCacheTest::TearDown() {}

/**
 * @tc.name: UpdateSubTreeSupportFlag
 * @tc.desc: test results of UpdateSubTreeSupportFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, UpdateSubTreeSupportFlag, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, false, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, false, true);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, true, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, false, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, true, false);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(false, true, true);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, false, true);
    ASSERT_TRUE(opincCache.GetSubTreeSupportFlag());

    opincCache.subTreeSupportFlag_ = true;
    opincCache.UpdateSubTreeSupportFlag(true, true, true);
    ASSERT_FALSE(opincCache.GetSubTreeSupportFlag());
}

/**
 * @tc.name: SetCurNodeTreeSupportFlag
 * @tc.desc: test results of SetCurNodeTreeSupportFlag
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSOpincCacheTest, SetCurNodeTreeSupportFlag, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    opincCache.SetCurNodeTreeSupportFlag(true);
    ASSERT_TRUE(opincCache.GetCurNodeTreeSupportFlag());
}

/**
 * @tc.name: OpincSetInAppStateStartEnd001
 * @tc.desc: test results of OpincSetInAppStateStart/End
 * @tc.type: FUNC
 * @tc.require: issueInAppFlagMove
 */
HWTEST_F(RSOpincCacheTest, OpincSetInAppStateStartEnd001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    bool unchangeMarkInApp = false;
    opincCache.OpincSetInAppStateStart(unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    opincCache.OpincSetInAppStateEnd(unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);

    opincCache.OpincSetInAppStateEnd(unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);
}

/**
 * @tc.name: OpincSetInAppStateStartEndCrossNode001
 * @tc.desc: verify start/end pairing keeps ownership on the node that started marking
 * @tc.type: FUNC
 * @tc.require: issueInAppFlagMove
 */
HWTEST_F(RSOpincCacheTest, OpincSetInAppStateStartEndCrossNode001, TestSize.Level1)
{
    RSRenderNode renderNodeA(0);
    RSRenderNode renderNodeB(1);
    auto& opincCacheA = renderNodeA.GetOpincCache();
    auto& opincCacheB = renderNodeB.GetOpincCache();

    bool unchangeMarkInApp = false;
    opincCacheA.OpincSetInAppStateStart(unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    opincCacheB.OpincSetInAppStateStart(unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    opincCacheB.OpincSetInAppStateEnd(unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    opincCacheA.OpincSetInAppStateEnd(unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);
}

/**
 * @tc.name: MarkSuggestLayerPartRenderNode001
 * @tc.desc: test results of MarkSuggestLayerPartRenderNode
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, MarkSuggestLayerPartRenderNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    opincCache.MarkSuggestLayerPartRenderNode(true);
    ASSERT_TRUE(opincCache.IsSuggestLayerPartRenderNode());

    opincCache.MarkSuggestLayerPartRenderNode(false);
    ASSERT_FALSE(opincCache.IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: IsSuggestLayerPartRenderNode001
 * @tc.desc: test results of IsSuggestLayerPartRenderNode
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsSuggestLayerPartRenderNode001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    ASSERT_FALSE(opincCache.IsSuggestLayerPartRenderNode());

    opincCache.MarkSuggestLayerPartRenderNode(true);
    ASSERT_TRUE(opincCache.IsSuggestLayerPartRenderNode());
}

/**
 * @tc.name: SetLayerPartRender001
 * @tc.desc: test results of SetLayerPartRender
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, SetLayerPartRender001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    opincCache.SetLayerPartRender(true);
    ASSERT_TRUE(opincCache.IsLayerPartRender());

    opincCache.SetLayerPartRender(false);
    ASSERT_FALSE(opincCache.IsLayerPartRender());
}

/**
 * @tc.name: IsLayerPartRender001
 * @tc.desc: test results of IsLayerPartRender
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsLayerPartRender001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    ASSERT_FALSE(opincCache.IsLayerPartRender());

    opincCache.SetLayerPartRender(true);
    ASSERT_TRUE(opincCache.IsLayerPartRender());
}

/**
 * @tc.name: IsLayerPartRenderUnchangeState001
 * @tc.desc: test results of IsLayerPartRenderUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsLayerPartRenderUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    // count=0, 0<=3 -> false, count=1
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=1, 1<=3 -> false, count=2
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=2, 2<=3 -> false, count=3
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=3, 3<=3 -> false, count=4
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    // count=4, 4>3 -> true
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());
}

/**
 * @tc.name: IsLayerPartRenderUnchangeState002
 * @tc.desc: test results of IsLayerPartRenderUnchangeState after reset
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, IsLayerPartRenderUnchangeState002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    for (int i = 0; i < 5; i++) {
        opincCache.IsLayerPartRenderUnchangeState();
    }
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());

    opincCache.ResetLayerPartRenderUnchangeState();

    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
}

/**
 * @tc.name: ResetLayerPartRenderUnchangeState001
 * @tc.desc: test results of ResetLayerPartRenderUnchangeState
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, ResetLayerPartRenderUnchangeState001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    for (int i = 0; i < 5; i++) {
        opincCache.IsLayerPartRenderUnchangeState();
    }
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());

    opincCache.ResetLayerPartRenderUnchangeState();

    // After reset, count=0, need 4 calls to reach count=4
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_FALSE(opincCache.IsLayerPartRenderUnchangeState());
    ASSERT_TRUE(opincCache.IsLayerPartRenderUnchangeState());
}

/**
 * @tc.name: GetLayerPartRenderDirtyManager001
 * @tc.desc: test results of GetLayerPartRenderDirtyManager
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, GetLayerPartRenderDirtyManager001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    auto& dirtyManager = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager, nullptr);

    auto& dirtyManager2 = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_EQ(dirtyManager, dirtyManager2);
}

/**
 * @tc.name: GetLayerPartRenderDirtyManager002
 * @tc.desc: test GetLayerPartRenderDirtyManager returns same instance
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, GetLayerPartRenderDirtyManager002, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetLayerPartRenderCache();

    auto& dirtyManager1 = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_NE(dirtyManager1, nullptr);

    auto& dirtyManager2 = opincCache.GetLayerPartRenderDirtyManager();
    ASSERT_EQ(dirtyManager1.get(), dirtyManager2.get());
}

/**
 * @tc.name: SetLayerPartRenderDirtyFlag001
 * @tc.desc: test results of SetLayerPartRenderDirtyFlag and GetLayerPartRenderDirtyFlag
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, SetLayerPartRenderDirtyFlag001, TestSize.Level1)
{
    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();

    opincCache.SetLayerPartRenderDirtyFlag(true);
    ASSERT_TRUE(opincCache.GetLayerPartRenderDirtyFlag());

    opincCache.SetLayerPartRenderDirtyFlag(false);
    ASSERT_FALSE(opincCache.GetLayerPartRenderDirtyFlag());
}

/**
 * @tc.name: SetLayerPartRenderOldAbsDrawRect001
 * @tc.desc: test results of SetLayerPartRenderOldAbsDrawRect and GetLayerPartRenderOldAbsDrawRect
 * @tc.type: FUNC
 * @tc.require: issueLayerPart
 */
HWTEST_F(RSOpincCacheTest, SetLayerPartRenderOldAbsDrawRect001, TestSize.Level1)
{
    constexpr int32_t rectLeft = 10;
    constexpr int32_t rectTop = 10;
    constexpr int32_t rectWidth = 100;
    constexpr int32_t rectHeight = 100;

    RSRenderNode renderNode(0);
    auto& opincCache = renderNode.GetOpincCache();
    RectI absDrawRect = { rectLeft, rectTop, rectWidth, rectHeight };

    opincCache.SetLayerPartRenderOldAbsDrawRect(absDrawRect);

    ASSERT_EQ(opincCache.GetLayerPartRenderOldAbsDrawRect(), absDrawRect);
}

} // namespace Rosen
} // namespace OHOS
