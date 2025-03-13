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

#include "utils/vertices.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class VerticesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

constexpr int VERTICES_COUNT = 3;

void VerticesTest::SetUpTestCase() {}
void VerticesTest::TearDownTestCase() {}
void VerticesTest::SetUp() {}
void VerticesTest::TearDown() {}

/**
 * @tc.name: MakeCopy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(VerticesTest, MakeCopy001, TestSize.Level1)
{
    std::unique_ptr<Vertices> vertices = std::make_unique<Vertices>();
    ASSERT_TRUE(vertices != nullptr);
    Point positions[VERTICES_COUNT] = { {0, 0}, {1, 1}, {2, 2} };
    Point texs[VERTICES_COUNT] = { {0, 1}, {1, 2}, {2, 3} };
    ColorQuad colors[VERTICES_COUNT] = { 0xFFFFFFFF, 0xFFFF00FF, 0xFFFF0000 };
    uint16_t indices[VERTICES_COUNT] = { 0, 1, 2 };
    bool flag1 = vertices->MakeCopy(VertexMode::LAST_VERTEXMODE,
        VERTICES_COUNT, positions, texs, colors, VERTICES_COUNT, indices);
    EXPECT_EQ(flag1, true);

    std::unique_ptr<Vertices> vertices2 = std::make_unique<Vertices>(nullptr);
    bool flag2 = vertices2->MakeCopy(VertexMode::LAST_VERTEXMODE,
        VERTICES_COUNT, positions, texs, colors, VERTICES_COUNT, indices);
    EXPECT_EQ(flag2, false);
}

/**
 * @tc.name: MakeCopy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(VerticesTest, MakeCopy002, TestSize.Level1)
{
    std::unique_ptr<Vertices> vertices = std::make_unique<Vertices>();
    ASSERT_TRUE(vertices != nullptr);
    Point positions[] = { {0, 0}, {1, 1}, {2, 2} };
    Point texs[] = { {0, 1}, {1, 2}, {2, 3} };
    ColorQuad colors[] = { 0xFFFFFFFF, 0xFFFF00FF, 0xFFFF0000 };
    bool flag1 = vertices->MakeCopy(VertexMode::LAST_VERTEXMODE, VERTICES_COUNT, positions, texs, colors);
    EXPECT_EQ(flag1, true);

    std::unique_ptr<Vertices> vertices2 = std::make_unique<Vertices>(nullptr);
    bool flag2 = vertices2->MakeCopy(VertexMode::LAST_VERTEXMODE, VERTICES_COUNT, positions, texs, colors);
    EXPECT_EQ(flag2, false);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS