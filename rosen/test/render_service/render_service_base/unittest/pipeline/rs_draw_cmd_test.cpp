/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "pipeline/rs_draw_cmd.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDrawCmdTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline std::shared_ptr<OHOS::Media::PixelMap> pixelMap;
    static inline Drawing::AdaptiveImageInfo imageInfo;
    static inline NodeId id = 1;
};

void RSDrawCmdTest::SetUpTestCase() {}
void RSDrawCmdTest::TearDownTestCase() {}
void RSDrawCmdTest::SetUp() {}
void RSDrawCmdTest::TearDown() {}

/**
 * @tc.name: SetNodeId001
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId001, TestSize.Level1)
{
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    NodeId id = 1;
    extendImageObject.SetNodeId(id);

    RSExtendImageObject extendImageObjectTwo;
    extendImageObjectTwo.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetDrawingImageFromSurfaceBuffer
 * @tc.desc: test results of GetDrawingImageFromSurfaceBuffer
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, GetDrawingImageFromSurfaceBuffer, TestSize.Level1)
{
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    Drawing::Canvas canvas(1, 1);
    OHOS::SurfaceBuffer* surfaceBuffer = SurfaceBuffer::Create().GetRefPtr();
    ASSERT_FALSE(extendImageObject.GetDrawingImageFromSurfaceBuffer(canvas, surfaceBuffer));
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling001, TestSize.Level1)
{
    RSExtendImageObject extendImageObject(pixelMap, imageInfo);
    Parcel parcel;
    ASSERT_EQ(extendImageObject.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: Playback007
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback007, TestSize.Level1)
{
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::SamplingOptions sampling;
    bool isBackground = true;
    std::shared_ptr<Drawing::Image> image = std::make_shared<Drawing::Image>();
    std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
    Drawing::AdaptiveImageInfo imageInfo;
    RSExtendImageObject extendImageObject(image, data, imageInfo);
    extendImageObject.Playback(canvas, rect, sampling, isBackground);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback001
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback001, TestSize.Level1)
{
    Drawing::Rect src;
    Drawing::Rect dst;
    RSExtendImageBaseObj extendImageBaseObj(pixelMap, src, dst);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    Drawing::SamplingOptions sampling;
    extendImageBaseObj.Playback(canvas, rect, sampling);

    RSExtendImageBaseObj obj;
    obj.Playback(canvas, rect, sampling);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId002
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId002, TestSize.Level1)
{
    Drawing::Rect src;
    Drawing::Rect dst;
    RSExtendImageBaseObj extendImageBaseObj(pixelMap, src, dst);
    extendImageBaseObj.SetNodeId(id);

    RSExtendImageBaseObj obj;
    obj.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: test results of Unmarshalling
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Unmarshalling002, TestSize.Level1)
{
    Drawing::Rect src;
    Drawing::Rect dst;
    RSExtendImageBaseObj extendImageBaseObj(pixelMap, src, dst);
    Parcel parcel;
    ASSERT_EQ(extendImageBaseObj.Unmarshalling(parcel), nullptr);
}

/**
 * @tc.name: Playback002
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback002, TestSize.Level1)
{
    Drawing::RecordingCanvas::DrawFunc drawFunc;
    RSExtendDrawFuncObj extendDrawFuncObj(std::move(drawFunc));
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    extendDrawFuncObj.Playback(&canvas, &rect);

    drawFunc = [](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    RSExtendDrawFuncObj obj(std::move(drawFunc));
    obj.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback003
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback003, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawImageWithParmOpItem drawImageWithParmOpItem(image, data, imageInfo, sampling, paint);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawImageWithParmOpItem.Playback(&canvas, &rect);
    drawImageWithParmOpItem.objectHandle_ = nullptr;
    drawImageWithParmOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId003
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId003, TestSize.Level1)
{
    std::shared_ptr<Drawing::Image> image;
    std::shared_ptr<Drawing::Data> data;
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawImageWithParmOpItem drawImageWithParmOpItem(image, data, imageInfo, sampling, paint);
    drawImageWithParmOpItem.SetNodeId(id);

    drawImageWithParmOpItem.objectHandle_ = nullptr;
    drawImageWithParmOpItem.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback004
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback004, TestSize.Level1)
{
    Drawing::DrawCmdList cmdList;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawPixelMapWithParmOpItem::ConstructorHandle handle(objectHandle, sampling, paintHandle);
    Drawing::DrawPixelMapWithParmOpItem drawPixelMapWithParmOpItem(cmdList, &handle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawPixelMapWithParmOpItem.Playback(&canvas, &rect);
    drawPixelMapWithParmOpItem.objectHandle_ = nullptr;
    drawPixelMapWithParmOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId004
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId004, TestSize.Level1)
{
    Drawing::SamplingOptions sampling;
    Drawing::Paint paint;
    Drawing::DrawPixelMapWithParmOpItem drawPixelMapWithParmOpItem(pixelMap, imageInfo, sampling, paint);
    drawPixelMapWithParmOpItem.SetNodeId(id);

    drawPixelMapWithParmOpItem.objectHandle_ = nullptr;
    drawPixelMapWithParmOpItem.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback005
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback005, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawPixelMapRectOpItem::ConstructorHandle constructorHandle(objectHandle, sampling, paintHandle);
    Drawing::DrawPixelMapRectOpItem drawPixelMapRectOpItem(list, &constructorHandle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawPixelMapRectOpItem.Playback(&canvas, &rect);

    drawPixelMapRectOpItem.objectHandle_ = nullptr;
    drawPixelMapRectOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: SetNodeId005
 * @tc.desc: test results of SetNodeId
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, SetNodeId005, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    Drawing::OpDataHandle objectHandle;
    Drawing::SamplingOptions sampling;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawPixelMapRectOpItem::ConstructorHandle constructorHandle(objectHandle, sampling, paintHandle);
    Drawing::DrawPixelMapRectOpItem drawPixelMapRectOpItem(list, &constructorHandle);
    drawPixelMapRectOpItem.SetNodeId(id);

    drawPixelMapRectOpItem.objectHandle_ = nullptr;
    drawPixelMapRectOpItem.SetNodeId(id);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback006
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback006, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    uint32_t funcObjectId = 1;
    Drawing::DrawFuncOpItem::ConstructorHandle constructorHandle(funcObjectId);
    Drawing::RecordingCanvas::DrawFunc drawFunc = [](Drawing::Canvas* canvas, const Drawing::Rect* rect) {};
    Drawing::DrawFuncOpItem drawFuncOpItem(std::move(drawFunc));
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawFuncOpItem.Playback(&canvas, &rect);
    ASSERT_NE(drawFuncOpItem.objectHandle_, nullptr);

    drawFuncOpItem.objectHandle_ = nullptr;
    drawFuncOpItem.Playback(&canvas, &rect);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Playback008
 * @tc.desc: test results of Playback
 * @tc.type:FUNC
 * @tc.require: issueI9H4AD
 */
HWTEST_F(RSDrawCmdTest, Playback008, TestSize.Level1)
{
    Drawing::DrawCmdList list;
    uint32_t surfaceBufferId = 1;
    int offSetX = 1;
    int offSetY = 1;
    int width = 1;
    int height = 1;
    Drawing::PaintHandle paintHandle;
    Drawing::DrawSurfaceBufferOpItem::ConstructorHandle constructorHandle(
        surfaceBufferId, offSetX, offSetY, width, height, paintHandle);
    Drawing::DrawSurfaceBufferOpItem drawSurfaceBufferOpItem(list, &constructorHandle);
    Drawing::Canvas canvas;
    Drawing::Rect rect;
    drawSurfaceBufferOpItem.Playback(&canvas, &rect);
    ASSERT_EQ(drawSurfaceBufferOpItem.nativeWindowBuffer_, nullptr);
}
} // namespace OHOS::Rosen