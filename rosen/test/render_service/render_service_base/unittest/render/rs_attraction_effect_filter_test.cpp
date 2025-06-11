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

#include "draw/color.h"
#include "image/image_info.h"
#include "gtest/gtest.h"
#include "render/rs_attraction_effect_filter.h"
#include "skia_image.h"
#include "skia_image_info.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSAttractionEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAttractionEffectFilterTest::SetUpTestCase() {}
void RSAttractionEffectFilterTest::TearDownTestCase() {}
void RSAttractionEffectFilterTest::SetUp() {}
void RSAttractionEffectFilterTest::TearDown() {}

static std::shared_ptr<Drawing::Image> CreateDrawingImage(int width, int height)
{
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    Drawing::ImageInfo imageInfo =
        Drawing::ImageInfo(1, 0, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    return image;
}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, DrawImageRect001, TestSize.Level1)
{
    RSAttractionEffectFilter effectFilter(0.f);
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Drawing::Rect src;
    Drawing::Rect dst;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_FALSE(image);

    effectFilter.attractionFraction_ = 1.0f;
    image = std::make_shared<Drawing::Image>();
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image);

    Drawing::ImageInfo imageInfo(1, 1, Drawing::ColorType::COLORTYPE_ALPHA_8, Drawing::AlphaType::ALPHATYPE_OPAQUE);
    auto skImageInfo = Drawing::SkiaImageInfo::ConvertToSkImageInfo(imageInfo);
    int addr1 = 1;
    int* addr = &addr1;
    auto skiaPixmap = SkPixmap(skImageInfo, addr, 1);
    Drawing::ReleaseContext releaseContext = nullptr;
    Drawing::RasterReleaseProc rasterReleaseProc = nullptr;
    sk_sp<SkImage> skImage = SkImage::MakeFromRaster(skiaPixmap, rasterReleaseProc, releaseContext);
    auto skiaImage = std::make_shared<Drawing::SkiaImage>(skImage);
    image->imageImplPtr = skiaImage;
    effectFilter.DrawImageRect(canvas, image, src, dst);
    EXPECT_TRUE(image->GetWidth());
    EXPECT_TRUE(image->GetHeight());
}

