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
#include "skia_adapter/skia_pixmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPixmapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaPixmapTest::SetUpTestCase() {}
void SkiaPixmapTest::TearDownTestCase() {}
void SkiaPixmapTest::SetUp() {}
void SkiaPixmapTest::TearDown() {}

/**
 * @tc.name: GetColorType001
 * @tc.desc: Test GetColorType
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetColorType001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetColorType();
    EXPECT_TRUE(ret == 0);
}

/**
 * @tc.name: GetAlphaType001
 * @tc.desc: Test GetAlphaType
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetAlphaType001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetAlphaType();
    EXPECT_TRUE(ret != AlphaType::ALPHATYPE_OPAQUE);
}

/**
 * @tc.name: GetColor001
 * @tc.desc: Test GetColor
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetColor001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetColor(0, 0);
    EXPECT_TRUE(ret == 0);
}

/**
 * @tc.name: GetRowBytes001
 * @tc.desc: Test GetRowBytes
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetRowBytes001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetRowBytes();
    EXPECT_TRUE(ret == 0);
}

/**
 * @tc.name: GetAddr001
 * @tc.desc: Test GetAddr
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetAddr001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetAddr();
    EXPECT_TRUE(ret == nullptr);
}

/**
 * @tc.name: GetWidth001
 * @tc.desc: Test GetWidth
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetWidth001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetWidth();
    EXPECT_TRUE(ret == 0);
}

/**
 * @tc.name: GetHeight001
 * @tc.desc: Test GetHeight
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPixmapTest, GetHeight001, TestSize.Level1)
{
    std::shared_ptr<SkiaPixmap> skiaPixmap = std::make_shared<SkiaPixmap>();
    SkPixmap skPixmap;
    skiaPixmap->ImportSkiaPixmap(skPixmap);
    auto ret = skiaPixmap->GetHeight();
    EXPECT_TRUE(ret == 0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS