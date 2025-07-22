/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "image/image.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "utils/vertices.h"
#include "pixel_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
#define TEST_MEM_SIZE 10
#define TEST_INVALIED_ID 9999
class DrawCmdListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DrawCmdListTest::SetUpTestCase() {}
void DrawCmdListTest::TearDownTestCase() {}
void DrawCmdListTest::SetUp() {}
void DrawCmdListTest::TearDown() {}

/**
 * @tc.name: GetOpsWithDesc001
 * @tc.desc: Test the GetOpsWithDesc function.
 * @tc.type: FUNC
 * @tc.require: IBJ4VQ
 */
HWTEST_F(DrawCmdListTest, GetOpsWithDesc001, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->drawOpItems_.emplace_back(nullptr);
    Brush brush;
    drawCmdList->drawOpItems_.emplace_back(std::make_shared<DrawBackgroundOpItem>(brush));
    std::string s = "";
    s = drawCmdList->GetOpsWithDesc();
    EXPECT_TRUE(drawCmdList->drawOpItems_.size() == 2); // 2 for test
    delete drawCmdList;
}

/**
 * @tc.name: Dump002
 * @tc.desc: Test the Dump function.
 * @tc.type: FUNC
 * @tc.require: IBJ4VQ
 */
HWTEST_F(DrawCmdListTest, Dump002, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->drawOpItems_.emplace_back(nullptr);
    Brush brush;
    drawCmdList->drawOpItems_.emplace_back(std::make_shared<DrawBackgroundOpItem>(brush));
    std::string s = "";
    drawCmdList->Dump(s);
    EXPECT_TRUE(drawCmdList->drawOpItems_.size() == 2); // 2 for test
    delete drawCmdList;
}

/**
 * @tc.name: MarshallingDrawOps003
 * @tc.desc: Test the MarshallingDrawOps function.
 * @tc.type: FUNC
 * @tc.require: IBJ4VQ
 */
HWTEST_F(DrawCmdListTest, MarshallingDrawOps003, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->performanceCaculateOpType_ = 1; // 1 for test
    drawCmdList->MarshallingDrawOps();
    drawCmdList->UnmarshallingDrawOps();
    EXPECT_TRUE(drawCmdList->performanceCaculateOpType_ == 1);
    delete drawCmdList;
}

/**
 * @tc.name: Playback004
 * @tc.desc: Test the Playback function.
 * @tc.type: FUNC
 * @tc.require: IBJ4VQ
 */
HWTEST_F(DrawCmdListTest, Playback004, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->SetWidth(0);
    Canvas canvas;
    Rect rect;
    drawCmdList->performanceCaculateOpType_ = 1; // 1 for test
    drawCmdList->Playback(canvas, &rect);
    EXPECT_TRUE(drawCmdList->performanceCaculateOpType_ == 1);
    delete drawCmdList;
}

/**
 * @tc.name: GenerateCacheByVector005
 * @tc.desc: Test the GenerateCacheByVector function.
 * @tc.type: FUNC
 * @tc.require: IBJ4VQ
 */
HWTEST_F(DrawCmdListTest, GenerateCacheByVector005, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    Brush brush;
    drawCmdList->drawOpItems_.emplace_back(std::make_shared<DrawBackgroundOpItem>(brush));
    Font font;
    auto textBlob = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    Paint paint;
    auto opItem = std::make_shared<DrawTextBlobOpItem>(textBlob.get(), 0, 0, paint);
    drawCmdList->drawOpItems_.emplace_back(opItem);
    Canvas canvas;
    Rect rect;
    drawCmdList->GenerateCacheByVector(&canvas, &rect);
    EXPECT_TRUE(drawCmdList->drawOpItems_.size() == 2); // 2 for test
    delete drawCmdList;
}

/**
 * @tc.name: GenerateCacheByBuffer006
 * @tc.desc: Test the GenerateCacheByBuffer function.
 * @tc.type: FUNC
 * @tc.require: IBJ4VQ
 */
HWTEST_F(DrawCmdListTest, GenerateCacheByBuffer006, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() <= drawCmdList->offset_);
    Canvas canvas;
    Rect rect;
    drawCmdList->GenerateCacheByBuffer(&canvas, &rect);
    EXPECT_FALSE(drawCmdList->isCached_);
    delete drawCmdList;
}

/**
 * @tc.name: GetBounds001
 * @tc.desc: Test the GetBounds function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, GetBounds001, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() <= drawCmdList->offset_);
    auto ret = drawCmdList->UnmarshallingDrawOpsSimple(drawCmdList->drawOpItems_, drawCmdList->lastOpGenSize_);
    EXPECT_EQ(ret, false);
    Rect rect;
    drawCmdList->GetBounds(rect);
    EXPECT_EQ(rect.IsValid(), false);
    delete drawCmdList;
}

/**
 * @tc.name: GetBounds002
 * @tc.desc: Test the GetBounds function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, GetBounds002, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->opAllocator_.size_ = drawCmdList->offset_ + 1;
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() > drawCmdList->offset_);
    auto ret = drawCmdList->UnmarshallingDrawOpsSimple(drawCmdList->drawOpItems_, drawCmdList->lastOpGenSize_);
    EXPECT_EQ(ret, true);
    Rect rect;
    drawCmdList->GetBounds(rect);
    delete drawCmdList;
}

/**
 * @tc.name: GetBounds003
 * @tc.desc: Test the GetBounds function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, GetBounds003, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->opAllocator_.size_ = drawCmdList->offset_ + 1;
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() > drawCmdList->offset_);
    drawCmdList->lastOpGenSize_ = drawCmdList->opAllocator_.GetSize();
    Brush brush;
    drawCmdList->drawOpItems_.emplace_back(std::make_shared<DrawBackgroundOpItem>(brush));
    Rect rect = Rect(0.0f, 0.0f, 100.0f, 100.0f);
    auto opItem = std::make_shared<HybridRenderPixelMapSizeOpItem>(rect.GetWidth(), rect.GetHeight());
    drawCmdList->drawOpItems_.emplace_back(opItem);
    auto ret = drawCmdList->UnmarshallingDrawOpsSimple(drawCmdList->drawOpItems_, drawCmdList->lastOpGenSize_);
    EXPECT_EQ(ret, true);
    Rect rectGet;
    drawCmdList->GetBounds(rectGet);
    EXPECT_EQ(rect, rectGet);
    delete drawCmdList;
}

/**
 * @tc.name: GetBounds004
 * @tc.desc: Test the GetBounds function.
 * @tc.type: FUNC
 * @tc.require: IC8J12
 */
HWTEST_F(DrawCmdListTest, GetBounds004, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->drawOpItems_.emplace_back(nullptr);
    Rect rect;
    drawCmdList->GetBounds(rect);
    EXPECT_EQ(rect.IsValid(), false);
    delete drawCmdList;
}

/**
 * @tc.name: IsHybridRenderEnabled001
 * @tc.desc: Test the IsHybridRenderEnabled function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, IsHybridRenderEnabled001, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() <= drawCmdList->offset_);
    drawCmdList->hybridRenderType_ = DrawCmdList::HybridRenderType::CANVAS;
    auto ret = drawCmdList->IsHybridRenderEnabled(0, 0);
    EXPECT_EQ(ret, true);
    delete drawCmdList;
}

/**
 * @tc.name: IsHybridRenderEnabled002
 * @tc.desc: Test the IsHybridRenderEnabled function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, IsHybridRenderEnabled002, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->opAllocator_.size_ = drawCmdList->offset_ + 1;
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() > drawCmdList->offset_);
    auto ret = drawCmdList->IsHybridRenderEnabled(INT_MAX, INT_MAX);
    EXPECT_EQ(ret, true);
    delete drawCmdList;
}

/**
 * @tc.name: IsHybridRenderEnabled003
 * @tc.desc: Test the IsHybridRenderEnabled function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, IsHybridRenderEnabled003, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->opAllocator_.size_ = drawCmdList->offset_ + 1;
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() > drawCmdList->offset_);
    drawCmdList->lastOpGenSize_ = drawCmdList->opAllocator_.GetSize();
    Brush brush;
    drawCmdList->drawOpItems_.emplace_back(std::make_shared<DrawBackgroundOpItem>(brush));
    Rect rect = Rect(0.0f, 0.0f, 100.0f, 100.0f);
    auto opItem = std::make_shared<HybridRenderPixelMapSizeOpItem>(rect.GetWidth(), rect.GetHeight());
    drawCmdList->drawOpItems_.emplace_back(opItem);
    auto ret = drawCmdList->IsHybridRenderEnabled(INT_MAX, INT_MAX);
    EXPECT_EQ(ret, false);
    delete drawCmdList;
}

/**
 * @tc.name: IsHybridRenderEnabled004
 * @tc.desc: Test the IsHybridRenderEnabled function.
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, IsHybridRenderEnabled004, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->opAllocator_.size_ = drawCmdList->offset_ + 1;
    EXPECT_TRUE(drawCmdList->opAllocator_.GetSize() > drawCmdList->offset_);
    drawCmdList->width_ = 5001;
    drawCmdList->height_ = 1000;
    auto ret = drawCmdList->IsHybridRenderEnabled(5000, 5000);
    EXPECT_EQ(ret, false);
    drawCmdList->width_ = 5001;
    drawCmdList->height_ = 5000;
    ret = drawCmdList->IsHybridRenderEnabled(5000, 5000);
    EXPECT_EQ(ret, false);
    drawCmdList->width_ = 1000;
    drawCmdList->height_ = 5001;
    ret = drawCmdList->IsHybridRenderEnabled(5000, 5000);
    EXPECT_EQ(ret, false);
    drawCmdList->width_ = -1;
    drawCmdList->height_ = 5001;
    ret = drawCmdList->IsHybridRenderEnabled(5000, 5000);
    EXPECT_EQ(ret, false);
    drawCmdList->width_ = 5001;
    drawCmdList->height_ = -1;
    ret = drawCmdList->IsHybridRenderEnabled(5000, 5000);
    EXPECT_EQ(ret, false);
    drawCmdList->width_ = -1;
    drawCmdList->height_ = -1;
    ret = drawCmdList->IsHybridRenderEnabled(5000, 5000);
    EXPECT_EQ(ret, true);
    delete drawCmdList;
}

/**
 * @tc.name: ProfilerMarshallingDrawOps
 * @tc.desc: Test ProfilerMarshallingDrawOps
 * @tc.type: FUNC
 * @tc.require: IC2UAC
 */
