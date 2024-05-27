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
    RSTagTracker tagTracker(nullptr, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER);
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
    Drawing::GPUResourceTag tag(0, 0, 0, 0);
    Drawing::GPUContext* gpuContext = nullptr;
    RSTagTracker tagTracker(gpuContext, tag);
    tagTracker.SetTagEnd();
}

/**
 * @tc.name: UpdateReleaseResourceEnabled
 * @tc.desc: Test UpdateReleaseResourceEnabled and SetTagEnd
 * @tc.type: FUNC
 * @tc.require: issueI9QIQO
 */
HWTEST_F(RSTagTrackerTest, UpdateReleaseResourceEnabled, TestSize.Level1)
{
    Drawing::GPUResourceTag tag(0, 0, 0, 0);
    Drawing::GPUContext gpuContext;
    RSTagTracker tagTracker(&gpuContext, tag);
    tagTracker.SetTagEnd();
    RSTagTracker::UpdateReleaseResourceEnabled(true);
    tagTracker.SetTagEnd();
    ASSERT_TRUE(tagTracker.isSetTagEnd_);
    NodeId nodeId = 1;
    RSTagTracker tagTrackerTwo(&gpuContext, nodeId, RSTagTracker::TAGTYPE::TAG_FILTER);
    RSTagTracker tagTrackerThree(&gpuContext, RSTagTracker::TAGTYPE::TAG_FILTER);
    ASSERT_NE(&gpuContext, nullptr);
}
} // namespace OHOS::Rosen