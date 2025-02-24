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

#include <gtest/gtest.h>

#include "gtest/hwext/gtest-tag.h"
#include "rs_irender_service_connection_ipc_interface_code.h"

namespace OHOS::Rosen {
class RSIRenderServiceConnectionInterfaceCodeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSIRenderServiceConnectionInterfaceCodeTest::SetUpTestCase() {}
void RSIRenderServiceConnectionInterfaceCodeTest::TearDownTestCase() {}
void RSIRenderServiceConnectionInterfaceCodeTest::SetUp() {}
void RSIRenderServiceConnectionInterfaceCodeTest::TearDown() {}

/**
 * @tc.name: FixEnumValue001
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue001, testing::ext::Level1 | testing::ext::Standard)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
    int expectedEnumValue = 0;
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW),
        expectedEnumValue++);
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_POINTER_LUMINANCE_CALLBACK),
        expectedEnumValue++);
#endif
}

/**
 * @tc.name: FixEnumValue002
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue002, testing::ext::Level1 | testing::ext::Standard)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int expectedEnumValue = 23;
#else
    int expectedEnumValue = 19;
#endif
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION), expectedEnumValue++);
}

/**
 * @tc.name: FixEnumValue003
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue003, testing::ext::Level1 | testing::ext::Standard)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int expectedEnumValue = 44;
#else
    int expectedEnumValue = 40;
#endif
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION), expectedEnumValue++);
}

/**
 * @tc.name: FixEnumValue004
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue004, testing::ext::Level1 | testing::ext::Standard)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int expectedEnumValue = 66;
#else
    int expectedEnumValue = 62;
#endif
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME), expectedEnumValue++);
}

/**
 * @tc.name: FixEnumValue005
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue005, testing::ext::Level1 | testing::ext::Standard)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int expectedEnumValue = 91;
#else
    int expectedEnumValue = 87;
#endif
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(
                  RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO),
        expectedEnumValue++);
}

/**
 * @tc.name: FixEnumValue006
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue006, testing::ext::Level1 | testing::ext::Standard)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    int expectedEnumValue = 113;
#else
    int expectedEnumValue = 109;
#endif
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP), expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS),
        expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING), expectedEnumValue++);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC),
        expectedEnumValue++);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER), expectedEnumValue++);
}

/**
 * @tc.name: FixEnumValue007
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue007, testing::ext::TestSize.Level1)
{
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS), 1000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT), 1001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT), 1002);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT), 1003);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT), 1004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED), 1005);
}
} // namespace OHOS::Rosen