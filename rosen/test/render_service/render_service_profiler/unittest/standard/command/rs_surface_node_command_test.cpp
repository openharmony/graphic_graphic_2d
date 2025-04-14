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

#include "command/rs_surface_node_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceNodeCommandTypeTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: FixSurfaceNodeCommandTest
 * @tc.desc: Fix enum class to prevent undetectable changes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceNodeCommandTypeTest, FixSurfaceNodeCommandTest, Level1 | Standard)
{
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_CREATE), 0);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_CREATE_WITH_CONFIG), 1);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_MATRIX), 2);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_ALPHA), 3);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_CLIP_REGION), 4);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SECURITY_LAYER), 5);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SKIP_LAYER), 6);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SNAPSHOT_SKIP_LAYER), 7);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FINGERPRINT), 8);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_COLOR_SPACE), 9);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_UPDATE_SURFACE_SIZE), 10);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE), 11);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CALLBACK_FOR_RENDER_THREAD), 12);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_BOUNDS), 13);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ABILITY_BG_ALPHA), 14);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_IS_NOTIFY_BUFFER_AVAILABLE), 15);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SURFACE_NODE_TYPE), 16);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTAINER_WINDOW), 17);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ANIMATION_FINISHED), 18);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_MARK_UIHIDDEN), 19);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_IS_TEXTURE_EXPORT_NODE), 20);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_ATTACH_TO_DISPLAY), 21);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_DETACH_TO_DISPLAY), 22);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FORCE_HARDWARE_AND_FIX_ROTATION), 23);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_BOOT_ANIMATION), 24);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_GLOBAL_POSITION_ENABLED), 25);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FOREGROUND), 26);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SURFACE_ID), 27);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CLONED_NODE_ID), 28);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FORCE_UIFIRST), 29);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ANCO_FLAGS), 30);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_HDR_PRESENT), 31);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SKIP_DRAW), 32);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_WATERMARK_ENABLED), 33);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ABILITY_STATE), 34);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_LEASH_PERSISTENT_ID), 35);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_API_COMPATIBLE_VERSION), 36);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_HARDWARE_ENABLE_HINT), 37);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_ATTACH_TO_WINDOW_CONTAINER), 38);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_DETACH_FROM_WINDOW_CONTAINER), 39);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SOURCE_VIRTUAL_DISPLAY_ID), 40);
}
} // namespace OHOS::Rosen