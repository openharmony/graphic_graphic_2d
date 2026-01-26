/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <securec.h>
#include <gtest/gtest.h>

#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenPropertyTest::SetUpTestCase() {}
void RSScreenPropertyTest::TearDownTestCase() {}
void RSScreenPropertyTest::SetUp() {}
void RSScreenPropertyTest::TearDown() {}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenPropertyTest, Marshalling001, TestSize.Level1)
{
    RSScreenProperty srcProperty;
    MessageParcel parcel;
    srcProperty.Marshalling(parcel);

    auto dstProperty = sptr<RSScreenProperty>(RSScreenProperty::Unmarshalling(parcel));
    ASSERT_NE(dstProperty, nullptr);
}

/**
 * @tc.name: Marshalling002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenPropertyTest, Marshalling002, TestSize.Level1)
{
    RSScreenProperty srcProperty;
    srcProperty.IsScreenSwitching();
    srcProperty.GetFrameGravity();
    srcProperty.IsVisibleRectSupportRotation();
    srcProperty.GetCanvasRotation();
 
    srcProperty.Set<ScreenPropertyType::ID>(INVALID_SCREEN_ID);
    srcProperty.Set<ScreenPropertyType::IS_VIRTUAL>(true);
    srcProperty.Set<ScreenPropertyType::NAME>("RSScreenPropertyTest");
    srcProperty.Set<ScreenPropertyType::RENDER_RESOLUTION>({1, 30});
    srcProperty.Set<ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE>(std::make_tuple(0, 0, 0));
    srcProperty.Set<ScreenPropertyType::OFFSET>({1, 30});
    srcProperty.Set<ScreenPropertyType::SAMPLING_OPTION>(std::make_tuple(false, 0.0, 0.0, 0.0));
    srcProperty.Set<ScreenPropertyType::COLOR_GAMUT>(static_cast<uint32_t>(ScreenColorGamut::COLOR_GAMUT_SRGB));
    srcProperty.Set<ScreenPropertyType::GAMUT_MAP>(static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_CONSTANT));
    srcProperty.Set<ScreenPropertyType::STATE>(static_cast<uint8_t>(ScreenState::UNKNOWN));
    srcProperty.Set<ScreenPropertyType::CORRECTION>(0);
    srcProperty.Set<ScreenPropertyType::CANVAS_ROTATION>(false);
    srcProperty.Set<ScreenPropertyType::AUTO_BUFFER_ROTATION>(false);
    srcProperty.Set<ScreenPropertyType::ACTIVE_RECT_OPTION>(activeRectValType(RectI(), RectI(), RectI()));
    srcProperty.Set<ScreenPropertyType::SKIP_FRAME_OPTION>({1, 30, SKIP_FRAME_BY_ACTIVE_REFRESH_RATE});
    srcProperty.Set<ScreenPropertyType::PIXEL_FORMAT>(static_cast<int32_t>(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888));
    srcProperty.Set<ScreenPropertyType::HDR_FORMAT>(static_cast<uint32_t>(ScreenHDRFormat::NOT_SUPPORT_HDR));
    srcProperty.Set<ScreenPropertyType::VISIBLE_RECT_OPTION>(visibleRectValType(false, Rect(), false));
    srcProperty.Set<ScreenPropertyType::WHITE_LIST>(std::unordered_set<NodeId>());
    srcProperty.Set<ScreenPropertyType::BLACK_LIST>(std::unordered_set<NodeId>());
    srcProperty.Set<ScreenPropertyType::TYPE_BLACK_LIST>(std::unordered_set<NodeType>());
    srcProperty.Set<ScreenPropertyType::SECURITY_EXEMPTION_LIST>(std::vector<NodeId>());
    srcProperty.Set<ScreenPropertyType::SECURITY_MASK>(nullptr);
    srcProperty.Set<ScreenPropertyType::ENABLE_SKIP_WINDOW>(false);
    srcProperty.Set<ScreenPropertyType::POWER_STATUS>(static_cast<uint32_t>(ScreenPowerStatus::INVALID_POWER_STATUS));
    srcProperty.Set<ScreenPropertyType::SCREEN_TYPE>(static_cast<uint32_t>(RSScreenType::UNKNOWN_TYPE_SCREEN));
    srcProperty.Set<ScreenPropertyType::PRODUCER_SURFACE>(nullptr);
    srcProperty.Set<ScreenPropertyType::SCALE_MODE>(static_cast<uint32_t>(ScreenScaleMode::UNISCALE_MODE));
    srcProperty.Set<ScreenPropertyType::SCREEN_STATUS>(static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    srcProperty.Set<ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION>(0);
    srcProperty.Set<ScreenPropertyType::IS_HARD_CURSOR_SUPPORT>(false);
    srcProperty.Set<ScreenPropertyType::SUPPORTED_COLOR_GAMUTS>(std::vector<ScreenColorGamut>());
    srcProperty.Set<ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL>(false);
    srcProperty.Set<ScreenPropertyType::SCREEN_SWITCH_STATUS>(false);
    srcProperty.Set<ScreenPropertyType::SCREEN_FRAME_GRAVITY>(static_cast<int32_t>(Gravity::DEFAULT));
 
    MessageParcel parcel;
    srcProperty.Marshalling(parcel);
 
    auto dstProperty = sptr<RSScreenProperty>(RSScreenProperty::Unmarshalling(parcel));
    ASSERT_NE(dstProperty, nullptr);
}
}