/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "memory/rs_tag_tracker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTagTrackerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTagTrackerTest::SetUpTestCase() {}
void RSTagTrackerTest::TearDownTestCase() {}
void RSTagTrackerTest::SetUp() {}
void RSTagTrackerTest::TearDown() {}

/**
 * @tc.name: TagType2String001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, TagType2String001, TestSize.Level1)
{
    NodeId nodeId = static_cast<NodeId>(0);
    RSTagTracker tagTracker(nullptr, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "TagType2String001");
    ASSERT_EQ("savelayer_draw_node", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_SAVELAYER_DRAW_NODE));
    ASSERT_EQ(
        "restorelayer_draw_node", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_RESTORELAYER_DRAW_NODE));
    ASSERT_EQ(
        "savelayer_color_filter", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_SAVELAYER_COLOR_FILTER));
    ASSERT_EQ("filter", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_FILTER));
    ASSERT_EQ("capture", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_CAPTURE));
    ASSERT_EQ("acquire_surface", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_ACQUIRE_SURFACE));
    ASSERT_EQ("render_frame", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_RENDER_FRAME));
    ASSERT_EQ("draw_surface_node", RSTagTracker::TagType2String(RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE));
    ASSERT_EQ("", RSTagTracker::TagType2String(static_cast<RSTagTracker::TAGTYPE>(15)));
}

/**
 * @tc.name: RSTagTracker001
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker001, TestSize.Level1)
{
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "RSTagTracker001");
    Drawing::GPUContext* gpuContext = nullptr;
    RSTagTracker tagTracker(gpuContext, tag);
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ == nullptr);
}

/**
 * @tc.name: RSTagTracker002
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker002, TestSize.Level1)
{
    RSTagTracker::TAGTYPE tagType = RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE;
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    RSTagTracker tagTracker(gpuContext, tagType);
    tagTracker.UpdateReleaseResourceEnabled(true);
    RSTagTracker tagTracker1(gpuContext, tagType);
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ != nullptr);
}

/**
 * @tc.name: RSTagTracker003
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker003, TestSize.Level1)
{
    RSTagTracker::TAGTYPE tagType = RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE;
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    RSTagTracker tagTracker(gpuContext, tagType);
    tagTracker.UpdateReleaseResourceEnabled(false);
    RSTagTracker tagTracker1(gpuContext, tagType);
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ != nullptr);
}

/**
 * @tc.name: RSTagTracker004
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker004, TestSize.Level1)
{
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    NodeId nodeId = 1;
    RSTagTracker tagTracker(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker004");
    tagTracker.UpdateReleaseResourceEnabled(false);
    RSTagTracker tagTracker1(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker004");
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ != nullptr);
}

/**
 * @tc.name: RSTagTracker005
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker005, TestSize.Level1)
{
    Drawing::GPUContext* gpuContext = nullptr;
    NodeId nodeId = 1;
    RSTagTracker tagTracker(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker005");
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ == nullptr);
}

/**
 * @tc.name: RSTagTracker006
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker006, TestSize.Level1)
{
    Drawing::GPUContext* gpuContext = nullptr;
    NodeId nodeId = 1;
    RSTagTracker tagTracker(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker005");
    tagTracker.UpdateReleaseResourceEnabled(false);
    RSTagTracker tagTracker1(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker005");
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ == nullptr);
}

/**
 * @tc.name: RSTagTracker007
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker007, TestSize.Level1)
{
    Drawing::GPUContext* gpuContext = new Drawing::GPUContext;
    NodeId nodeId = 1;
    RSTagTracker tagTracker(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker007");
    tagTracker.UpdateReleaseResourceEnabled(true);
    RSTagTracker tagTracker1(gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "RSTagTracker007");
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ != nullptr);
}

/**
 * @tc.name: RSTagTracker008
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker008, TestSize.Level1)
{
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "RSTagTracker008");
    Drawing::GPUContext gpuContext;
    RSTagTracker tagTracker(&gpuContext, tag);
    tagTracker.UpdateReleaseResourceEnabled(true);
    RSTagTracker tagTracker1(&gpuContext, tag);
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ != nullptr);
}

/**
 * @tc.name: RSTagTracker009
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker009, TestSize.Level1)
{
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "RSTagTracker009");
    Drawing::GPUContext gpuContext;
    RSTagTracker tagTracker(&gpuContext, tag);
    tagTracker.UpdateReleaseResourceEnabled(false);
    RSTagTracker tagTracker1(&gpuContext, tag);
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ != nullptr);
}

/**
 * @tc.name: RSTagTracker010
 * @tc.desc: test
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTagTrackerTest, RSTagTracker010, TestSize.Level1)
{
    RSTagTracker::TAGTYPE tagType = RSTagTracker::TAGTYPE::TAG_DRAW_SURFACENODE;
    Drawing::GPUContext* gpuContext = nullptr;
    RSTagTracker tagTracker(gpuContext, tagType);
    tagTracker.UpdateReleaseResourceEnabled(false);
    RSTagTracker tagTracker1(gpuContext, tagType);
    tagTracker.SetTagEnd();
    EXPECT_TRUE(tagTracker.gpuContext_ == nullptr);
}

/**
 * @tc.name: UpdateReleaseResourceEnabled
 * @tc.desc: Test UpdateReleaseResourceEnabled and SetTagEnd
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTagTrackerTest, UpdateReleaseResourceEnabled, TestSize.Level1)
{
    Drawing::GPUResourceTag tag(0, 0, 0, 0, "UpdateReleaseResourceEnabled");
    Drawing::GPUContext gpuContext;
    RSTagTracker tagTracker(&gpuContext, tag);
    tagTracker.SetTagEnd();
    RSTagTracker::UpdateReleaseResourceEnabled(true);
    tagTracker.SetTagEnd();
    ASSERT_TRUE(tagTracker.isSetTagEnd_);
    NodeId nodeId = 1;
    RSTagTracker tagTrackerTwo(&gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER, "UpdateReleaseResEnabled");
    RSTagTracker tagTrackerThree(&gpuContext, RSTagTracker::TAGTYPE::TAG_FILTER);
    ASSERT_NE(&gpuContext, nullptr);
}
} // namespace OHOS::Rosen