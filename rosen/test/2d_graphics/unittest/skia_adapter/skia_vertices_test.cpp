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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_vertices.h"
#include "utils/vertices.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaVerticesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaVerticesTest::SetUpTestCase() {}
void SkiaVerticesTest::TearDownTestCase() {}
void SkiaVerticesTest::SetUp() {}
void SkiaVerticesTest::TearDown() {}

/**
 * @tc.name: MakeCopy001
 * @tc.desc: Test MakeCopy
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaVerticesTest, MakeCopy001, TestSize.Level1)
{
    SkiaVertices::SkiaBuilder skiaBuilder;
    int vertCounts = 4;
    int indexCount = 6;
    skiaBuilder.Init(VertexMode::TRIANGLES_VERTEXMODE, vertCounts, indexCount, 1);
    ASSERT_TRUE(skiaBuilder.IsValid());
    SkiaVertices skiaVertices;
    bool ret = skiaVertices.MakeCopy(VertexMode::TRIANGLES_VERTEXMODE, vertCounts,
        skiaBuilder.Positions(), skiaBuilder.TexCoords(), skiaBuilder.Colors(),
        indexCount, skiaBuilder.Indices());
    ASSERT_TRUE(ret);
    ColorQuad colors[] = {0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000};
    ret = skiaVertices.MakeCopy(VertexMode::TRIANGLES_VERTEXMODE, vertCounts,
        skiaBuilder.Positions(), skiaBuilder.TexCoords(), colors,
        indexCount, skiaBuilder.Indices());
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: MakeCopy002
 * @tc.desc: Test MakeCopy
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaVerticesTest, MakeCopy002, TestSize.Level1)
{
    SkiaVertices::SkiaBuilder skiaBuilder;
    int vertCounts = 4;
    int indexCount = 6;
    skiaBuilder.Init(VertexMode::TRIANGLES_VERTEXMODE, vertCounts, indexCount, 1);
    ASSERT_TRUE(skiaBuilder.IsValid());
    SkiaVertices skiaVertices;
    bool ret = skiaVertices.MakeCopy(VertexMode::TRIANGLES_VERTEXMODE, vertCounts,
        skiaBuilder.Positions(), skiaBuilder.TexCoords(), skiaBuilder.Colors());
    ASSERT_TRUE(ret);
    ColorQuad colors[] = {0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000};
    ret = skiaVertices.MakeCopy(VertexMode::TRIANGLES_VERTEXMODE, vertCounts,
        skiaBuilder.Positions(), skiaBuilder.TexCoords(), colors);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: GetVertices001
 * @tc.desc: Test GetVertices
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaVerticesTest, GetVertices001, TestSize.Level1)
{
    SkiaVertices skiaVertices;
    ASSERT_TRUE(skiaVertices.GetVertices() == nullptr);
}

/**
 * @tc.name: SkiaBuilder001
 * @tc.desc: Test SkiaBuilder
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(SkiaVerticesTest, SkiaBuilder001, TestSize.Level1)
{
    SkiaVertices::SkiaBuilder skiaBuilder;
    ASSERT_TRUE(!skiaBuilder.IsValid());
    ASSERT_TRUE(skiaBuilder.Positions() == nullptr);
    ASSERT_TRUE(skiaBuilder.Indices() == nullptr);
    ASSERT_TRUE(skiaBuilder.TexCoords() == nullptr);
    ASSERT_TRUE(skiaBuilder.Colors() == nullptr);
    ASSERT_TRUE(skiaBuilder.Detach() == nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS