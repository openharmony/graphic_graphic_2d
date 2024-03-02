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
#include "draw/canvas.h"
#include "draw/path.h"
#include "effect/image_filter.h"
#include "image/image.h"
#include "image/image_info.h"
#include "image/picture.h"
#include "text/text_blob.h"
#include "text/typeface.h"
#include "utils/data.h"
#include "utils/rect.h"
#include "utils/region.h"
#include "utils/vertices.h"
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

static void TestDrawingDataSerialization(size_t size)
{
    /**
     * @tc.steps: step1. create SkData with size
     */
    auto drawingData = std::make_shared<Drawing::Data>();
    if (size == 0) {
        drawingData->BuildEmpty();
    } else {
        unsigned char originalData[size];
        drawingData->BuildWithCopy(originalData, size);
    }

    /**
     * @tc.steps: step2. serialize SkData
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, drawingData));

    /**
     * @tc.steps: step3. deserialize SkData
     */
    std::shared_ptr<Drawing::Data> dataUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, dataUnmarshal));
    ASSERT_TRUE(dataUnmarshal != nullptr);
    ASSERT_EQ(dataUnmarshal->GetSize(), size);
}

static void TestDrawingDataSerializationWithCopy(size_t size)
{
    /**
     * @tc.steps: step1. create SkData with size
     */
    auto drawingData = std::make_shared<Drawing::Data>();
    if (size == 0) {
        drawingData->BuildEmpty();
    } else {
        unsigned char originalData[size];
        drawingData->BuildWithoutCopy(originalData, size);
    }

    /**
     * @tc.steps: step2. serialize SkData
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, drawingData));

    /**
     * @tc.steps: step3. deserialize SkData
     */
    std::shared_ptr<Drawing::Data> dataUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::UnmarshallingWithCopy(parcel, dataUnmarshal));
    ASSERT_TRUE(dataUnmarshal != nullptr);
    ASSERT_EQ(dataUnmarshal->GetSize(), size);
}

static std::shared_ptr<Drawing::Image> CreateDrawingImage()
{
    const Drawing::ImageInfo info =
        Drawing::ImageInfo(200, 200, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE);
    auto surface(Drawing::Surface::MakeRaster(info));
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(50, 50, 100, 100));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}



static Drawing::Path CreateDrawingPath()
{
    Drawing::Path path;
    path.MoveTo(20, 20);
    path.QuadTo(20, 60, 80, 50);
    path.QuadTo(20, 60, 20, 80);
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
    std::shared_ptr<Drawing::Data> data;
    TestNullptrObjectSerialization(data);

    std::shared_ptr<Drawing::TextBlob> textBlob;
    TestNullptrObjectSerialization(textBlob);

    std::shared_ptr<Drawing::Image> image;
    TestNullptrObjectSerialization(image);

    std::shared_ptr<Drawing::Picture> picture;
    TestNullptrObjectSerialization(picture);

    std::shared_ptr<Drawing::Vertices> vertices;
    TestNullptrObjectSerialization(vertices);

    std::shared_ptr<Drawing::ImageFilter> imageFilter;
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
    TestDrawingDataSerialization(0);
}

/**
 * @tc.name: SkDataSerialization002
 * @tc.desc: test results of serialization and deserialization of SkData with small size
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization002, Function | MediumTest | Level2)
{
    TestDrawingDataSerialization(1024 * 2); // 2kb
}

/**
 * @tc.name: SkDataSerialization003
 * @tc.desc: test results of serialization and deserialization of SkData using Ashmem
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization003, Function | MediumTest | Level2)
{
    TestDrawingDataSerialization(1024 * 9); // 9kb
}

/**
 * @tc.name: SkDataSerialization004
 * @tc.desc: test results of serialization and deserialization of empty SkData
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization004, Function | MediumTest | Level2)
{
    TestDrawingDataSerializationWithCopy(0);
}

/**
 * @tc.name: SkDataSerialization005
 * @tc.desc: test results of serialization and deserialization of SkData with small size
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization005, Function | MediumTest | Level2)
{
    TestDrawingDataSerializationWithCopy(1024 * 2); // 2kb
}

/**
 * @tc.name: SkDataSerialization006
 * @tc.desc: test results of serialization and deserialization of SkData using Ashmem
 * @tc.type:FUNC
 * @tc.require: issueI5Q60U
 */
