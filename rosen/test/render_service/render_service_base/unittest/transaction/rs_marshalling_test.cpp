/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "include/core/SkCanvas.h"
#include "include/core/SkData.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPicture.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkRegion.h"
#include "include/core/SkSerialProcs.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkVertices.h"
#include "include/effects/SkImageFilters.h"
#include "message_parcel.h"

#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "render/rs_image.h"
#include "render/rs_path.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMarshallingTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMarshallingTest::SetUpTestCase() {}
void RSMarshallingTest::TearDownTestCase() {}
void RSMarshallingTest::SetUp() {}
void RSMarshallingTest::TearDown() {}

template<typename T>
static void TestNullptrObjectSerialization(T& testObj)
{
    // test result of serialization
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, testObj));

    // test result of deserialization
    T objUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, objUnmarshal));
    ASSERT_TRUE(objUnmarshal == nullptr);
}

static void TestSkDataSerialization(size_t size)
{
    /**
     * @tc.steps: step1. create SkData with size
     */
    sk_sp<SkData> skData;
    if (size == 0) {
        skData = SkData::MakeEmpty();
    } else {
        unsigned char originalData[size];
        skData = SkData::MakeWithoutCopy(originalData, size);
    }

    /**
     * @tc.steps: step2. serialize SkData
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, skData));

    /**
     * @tc.steps: step3. deserialize SkData
     */
    sk_sp<SkData> skDataUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, skDataUnmarshal));
    ASSERT_TRUE(skDataUnmarshal != nullptr);
    ASSERT_EQ(skDataUnmarshal->size(), size);
}

static void TestSkDataSerializationWithCopy(size_t size)
{
    /**
     * @tc.steps: step1. create SkData with size
     */
    sk_sp<SkData> skData;
    if (size == 0) {
        skData = SkData::MakeEmpty();
    } else {
        unsigned char originalData[size];
        skData = SkData::MakeWithoutCopy(originalData, size);
    }

    /**
     * @tc.steps: step2. serialize SkData
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, skData));

    /**
     * @tc.steps: step3. deserialize SkData
     */
    sk_sp<SkData> skDataUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::UnmarshallingWithCopy(parcel, skDataUnmarshal));
    ASSERT_TRUE(skDataUnmarshal != nullptr);
    ASSERT_EQ(skDataUnmarshal->size(), size);
}

static sk_sp<SkImage> CreateSkImage()
{
    const SkImageInfo info = SkImageInfo::MakeN32(200, 200, kOpaque_SkAlphaType);
    auto surface(SkSurface::MakeRaster(info));
    auto canvas = surface->getCanvas();
    canvas->clear(SK_ColorYELLOW);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    canvas->drawRect(SkRect::MakeXYWH(50, 50, 100, 100), paint);
    return surface->makeImageSnapshot();
}

static sk_sp<SkPicture> CreateSkPicture()
{
    SkPictureRecorder recorder;
    SkCanvas* canvas = recorder.beginRecording(200, 200);
    auto image = CreateSkImage();
    canvas->drawImage(image, 0, 0);
    sk_sp<SkPicture> picture(recorder.finishRecordingAsPicture());
    return picture;
}

static sk_sp<SkVertices> CreateSkVertices()
{
    int vCount = 5;
    int iCount = 9;
    uint32_t flags = SkVertices::kHasTexCoords_BuilderFlag | SkVertices::kHasColors_BuilderFlag;
    SkVertices::Builder builder(SkVertices::kTriangles_VertexMode, vCount, iCount, flags);

    for (int i = 0; i < vCount; ++i) {
        float x = static_cast<float>(i);
        builder.positions()[i].set(x, 1);
        builder.texCoords()[i].set(x, 2);
        builder.colors()[i] = SkColorSetARGB(0x5F, i, 0x80, 0);
    }
    for (int i = 0; i < builder.indexCount(); ++i) {
        builder.indices()[i] = i % vCount;
    }

    sk_sp<SkVertices> vertices = builder.detach();
    return vertices;
}

static SkPath CreateSkPath()
{
    SkPath path;
    path.moveTo(20, 20);
    path.quadTo(20, 60, 80, 50);
    path.quadTo(20, 60, 20, 80);
    return path;
}

