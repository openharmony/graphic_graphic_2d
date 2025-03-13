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

#include <gtest/gtest.h>

#include "transaction/rs_uiextension_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

constexpr uint64_t DEFAULT_WIDTH = 30;
constexpr uint32_t DEFAULT_HEIGHT = 30;
constexpr uint64_t DEFAULT_HOST_PID = 1;
constexpr uint32_t DEFAULT_UIEXTENSION_PID = 2;

class RSRenderServiceUIExtensionDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceUIExtensionDataTest::SetUpTestCase() {}
void RSRenderServiceUIExtensionDataTest::TearDownTestCase() {}
void RSRenderServiceUIExtensionDataTest::SetUp() {}
void RSRenderServiceUIExtensionDataTest::TearDown() {}

/**
 * @tc.name: TestRSRenderServiceUIExtensionData001
 * @tc.desc: test marshalling and unmarshalling one node.
 * @tc.type:FUNC
 * @tc.require: issueIAAOIJ
 */
HWTEST_F(RSRenderServiceUIExtensionDataTest, TestRSRenderServiceUIExtensionData001, TestSize.Level1)
{
    Parcel parcel;
    RSUIExtensionData uiExtensionData;
    auto& secData = uiExtensionData.secData_;
    NodeId id = 1;
    secData.insert(std::make_pair(id, std::vector<SecSurfaceInfo>()));
    SecSurfaceInfo surfaceInfo;
    secData[id].push_back(surfaceInfo);
    secData[id][0].uiExtensionRectInfo.relativeCoords.SetAll(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    secData[id][0].hostPid = DEFAULT_HOST_PID;
    secData[id][0].uiExtensionPid = DEFAULT_UIEXTENSION_PID;

    SecRectInfo rectInfo;
    rectInfo.relativeCoords.SetAll(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    secData[id][0].upperNodes.push_back(rectInfo);

    uiExtensionData.Marshalling(parcel);
    auto result = RSUIExtensionData::Unmarshalling(parcel);
    auto resultData = result->GetSecData();

    ASSERT_EQ(secData.size(), resultData.size());
    ASSERT_EQ(secData[id].size(), resultData[id].size());
    ASSERT_EQ(secData[id][0].uiExtensionRectInfo.relativeCoords.GetWidth(),
        resultData[id][0].uiExtensionRectInfo.relativeCoords.GetWidth());
    ASSERT_EQ(secData[id][0].hostPid, resultData[id][0].hostPid);
    ASSERT_EQ(secData[id][0].uiExtensionPid, resultData[id][0].uiExtensionPid);

    ASSERT_EQ(secData[id][0].upperNodes.size(), resultData[id][0].upperNodes.size());
    ASSERT_EQ(secData[id][0].upperNodes[0].relativeCoords.GetWidth(),
        resultData[id][0].upperNodes[0].relativeCoords.GetWidth());
}

/**
 * @tc.name: TestRSRenderServiceUIExtensionData002
 * @tc.desc: test marshalling and unmarshalling multiple nodes.
 * @tc.type:FUNC
 * @tc.require: issueIAAOIJ
 */
HWTEST_F(RSRenderServiceUIExtensionDataTest, TestRSRenderServiceUIExtensionData002, TestSize.Level1)
{
    Parcel parcel;
    RSUIExtensionData uiExtensionData;
    auto& secData = uiExtensionData.secData_;
    NodeId id1 = 1;
    NodeId id2 = 2;
    secData.insert(std::make_pair(id1, std::vector<SecSurfaceInfo>()));
    SecSurfaceInfo surfaceInfo1;
    secData[id1].push_back(surfaceInfo1);
    secData[id1][0].uiExtensionRectInfo.relativeCoords.SetAll(0, 0, 0, 0);
    secData[id1][0].hostPid = 0;
    secData[id1][0].uiExtensionPid = 0;

    secData.insert(std::make_pair(id2, std::vector<SecSurfaceInfo>()));
    SecSurfaceInfo surfaceInfo2;
    secData[id2].push_back(surfaceInfo2);
    secData[id2][0].uiExtensionRectInfo.relativeCoords.SetAll(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    secData[id2][0].hostPid = DEFAULT_HOST_PID;
    secData[id2][0].uiExtensionPid = DEFAULT_UIEXTENSION_PID;

    SecRectInfo rectInfo;
    rectInfo.relativeCoords.SetAll(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    secData[id2][0].upperNodes.push_back(rectInfo);

    uiExtensionData.Marshalling(parcel);
    auto result = RSUIExtensionData::Unmarshalling(parcel);
    ASSERT_NE(result, nullptr);

    auto resultData = result->GetSecData();

    ASSERT_EQ(secData.size(), resultData.size());

    ASSERT_EQ(secData[id1].size(), resultData[id1].size());
    ASSERT_EQ(secData[id1][0].uiExtensionRectInfo.relativeCoords.GetWidth(),
        resultData[id1][0].uiExtensionRectInfo.relativeCoords.GetWidth());
    ASSERT_EQ(secData[id1][0].hostPid, resultData[id1][0].hostPid);
    ASSERT_EQ(secData[id1][0].uiExtensionPid, resultData[id1][0].uiExtensionPid);

    ASSERT_EQ(secData[id2].size(), resultData[id2].size());
    ASSERT_EQ(secData[id2][0].upperNodes.size(), resultData[id2][0].upperNodes.size());
    ASSERT_EQ(secData[id2][0].upperNodes[0].relativeCoords.GetWidth(),
        resultData[id2][0].upperNodes[0].relativeCoords.GetWidth());
}
} // namespace Rosen
} // namespace OHOS