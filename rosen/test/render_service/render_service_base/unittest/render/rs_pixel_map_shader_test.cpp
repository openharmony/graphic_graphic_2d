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
class RSPixelMapShaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPixelMapShaderTest::SetUpTestCase() {}
void RSPixelMapShaderTest::TearDownTestCase() {}
void RSPixelMapShaderTest::SetUp() {}
void RSPixelMapShaderTest::TearDown() {}

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
 * @tc.name: CreateRSPixelMapShaderTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, CreateRSPixelMapShaderTest001, TestSize.Level1)
{
    RSPixelMapShader* rsPixelMapShader = new RSPixelMapShader();
    ASSERT_NE(rsPixelMapShader, nullptr);
    delete rsPixelMapShader;
}

/**
 * @tc.name: CreateRSPixelMapShaderTest002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, CreateRSPixelMapShaderTest002, TestSize.Level1)
{
    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* rsPixelMapShader = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader, nullptr);
    delete rsPixelMapShader;
}

/**
 * @tc.name: CreateRSPixelMapShaderTest003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, CreateRSPixelMapShaderTest003, TestSize.Level1)
{
    RSPixelMapShader* rsPixelMapShader = new RSPixelMapShader(nullptr, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader, nullptr);
    delete rsPixelMapShader;
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, MarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSPixelMapShader* rsPixelMapShader1 = new RSPixelMapShader();
    ASSERT_NE(rsPixelMapShader1, nullptr);
    ASSERT_EQ(rsPixelMapShader1->Marshalling(parcel), true);
    delete rsPixelMapShader1;

    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* rsPixelMapShader2 = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader2, nullptr);
    ASSERT_EQ(rsPixelMapShader2->Marshalling(parcel), true);
    delete rsPixelMapShader2;

    RSPixelMapShader* rsPixelMapShader3 = new RSPixelMapShader(nullptr, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader3, nullptr);
    ASSERT_EQ(rsPixelMapShader3->Marshalling(parcel), true);
    delete rsPixelMapShader3;
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, UnmarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* rsPixelMapShader1 = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader1, nullptr);
    ASSERT_EQ(rsPixelMapShader1->Marshalling(parcel), true);
    delete rsPixelMapShader1;

    RSPixelMapShader* rsPixelMapShader2 = new RSPixelMapShader();
    ASSERT_NE(rsPixelMapShader2, nullptr);
    ASSERT_EQ(rsPixelMapShader2->Unmarshalling(parcel), true);
    delete rsPixelMapShader2;
}

/**
 * @tc.name: GenerateBaseObjectTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, GenerateBaseObjectTest, TestSize.Level1)
{
    RSPixelMapShader* rsPixelMapShader1 = new RSPixelMapShader();
    ASSERT_NE(rsPixelMapShader1, nullptr);
    void* base1 = rsPixelMapShader1->GenerateBaseObject();
    ASSERT_EQ(base1, nullptr);
    delete rsPixelMapShader1;

    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    RSPixelMapShader* rsPixelMapShader2 = new RSPixelMapShader(pixelMap, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader2, nullptr);
    void* base2 = rsPixelMapShader2->GenerateBaseObject();
    ASSERT_NE(base2, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> shaderEffect;
    shaderEffect.reset(reinterpret_cast<Drawing::ShaderEffect*>(base2));
    delete rsPixelMapShader2;

    RSPixelMapShader* rsPixelMapShader3 = new RSPixelMapShader(nullptr, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader3, nullptr);
    void* base3 = rsPixelMapShader3->GenerateBaseObject();
    ASSERT_EQ(base3, nullptr);
    delete rsPixelMapShader3;
}

/**
 * @tc.name: ExtendShaderTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSPixelMapShaderTest, ExtendShaderTest, TestSize.Level1)
{
    int width = 200;
    int height = 300;
    std::shared_ptr<Media::PixelMap> pixelMap = CreatePixelMap(width, height);
    std::shared_ptr<RSPixelMapShader> rsPixelMapShader = std::make_shared<RSPixelMapShader>(
        pixelMap, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, Drawing::SamplingOptions(), Drawing::Matrix());
    ASSERT_NE(rsPixelMapShader, nullptr);
    std::shared_ptr<Drawing::ShaderEffect> effect = Drawing::ShaderEffect::CreateExtendShader(rsPixelMapShader);
    ASSERT_NE(effect, nullptr);
    ASSERT_EQ(effect->GetExtendObject(), rsPixelMapShader);

    auto cmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, 0 }, false);
    Drawing::FlattenableHandle handle = Drawing::CmdListHelper::AddShaderEffectToCmdList(*cmdList, effect);
    EXPECT_EQ(handle.size, 1);
    std::shared_ptr<Drawing::ShaderEffect> effect_ =
        Drawing::CmdListHelper::GetShaderEffectFromCmdList(*cmdList, handle);
    ASSERT_NE(effect_, nullptr);
}
} // namespace OHOS::Rosen