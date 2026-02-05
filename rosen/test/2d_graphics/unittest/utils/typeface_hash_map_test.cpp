/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "utils/typeface_hash_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TypefaceHashMapTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TypefaceHashMapTest::SetUpTestCase() {}
void TypefaceHashMapTest::TearDownTestCase() {}
void TypefaceHashMapTest::SetUp() {}
void TypefaceHashMapTest::TearDown() {}

/**
 * @tc.name: TypefaceHashMapTest001
 * @tc.desc: test GetTypefaceByFullHash
 * @tc.type: FUNC
 * @tc.require:22161
 */
HWTEST_F(TypefaceHashMapTest, TypefaceHashMapTest001, TestSize.Level0)
{
    TypefaceHashMap& instance = TypefaceHashMap::GetInstance();
    instance.InsertTypefaceByFullHash(0, nullptr);
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t hash = typeface->GetHash();
    instance.InsertTypefaceByFullHash(hash, typeface);
    {
        auto result = instance.GetTypefaceByFullHash(hash);
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result.get(), typeface.get());
    }
    typeface.reset();
    EXPECT_EQ(instance.typefaceMap_.size(), 1);
    EXPECT_EQ(instance.GetTypefaceByFullHash(hash), nullptr);
    EXPECT_TRUE(instance.typefaceMap_.empty());
}

/**
 * @tc.name: TypefaceHashMapTest002
 * @tc.desc: test Release
 * @tc.type: FUNC
 * @tc.require:22161
 */
HWTEST_F(TypefaceHashMapTest, TypefaceHashMapTest002, TestSize.Level0)
{
    TypefaceHashMap& instance = TypefaceHashMap::GetInstance();
    instance.Release(0);
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t hash = typeface->GetHash();
    instance.InsertTypefaceByFullHash(hash, typeface);
    instance.Release(hash);
    EXPECT_EQ(instance.GetTypefaceByFullHash(hash), nullptr);
    EXPECT_TRUE(instance.typefaceMap_.empty());
}

/**
 * @tc.name: TypefaceHashMapTest003
 * @tc.desc: test Release
 * @tc.type: FUNC
 * @tc.require:22161
 */
HWTEST_F(TypefaceHashMapTest, TypefaceHashMapTest003, TestSize.Level0)
{
    TypefaceHashMap& instance = TypefaceHashMap::GetInstance();
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t hash = typeface->GetHash();
    instance.InsertTypefaceByFullHash(hash, typeface);
    typeface.reset();
    instance.Release(hash);
    EXPECT_EQ(instance.GetTypefaceByFullHash(hash), nullptr);
    EXPECT_TRUE(instance.typefaceMap_.empty());
}

/**
 * @tc.name: TypefaceHashMapTest004
 * @tc.desc: test Release
 * @tc.type: FUNC
 * @tc.require:22161
 */
HWTEST_F(TypefaceHashMapTest, TypefaceHashMapTest004, TestSize.Level0)
{
    TypefaceHashMap& instance = TypefaceHashMap::GetInstance();
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t hash = typeface->GetHash();
    instance.InsertTypefaceByFullHash(hash, typeface);
    auto result = instance.GetTypefaceByFullHash(hash);
    instance.Release(hash);
    EXPECT_EQ(instance.typefaceMap_.size(), 1);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