/**
 * @tc.name: NullptrObjectSerialization001
 * @tc.desc: test results of serialization and deserialization of nullptr object
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, NullptrObjectSerialization001, Function | MediumTest | Level2)
{
    sk_sp<SkData> skData;
    TestNullptrObjectSerialization(skData);

    sk_sp<SkTextBlob> textBlob;
    TestNullptrObjectSerialization(textBlob);

    sk_sp<SkImage> image;
    TestNullptrObjectSerialization(image);

    sk_sp<SkPicture> picture;
    TestNullptrObjectSerialization(picture);

    sk_sp<SkVertices> vertices;
    TestNullptrObjectSerialization(vertices);

    sk_sp<SkFlattenable> flattenable;
    TestNullptrObjectSerialization(flattenable);

    sk_sp<SkDrawable> drawable;
    TestNullptrObjectSerialization(drawable);

    sk_sp<SkImageFilter> imageFilter;
    TestNullptrObjectSerialization(imageFilter);

    std::shared_ptr<RSShader> rsShader;
    TestNullptrObjectSerialization(rsShader);

    std::shared_ptr<RSPath> rsPath;
    TestNullptrObjectSerialization(rsPath);

    std::shared_ptr<RSMask> rsMask;
    TestNullptrObjectSerialization(rsMask);

    std::shared_ptr<RSFilter> rsFilter;
    TestNullptrObjectSerialization(rsFilter);

    std::shared_ptr<RSImage> rsImage;
    TestNullptrObjectSerialization(rsImage);

    std::shared_ptr<Media::PixelMap> pixelMap;
    TestNullptrObjectSerialization(pixelMap);
}

/**
 * @tc.name: SkDataSerialization001
 * @tc.desc: test results of serialization and deserialization of empty SkData
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization001, Function | MediumTest | Level2)
{
    TestSkDataSerialization(0);
}

/**
 * @tc.name: SkDataSerialization002
 * @tc.desc: test results of serialization and deserialization of SkData with small size
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization002, Function | MediumTest | Level2)
{
    TestSkDataSerialization(1024 * 2); // 2kb
}

/**
 * @tc.name: SkDataSerialization003
 * @tc.desc: test results of serialization and deserialization of SkData using Ashmem
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization003, Function | MediumTest | Level2)
{
    TestSkDataSerialization(1024 * 9); // 9kb
}

/**
 * @tc.name: SkDataSerialization004
 * @tc.desc: test results of serialization and deserialization of empty SkData
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization004, Function | MediumTest | Level2)
{
    TestSkDataSerializationWithCopy(0);
}

/**
 * @tc.name: SkDataSerialization005
 * @tc.desc: test results of serialization and deserialization of SkData with small size
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization005, Function | MediumTest | Level2)
{
    TestSkDataSerializationWithCopy(1024 * 2); // 2kb
}

/**
 * @tc.name: SkDataSerialization006
 * @tc.desc: test results of serialization and deserialization of SkData using Ashmem
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization006, Function | MediumTest | Level2)
{
    TestSkDataSerializationWithCopy(1024 * 9); // 9kb
}

/**
 * @tc.name: SkTextBlobSerialization001
 * @tc.desc: test results of serialization and deserialization of SkTextBlob
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkTextBlobSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkTextBlob with SkTypeface
     */
    sk_sp<SkTypeface> tf = SkTypeface::MakeFromName(nullptr, SkFontStyle::BoldItalic());
    SkFont font;
    font.setSize(16);
    font.setTypeface(tf);
    sk_sp<SkTextBlob> textBlob = SkTextBlob::MakeFromString("TextBlob", font);

    /**
     * @tc.steps: step2. serialize SkTextBlob
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, textBlob));

    /**
     * @tc.steps: step3. deserialize SkTextBlob
     */
    sk_sp<SkTextBlob> textBlobUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, textBlobUnmarshal));
    ASSERT_TRUE(textBlobUnmarshal != nullptr);
    ASSERT_EQ(textBlobUnmarshal->bounds(), textBlob->bounds());
}

/**
 * @tc.name: SkPaintSerialization001
 * @tc.desc: test results of serialization and deserialization of SkPaint
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkPaintSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkPaint with params
     */
    SkPaint paint;
    paint.setColor(0x00AABBCC);
    paint.setBlendMode(SkBlendMode::kModulate);
    paint.setStrokeWidth(10);

    /**
     * @tc.steps: step2. serialize SkPaint
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, paint));

    /**
     * @tc.steps: step3. deserialize SkPaint
     */
    SkPaint paintUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, paintUnmarshal));
    ASSERT_EQ(paintUnmarshal.getColor(), paint.getColor());
    ASSERT_EQ(paintUnmarshal.getBlendMode(), paint.getBlendMode());
    ASSERT_EQ(paintUnmarshal.getStrokeWidth(), paint.getStrokeWidth());
}

/**
 * @tc.name: SkImageSerialization001
 * @tc.desc: test results of serialization and deserialization of SkImage
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkImageSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkImage
     */
    auto skImage = CreateSkImage();

    /**
     * @tc.steps: step2. serialize SkImage
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, skImage));

    /**
     * @tc.steps: step3. deserialize SkImage
     */
    sk_sp<SkImage> skImageUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, skImageUnmarshal));
    ASSERT_TRUE(skImageUnmarshal != nullptr);
}

/**
 * @tc.name: RSImageSerialization001
 * @tc.desc: test results of serialization and deserialization of RSImage
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, RSImageSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create RSImage
     */
    auto skImage = CreateSkImage();

    auto rsImage = std::make_shared<RSImage>();
    rsImage->SetImage(skImage);
    /**
     * @tc.steps: step2. serialize RSImage
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, rsImage));

    /**
     * @tc.steps: step3. deserialize RSImage
     */
    std::shared_ptr<RSImage> rsImageUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, rsImageUnmarshal));
    ASSERT_TRUE(rsImageUnmarshal != nullptr);
}

/**
 * @tc.name: RSImageSerialization002
 * @tc.desc: test results of serialization and deserialization of RSImage
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, RSImageSerialization002, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create RSImage
     */
    auto skImage = CreateSkImage();
    auto skData = SkData::MakeWithCString("test");

    auto rsImage = std::make_shared<RSImage>();
    rsImage->SetImage(skImage);
    rsImage->SetCompressData(skData, 0, 10, 10);
    /**
     * @tc.steps: step2. serialize RSImage
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, rsImage));

    /**
     * @tc.steps: step3. deserialize RSImage
     */
    std::shared_ptr<RSImage> rsImageUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, rsImageUnmarshal));
    ASSERT_TRUE(rsImageUnmarshal != nullptr);
}

/**
 * @tc.name: SkPictureSerialization001
 * @tc.desc: test results of serialization and deserialization of SkPicture
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkPictureSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkPicture
     */
    auto skPicture = CreateSkPicture();

    /**
     * @tc.steps: step2. serialize SkPicture
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, skPicture));

    /**
     * @tc.steps: step3. deserialize SkPicture
     */
    sk_sp<SkPicture> skPictureUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, skPictureUnmarshal));
    ASSERT_TRUE(skPictureUnmarshal != nullptr);
}

/**
 * @tc.name: SkVerticesSerialization001
 * @tc.desc: test results of serialization and deserialization of SkVertices
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkVerticesSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkVertices
     */
    sk_sp<SkVertices> vertices = CreateSkVertices();

    /**
     * @tc.steps: step2. serialize SkVertices
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, vertices));

    /**
     * @tc.steps: step3. deserialize SkVertices
     */
    sk_sp<SkVertices> verticesUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, verticesUnmarshal));
    ASSERT_TRUE(verticesUnmarshal != nullptr);
    ASSERT_EQ(verticesUnmarshal->vertexCount(), vertices->vertexCount());
    ASSERT_EQ(verticesUnmarshal->indexCount(), vertices->indexCount());
    ASSERT_EQ(verticesUnmarshal->mode(), vertices->mode());
}

/**
 * @tc.name: SkRegionSerialization001
 * @tc.desc: test results of serialization and deserialization of SkRegion
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkRegionSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkRegion
     */
    SkRegion region;
    region.op(SkIRect::MakeXYWH(0, 0, 10, 10), SkRegion::kUnion_Op);
    region.op(SkIRect::MakeLTRB(5, 10, 20, 20), SkRegion::kUnion_Op);

    /**
     * @tc.steps: step2. serialize SkVertices
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, region));

    /**
     * @tc.steps: step3. deserialize SkVertices
     */
    SkRegion regionUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, regionUnmarshal));
    ASSERT_TRUE(regionUnmarshal == region);
}

/**
 * @tc.name: SkPathSerialization001
 * @tc.desc: test results of serialization and deserialization of SkPath
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkPathSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkPath
     */
    SkPath path = CreateSkPath();

    /**
     * @tc.steps: step2. serialize SkPath
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, path));

    /**
     * @tc.steps: step3. deserialize SkPath
     */
    SkPath pathUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, pathUnmarshal));
    ASSERT_TRUE(pathUnmarshal == path);
}

/**
 * @tc.name: RSPathSerialization001
 * @tc.desc: test results of serialization and deserialization of RSPath
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, RSPathSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create RSPath
     */
    SkPath path = CreateSkPath();
    auto rsPath = RSPath::CreateRSPath(path);

    /**
     * @tc.steps: step2. serialize RSPath
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, rsPath));

    /**
     * @tc.steps: step3. deserialize RSPath
     */
    std::shared_ptr<RSPath> rsPathUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, rsPathUnmarshal));
    ASSERT_TRUE(rsPathUnmarshal != nullptr);
    ASSERT_EQ(rsPathUnmarshal->GetSkiaPath(), path);
}

/**
 * @tc.name: SkImageFilterSerialization001
 * @tc.desc: test results of serialization and deserialization of SkImageFilter
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkImageFilterSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkImageFilter
     */
    sk_sp<SkImage> image = CreateSkImage();
    sk_sp<SkImageFilter> filter(SkImageFilters::Image(image));
    /**
     * @tc.steps: step2. serialize SkImageFilter
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, filter));

    /**
     * @tc.steps: step3. deserialize SkImageFilter
     */
    sk_sp<SkImageFilter> filterUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, filterUnmarshal));
    ASSERT_TRUE(filterUnmarshal != nullptr);
    ASSERT_EQ(filterUnmarshal->GetFlattenableType(), filter->GetFlattenableType());
}

/**
 * @tc.name: SkShaderSerialization001
 * @tc.desc: test results of serialization and deserialization of SkShader
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkShaderSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkShader
     */
    SkBitmap bitmap;
    bitmap.allocN32Pixels(200, 200);
    bitmap.eraseColor(0xffff0000);
    bitmap.erase(0xff47f07f, SkIRect::MakeWH(150, 150));
    bitmap.erase(0xff3f53ff, SkIRect::MakeXYWH(150, 150, 50, 50));
    auto shader = bitmap.makeShader(SkTileMode::kRepeat, SkTileMode::kRepeat);

    /**
     * @tc.steps: step2. serialize SkShader
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, shader));

    /**
     * @tc.steps: step3. deserialize SkShader
     */
    sk_sp<SkShader> shaderUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, shaderUnmarshal));
    ASSERT_TRUE(shaderUnmarshal != nullptr);
}

/**
 * @tc.name: DrawCmdListSerialization001
 * @tc.desc: test results of serialization and deserialization of DrawCmdList
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, DrawCmdListSerialization001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create DrawCmdList
     */
    std::shared_ptr<DrawCmdList> drawCmdList = std::make_shared<DrawCmdList>(100, 200);

    SkFont font;
    font.setSize(16);
    sk_sp<SkTextBlob> textBlob = SkTextBlob::MakeFromString("TextBlob", font);
    SkPaint paint;
    paint.setColor(SK_ColorRED);
    auto op1 = std::make_unique<TextBlobOpItem>(textBlob, 10.0f, 20.0f, paint);
    drawCmdList->AddOp(std::move(op1));

    SkRect rect = SkRect::MakeWH(50, 100);
    auto op2 = std::make_unique<ClipRectOpItem>(rect, SkClipOp::kDifference, false);
    drawCmdList->AddOp(std::move(op2));

    auto image = CreateSkImage();
    SkVector radii[4] = { { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 } };
    Rosen::RsImageInfo rsImageInfo(0, 0, radii, 0, 0, 0, 0);
    auto op3 = std::make_unique<ImageWithParmOpItem>(image, nullptr, rsImageInfo, paint);
    drawCmdList->AddOp(std::move(op3));

    /**
     * @tc.steps: step2. serialize DrawCmdList
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, drawCmdList));

    /**
     * @tc.steps: step3. deserialize DrawCmdList
     */
    std::shared_ptr<DrawCmdList> drawCmdListUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, drawCmdListUnmarshal));
    ASSERT_TRUE(drawCmdListUnmarshal != nullptr);
    ASSERT_EQ(drawCmdListUnmarshal->GetWidth(), drawCmdList->GetWidth());
    ASSERT_EQ(drawCmdListUnmarshal->GetHeight(), drawCmdList->GetHeight());
    ASSERT_EQ(drawCmdListUnmarshal->GetSize(), drawCmdList->GetSize());
}

/**
 * @tc.name: SkipSkImage001
 * @tc.desc: test results of serialization and deserialization of SkPath
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkipSkImage001, Function | MediumTest | Level2)
{
    Parcel parcel;
    RSMarshallingHelper::SkipSkImage(parcel);
}
} // namespace OHOS::Rosen