HWTEST_F(DrawCmdListTest, ProfilerMarshallingDrawOps, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::DEFERRED);
    auto secondDrawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::DEFERRED);
    Brush brush;
    drawCmdList->drawOpItems_.emplace_back(std::make_shared<DrawBackgroundOpItem>(brush));
    drawCmdList->ProfilerMarshallingDrawOps(secondDrawCmdList);
    EXPECT_EQ(secondDrawCmdList->drawOpItems_.size(), 1);
    delete drawCmdList;
}

/**
 * @tc.name: SetIsReplayMode
 * @tc.desc: Test SetIsReplayMode
 * @tc.require:
 */
HWTEST_F(DrawCmdListTest, SetIsReplayMode, Level1)
{
    auto drawCmdList = new DrawCmdList();
    drawCmdList->SetIsReplayMode(true);
    EXPECT_TRUE(drawCmdList->isReplayMode);
    drawCmdList->SetIsReplayMode(false);
    EXPECT_FALSE(drawCmdList->isReplayMode);
}

/**
 * @tc.name: Dump003
 * @tc.desc: Test the Dump function.
 * @tc.type: UnmarshallingDrawOpsSimple
 */
HWTEST_F(DrawCmdListTest, Dump003, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList(DrawCmdList::UnmarshalMode::IMMEDIATE);
    drawCmdList->opAllocator_.size_ = drawCmdList->offset_ + 1;
    EXPECT_FALSE(drawCmdList->IsEmpty());
    EXPECT_TRUE(drawCmdList->drawOpItems_.empty());
    
    std::string out;
    drawCmdList->Dump(out);
    EXPECT_TRUE(out.empty());
    delete drawCmdList;
}

/**
 * @tc.name: GetCmdlistDrawRegion001
 * @tc.desc: Test the GetCmdlistDrawRegion function.
 * @tc.type: FUNC
 * @tc.require: ICI6YB
 */
HWTEST_F(DrawCmdListTest, GetCmdlistDrawRegion001, TestSize.Level1)
{
    auto drawCmdList = new DrawCmdList();
    EXPECT_TRUE(drawCmdList->GetCmdlistDrawRegion().IsEmpty());

    drawCmdList->drawOpItems_.emplace_back(nullptr);
    EXPECT_TRUE(drawCmdList->GetCmdlistDrawRegion().IsEmpty());
    delete drawCmdList;
}

/**
 * @tc.name: GetCmdlistDrawRegion002
 * @tc.desc: Test the GetCmdlistDrawRegion function.
 * @tc.type: FUNC
 * @tc.require: ICI6YB
 */
HWTEST_F(DrawCmdListTest, GetCmdlistDrawRegion002, TestSize.Level1)
 {
    Path path;
    path.Offset(1.0f, 1.0f);
    Paint paint;
    DrawPathOpItem pathOpItem{path, paint};
    pathOpItem.path_->MoveTo(1.0f, 2.0f);
    pathOpItem.path_->LineTo(3.0f, 4.0f);
    ASSERT_TRUE(!(pathOpItem.GetOpItemCmdlistDrawRegion().IsEmpty()));

    auto pathOpItemPtr = std::make_shared<DrawPathOpItem>(pathOpItem);
    auto saveOpItemPtr = std::make_shared<SaveOpItem>();
    auto drawCmdList = new DrawCmdList();
    drawCmdList->drawOpItems_.emplace_back(pathOpItemPtr);
    drawCmdList->drawOpItems_.emplace_back(saveOpItemPtr);
    EXPECT_FALSE(drawCmdList->GetCmdlistDrawRegion().IsEmpty());

    auto fulshOpItemPtr = std::make_shared<FlushOpItem>();
    drawCmdList->drawOpItems_.emplace_back(fulshOpItemPtr);
    EXPECT_TRUE(drawCmdList->GetCmdlistDrawRegion().IsEmpty());
    delete drawCmdList;
 }
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS