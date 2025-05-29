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
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION), 0x000000);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED), 0x000001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE), 0x000002);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE), 0x000003);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO), 0x000004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION),
        0x000005);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK), 0x000006);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT),
        0x000007);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW),
        0x000008);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME),
        0x000009);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN),
        0x00000A);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS), 0x00000B);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT), 0x00000C);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE), 0x00000D);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY),
        0x00000E);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION), 0x00000F);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID), 0x000010);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT),
        0x000011);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER),
        0x000012);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER), 0x000013);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION), 0x000014);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK),
        0x000015);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM), 0x000016);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES), 0x000017);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK), 0x000018);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED),
        0x000019);
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
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG), 0x00001A);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS),
        0x00001B);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID), 0x00001C);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO), 0x00001D);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO),
        0x00001E);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION), 0x00001F);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK),
        0x000020);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT), 0x000021);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS), 0x000022);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_DISPLAY_NODE), 0x000023);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS),
        0x000024);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK),
        0x000025);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK),
        0x000026);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP), 0x000027);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT), 0x000028);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT), 0x000029);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING), 0x00002A);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC),
        0x00002B);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER), 0x00002C);
    EXPECT_EQ(
        static_cast<uint32_t>(
            RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK
        ), 0x00002D);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME), 0x00002E);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TEST_LOAD_FILE_SUB_TREE), 0x00002F);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE), 0x000030);

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DEFAULT_SCREEN_ID), 0x001000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_SCREEN_ID), 0x001001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ALL_SCREEN_IDS), 0x001002);
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
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN), 0x002000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION),
        0x002001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE),
        0x002002);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST),
        0x002003);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST),
        0x002004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST),
        0x002005);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST),
        0x002006);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN), 0x002007);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION),
        0x002008);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN), 0x002009);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS),
        0x00200A);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS), 0x00200B);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE),
        0x00200C);

    #ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
        EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG),
            0x004000);
        EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED),
            0x004001);
        EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK),
            0x004002);
        EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_POINTER_LUMINANCE_CALLBACK),
            0x004003);
    #endif

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK),
        0x005000);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE), 0x005001);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE), 0x005002);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE), 0x005003);
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
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE), 0x005004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER),
        0x005005);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE),
        0x005006);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE),
        0x005007);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES),
        0x005008);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED),
        0x005009);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED),
        0x00500A);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE), 0x00500B);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO), 0x00500C);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK),
        0x00500D);
    EXPECT_EQ(static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK), 0x00500E);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK),
        0x00500F);

    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE), 0x006000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES),
        0x006001);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY), 0x006002);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS), 0x006003);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT), 0x006004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_DATA), 0x006005);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS),
        0x006006);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS),
        0x006007);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT), 0x006008);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP), 0x006009);

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT), 0x007000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP), 0x007001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION), 0x007002);
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
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION),
        0x007003);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE),
        0x007004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE),
        0x007005);
    
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE),
        0x008000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY), 0x008001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT), 0x008002);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT), 0x008003);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS),
        0x008004);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT), 0x008005);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT), 0x008006);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES),
        0x008007);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE), 0x008008);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE), 0x008009);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE), 0x00800A);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL),
        0x00800B);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP), 0x00800C);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP), 0x00800D);

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK), 0x009000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK), 0x009001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK), 0x009002);
    
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC), 0x00A000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS), 0x00A001);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_TOTAL_APP_MEM_SIZE), 0x00A002);
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
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS), 0x00B000);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE), 0x00B001);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE), 0x00B002);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME), 0x00B003);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START),
        0x00B004);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END), 0x00B005);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE), 0x00B006);
    
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK), 0x00C000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT), 0x00C001);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED), 0x00C002);

    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
        0x00D000);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK),
        0x00D001);

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_TYPEFACE), 0x00E000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_TYPEFACE), 0x00E001);
    EXPECT_EQ(
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NEED_REGISTER_TYPEFACE), 0x00E002);

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_ACTIVE_DIRTY_REGION_INFO),
        0x00F000);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_GLOBAL_DIRTY_REGION_INFO),
        0x00F001);

    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA),
        0x010000);
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE),
        0x010001);
#endif
}

/**
 * @tc.name: FixEnumValue007
 * @tc.desc: Fix enum values to prevent undetectable changes.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSIRenderServiceConnectionInterfaceCodeTest, FixEnumValue007, testing::ext::TestSize.Level1)
{
    using OHOS::Rosen::RSIRenderServiceConnectionInterfaceCode;
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS), 0x3E8);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT), 0x3E9);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT), 0x3EA);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT), 0x3EB);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT), 0x3EC);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT),
        0x3ED);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_HGMCONFIG_EVENT), 0x3EE);
    EXPECT_EQ(static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED), 0x3EF);
}
} // namespace OHOS::Rosen