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

#include "recording/mask_cmd_list.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen::Drawing {
class MaskCmdListTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MaskCmdListTest::SetUpTestCase() {}
void MaskCmdListTest::TearDownTestCase() {}
void MaskCmdListTest::SetUp() {}
void MaskCmdListTest::TearDown() {}

class MaskPlayerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MaskPlayerTest::SetUpTestCase() {}
void MaskPlayerTest::TearDownTestCase() {}
void MaskPlayerTest::SetUp() {}
void MaskPlayerTest::TearDown() {}

class MaskBrushOpItemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MaskBrushOpItemTest::SetUpTestCase() {}
void MaskBrushOpItemTest::TearDownTestCase() {}
void MaskBrushOpItemTest::SetUp() {}
void MaskBrushOpItemTest::TearDown() {}

class MaskPathOpItemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MaskPathOpItemTest::SetUpTestCase() {}
void MaskPathOpItemTest::TearDownTestCase() {}
void MaskPathOpItemTest::SetUp() {}
void MaskPathOpItemTest::TearDown() {}

class MaskPenOpItemTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MaskPenOpItemTest::SetUpTestCase() {}
void MaskPenOpItemTest::TearDownTestCase() {}
void MaskPenOpItemTest::SetUp() {}
void MaskPenOpItemTest::TearDown() {}

/**
 * @tc.name: CreateFromDataTest001
 * @tc.desc: Test the CreateFromData function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MaskCmdListTest, CreateFromDataTest001, TestSize.Level1)
{
    auto cmdList = std::make_shared<MaskCmdList>();
    int array[] = { 1, 2, 3, 4, 5 };
    size_t arraySize = sizeof(array) / sizeof(array[0]);
    CmdListData data;
    data.first = static_cast<const void*>(array);
    data.second = arraySize;
    ASSERT_TRUE(cmdList->CreateFromData(data, true));
    ASSERT_TRUE(cmdList->CreateFromData(data, false));
}

/**
 * @tc.name: PlaybackTest001
 * @tc.desc: Test the Playback function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MaskCmdListTest, PlaybackTest001, TestSize.Level1)
{
    auto cmdList = std::make_shared<MaskCmdList>();
    auto path = std::make_shared<Path>();
    Brush brush;
    const CmdList list;
    auto maskPlayer = std::make_shared<MaskPlayer>(path, brush, list);
    ASSERT_FALSE(cmdList->Playback(*maskPlayer));

    cmdList->opAllocator_.size_ = sizeof(OpItem);
    ASSERT_TRUE(cmdList->Playback(*maskPlayer));

    Pen pen;
    ASSERT_TRUE(cmdList->Playback(path, brush));
    ASSERT_TRUE(cmdList->Playback(path, pen, brush));
}

/**
 * @tc.name: PlaybackTest002
 * @tc.desc: Test the Playback function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MaskPlayerTest, PlaybackTest002, TestSize.Level1)
{
    auto path = std::make_shared<Path>();
    Brush brush;
    const CmdList list;
    auto maskPlayer = std::make_shared<MaskPlayer>(path, brush, list);
    uint32_t type = MaskOpItem::OPITEM_HEAD;
    PenHandle penHandle;
    auto maskPenOpItem = std::make_shared<MaskPenOpItem>(penHandle);
    const void* opItem = maskPenOpItem.get();
    size_t leftOpAllocatorSize = 0;
    ASSERT_TRUE(maskPlayer->Playback(type, opItem, leftOpAllocatorSize));

    int value = 4;
    type = static_cast<uint32_t>(value);
    ASSERT_FALSE(maskPlayer->opPlaybackFuncLUT_.empty());
    ASSERT_FALSE(maskPlayer->Playback(type, opItem, leftOpAllocatorSize));

    type = MaskOpItem::MASK_PATH_OPITEM;
    ASSERT_TRUE(maskPlayer->Playback(type, opItem, leftOpAllocatorSize));
}

/**
 * @tc.name: PlaybackTest003
 * @tc.desc: Test the Playback function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MaskBrushOpItemTest, PlaybackTest003, TestSize.Level1)
{
    BrushHandle brushHandle;
    auto maskBrushOpItem = std::make_shared<MaskBrushOpItem>(brushHandle);
    auto path = std::make_shared<Path>();
    Brush brush;
    const CmdList list;
    auto maskPlayer = std::make_shared<MaskPlayer>(path, brush, list);
    const void* opItem = maskBrushOpItem.get();
    size_t leftOpAllocatorSize = 0;
    MaskBrushOpItem::Playback(*maskPlayer, opItem, leftOpAllocatorSize);

    leftOpAllocatorSize = sizeof(MaskBrushOpItem);
    MaskBrushOpItem::Playback(*maskPlayer, opItem, leftOpAllocatorSize);
    const void* item = nullptr;
    MaskBrushOpItem::Playback(*maskPlayer, item, leftOpAllocatorSize);
    maskBrushOpItem->Playback(brush, list);
    ASSERT_TRUE(opItem != nullptr);
}

/**
 * @tc.name: PlaybackTest004
 * @tc.desc: Test the Playback function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MaskPathOpItemTest, PlaybackTest004, TestSize.Level1)
{
    OpDataHandle pathHandle;
    auto maskPathOpItem = std::make_shared<MaskPathOpItem>(pathHandle);
    auto path = std::make_shared<Path>();
    Brush brush;
    const CmdList list;
    auto maskPlayer = std::make_shared<MaskPlayer>(path, brush, list);
    const void* opItem = maskPathOpItem.get();
    size_t leftOpAllocatorSize = 0;
    MaskPathOpItem::Playback(*maskPlayer, opItem, leftOpAllocatorSize);

    leftOpAllocatorSize = sizeof(MaskPathOpItem);
    MaskPathOpItem::Playback(*maskPlayer, opItem, leftOpAllocatorSize);
    const void* item = nullptr;
    MaskPathOpItem::Playback(*maskPlayer, item, leftOpAllocatorSize);
    maskPathOpItem->Playback(path, list);
    ASSERT_TRUE(opItem != nullptr);
}

/**
 * @tc.name: PlaybackTest005
 * @tc.desc: Test the Playback function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MaskPenOpItemTest, PlaybackTest005, TestSize.Level1)
{
    PenHandle penHandle;
    auto maskPenOpItem = std::make_shared<MaskPenOpItem>(penHandle);
    auto path = std::make_shared<Path>();
    Brush brush;
    const CmdList list;
    auto maskPlayer = std::make_shared<MaskPlayer>(path, brush, list);
    const void* opItem = maskPenOpItem.get();
    size_t leftOpAllocatorSize = 0;
    MaskPenOpItem::Playback(*maskPlayer, opItem, leftOpAllocatorSize);

    leftOpAllocatorSize = sizeof(MaskPenOpItem);
    MaskPenOpItem::Playback(*maskPlayer, opItem, leftOpAllocatorSize);
    const void* item = nullptr;
    MaskPenOpItem::Playback(*maskPlayer, item, leftOpAllocatorSize);
    Pen pen;
    maskPenOpItem->Playback(pen, list);
    ASSERT_TRUE(opItem != nullptr);
}
} // namespace OHOS::Rosen::Drawing