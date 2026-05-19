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
 
#include <atomic>
#include "gtest/gtest.h"
 
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_simple_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "recording/draw_cmd.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace Rosen {
class RSSimpleDrawCmdListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RSSimpleDrawCmdListTest::SetUpTestCase() {}
void RSSimpleDrawCmdListTest::TearDownTestCase() {}
void RSSimpleDrawCmdListTest::SetUp() {}
void RSSimpleDrawCmdListTest::TearDown() {}
 
/**
 * @tc.name: CreateFromDrawCmdList_NullInput
 * @tc.desc: Test CreateFromDrawCmdList with null DrawCmdList input
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, CreateFromDrawCmdList_NullInput, TestSize.Level1)
{
    Drawing::DrawCmdListPtr nullDrawCmdList = nullptr;
    auto result = RSSimpleDrawCmdList::CreateFromDrawCmdList(nullDrawCmdList);
    EXPECT_EQ(result, nullptr);
}
 
/**
 * @tc.name: CreateFromDrawCmdList_ValidInput
 * @tc.desc: Test CreateFromDrawCmdList with valid DrawCmdList input
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, CreateFromDrawCmdList_ValidInput, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(100, 200, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto result = RSSimpleDrawCmdList::CreateFromDrawCmdList(drawCmdList);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetWidth(), 100);
    EXPECT_EQ(result->GetHeight(), 200);
    EXPECT_EQ(result->GetOpItemSize(), 0);
}
 
/**
 * @tc.name: CreateFromDrawCmdList_WithOpItems
 * @tc.desc: Test CreateFromDrawCmdList with DrawCmdList containing OpItems
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, CreateFromDrawCmdList_WithOpItems, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(50, 75, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmdList->AddDrawOp(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto result = RSSimpleDrawCmdList::CreateFromDrawCmdList(drawCmdList);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->GetWidth(), 50);
    EXPECT_EQ(result->GetHeight(), 75);
    EXPECT_EQ(result->GetOpItemSize(), 1);
}
 
/**
 * @tc.name: DefaultConstructor
 * @tc.desc: Test RSSimpleDrawCmdList default constructor
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, DefaultConstructor, TestSize.Level1)
{
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>();
    ASSERT_NE(simpleDrawCmdList, nullptr);
    EXPECT_EQ(simpleDrawCmdList->GetWidth(), 0);
    EXPECT_EQ(simpleDrawCmdList->GetHeight(), 0);
    EXPECT_TRUE(simpleDrawCmdList->IsEmpty());
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 0);
}
 
/**
 * @tc.name: ConstructorWithParams
 * @tc.desc: Test RSSimpleDrawCmdList constructor with width/height/opItems
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, ConstructorWithParams, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(150, 250, opItems);
    ASSERT_NE(simpleDrawCmdList, nullptr);
    EXPECT_EQ(simpleDrawCmdList->GetWidth(), 150);
    EXPECT_EQ(simpleDrawCmdList->GetHeight(), 250);
    EXPECT_FALSE(simpleDrawCmdList->IsEmpty());
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 1);
}
 
/**
 * @tc.name: GetType
 * @tc.desc: Test GetType returns DRAW_CMD_LIST
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, GetType, TestSize.Level1)
{
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>();
    EXPECT_EQ(simpleDrawCmdList->GetType(), Drawing::CmdList::Type::DRAW_CMD_LIST);
}
 
/**
 * @tc.name: GetDrawOpItems
 * @tc.desc: Test GetDrawOpItems returns thread-safe copy
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, GetDrawOpItems, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    Drawing::Path path;
    path.Offset(1.0f, 1.0f);
    opItems.push_back(std::make_shared<Drawing::DrawPathOpItem>(path, Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
    auto retrievedItems = simpleDrawCmdList->GetDrawOpItems();
    EXPECT_EQ(retrievedItems.size(), 2);
}
 
/**
 * @tc.name: IsEmpty
 * @tc.desc: Test IsEmpty with empty and non-empty lists
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, IsEmpty, TestSize.Level1)
{
    auto emptyList = std::make_shared<RSSimpleDrawCmdList>();
    EXPECT_TRUE(emptyList->IsEmpty());
    
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 1, 1), Drawing::Paint()));
    auto nonEmptyList = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    EXPECT_FALSE(nonEmptyList->IsEmpty());
}
 
/**
 * @tc.name: Dump
 * @tc.desc: Test Dump with various scenarios
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Dump, TestSize.Level1)
{
    std::string out;
    auto emptyList = std::make_shared<RSSimpleDrawCmdList>();
    emptyList->Dump(out);
    EXPECT_TRUE(out.empty());
    
    out.clear();
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint()));
    auto listWithItem = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    listWithItem->Dump(out);
    EXPECT_FALSE(out.empty());
}
 
/**
 * @tc.name: GetSize
 * @tc.desc: Test GetSize returns correct total memory size
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, GetSize, TestSize.Level1)
{
    auto emptyList = std::make_shared<RSSimpleDrawCmdList>();
    size_t emptySize = emptyList->GetSize();
    EXPECT_GT(emptySize, 0);
    
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint()));
    auto listWithItem = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    size_t itemSize = listWithItem->GetSize();
    EXPECT_GT(itemSize, emptySize);
}
 
/**
 * @tc.name: GetOpItemSize
 * @tc.desc: Test GetOpItemSize returns correct count
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, GetOpItemSize, TestSize.Level1)
{
    auto emptyList = std::make_shared<RSSimpleDrawCmdList>();
    EXPECT_EQ(emptyList->GetOpItemSize(), 0);
    
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    constexpr int itemCount = 5;
    for (int i = 0; i < itemCount; i++) {
        opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 1, 1), Drawing::Paint()));
    }
    auto listWithItems = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    EXPECT_EQ(listWithItems->GetOpItemSize(), itemCount);
}
 
/**
 * @tc.name: ClearOp
 * @tc.desc: Test ClearOp clears all op items
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, ClearOp, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    EXPECT_FALSE(simpleDrawCmdList->IsEmpty());
    
    simpleDrawCmdList->ClearOp();
    EXPECT_TRUE(simpleDrawCmdList->IsEmpty());
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 0);
}
 
/**
 * @tc.name: AddDrawOp
 * @tc.desc: Test AddDrawOp adds op items
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, AddDrawOp, TestSize.Level1)
{
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(10, 10);
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 0);
    
    auto opItem = std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint());
    simpleDrawCmdList->AddDrawOp(std::move(opItem));
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 1);
    EXPECT_FALSE(simpleDrawCmdList->IsEmpty());
}
 
/**
 * @tc.name: UpdateNodeIdToPicture
 * @tc.desc: Test UpdateNodeIdToPicture updates node ID for all items
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, UpdateNodeIdToPicture, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    constexpr NodeId nodeId = 12345;
    simpleDrawCmdList->UpdateNodeIdToPicture(nodeId);
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 1);
}
 
/**
 * @tc.name: GetCmdlistDrawRegion_Empty
 * @tc.desc: Test GetCmdlistDrawRegion with empty list
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, GetCmdlistDrawRegion_Empty, TestSize.Level1)
{
    auto emptyList = std::make_shared<RSSimpleDrawCmdList>();
    auto region = emptyList->GetCmdlistDrawRegion();
    EXPECT_TRUE(region.IsEmpty());
}
 
/**
 * @tc.name: GetCmdlistDrawRegion_RectOpItem
 * @tc.desc: Test GetCmdlistDrawRegion with RectOpItem
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, GetCmdlistDrawRegion_RectOpItem, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 100, 100), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(200, 200, opItems);
    auto region = simpleDrawCmdList->GetCmdlistDrawRegion();
    EXPECT_FALSE(region.IsEmpty());
}
 
/**
 * @tc.name: Playback_WidthZero
 * @tc.desc: Test Playback with width <= 0
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Playback_WidthZero, TestSize.Level1)
{
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(0, 100);
    auto canvas = std::make_unique<Drawing::Canvas>();
    simpleDrawCmdList->Playback(*canvas, nullptr);
    EXPECT_TRUE(simpleDrawCmdList->IsEmpty());
}
 
/**
 * @tc.name: Playback_HeightZero
 * @tc.desc: Test Playback with height <= 0
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Playback_HeightZero, TestSize.Level1)
{
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 0);
    auto canvas = std::make_unique<Drawing::Canvas>();
    simpleDrawCmdList->Playback(*canvas, nullptr);
    EXPECT_TRUE(simpleDrawCmdList->IsEmpty());
}
 
/**
 * @tc.name: Playback_ValidDimensions
 * @tc.desc: Test Playback with valid dimensions
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Playback_ValidDimensions, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
    auto canvas = std::make_unique<Drawing::Canvas>();
    Drawing::Rect rect(0, 0, 50, 50);
    simpleDrawCmdList->Playback(*canvas, &rect);
    EXPECT_FALSE(simpleDrawCmdList->IsEmpty());
}
 
/**
 * @tc.name: Playback_WithoutRect
 * @tc.desc: Test Playback without rect parameter
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Playback_WithoutRect, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 10, 10), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(100, 100, opItems);
    auto canvas = std::make_unique<Drawing::Canvas>();
    simpleDrawCmdList->Playback(*canvas, nullptr);
    EXPECT_FALSE(simpleDrawCmdList->IsEmpty());
}
 
/**
 * @tc.name: SimpleDrawCmdListPtr_OperatorPlus
 * @tc.desc: Test SimpleDrawCmdListPtr operator+
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, SimpleDrawCmdListPtr_OperatorPlus, TestSize.Level1)
{
    auto list1 = std::make_shared<RSSimpleDrawCmdList>(100, 100);
    auto list2 = std::make_shared<RSSimpleDrawCmdList>(200, 200);
    
    auto resultPlus = list1 + list2;
    EXPECT_EQ(resultPlus.get(), list1.get());
}
 
/**
 * @tc.name: SimpleDrawCmdListPtr_OperatorMinus
 * @tc.desc: Test SimpleDrawCmdListPtr operator-
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, SimpleDrawCmdListPtr_OperatorMinus, TestSize.Level1)
{
    auto list1 = std::make_shared<RSSimpleDrawCmdList>(100, 100);
    auto list2 = std::make_shared<RSSimpleDrawCmdList>(200, 200);
    
    auto resultMinus = list1 - list2;
    EXPECT_EQ(resultMinus.get(), list1.get());
}
 
/**
 * @tc.name: SimpleDrawCmdListPtr_OperatorMultiply
 * @tc.desc: Test SimpleDrawCmdListPtr operator*
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, SimpleDrawCmdListPtr_OperatorMultiply, TestSize.Level1)
{
    auto list1 = std::make_shared<RSSimpleDrawCmdList>(100, 100);
    
    auto resultMultiply = list1 * 0.5f;
    EXPECT_EQ(resultMultiply.get(), list1.get());
}
 
/**
 * @tc.name: SimpleDrawCmdListPtr_OperatorEqual
 * @tc.desc: Test SimpleDrawCmdListPtr operator==
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, SimpleDrawCmdListPtr_OperatorEqual, TestSize.Level1)
{
    auto list1 = std::make_shared<RSSimpleDrawCmdList>(100, 100);
    auto list2 = std::make_shared<RSSimpleDrawCmdList>(200, 200);
    
    EXPECT_FALSE(list1 == list2);
}
 
/**
 * @tc.name: Purge_EmptyList
 * @tc.desc: Test Purge with empty list
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Purge_EmptyList, TestSize.Level1)
{
    auto emptyList = std::make_shared<RSSimpleDrawCmdList>();
    emptyList->Purge();
    EXPECT_TRUE(emptyList->IsEmpty());
}
 
/**
 * @tc.name: Purge_WithPixelmapOpItems
 * @tc.desc: Test Purge with pixelmap-related OpItems
 * @tc.type: FUNC
 */
HWTEST_F(RSSimpleDrawCmdListTest, Purge_WithPixelmapOpItems, TestSize.Level1)
{
    std::vector<std::shared_ptr<Drawing::DrawOpItem>> opItems;
    opItems.push_back(std::make_shared<Drawing::DrawRectOpItem>(Drawing::Rect(0, 0, 5, 5), Drawing::Paint()));
    auto simpleDrawCmdList = std::make_shared<RSSimpleDrawCmdList>(10, 10, opItems);
    simpleDrawCmdList->Purge();
    EXPECT_EQ(simpleDrawCmdList->GetOpItemSize(), 1);
}
 
} // namespace Rosen
} // namespace OHOS