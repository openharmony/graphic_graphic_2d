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

#include "transaction/rs_self_drawing_node_rect_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSelfDrawingNodeRectDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSelfDrawingNodeRectDataTest::SetUpTestCase() {}
void RSSelfDrawingNodeRectDataTest::TearDownTestCase() {}
void RSSelfDrawingNodeRectDataTest::SetUp() {}
void RSSelfDrawingNodeRectDataTest::TearDown() {}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test marshalling and unmarshalling RSSelfDrawingNodeRectData.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSSelfDrawingNodeRectDataTest, MarshallingTest, TestSize.Level1)
{
    Parcel parcel;
    RSSelfDrawingNodeRectData SelfDrawingNodeRectData;
    auto& rectData = SelfDrawingNodeRectData.rectData_;
    NodeId id = 1;
    rectData.insert(std::make_pair(id, RectI(0, 0, 1, 1)));
    
    SelfDrawingNodeRectData.Marshalling(parcel);
    auto result = RSSelfDrawingNodeRectData::Unmarshalling(parcel);
    auto resultData = result->GetRectData();

    ASSERT_EQ(rectData.size(), resultData.size());
    ASSERT_EQ(rectData[id], resultData[id]);
}
} // namespace Rosen
} // namespace OHOS