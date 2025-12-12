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

#include <gtest/gtest.h>

#include "utils/typeface_map.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class TypefaceMapTest : public testing::Test {};

/**
 * @tc.name: TypefaceMapTest001
 * @tc.desc: test TypefaceMap
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceMapTest, TypefaceMapTest001, TestSize.Level0)
{
    TypefaceMap& instance = TypefaceMap::GetInstance();
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t uniqueId = typeface->GetUniqueID();
    instance.InsertTypeface(typeface->GetHash(), typeface);
    {
        auto result = instance.GetTypefaceByUniqueId(uniqueId);
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result.get(), typeface.get());
    }
    typeface.reset();
    EXPECT_EQ(instance.typefaceMap_.size(), 1);
    EXPECT_EQ(instance.GetTypefaceByUniqueId(uniqueId), nullptr);
    EXPECT_TRUE(instance.typefaceMap_.empty());
}

/**
 * @tc.name: TypefaceMapTest002
 * @tc.desc: test GetTypefaceByHash
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceMapTest, TypefaceMapTest002, TestSize.Level0)
{
    TypefaceMap& instance = TypefaceMap::GetInstance();
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t hash = typeface->GetHash();
    instance.InsertTypeface(hash, typeface);
    {
        auto result = instance.GetTypefaceByHash(hash);
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result.get(), typeface.get());
    }
    typeface.reset();
    EXPECT_EQ(instance.typefaceMap_.size(), 1);
    EXPECT_EQ(instance.GetTypefaceByHash(hash), nullptr);
    EXPECT_TRUE(instance.typefaceMap_.empty());
}

/**
 * @tc.name: TypefaceMapTest003
 * @tc.desc: test GetTypefaceByHashInner
 * @tc.type: FUNC
 */
HWTEST_F(TypefaceMapTest, TypefaceMapTest003, TestSize.Level0)
{
    TypefaceMap& instance = TypefaceMap::GetInstance();
    auto typeface = Drawing::Typeface::MakeDefault();
    ASSERT_NE(typeface, nullptr);
    uint32_t hash = typeface->GetHash();
    instance.InsertTypeface(hash, typeface);
    {
        auto result = instance.GetTypefaceByHashInner(hash);
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result.get(), typeface.get());
    }
    typeface.reset();
    EXPECT_EQ(instance.typefaceMap_.size(), 1);
    EXPECT_EQ(instance.GetTypefaceByHashInner(hash), nullptr);
    EXPECT_TRUE(instance.typefaceMap_.empty());
}

} // namespace Rosen
} // namespace OHOS