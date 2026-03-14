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
 * @tc.name: MarshallingTest001
 * @tc.desc: Test RSScreenProperty Marshalling when parcel WriteUint32 failed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenPropertyTest, MarshallingTest001, TestSize.Level1)
{
    RSScreenProperty srcProperty;
    MessageParcel parcel;
    parcel.writable_ = false;
    parcel.data_ = nullptr;
    bool res = srcProperty.Marshalling(parcel);
    ASSERT_FALSE(res);

    RSScreenProperty dstProperty;
    res = dstProperty.Unmarshalling(parcel);
    ASSERT_FALSE(res);
}

/**
 * @tc.name: MarshallingTest002
 * @tc.desc: Test RSScreenProperty Marshalling all case
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSScreenPropertyTest, MarshallingTest002, TestSize.Level1)
{
    RSScreenProperty srcProperty;

#define ADD_PROP(TYPE, VALUE)                                    \
    {                                                            \
        using T = PropertyTypeMapper<TYPE>::value_type;          \
        auto prop = sptr<ScreenProperty<T>>::MakeSptr(T(VALUE)); \
        srcProperty.Set(TYPE, prop);                             \
    }

    ADD_PROP(ScreenPropertyType::ID, INVALID_SCREEN_ID);
    ADD_PROP(ScreenPropertyType::IS_VIRTUAL, true);
    ADD_PROP(ScreenPropertyType::NAME, "RSScreenPropertyTest");
    ADD_PROP(ScreenPropertyType::RENDER_RESOLUTION, std::make_pair(1920u, 1080u));
    ADD_PROP(ScreenPropertyType::PHYSICAL_RESOLUTION_REFRESHRATE, std::make_tuple(0, 0, 0));
    ADD_PROP(ScreenPropertyType::OFFSET, std::make_pair(1, 30));
    ADD_PROP(ScreenPropertyType::SAMPLING_OPTION, std::make_tuple(false, 0.0, 0.0, 0.0));
    ADD_PROP(ScreenPropertyType::COLOR_GAMUT, static_cast<uint32_t>(ScreenColorGamut::COLOR_GAMUT_SRGB));
    ADD_PROP(ScreenPropertyType::GAMUT_MAP, static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_CONSTANT));
    ADD_PROP(ScreenPropertyType::STATE, static_cast<uint8_t>(ScreenState::UNKNOWN));
    ADD_PROP(ScreenPropertyType::CORRECTION, 0);
    ADD_PROP(ScreenPropertyType::CANVAS_ROTATION, false);
    ADD_PROP(ScreenPropertyType::AUTO_BUFFER_ROTATION, false);
    ADD_PROP(ScreenPropertyType::ACTIVE_RECT_OPTION, activeRectValType(RectI(), RectI(), RectI()));
    ADD_PROP(ScreenPropertyType::SKIP_FRAME_OPTION, std::make_tuple(1, 30, SKIP_FRAME_BY_ACTIVE_REFRESH_RATE));
    ADD_PROP(ScreenPropertyType::PIXEL_FORMAT, static_cast<int32_t>(GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888));
    ADD_PROP(ScreenPropertyType::HDR_FORMAT, static_cast<uint32_t>(ScreenHDRFormat::NOT_SUPPORT_HDR));
    ADD_PROP(ScreenPropertyType::VISIBLE_RECT_OPTION, visibleRectValType(false, Rect(), false));
    ADD_PROP(ScreenPropertyType::WHITE_LIST, std::unordered_set<NodeId>());
    ADD_PROP(ScreenPropertyType::BLACK_LIST, std::unordered_set<NodeId>());
    ADD_PROP(ScreenPropertyType::TYPE_BLACK_LIST, std::unordered_set<NodeType>());
    ADD_PROP(ScreenPropertyType::SECURITY_EXEMPTION_LIST, std::vector<NodeId>());
    ADD_PROP(ScreenPropertyType::SECURITY_MASK, nullptr);
    ADD_PROP(ScreenPropertyType::ENABLE_SKIP_WINDOW, false);
    ADD_PROP(ScreenPropertyType::POWER_STATUS, static_cast<uint32_t>(ScreenPowerStatus::INVALID_POWER_STATUS));
    ADD_PROP(ScreenPropertyType::SCREEN_TYPE, static_cast<uint32_t>(RSScreenType::UNKNOWN_TYPE_SCREEN));
    ADD_PROP(ScreenPropertyType::PRODUCER_SURFACE, nullptr);
    ADD_PROP(ScreenPropertyType::SCALE_MODE, static_cast<uint32_t>(ScreenScaleMode::UNISCALE_MODE));
    ADD_PROP(ScreenPropertyType::SCREEN_STATUS, static_cast<uint32_t>(VIRTUAL_SCREEN_PLAY));
    ADD_PROP(ScreenPropertyType::VIRTUAL_SEC_LAYER_OPTION, 0);
    ADD_PROP(ScreenPropertyType::IS_HARD_CURSOR_SUPPORT, false);
    ADD_PROP(ScreenPropertyType::SUPPORTED_COLOR_GAMUTS, std::vector<ScreenColorGamut>());
    ADD_PROP(ScreenPropertyType::DISABLE_POWER_OFF_RENDER_CONTROL, false);
    ADD_PROP(ScreenPropertyType::SCREEN_SWITCH_STATUS, false);
    ADD_PROP(ScreenPropertyType::SCREEN_FRAME_GRAVITY, static_cast<int32_t>(Gravity::DEFAULT));

#undef ADD_PROP

    MessageParcel parcel;
    bool ret = srcProperty.Marshalling(parcel);
    ASSERT_TRUE(ret);

    auto dstProperty = sptr<RSScreenProperty>(RSScreenProperty::Unmarshalling(parcel));
    ASSERT_NE(dstProperty, nullptr);
}
}