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

#include <cstddef>

#include "gtest/gtest.h"
#include "skia_adapter/skia_static_factory.h"

#include "text/rs_xform.h"
#include "text/text_blob.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaStaticFactoryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaStaticFactoryTest::SetUpTestCase() {}
void SkiaStaticFactoryTest::TearDownTestCase() {}
void SkiaStaticFactoryTest::SetUp() {}
void SkiaStaticFactoryTest::TearDown() {}

/**
 * @tc.name: MakeFromRSXform001
 * @tc.desc: Test MakeFromRSXform
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaStaticFactoryTest, MakeFromRSXform001, TestSize.Level1)
{
    const char* str = "asdf";
    Font font;
    font.SetSize(100);
    RSXform xform[] = { RSXform::Make(10, 10, 10, 10) }; // 10: cos, sin, tx, ty
    auto skiaStatic = SkiaStaticFactory::MakeFromRSXform(str, strlen(str), xform, font, TextEncoding::UTF8);
    ASSERT_TRUE(skiaStatic != nullptr);
}

/**
 * @tc.name: MakeFromStream001
 * @tc.desc: Test MakeFromStream
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, MakeFromStream001, TestSize.Level1)
{
    MemoryStream memoryStream;
    int index = 0;
    auto skiaStatic = SkiaStaticFactory::MakeFromStream(std::make_unique<MemoryStream>(memoryStream), index);

    ASSERT_TRUE(skiaStatic == nullptr);
}

/**
 * @tc.name: MakeFromName001
 * @tc.desc: Test MakeFromName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, MakeFromName001, TestSize.Level1)
{
    char familyName[] = "qewe";

    FontStyle foustyle;

    auto skiaStatic = SkiaStaticFactory::MakeFromName(familyName, foustyle);
    ASSERT_TRUE(skiaStatic != nullptr);
}

/**
 * @tc.name: MakeFromBackendRenderTarget001
 * @tc.desc: Test MakeFromYUVAPixmaps
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

#ifdef RS_ENABLE_VK
HWTEST_F(SkiaStaticFactoryTest, MakeFromBackendRenderTarget001, TestSize.Level1)
{
    OHOS::Rosen::Drawing::GPUContext gpuContext;
    TextureInfo texttureinfo;
    auto colorSpace = std::make_shared<ColorSpace>(ColorSpace::ColorSpaceType::NO_TYPE);
    void (*deleteVkImage)(void*) = nullptr;

    void* cleanHelper = nullptr;
    auto skiaStatic = SkiaStaticFactory::MakeFromBackendRenderTarget(gpuContext, texttureinfo, TextureOrigin::TOP_LEFT,
        ColorType::COLORTYPE_ALPHA_8, colorSpace, deleteVkImage, cleanHelper);
    ASSERT_TRUE(skiaStatic, nullptr);
    delete (skiaStatic);
}
#endif

/**
 * @tc.name: MakeFromYUVAPixmaps001
 * @tc.desc: Test MakeFromYUVAPixmaps
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, MakeFromYUVAPixmaps001, TestSize.Level1)
{
    OHOS::Rosen::Drawing::GPUContext gpuContext;
    YUVInfo info(100, 100, YUVInfo::PlaneConfig::Y_UV, YUVInfo::SubSampling::K420,
        YUVInfo::YUVColorSpace::JPEG_FULL_YUVCOLORSPACE,
        YUVInfo::YUVDataType::UNORM_8);

    auto skiaStatic = SkiaStaticFactory::MakeFromYUVAPixmaps(gpuContext, info, nullptr);

    ASSERT_TRUE(skiaStatic == nullptr);
}

/**
 * @tc.name: DeserializeTypeface001
 * @tc.desc: Test DeserializeTypeface
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, DeserializeTypeface001, TestSize.Level1)
{
    const char* data = "fcdsafsa";

    auto skiaStatic = SkiaStaticFactory::DeserializeTypeface(data, strlen(data));
    ASSERT_TRUE(skiaStatic == nullptr);
}
/**
 * @tc.name: AsBlendMode001
 * @tc.desc: Test AsBlendMode
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, AsBlendMode001, TestSize.Level1)
{
    Brush brush;
    brush.SetAntiAlias(true);

    bool skiaStatic = SkiaStaticFactory::AsBlendMode(brush);
    ASSERT_TRUE(skiaStatic);
}

/**
 * @tc.name: MakeDataFromFileName001
 * @tc.desc: Test MakeDataFromFileName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, MakeDataFromFileName001, TestSize.Level1)
{
    const char data[] = "";
    auto skiaStatic = SkiaStaticFactory::MakeDataFromFileName(data);
    ASSERT_TRUE(skiaStatic == nullptr);
}

/**
 * @tc.name: GetDrawingPathforTextBlob001
 * @tc.desc: Test GetDrawingPathforTextBlob
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaStaticFactoryTest, GetDrawingPathforTextBlob001, TestSize.Level1)
{
    uint16_t glyphId = 65;

    Rosen::Drawing::Font font;
    font.SetSize(100);

    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    ASSERT_TRUE(textBlob.get() != nullptr);
    auto skiaStatic = SkiaStaticFactory::GetDrawingPathforTextBlob(glyphId, textBlob.get());
    ASSERT_TRUE(skiaStatic.IsValid());
}

/**
 * @tc.name: GetDrawingPointsForTextBlob001
 * @tc.desc: Test GetDrawingPointsForTextBlob
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */

HWTEST_F(SkiaStaticFactoryTest, GetDrawingPointsForTextBlob001, TestSize.Level1)
{
    Rosen::Drawing::Font font;
    font.SetSize(100);
    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    Point p1 { 0, 0 };
    Point p2 { 1, 1 };
    vector<Point> point;
    point.push_back(p1);
    point.push_back(p2);
    ASSERT_TRUE(textBlob.get() != nullptr);
    SkiaStaticFactory::GetDrawingPointsForTextBlob(textBlob.get(), point);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS