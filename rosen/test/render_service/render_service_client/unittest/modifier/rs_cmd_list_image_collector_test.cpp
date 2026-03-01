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

#include "gtest/gtest.h"
#include "modifier/rs_cmd_list_image_collector.h"
#include "pipeline/rs_draw_cmd.h"
#include "recording/cmd_list_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSCmdListImageCollectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCmdListImageCollectorTest::SetUpTestCase() {}
void RSCmdListImageCollectorTest::TearDownTestCase() {}
void RSCmdListImageCollectorTest::SetUp() {}
void RSCmdListImageCollectorTest::TearDown() {}

/**
 * @tc.name: CollectCmdListImage001
 * @tc.desc: Test CollectCmdListImage with nullptr DrawCmdList
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage001, TestSize.Level1)
{
    auto result = RSCmdListImageCollector::CollectCmdListImage(nullptr);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CollectCmdListImage002
 * @tc.desc: Test CollectCmdListImage with empty DrawCmdList
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage002, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(1, 1);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CollectCmdListImage003
 * @tc.desc: Test CollectCmdListImage with DrawCmdList containing ExtendImageObject
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage003, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageObject = std::make_shared<RSExtendImageObject>();
    Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCmdList, imageObject);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageObjectVec, nullptr);
    EXPECT_GT(result->imageObjectVec->size(), 0);
}

/**
 * @tc.name: CollectCmdListImage004
 * @tc.desc: Test CollectCmdListImage with DrawCmdList containing ExtendImageBaseObj
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage004, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageBaseObj = std::make_shared<RSExtendImageBaseObj>();
    Drawing::CmdListHelper::AddImageBaseObjToCmdList(*drawCmdList, imageBaseObj);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageBaseObjVec, nullptr);
    EXPECT_GT(result->imageBaseObjVec->size(), 0);
}

/**
 * @tc.name: CollectCmdListImage005
 * @tc.desc: Test CollectCmdListImage with DrawCmdList containing ExtendImageNineObject
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage005, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageNineObject = std::make_shared<RSExtendImageNineObject>();
    Drawing::CmdListHelper::AddImageNineObjecToCmdList(*drawCmdList, imageNineObject);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageNineObjectVec, nullptr);
    EXPECT_GT(result->imageNineObjectVec->size(), 0);
}

/**
 * @tc.name: CollectCmdListImage006
 * @tc.desc: Test CollectCmdListImage with DrawCmdList containing ExtendImageLatticeObject
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage006, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageLatticeObject = std::make_shared<RSExtendImageLatticeObject>();
    Drawing::CmdListHelper::AddImageLatticeObjecToCmdList(*drawCmdList, imageLatticeObject);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageLatticeObjectVec, nullptr);
    EXPECT_GT(result->imageLatticeObjectVec->size(), 0);
}

/**
 * @tc.name: CollectCmdListImage007
 * @tc.desc: Test CollectCmdListImage with imageObjectVec empty and others not empty
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage007, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageBaseObj = std::make_shared<RSExtendImageBaseObj>();
    Drawing::CmdListHelper::AddImageBaseObjToCmdList(*drawCmdList, imageBaseObj);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->imageObjectVec, nullptr);
    EXPECT_NE(result->imageBaseObjVec, nullptr);
}

/**
 * @tc.name: CollectCmdListImage008
 * @tc.desc: Test CollectCmdListImage with imageBaseObjVec empty and others not empty
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage008, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageObject = std::make_shared<RSExtendImageObject>();
    Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCmdList, imageObject);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageObjectVec, nullptr);
    EXPECT_EQ(result->imageBaseObjVec, nullptr);
}

/**
 * @tc.name: CollectCmdListImage009
 * @tc.desc: Test CollectCmdListImage with imageNineObjectVec empty and others not empty
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage009, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageObject = std::make_shared<RSExtendImageObject>();
    Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCmdList, imageObject);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageObjectVec, nullptr);
    EXPECT_EQ(result->imageNineObjectVec, nullptr);
}

/**
 * @tc.name: CollectCmdListImage010
 * @tc.desc: Test CollectCmdListImage with imageLatticeObjectVec empty and others not empty
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage010, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto imageObject = std::make_shared<RSExtendImageObject>();
    Drawing::CmdListHelper::AddImageObjectToCmdList(*drawCmdList, imageObject);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_NE(result, nullptr);
    EXPECT_NE(result->imageObjectVec, nullptr);
    EXPECT_EQ(result->imageLatticeObjectVec, nullptr);
}

/**
 * @tc.name: CollectCmdListImage011
 * @tc.desc: Test CollectCmdListImage with all vectors empty
 * @tc.type:FUNC
 */
HWTEST_F(RSCmdListImageCollectorTest, CollectCmdListImage011, TestSize.Level1)
{
    auto drawCmdList = std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    auto saveOpItem = std::make_shared<Drawing::SaveOpItem>();
    drawCmdList->drawOpItems_.push_back(saveOpItem);
    auto result = RSCmdListImageCollector::CollectCmdListImage(drawCmdList);
    EXPECT_EQ(result, nullptr);
}
} // namespace OHOS::Rosen