/**
 * @tc.name: IsValid001
 * @tc.desc: test results of IsValid
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, IsValid001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.f);
    EXPECT_FALSE(firstEffectFilter.IsValid());

    RSAttractionEffectFilter secondEffectFilter(1.f);
    EXPECT_TRUE(secondEffectFilter.IsValid());
}

/**
 * @tc.name: GetAttractionFraction001
 * @tc.desc: test results of GetAttractionFraction
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, GetAttractionFraction001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    EXPECT_EQ(firstEffectFilter.GetAttractionFraction(), 0.5f);

    RSAttractionEffectFilter secondEffectFilter(1.0f);
    EXPECT_NE(secondEffectFilter.GetAttractionFraction(), 0.5f);
}

/**
 * @tc.name: GetAttractionDirtyRegion001
 * @tc.desc: test results of GetAttractionDirtyRegion
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, GetAttractionDirtyRegion001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    RectI dirtyRegion = firstEffectFilter.GetAttractionDirtyRegion();
    EXPECT_TRUE(dirtyRegion.left_ == 0);

    float leftPoint = 10.f;
    float topPonit = 10.f;
    firstEffectFilter.UpdateDirtyRegion(leftPoint, topPonit);
    dirtyRegion = firstEffectFilter.GetAttractionDirtyRegion();
    EXPECT_FALSE(dirtyRegion.left_ == 0);
}

/**
 * @tc.name: IsWithinThreshold001
 * @tc.desc: test results of IsWithinThreshold
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, IsWithinThreshold001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    float firstPoint = 10.f;
    float secondPoint = 10.f;
    float error = 0.001f;
    EXPECT_TRUE(firstEffectFilter.IsWithinThreshold(firstPoint, secondPoint, error));

    float thirdPoint = 5.f;
    EXPECT_FALSE(firstEffectFilter.IsWithinThreshold(firstPoint, thirdPoint, error));
}

/**
 * @tc.name: LerpPoint001
 * @tc.desc: test results of LerpPoint
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, LerpPoint001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Drawing::Point p1 = { 0.0f, 0.0f };
    Drawing::Point p2 = { 10.0f, 10.0f };
    float firstFactor = 0.5f;
    float secondFactor = 0.5f;
    Drawing::Point point = firstEffectFilter.LerpPoint(p1, p2, firstFactor, secondFactor);
    EXPECT_EQ(point.GetX(), 5.0f);
}

/**
 * @tc.name: CubicBezier001
 * @tc.desc: test results of CubicBezier
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CubicBezier001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Drawing::Point p1 = { 0.0f, 0.0f };
    Drawing::Point p2 = { 10.0f, 10.0f };
    Drawing::Point p3 = { 20.0f, 20.0f };
    Drawing::Point p4 = { 30.0f, 30.0f };
    float t = 0.5f;
    Drawing::Point point = firstEffectFilter.CubicBezier(p1, p2, p3, p4, t);
    EXPECT_NE(point.GetX(), 0.0f);
}

/**
 * @tc.name: CalculateCubic001
 * @tc.desc: test results of CalculateCubic
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CalculateCubic001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    float p1 = 0.5f;
    float p2 = 0.8f;
    float t = 0.5f;
    EXPECT_NE(firstEffectFilter.CalculateCubic(p1, p2, t), 0.0f);
}

/**
 * @tc.name: BinarySearch001
 * @tc.desc: test results of BinarySearch
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, BinarySearch001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Drawing::Point p1 = { 0.2f, 0.0f };
    Drawing::Point p2 = { 0.2f, 1.0f };
    float targetX = 0.5f;
    EXPECT_NE(firstEffectFilter.BinarySearch(targetX, p1, p2), 0.0f);
}

/**
 * @tc.name: CalculateCubicsCtrlPointOffset001
 * @tc.desc: test results of CalculateCubicsCtrlPointOffset
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CalculateCubicsCtrlPointOffset001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    int pointNum = 4;
    std::vector<Drawing::Point> pathCtrlPointList(pointNum, Drawing::Point(0.0f, 0.0f));
    std::vector<Drawing::Point> pathList = firstEffectFilter.CalculateCubicsCtrlPointOffset(pathCtrlPointList);
    EXPECT_FALSE(pathList.empty());
}

/**
 * @tc.name: CreateIndexSequence001
 * @tc.desc: test results of CreateIndexSequence
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CreateIndexSequence001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    float direction = 1.0f;
    std::vector<int> pathList = firstEffectFilter.CreateIndexSequence(direction);
    EXPECT_TRUE(pathList[0] == 0);

    direction = -1.0f;
    pathList = firstEffectFilter.CreateIndexSequence(direction);
    EXPECT_TRUE(pathList[0] == 3);
}

/**
 * @tc.name: CreateIndexSequence002
 * @tc.desc: test results of CreateIndexSequence
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CreateIndexSequence002, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Vector2f destinationPoint(5.0f, 3.0f);
    float canvasWidth = 10.f;
    float canvasHeight = 10.f;
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
    float direction = 1.0f;
    std::vector<int> pathList = firstEffectFilter.CreateIndexSequence(direction);
    EXPECT_TRUE(pathList[0] == 9);

    direction = -1.0f;
    pathList = firstEffectFilter.CreateIndexSequence(direction);
    EXPECT_TRUE(pathList[0] == 0);
}

/**
 * @tc.name: CalculateVelocityCtrlPointUpper001
 * @tc.desc: test results of CalculateVelocityCtrlPointUpper
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CalculateVelocityCtrlPointUpper001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Vector2f destinationPoint(5.0f, 3.0f);
    float canvasWidth = 10.f;
    float canvasHeight = 10.f;
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
    std::vector<Drawing::Point> velocityCtrlPointUpper = firstEffectFilter.CalculateVelocityCtrlPointUpper();
    EXPECT_EQ(velocityCtrlPointUpper[0].GetX(), 0.5f);
    EXPECT_EQ(velocityCtrlPointUpper[1].GetX(), 0.2f);

    Vector2f destinationPointBelow(5.0f, 15.0f);
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPointBelow);
    std::vector<Drawing::Point> velocityCtrlPointUpperBelow = firstEffectFilter.CalculateVelocityCtrlPointUpper();
    EXPECT_EQ(velocityCtrlPointUpperBelow[0].GetX(), 0.5f);
    EXPECT_EQ(velocityCtrlPointUpperBelow[1].GetX(), 0.0f);
}

/**
 * @tc.name: CalculateVelocityCtrlPointLower001
 * @tc.desc: test results of CalculateVelocityCtrlPointLower
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CalculateVelocityCtrlPointLower001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Vector2f destinationPoint(5.0f, 3.0f);
    float canvasWidth = 10.f;
    float canvasHeight = 10.f;
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
    std::vector<Drawing::Point> velocityCtrlPointUpper = firstEffectFilter.CalculateVelocityCtrlPointLower();
    EXPECT_EQ(velocityCtrlPointUpper[0].GetX(), 0.5f);
    EXPECT_EQ(velocityCtrlPointUpper[1].GetX(), 0.2f);

    Vector2f destinationPointBelow(5.0f, 15.0f);
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPointBelow);
    std::vector<Drawing::Point> velocityCtrlPointUpperBelow = firstEffectFilter.CalculateVelocityCtrlPointLower();
    EXPECT_EQ(velocityCtrlPointUpperBelow[0].GetX(), 0.5f);
    EXPECT_EQ(velocityCtrlPointUpperBelow[1].GetX(), 0.0f);
}

/**
 * @tc.name: CalculateUpperCtrlPointOfVertex001
 * @tc.desc: test results of CalculateUpperCtrlPointOfVertex
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CalculateUpperCtrlPointOfVertex001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Vector2f destinationPoint(5.0f, 15.0f);
    float canvasWidth = 10.f;
    float canvasHeight = 10.f;
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
    float deltaX = 1.f;
    float deltaY = 1.f;
    float width = 10.0f;
    float height = 10.0f;
    int direction = 1.0;
    std::vector<Drawing::Point> upperControlPointOfVertex1 =
        firstEffectFilter.CalculateUpperCtrlPointOfVertex(deltaX, deltaY, width, height, direction);

    Vector2f destinationPointBelow(-5.0f, 3.0f);
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPointBelow);
    std::vector<Drawing::Point> upperControlPointOfVertex2 =
        firstEffectFilter.CalculateUpperCtrlPointOfVertex(deltaX, deltaY, width, height, direction);
    
    EXPECT_NE(upperControlPointOfVertex1[0].GetX(), upperControlPointOfVertex2[0].GetX());
    EXPECT_EQ(upperControlPointOfVertex1[1].GetX(), upperControlPointOfVertex2[1].GetX());
    EXPECT_NE(upperControlPointOfVertex1[2].GetX(), upperControlPointOfVertex2[2].GetX());
    EXPECT_EQ(upperControlPointOfVertex1[3].GetX(), upperControlPointOfVertex2[3].GetX());
}

/**
 * @tc.name: CalculateLowerCtrlPointOfVertex001
 * @tc.desc: test results of CalculateLowerCtrlPointOfVertex
 * @tc.type: FUNC
 * @tc.require: issueI9UX8W
 */
HWTEST_F(RSAttractionEffectFilterTest, CalculateLowerCtrlPointOfVertex001, TestSize.Level1)
{
    RSAttractionEffectFilter firstEffectFilter(0.5f);
    Vector2f destinationPoint(-5.0f, 15.0f);
    float canvasWidth = 10.f;
    float canvasHeight = 10.f;
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPoint);
    float deltaX = 1.f;
    float deltaY = 1.f;
    float width = 10.0f;
    float height = 10.0f;
    int direction = 1.0;
    std::vector<Drawing::Point> upperControlPointOfVertex1 =
        firstEffectFilter.CalculateLowerCtrlPointOfVertex(deltaX, deltaY, width, height, direction);

    Vector2f destinationPointBelow(5.0f, 3.0f);
    firstEffectFilter.CalculateWindowStatus(canvasWidth, canvasHeight, destinationPointBelow);
    std::vector<Drawing::Point> upperControlPointOfVertex2 =
        firstEffectFilter.CalculateLowerCtrlPointOfVertex(deltaX, deltaY, width, height, direction);
    
    EXPECT_NE(upperControlPointOfVertex1[0].GetX(), upperControlPointOfVertex2[0].GetX());
    EXPECT_NE(upperControlPointOfVertex1[1].GetX(), upperControlPointOfVertex2[1].GetX());
    EXPECT_NE(upperControlPointOfVertex1[2].GetX(), upperControlPointOfVertex2[2].GetX());
    EXPECT_NE(upperControlPointOfVertex1[3].GetX(), upperControlPointOfVertex2[3].GetX());
}

/**
 * @tc.name: GetDescriptionTest
 * @tc.desc: test results of GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterTest, GetDescriptionTest, TestSize.Level1)
{
    float attractionFraction = 0.5f;
    RSAttractionEffectFilter effectFilter(attractionFraction);
    std::string expectRes = "RSAttractionEffectFilter " + std::to_string(attractionFraction);
    EXPECT_EQ(effectFilter.GetDescription(), expectRes);
}

/**
 * @tc.name: GetBrush001
 * @tc.desc: test GetBrush image is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSAttractionEffectFilterTest, GetBrush001, TestSize.Level1)
{
    RSAttractionEffectFilter effectFilter(0.5f);
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto brush = effectFilter.GetBrush(image);
    EXPECT_EQ(brush.GetShaderEffect(), nullptr);

    image = std::make_shared<Drawing::Image>();
    EXPECT_TRUE(image);

    brush = effectFilter.GetBrush(image);
    EXPECT_TRUE(brush.GetShaderEffect());
}

/**
 * @tc.name: GetBrush002
 * @tc.desc: test GetBrush image GetBlendMode
 * @tc.type: FUNC
 */
HWTEST_F(RSAttractionEffectFilterTest, GetBrush002, TestSize.Level1)
{
    RSAttractionEffectFilter effectFilter(0.5f);
    effectFilter.isBelowTarget_ = true;
    effectFilter.location_ = 1;
    effectFilter.attractionFraction_ = 0.5f;

    auto image = CreateDrawingImage(2, 1);
    EXPECT_TRUE(image);
    Drawing::Brush brush = effectFilter.GetBrush(image);

    EXPECT_NE(brush.GetShaderEffect(), nullptr);
    EXPECT_EQ(brush.GetBlendMode(), Drawing::BlendMode::SRC_OVER);
}

/**
 * @tc.name: GetBrush003
 * @tc.desc: test GetBrush with different isBelowTarget_ and location_ combinations
 * @tc.type: FUNC
 */
HWTEST_F(RSAttractionEffectFilterTest, GetBrush003, TestSize.Level1)
{
    RSAttractionEffectFilter effectFilter(0.5f);
    auto image = CreateDrawingImage(10, 10);

    // Test isBelowTarget_ = false, location_ = 1
    effectFilter.isBelowTarget_ = false;
    effectFilter.location_ = 1;
    auto brush1 = effectFilter.GetBrush(image);
    EXPECT_NE(brush1.GetShaderEffect(), nullptr);

    // Test isBelowTarget_ = false, location_ = -1
    effectFilter.isBelowTarget_ = false;
    effectFilter.location_ = -1;
    auto brush2 = effectFilter.GetBrush(image);
    EXPECT_NE(brush2.GetShaderEffect(), nullptr);

    // Test isBelowTarget_ = true, location_ = 1
    effectFilter.isBelowTarget_ = true;
    effectFilter.location_ = 1;
    auto brush3 = effectFilter.GetBrush(image);
    EXPECT_NE(brush3.GetShaderEffect(), nullptr);

    // Test isBelowTarget_ = true, location_ = -1
    effectFilter.isBelowTarget_ = true;
    effectFilter.location_ = -1;
    auto brush4 = effectFilter.GetBrush(image);
    EXPECT_NE(brush4.GetShaderEffect(), nullptr);
}

/**
 * @tc.name: GetBrush004
 * @tc.desc: test GetBrush with different attractionFraction_ values
 * @tc.type: FUNC
 */
HWTEST_F(RSAttractionEffectFilterTest, GetBrush004, TestSize.Level1)
{
    auto image = CreateDrawingImage(5, 5);

    // Test attractionFraction_ = 0.0f
    RSAttractionEffectFilter effectFilter1(0.0f);
    effectFilter1.isBelowTarget_ = false;
    effectFilter1.location_ = 1;
    auto brush1 = effectFilter1.GetBrush(image);
    EXPECT_NE(brush1.GetShaderEffect(), nullptr);

    // Test attractionFraction_ = 0.5f
    RSAttractionEffectFilter effectFilter2(0.5f);
    effectFilter2.isBelowTarget_ = false;
    effectFilter2.location_ = 1;
    auto brush2 = effectFilter2.GetBrush(image);
    EXPECT_NE(brush2.GetShaderEffect(), nullptr);

    // Test attractionFraction_ = 1.0f
    RSAttractionEffectFilter effectFilter3(1.0f);
    effectFilter3.isBelowTarget_ = false;
    effectFilter3.location_ = 1;
    auto brush3 = effectFilter3.GetBrush(image);
    EXPECT_NE(brush3.GetShaderEffect(), nullptr);

     // Test attractionFraction_ > 0.5f
    RSAttractionEffectFilter effectFilter4(0.8f);
    effectFilter4.isBelowTarget_ = false;
    effectFilter4.location_ = 1;
    auto brush4 = effectFilter4.GetBrush(image);
    EXPECT_NE(brush4.GetShaderEffect(), nullptr);
}

} // namespace Rosen
} // namespace OHOS
