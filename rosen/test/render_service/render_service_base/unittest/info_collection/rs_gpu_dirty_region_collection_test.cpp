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

#include "info_collection/rs_gpu_dirty_region_collection.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class GpuDirtyRegionCollectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void GpuDirtyRegionCollectionTest::SetUpTestCase() {}
void GpuDirtyRegionCollectionTest::TearDownTestCase() {}
void GpuDirtyRegionCollectionTest::SetUp() {}
void GpuDirtyRegionCollectionTest::TearDown() {}

/**
 * @tc.name: UpdateActiveDirtyInfoForDFXTest001
 * @tc.desc: UpdateActiveDirtyInfoForDFX test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, UpdateActiveDirtyInfoForDFXTest001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    std::string windowName = "TestWindow";
    NodeId id = 1;
    std::vector<RectI> rectIs;
    EXPECT_TRUE(rectIs.empty());
    gpuDirtyRegionCollection->UpdateActiveDirtyInfoForDFX(id, windowName, rectIs);
    EXPECT_TRUE(gpuDirtyRegionCollection->activeDirtyRegionInfoMap_[id].activeFramesNumber == 0);

    RectI rect = { 1, 1, 1, 1 };
    rectIs.emplace_back(rect);
    EXPECT_FALSE(rectIs.empty());
    gpuDirtyRegionCollection->UpdateActiveDirtyInfoForDFX(id, windowName, rectIs);
    EXPECT_TRUE(gpuDirtyRegionCollection->activeDirtyRegionInfoMap_[id].activeFramesNumber > 0);
}

/**
 * @tc.name: UpdateActiveDirtyInfoForDFXTest002
 * @tc.desc: UpdateActiveDirtyInfoForDFX test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, UpdateActiveDirtyInfoForDFXTest002, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    std::string windowName = "TestWindow";
    NodeId id = 1;
    Rect rect = { 1, 1, 1, 1 };
    gpuDirtyRegionCollection->UpdateActiveDirtyInfoForDFX(id, windowName, rect);
    EXPECT_TRUE(gpuDirtyRegionCollection->activeDirtyRegionInfoMap_[id].activeFramesNumber > 0);
}

/**
 * @tc.name: UpdateGlobalDirtyInfoForDFX001
 * @tc.desc: UpdateGlobalDirtyInfoForDFX test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, UpdateGlobalDirtyInfoForDFX001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    std::string windowName = "TestWindow";
    RectI rect = { 1, 1, 1, 1 };
    gpuDirtyRegionCollection->UpdateGlobalDirtyInfoForDFX(rect);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.globalFramesNumber > 0);
}

/**
 * @tc.name: AddSkipProcessFramesNumberForDFX001
 * @tc.desc: AddSkipProcessFramesNumberForDFX test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, AddSkipProcessFramesNumberForDFX001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.skipProcessFramesNumber == 0);

    pid_t pid = 1;
    gpuDirtyRegionCollection->AddSkipProcessFramesNumberForDFX(pid);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.skipProcessFramesNumber > 0);
}

/**
 * @tc.name: GetActiveDirtyRegionInfo001
 * @tc.desc: GetActiveDirtyRegionInfo test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, GetActiveDirtyRegionInfo001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    auto res1 = gpuDirtyRegionCollection->GetActiveDirtyRegionInfo();
    EXPECT_TRUE(res1.size() == 0);

    std::string windowName = "TestWindow";
    NodeId id = 1;
    Rect rect = { 1, 1, 1, 1 };
    gpuDirtyRegionCollection->UpdateActiveDirtyInfoForDFX(id, windowName, rect);

    auto res2 = gpuDirtyRegionCollection->GetActiveDirtyRegionInfo();
    EXPECT_FALSE(res1.size() == 1);
}

/**
 * @tc.name: GetGlobalDirtyRegionInfo001
 * @tc.desc: GetGlobalDirtyRegionInfo test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, GetGlobalDirtyRegionInfo001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    auto res1 = gpuDirtyRegionCollection->GetGlobalDirtyRegionInfo();
    EXPECT_TRUE(res1.globalFramesNumber == 0);

    std::string windowName = "TestWindow";
    RectI rect = { 1, 1, 1, 1 };
    gpuDirtyRegionCollection->UpdateGlobalDirtyInfoForDFX(rect);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.globalFramesNumber > 0);

    auto res2 = gpuDirtyRegionCollection->GetGlobalDirtyRegionInfo();
    EXPECT_TRUE(res2.globalFramesNumber > 0);
}

/**
 * @tc.name: ResetActiveDirtyRegionInfo001
 * @tc.desc: ResetActiveDirtyRegionInfo test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, ResetActiveDirtyRegionInfo001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    std::string windowName = "TestWindow";
    NodeId id = 1;
    Rect rect = { 1, 1, 1, 1 };
    gpuDirtyRegionCollection->UpdateActiveDirtyInfoForDFX(id, windowName, rect);

    auto res2 = gpuDirtyRegionCollection->GetActiveDirtyRegionInfo();
    EXPECT_FALSE(gpuDirtyRegionCollection->activeDirtyRegionInfoMap_.empty());

    gpuDirtyRegionCollection->ResetActiveDirtyRegionInfo();
    EXPECT_TRUE(gpuDirtyRegionCollection->activeDirtyRegionInfoMap_.empty());
}

/**
 * @tc.name: ResetGlobalDirtyRegionInfo001
 * @tc.desc: ResetGlobalDirtyRegionInfo test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, ResetGlobalDirtyRegionInfo001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);

    std::string windowName = "TestWindow";
    RectI rect = { 1, 1, 1, 1 };
    gpuDirtyRegionCollection->UpdateGlobalDirtyInfoForDFX(rect);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.globalFramesNumber > 0);

    gpuDirtyRegionCollection->ResetGlobalDirtyRegionInfo();
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.globalFramesNumber == 0);
}

/**
 * @tc.name: GetMostSendingPidWhenDisplayNodeSkip001
 * @tc.desc: GetMostSendingPidWhenDisplayNodeSkip test
 * @tc.type:FUNC
 * @tc.require: issueI9VSPU
 */
HWTEST_F(GpuDirtyRegionCollectionTest, GetMostSendingPidWhenDisplayNodeSkip001, TestSize.Level1)
{
    std::shared_ptr<GpuDirtyRegionCollection> gpuDirtyRegionCollection = std::make_shared<GpuDirtyRegionCollection>();
    EXPECT_NE(gpuDirtyRegionCollection, nullptr);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.skipProcessFramesNumber == 0);

    auto res1 = gpuDirtyRegionCollection->GetMostSendingPidWhenDisplayNodeSkip();
    EXPECT_TRUE(res1 == 0);

    pid_t pid = 1;
    gpuDirtyRegionCollection->AddSkipProcessFramesNumberForDFX(pid);
    EXPECT_TRUE(gpuDirtyRegionCollection->globalDirtyRegionInfo_.skipProcessFramesNumber > 0);
    auto res2 = gpuDirtyRegionCollection->GetMostSendingPidWhenDisplayNodeSkip();
    EXPECT_TRUE(res2 == 1);
}
} // namespace OHOS::Rosen