HWTEST_F(RSMarshallingTest, SkDataSerialization006, Function | MediumTest | Level2)
{
    TestDrawingDataSerializationWithCopy(1024 * 9); // 9kb
}

/**
 * @tc.name: SkTextBlobSerialization001
 * @tc.desc: test results of serialization and deserialization of SkTextBlob
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */

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
    Drawing::Pen pen;
    pen.SetColor(0x00AABBCC);
    pen.SetBlendMode(Drawing::BlendMode::MODULATE);
    pen.SetWidth(10);

    /**
     * @tc.steps: step2. serialize SkPaint
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, pen));

    /**
     * @tc.steps: step3. deserialize SkPaint
     */
    Drawing::Pen penUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, penUnmarshal));
    ASSERT_EQ(penUnmarshal.GetColor(), pen.GetColor());
#ifndef NEW_SKIA
    ASSERT_EQ(penUnmarshal.GetBlendMode(), pen.GetBlendMode());
#endif
    ASSERT_EQ(penUnmarshal.GetWidth(), pen.GetWidth());
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
    auto image = CreateDrawingImage();

    /**
     * @tc.steps: step2. serialize SkImage
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, image));

    /**
     * @tc.steps: step3. deserialize SkImage
     */
    std::shared_ptr<Drawing::Image> imageUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, imageUnmarshal));
    ASSERT_TRUE(imageUnmarshal != nullptr);
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
    auto image = CreateDrawingImage();

    auto rsImage = std::make_shared<RSImage>();
    rsImage->SetImage(image);
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

/**
 * @tc.name: SkPictureSerialization001
 * @tc.desc: test results of serialization and deserialization of SkPicture
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */

/**
 * @tc.name: SkVerticesSerialization001
 * @tc.desc: test results of serialization and deserialization of SkVertices
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */

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
    Drawing::Region region;
    Drawing::Region tmpRegion1;
    tmpRegion1.SetRect(Drawing::RectI(0, 0, 10, 10));
    region.Op(tmpRegion1, Drawing::RegionOp::UNION);
    Drawing::Region tmpRegion2;
    tmpRegion2.SetRect(Drawing::RectI(5, 10, 20, 20));
    region.Op(tmpRegion2, Drawing::RegionOp::UNION);

    /**
     * @tc.steps: step2. serialize SkVertices
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, region));

    /**
     * @tc.steps: step3. deserialize SkVertices
     */
    Drawing::Region regionUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, regionUnmarshal));
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
    Drawing::Path path = CreateDrawingPath();

    /**
     * @tc.steps: step2. serialize SkPath
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, path));

    /**
     * @tc.steps: step3. deserialize SkPath
     */
    Drawing::Path pathUnmarshal;
    ASSERT_TRUE(RSMarshallingHelper::Unmarshalling(parcel, pathUnmarshal));
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
    Drawing::Path path = CreateDrawingPath();
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
}

/**
 * @tc.name: SkImageFilterSerialization001
 * @tc.desc: test results of serialization and deserialization of SkImageFilter
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */

/**
 * @tc.name: SkShaderSerialization001
 * @tc.desc: test results of serialization and deserialization of SkShader
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */

/**
 * @tc.name: DrawCmdListSerialization001
 * @tc.desc: test results of serialization and deserialization of DrawCmdList
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */

/**
 * @tc.name: SkipSkImage001
 * @tc.desc: test results of serialization and deserialization of SkPath
 * @tc.type:FUNC
 * @tc.require: issueI54AGD
 */
HWTEST_F(RSMarshallingTest, SkipSkImage001, Function | MediumTest | Level2)
{
    Parcel parcel;
    RSMarshallingHelper::SkipImage(parcel);
}
} // namespace OHOS::Rosen