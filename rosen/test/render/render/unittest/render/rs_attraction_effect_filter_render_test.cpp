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

class RSAttractionEffectFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAttractionEffectFilterRenderTest::SetUpTestCase() {}
void RSAttractionEffectFilterRenderTest::TearDownTestCase() {}
void RSAttractionEffectFilterRenderTest::SetUp() {}
void RSAttractionEffectFilterRenderTest::TearDown() {}

/**
 * @tc.name: DrawImageRect001
 * @tc.desc: test results of DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, DrawImageRect001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, IsValid001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, GetAttractionFraction001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, GetAttractionDirtyRegion001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, IsWithinThreshold001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, LerpPoint001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CubicBezier001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CalculateCubic001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, BinarySearch001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CalculateCubicsCtrlPointOffset001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CreateIndexSequence001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CreateIndexSequence002, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CalculateVelocityCtrlPointUpper001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CalculateVelocityCtrlPointLower001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CalculateUpperCtrlPointOfVertex001, TestSize.Level1)
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
 * @tc.require:
 */
HWTEST_F(RSAttractionEffectFilterRenderTest, CalculateLowerCtrlPointOfVertex001, TestSize.Level1)
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
HWTEST_F(RSAttractionEffectFilterRenderTest, GetDescriptionTest, TestSize.Level1)
{
    float attractionFraction = 0.5f;
    RSAttractionEffectFilter effectFilter(attractionFraction);
    std::string expectRes = "RSAttractionEffectFilter " + std::to_string(attractionFraction);
    EXPECT_EQ(effectFilter.GetDescription(), expectRes);
}

} // namespace Rosen
} // namespace OHOS
