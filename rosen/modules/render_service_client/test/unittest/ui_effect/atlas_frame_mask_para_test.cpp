/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "mask/include/atlas_frame_mask_para.h"
#include "mask/include/mask.h"
#include "mask/include/mask_para.h"
#include "mask/include/mask_unmarshalling_singleton.h"
#include "pixel_map.h"

namespace OHOS {
namespace Rosen {

using namespace testing;
using namespace testing::ext;

namespace {
constexpr int32_t ATLAS_TEST_MODE_NONE = 0;
constexpr int32_t ATLAS_TEST_MODE_FRAME_BLEND = 1;
constexpr int32_t ATLAS_TEST_ROWS = 4;
constexpr int32_t ATLAS_TEST_COLS = 4;
constexpr int32_t ATLAS_TEST_TOTAL_FRAME = 16;
constexpr float ATLAS_TEST_FRAME_WIDTH = 360.0f;
constexpr float ATLAS_TEST_FRAME_HEIGHT = 360.0f;
constexpr float ATLAS_TEST_PADDING = 2.0f;
constexpr float ATLAS_TEST_FRAME_INDEX = 8.0f;

std::shared_ptr<Media::PixelMap> CreateTestPixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    return Media::PixelMap::Create(opts);
}

AtlasInfo MakeFullAtlasInfo()
{
    AtlasInfo info;
    info.mode = ATLAS_TEST_MODE_FRAME_BLEND;
    info.rows = ATLAS_TEST_ROWS;
    info.cols = ATLAS_TEST_COLS;
    info.frameWidth = ATLAS_TEST_FRAME_WIDTH;
    info.frameHeight = ATLAS_TEST_FRAME_HEIGHT;
    info.padding = ATLAS_TEST_PADDING;
    info.totalFrame = ATLAS_TEST_TOTAL_FRAME;
    info.frameIndex = ATLAS_TEST_FRAME_INDEX;
    info.pixelMap = CreateTestPixelMap(64, 64); // 64 for image width and height
    return info;
}
} // namespace

class RSAtlasFrameMaskParaTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAtlasFrameMaskParaTest::SetUpTestCase()
{
    Mask::RegisterUnmarshallingCallback();
    AtlasFrameMaskPara::RegisterUnmarshallingCallback();
}
void RSAtlasFrameMaskParaTest::TearDownTestCase() {}
void RSAtlasFrameMaskParaTest::SetUp() {}
void RSAtlasFrameMaskParaTest::TearDown() {}

/**
 * @tc.name: AtlasFrameMaskParaType001
 * @tc.desc: Verify AtlasFrameMaskPara type is ATLAS_FRAME_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaType001, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    EXPECT_EQ(para->GetMaskParaType(), MaskPara::Type::ATLAS_FRAME_MASK);
}

/**
 * @tc.name: AtlasFrameMaskParaSetAtlasInfoClamp001
 * @tc.desc: Verify SetAtlasInfo preserves valid values
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaSetAtlasInfoClamp001, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    para->SetAtlasInfo(info);
    const auto& got = para->GetAtlasInfo();
    EXPECT_EQ(got.mode, ATLAS_TEST_MODE_FRAME_BLEND);
    EXPECT_EQ(got.rows, ATLAS_TEST_ROWS);
    EXPECT_EQ(got.cols, ATLAS_TEST_COLS);
    EXPECT_EQ(got.totalFrame, ATLAS_TEST_TOTAL_FRAME);
    EXPECT_FLOAT_EQ(got.frameWidth, ATLAS_TEST_FRAME_WIDTH);
    EXPECT_FLOAT_EQ(got.frameHeight, ATLAS_TEST_FRAME_HEIGHT);
    EXPECT_FLOAT_EQ(got.padding, ATLAS_TEST_PADDING);
    EXPECT_FLOAT_EQ(got.frameIndex, ATLAS_TEST_FRAME_INDEX);
    EXPECT_NE(got.pixelMap, nullptr);
}

/**
 * @tc.name: AtlasFrameMaskParaClampModeOutOfRange
 * @tc.desc: Verify mode is clamped to [0, 1]
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampModeOutOfRange, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.mode = -1;
    para->SetAtlasInfo(info);
    EXPECT_EQ(para->GetAtlasInfo().mode, ATLAS_TEST_MODE_NONE);

    info.mode = 2;
    para->SetAtlasInfo(info);
    EXPECT_EQ(para->GetAtlasInfo().mode, ATLAS_TEST_MODE_FRAME_BLEND);
}

/**
 * @tc.name: AtlasFrameMaskParaClampRowsColsMin
 * @tc.desc: Verify rows/cols are clamped to minimum 1
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampRowsColsMin, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.rows = 0;
    info.cols = -5;
    info.totalFrame = 3;
    para->SetAtlasInfo(info);
    EXPECT_GE(para->GetAtlasInfo().rows, 1);
    EXPECT_GE(para->GetAtlasInfo().cols, 1);
}

/**
 * @tc.name: AtlasFrameMaskParaClampTotalFrameDynamic
 * @tc.desc: Verify totalFrame is clamped to rows*cols upper bound
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampTotalFrameDynamic, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.rows = 4;
    info.cols = 4;
    info.totalFrame = 999;
    para->SetAtlasInfo(info);
    EXPECT_EQ(para->GetAtlasInfo().totalFrame, 16);
}

/**
 * @tc.name: AtlasFrameMaskParaClampRowsColsToTotalFrame
 * @tc.desc: Verify rows/cols are clamped to not exceed totalFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampRowsColsToTotalFrame, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.rows = 10;
    info.cols = 10;
    info.totalFrame = 3;
    para->SetAtlasInfo(info);
    EXPECT_EQ(para->GetAtlasInfo().rows, 3);
    EXPECT_EQ(para->GetAtlasInfo().cols, 3);
}

/**
 * @tc.name: AtlasFrameMaskParaClampFrameSizeLimits
 * @tc.desc: Verify frameWidth/frameHeight are clamped to [1, 8192]
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampFrameSizeLimits, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.frameWidth = 0.5f;
    info.frameHeight = 0.5f;
    para->SetAtlasInfo(info);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().frameWidth, 1.0f);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().frameHeight, 1.0f);

    info.frameWidth = 9999.0f;
    info.frameHeight = 9999.0f;
    para->SetAtlasInfo(info);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().frameWidth, 8192.0f);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().frameHeight, 8192.0f);
}

/**
 * @tc.name: AtlasFrameMaskParaClampPaddingLimits
 * @tc.desc: Verify padding is clamped to [0, 64]
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampPaddingLimits, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.padding = -1.0f;
    para->SetAtlasInfo(info);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().padding, 0.0f);

    info.padding = 100.0f;
    para->SetAtlasInfo(info);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().padding, 64.0f);
}

/**
 * @tc.name: AtlasFrameMaskParaClampFrameIndexRange
 * @tc.desc: Verify frameIndex is clamped to [0, totalFrame-1]
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaClampFrameIndexRange, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.totalFrame = 16;
    info.frameIndex = -5.0f;
    para->SetAtlasInfo(info);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().frameIndex, 0.0f);

    info.frameIndex = 100.0f;
    para->SetAtlasInfo(info);
    EXPECT_FLOAT_EQ(para->GetAtlasInfo().frameIndex, 15.0f);
}

/**
 * @tc.name: AtlasFrameMaskParaCloneWithPixelMap
 * @tc.desc: Verify Clone copies fields and clones pixelMap
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaCloneWithPixelMap, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    para->SetAtlasInfo(MakeFullAtlasInfo());
    auto clone = para->Clone();
    ASSERT_NE(clone, nullptr);
    EXPECT_EQ(clone->GetMaskParaType(), MaskPara::Type::ATLAS_FRAME_MASK);
    auto atlasClone = std::static_pointer_cast<AtlasFrameMaskPara>(clone);
    const auto& orig = para->GetAtlasInfo();
    const auto& cloned = atlasClone->GetAtlasInfo();
    EXPECT_EQ(cloned.mode, orig.mode);
    EXPECT_EQ(cloned.rows, orig.rows);
    EXPECT_EQ(cloned.totalFrame, orig.totalFrame);
    EXPECT_FLOAT_EQ(cloned.frameIndex, orig.frameIndex);
    EXPECT_NE(cloned.pixelMap, nullptr);
}

/**
 * @tc.name: AtlasFrameMaskParaCloneNullPixelMap
 * @tc.desc: Verify Clone handles null pixelMap
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaCloneNullPixelMap, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    auto info = MakeFullAtlasInfo();
    info.pixelMap = nullptr;
    para->SetAtlasInfo(info);
    auto clone = para->Clone();
    ASSERT_NE(clone, nullptr);
    auto atlasClone = std::static_pointer_cast<AtlasFrameMaskPara>(clone);
    EXPECT_EQ(atlasClone->GetAtlasInfo().pixelMap, nullptr);
}

/**
 * @tc.name: AtlasFrameMaskParaMarshallingRoundTrip
 * @tc.desc: Verify Marshalling/OnUnmarshalling round trip with pixelMap
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaMarshallingRoundTrip, TestSize.Level1)
{
    auto para = std::make_shared<AtlasFrameMaskPara>();
    ASSERT_NE(para, nullptr);
    para->SetAtlasInfo(MakeFullAtlasInfo());

    Parcel parcel;
    EXPECT_TRUE(para->Marshalling(parcel));
    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_TRUE(MaskPara::Unmarshalling(parcel, val));
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(val->GetMaskParaType(), MaskPara::Type::ATLAS_FRAME_MASK);
    auto recovered = std::static_pointer_cast<AtlasFrameMaskPara>(val);
    const auto& got = recovered->GetAtlasInfo();
    EXPECT_EQ(got.mode, ATLAS_TEST_MODE_FRAME_BLEND);
    EXPECT_EQ(got.rows, ATLAS_TEST_ROWS);
    EXPECT_EQ(got.cols, ATLAS_TEST_COLS);
    EXPECT_EQ(got.totalFrame, ATLAS_TEST_TOTAL_FRAME);
    EXPECT_FLOAT_EQ(got.frameWidth, ATLAS_TEST_FRAME_WIDTH);
    EXPECT_FLOAT_EQ(got.frameHeight, ATLAS_TEST_FRAME_HEIGHT);
    EXPECT_FLOAT_EQ(got.padding, ATLAS_TEST_PADDING);
    EXPECT_NE(got.pixelMap, nullptr);
}

/**
 * @tc.name: AtlasFrameMaskParaOnUnmarshallingBadType
 * @tc.desc: Verify OnUnmarshalling fails on wrong type
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaOnUnmarshallingBadType, TestSize.Level1)
{
    Parcel parcel;
    parcel.WriteUint16(static_cast<uint16_t>(MaskPara::Type::RIPPLE_MASK));
    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_FALSE(AtlasFrameMaskPara::OnUnmarshalling(parcel, val));
    EXPECT_EQ(val, nullptr);
}

/**
 * @tc.name: AtlasFrameMaskParaOnUnmarshallingEmpty
 * @tc.desc: Verify OnUnmarshalling fails on empty parcel
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaOnUnmarshallingEmpty, TestSize.Level1)
{
    Parcel parcel;
    std::shared_ptr<MaskPara> val = nullptr;
    EXPECT_FALSE(AtlasFrameMaskPara::OnUnmarshalling(parcel, val));
    EXPECT_EQ(val, nullptr);
}

/**
 * @tc.name: AtlasFrameMaskParaRegisterCallback
 * @tc.desc: Verify RegisterUnmarshallingCallback registers ATLAS_FRAME_MASK
 * @tc.type: FUNC
 */
HWTEST_F(RSAtlasFrameMaskParaTest, AtlasFrameMaskParaRegisterCallback, TestSize.Level1)
{
    auto& singleton = MaskUnmarshallingSingleton::GetInstance();
    auto cb = singleton.GetCallback(static_cast<uint16_t>(MaskPara::Type::ATLAS_FRAME_MASK));
    EXPECT_NE(cb, nullptr);
}
} // namespace Rosen
} // namespace OHOS
