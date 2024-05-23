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

#include "text/typeface.h"
#include "utils/memory_stream.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TypefaceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TypefaceTest::SetUpTestCase() {}
void TypefaceTest::TearDownTestCase() {}
void TypefaceTest::SetUp() {}
void TypefaceTest::TearDown() {}

/**
 * @tc.name: MakeFromStream001
 * @tc.desc: Test MakeFromStream
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeFromStream001, TestSize.Level1)
{
    char data[10] = { 0 };
    auto stream = std::make_unique<MemoryStream>(data, 10);
    ASSERT_TRUE(stream != nullptr);
    auto typeface = Typeface::MakeFromStream(std::move(stream));
    ASSERT_TRUE(typeface != nullptr);
}

/**
 * @tc.name: MakeFromName001
 * @tc.desc: Test MakeFromName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeFromName001, TestSize.Level1)
{
    FontStyle style;
    std::string familyName = "Arial";
    auto ret = Typeface::MakeFromName(familyName.c_str(), style);

    ASSERT_TRUE(ret != nullptr);
}

/**
 * @tc.name: GetFamilyName001
 * @tc.desc: Test GetFamilyName
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetFamilyName001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->GetFamilyName() == "");
}

/**
 * @tc.name: GetTableSize001
 * @tc.desc: Test GetTableSize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetTableSize001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->GetTableSize(0) == 0);
}

/**
 * @tc.name: GetTableData001
 * @tc.desc: Test GetTableData
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetTableData001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    char data[10] = { 0 };
    ASSERT_TRUE(typeface->GetTableData(0, 0, 10, data) == 0);
}

/**
 * @tc.name: GetItalic001
 * @tc.desc: Test GetItalic
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetItalic001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->GetItalic() == false);
}

/**
 * @tc.name: GetUniqueID001
 * @tc.desc: Test GetUniqueID
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetUniqueID001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->GetUniqueID() == 0);
}

/**
 * @tc.name: GetUnitsPerEm001
 * @tc.desc: Test GetUnitsPerEm
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, GetUnitsPerEm001, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->GetUnitsPerEm() == 0);
}

/**
 * @tc.name: MakeClone001
 * @tc.desc: Test MakeClone
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeClone001, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    FontArguments arg;
    ASSERT_TRUE(typeface->MakeClone(arg) != nullptr);
}

/**
 * @tc.name: MakeClone002
 * @tc.desc: Test MakeClone
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, MakeClone002, TestSize.Level1)
{
    auto typeface = std::make_unique<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    FontArguments arg;
    ASSERT_TRUE(typeface->MakeClone(arg) == nullptr);
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, Serialize001, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->Serialize() == nullptr);
}

/**
 * @tc.name: Serialize002
 * @tc.desc: Test Serialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, Serialize002, TestSize.Level1)
{
    auto typeface = std::make_shared<Typeface>(nullptr);
    ASSERT_TRUE(typeface != nullptr);
    ASSERT_TRUE(typeface->Serialize() == nullptr);
}

/**
 * @tc.name: Deserialize001
 * @tc.desc: Test Deserialize
 * @tc.type: FUNC
 * @tc.require:I91EDT
 */
HWTEST_F(TypefaceTest, Deserialize001, TestSize.Level1)
{
    auto typeface = Typeface::MakeDefault();
    ASSERT_TRUE(typeface != nullptr);
    char data[10] = { 0 };
    ASSERT_TRUE(typeface->Deserialize(data, 10) == nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS