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
#include "skia_adapter/skia_hm_symbol.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaHmSymbolTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaHmSymbolTest::SetUpTestCase() {}
void SkiaHmSymbolTest::TearDownTestCase() {}
void SkiaHmSymbolTest::SetUp() {}
void SkiaHmSymbolTest::TearDown() {}

/**
 * @tc.name: PathOutlineDecompose001
 * @tc.desc: Test PathOutlineDecompose
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolTest, PathOutlineDecompose001, TestSize.Level1)
{
    std::shared_ptr<SkiaHMSymbol> skiaHmSymbol = std::make_shared<SkiaHMSymbol>();
    ASSERT_TRUE(skiaHmSymbol != nullptr);
    Path path;
    Path path1;
    Path path2;
    std::vector<Path> paths { path1, path2 };
    skiaHmSymbol->PathOutlineDecompose(path, paths);
}

/**
 * @tc.name: MultilayerPath001
 * @tc.desc: Test MultilayerPath
 * @tc.type: FUNC
 * @tc.require: I91EQ7
 */
HWTEST_F(SkiaHmSymbolTest, MultilayerPath001, TestSize.Level1)
{
    std::shared_ptr<SkiaHMSymbol> skiaHmSymbol = std::make_shared<SkiaHMSymbol>();
    ASSERT_TRUE(skiaHmSymbol != nullptr);
    Path path1;
    Path path2;
    std::vector<Path> paths { path1, path2 };
    std::vector<Path> multPaths;
    skiaHmSymbol->MultilayerPath({}, paths, multPaths);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS