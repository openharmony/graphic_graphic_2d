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

#include "gtest/gtest.h"
#include "draw/surface.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "render/rs_pixel_map_shader.h"
#include "pixel_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSPixelMapShaderUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelMapShaderUnitTest::SetUpTestCase() {}
void RSPixelMapShaderUnitTest::TearDownTestCase() {}
void RSPixelMapShaderUnitTest::SetUp() {}
void RSPixelMapShaderUnitTest::TearDown() {}

static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height)
{
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelmap = Media::PixelMap::Create(opts);
    if (pixelmap == nullptr) {
        return nullptr;
    }
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    Drawing::ImageInfo info =
        Drawing::ImageInfo(pixelmap->GetWidth(), pixelmap->GetHeight(),
        Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL);
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    if (surface == nullptr) {
        return nullptr;
    }
    auto canvas = surface->GetCanvas();
    if (canvas == nullptr) {
        return nullptr;
    }
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(0, 0, width, height));
    canvas->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: TestCreateRSPixelMapShader01
 * @tc.desc: Verify function CreateRSPixelMapShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestCreateRSPixelMapShader01, TestSize.Level1)
{
    RSPixelMapShader* testShader = new RSPixelMapShader();
    ASSERT_NE(testShader, nullptr);
    delete testShader;
}

/**
 * @tc.name: TestCreateRSPixelMapShader02
 * @tc.desc: Verify function CreateRSPixelMapShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestCreateRSPixelMapShader02, TestSize.Level1)
{
    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* testShader = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader, nullptr);
    delete testShader;
}

/**
 * @tc.name: TestCreateRSPixelMapShader03
 * @tc.desc: Verify function CreateRSPixelMapShader
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestCreateRSPixelMapShader03, TestSize.Level1)
{
    RSPixelMapShader* testShader = new RSPixelMapShader(nullptr, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader, nullptr);
    delete testShader;
}

/**
 * @tc.name: TestMarshalling01
 * @tc.desc: Verify function Marshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestMarshalling01, TestSize.Level1)
{
    Parcel parcel;
    RSPixelMapShader* testShader1 = new RSPixelMapShader();
    ASSERT_NE(testShader1, nullptr);
    ASSERT_EQ(testShader1->Marshalling(parcel), true);
    delete testShader1;

    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* testShader2 = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader2, nullptr);
    ASSERT_EQ(testShader2->Marshalling(parcel), true);
    delete rsPixelMapShader2;

    RSPixelMapShader* testShader3 = new RSPixelMapShader(nullptr, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader3, nullptr);
    ASSERT_EQ(testShader3->Marshalling(parcel), true);
    delete testShader3;
}

/**
 * @tc.name: TestUnmarshalling01
 * @tc.desc: Verify function Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestUnmarshalling01, TestSize.Level1)
{
    Parcel parcel;
    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* testShader1 = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader1, nullptr);
    ASSERT_EQ(testShader1->Marshalling(parcel), true);
    delete testShader1;

    RSPixelMapShader* testShader2 = new RSPixelMapShader();
    ASSERT_NE(testShader2, nullptr);
    ASSERT_EQ(testShader2->Unmarshalling(parcel), true);
    delete testShader2;
}

/**
 * @tc.name: TestGenerateBaseObject01
 * @tc.desc: Verify function Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestGenerateBaseObject01, TestSize.Level1)
{
    RSPixelMapShader* testShader1 = new RSPixelMapShader();
    ASSERT_NE(testShader1, nullptr);
    void* base1 = testShader1->GenerateBaseObject();
    ASSERT_EQ(base1, nullptr);
    delete testShader1;

    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* testShader2 = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader2, nullptr);
    void* base2 = testShader2->GenerateBaseObject();
    ASSERT_NE(base2, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect;
    shaderEffect.reset(reinterpret_cast<Drawing::ShaderEffect*>(base2));
    delete testShader2;

    RSPixelMapShader* testShader3 = new RSPixelMapShader(nullptr, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader3, nullptr);
    void* base3 = testShader3->GenerateBaseObject();
    ASSERT_EQ(base3, nullptr);
    delete testShader3;
}

/**
 * @tc.name: TestExtendShader01
 * @tc.desc: Verify function Unmarshalling
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSPixelMapShaderUnitTest, TestExtendShader01, TestSize.Level1)
{
    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    std::shared_ptr<RSPixelMapShader> testShader = std::make_shared<RSPixelMapShader>(
        pixelMap, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(testShader, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> effect = Drawing::ShaderEffect::CreateExtendShader(testShader);
    ASSERT_NE(effect, nullptr);
    ASSERT_EQ(effect->GetExtendObject(), testShader);

    auto cmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Drawing::FlattenableHandle handle = Drawing::CmdListHelper::AddShaderEffectToCmdList(*cmdList, effect);
    EXPECT_EQ(handle.size, 1);
    std::shared_ptr<Drawing::ShaderEffect> effect_ =
        Drawing::CmdListHelper::GetShaderEffectFromCmdList(*cmdList, handle);
    ASSERT_NE(effect_, nullptr);
}
} // namespace OHOS::Rosen