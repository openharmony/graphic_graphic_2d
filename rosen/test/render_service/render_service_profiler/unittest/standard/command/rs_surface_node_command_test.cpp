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
    auto value = 0;
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_CREATE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_CREATE_WITH_CONFIG), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_MATRIX), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_ALPHA), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_CLIP_REGION), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SECURITY_LAYER), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SKIP_LAYER), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SNAPSHOT_SKIP_LAYER), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FINGERPRINT), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_COLOR_SPACE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_UPDATE_SURFACE_SIZE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_CONNECT_TO_NODE_IN_RENDER_SERVICE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CALLBACK_FOR_RENDER_THREAD), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTEXT_BOUNDS), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ABILITY_BG_ALPHA), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_IS_NOTIFY_BUFFER_AVAILABLE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SURFACE_NODE_TYPE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CONTAINER_WINDOW), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ANIMATION_FINISHED), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_MARK_UIHIDDEN), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_IS_TEXTURE_EXPORT_NODE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_ATTACH_TO_DISPLAY), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_DETACH_TO_DISPLAY), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FORCE_HARDWARE_AND_FIX_ROTATION), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_BOOT_ANIMATION), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_GLOBAL_POSITION_ENABLED), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FOREGROUND), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SURFACE_ID), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_CLONED_NODE_ID), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_FORCE_UIFIRST), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ANCO_FLAGS), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_HDR_PRESENT), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SKIP_DRAW), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_WATERMARK_ENABLED), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_ABILITY_STATE), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_LEASH_PERSISTENT_ID), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_API_COMPATIBLE_VERSION), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_HARDWARE_ENABLE_HINT), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_ATTACH_TO_WINDOW_CONTAINER), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_DETACH_FROM_WINDOW_CONTAINER), value++);
    EXPECT_EQ(static_cast<uint16_t>(SURFACE_NODE_SET_SOURCE_VIRTUAL_SCREEN_ID), value++);
}
} // namespace OHOS::Rosen