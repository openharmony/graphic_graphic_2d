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

#include "feature/render_group/rs_render_group_cache.h"

#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
constexpr NodeId DEFAULT_ID = 10086;

class RSRenderGroupCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline NodeId id = DEFAULT_ID;
};

void RSRenderGroupCacheTest::SetUpTestCase() {}
void RSRenderGroupCacheTest::TearDownTestCase() {}
void RSRenderGroupCacheTest::SetUp() {}
void RSRenderGroupCacheTest::TearDown() {}

/**
 * @tc.name: ExcludedFromNodeGroupTest
 * @tc.desc: Test ExcludedFromNodeGroup
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderGroupCacheTest, ExcludedFromNodeGroupTest, TestSize.Level1)
{
    auto renderGroupCache = std::make_unique<RSRenderGroupCache>();
    ASSERT_NE(renderGroupCache, nullptr);

    bool rst = renderGroupCache->ExcludedFromNodeGroup(false);
    EXPECT_FALSE(renderGroupCache->IsExcludedFromNodeGroup());
    EXPECT_FALSE(rst);

    rst = renderGroupCache->ExcludedFromNodeGroup(true);
    EXPECT_TRUE(renderGroupCache->IsExcludedFromNodeGroup());
    EXPECT_TRUE(rst);
}

/**
 * @tc.name: SetHasChildExcludedFromNodeGroupTest
 * @tc.desc: Test SetHasChildExcludedFromNodeGroup
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderGroupCacheTest, SetHasChildExcludedFromNodeGroupTest, TestSize.Level1)
{
    auto renderGroupCache = std::make_unique<RSRenderGroupCache>();
    ASSERT_NE(renderGroupCache, nullptr);

    bool rst = renderGroupCache->SetHasChildExcludedFromNodeGroup(false);
    EXPECT_FALSE(renderGroupCache->HasChildExcludedFromNodeGroup());
    EXPECT_FALSE(rst);

    rst = renderGroupCache->SetHasChildExcludedFromNodeGroup(true);
    EXPECT_TRUE(renderGroupCache->HasChildExcludedFromNodeGroup());
    EXPECT_TRUE(rst);
}

/**
 * @tc.name: SetRenderGroupExcludedStateChangedTest
 * @tc.desc: Test SetRenderGroupExcludedStateChanged
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderGroupCacheTest, SetRenderGroupExcludedStateChangedTest, TestSize.Level1)
{
    auto renderGroupCache = std::make_unique<RSRenderGroupCache>();
    ASSERT_NE(renderGroupCache, nullptr);

    bool rst = renderGroupCache->SetRenderGroupExcludedStateChanged(false);
    EXPECT_FALSE(renderGroupCache->IsRenderGroupExcludedStateChanged());
    EXPECT_FALSE(rst);

    rst = renderGroupCache->SetRenderGroupExcludedStateChanged(true);
    EXPECT_TRUE(renderGroupCache->IsRenderGroupExcludedStateChanged());
    EXPECT_TRUE(rst);
}

/**
 * @tc.name: SetCachedSubTreeDirtyTest
 * @tc.desc: Test SetCachedSubTreeDirty
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderGroupCacheTest, SetCachedSubTreeDirtyTest, TestSize.Level1)
{
    auto renderGroupCache = std::make_unique<RSRenderGroupCache>();
    ASSERT_NE(renderGroupCache, nullptr);

    bool rst = renderGroupCache->SetCachedSubTreeDirty(false);
    EXPECT_FALSE(renderGroupCache->IsCachedSubTreeDirty());
    EXPECT_FALSE(rst);

    rst = renderGroupCache->SetCachedSubTreeDirty(true);
    EXPECT_TRUE(renderGroupCache->IsCachedSubTreeDirty());
    EXPECT_TRUE(rst);
}

/**
 * @tc.name: AutoRenderGroupExcludedSubTreeGuardTest
 * @tc.desc: Test AutoRenderGroupExcludedSubTreeGuard contructor and destructor
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderGroupCacheTest, AutoRenderGroupExcludedSubTreeGuardTest001, TestSize.Level1)
{
    NodeId curExcludedRootNodeId = INVALID_NODEID;
    bool isCurNodeExcluded = false;
    NodeId curNodeId = 101;
    {
        AutoRenderGroupExcludedSubTreeGuard guard(curExcludedRootNodeId, isCurNodeExcluded, curNodeId);
        EXPECT_EQ(curExcludedRootNodeId, INVALID_NODEID);
        EXPECT_FALSE(guard.isExcluded_);
    }
    EXPECT_EQ(curExcludedRootNodeId, INVALID_NODEID);

    curExcludedRootNodeId = INVALID_NODEID;
    isCurNodeExcluded = true;
    curNodeId = 101;
    {
        AutoRenderGroupExcludedSubTreeGuard guard(curExcludedRootNodeId, isCurNodeExcluded, curNodeId);
        EXPECT_EQ(curExcludedRootNodeId, 101);
        EXPECT_TRUE(guard.isExcluded_);
    }
    EXPECT_EQ(curExcludedRootNodeId, INVALID_NODEID);

    curExcludedRootNodeId = 97;
    isCurNodeExcluded = true;
    curNodeId = 101;
    {
        AutoRenderGroupExcludedSubTreeGuard guard(curExcludedRootNodeId, isCurNodeExcluded, curNodeId);
        EXPECT_EQ(curExcludedRootNodeId, 97);
        EXPECT_FALSE(guard.isExcluded_);
    }
    EXPECT_EQ(curExcludedRootNodeId, 97);
}

/**
 * @tc.name: SetChildHasTranslateOnSqueezeTest
 * @tc.desc: Test SetChildHasTranslateOnSqueeze
 * @tc.type: FUNC
 * @tc.require: issues/20738
 */
HWTEST_F(RSRenderGroupCacheTest, SetChildHasTranslateOnSqueezeTest, TestSize.Level1)
{
    auto renderGroupCache = std::make_unique<RSRenderGroupCache>();
    ASSERT_NE(renderGroupCache, nullptr);

    bool rst = renderGroupCache->SetChildHasTranslateOnSqueeze(false);
    EXPECT_FALSE(renderGroupCache->ChildHasTranslateOnSqueeze());
    EXPECT_FALSE(rst);

    rst = renderGroupCache->SetChildHasTranslateOnSqueeze(true);
    EXPECT_TRUE(renderGroupCache->ChildHasTranslateOnSqueeze());
    EXPECT_TRUE(rst);
}
} // namespace Rosen
} // namespace OHOS