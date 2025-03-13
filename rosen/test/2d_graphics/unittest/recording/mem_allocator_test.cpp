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

#include "gtest/gtest.h"

#include "recording/mem_allocator.h"

using namespace testing;
using namespace testing::ext;

namespace {
constexpr size_t LARGE_MALLOC = 200000000;
}

namespace OHOS::Rosen::Drawing {
class MemAllocatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MemAllocatorTest::SetUpTestCase() {}
void MemAllocatorTest::TearDownTestCase() {}
void MemAllocatorTest::SetUp() {}
void MemAllocatorTest::TearDown() {}

/**
 * @tc.name: BuildFromDataWithCopyTest001
 * @tc.desc: Test the BuildFromData and BuildFromDataWithCopy functions.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MemAllocatorTest, BuildFromDataWithCopyTest001, TestSize.Level1)
{
    auto memAllocator = std::make_shared<MemAllocator>();
    int number = 0;
    const void* data = &number;
    size_t size = 1;
    bool buildFromData = memAllocator->BuildFromData(data, size);
    bool buildFromDataWithCopy = memAllocator->BuildFromDataWithCopy(data, size);
    ASSERT_TRUE(buildFromData);
    ASSERT_TRUE(buildFromDataWithCopy);
    size = SIZE_MAX + 1;
    buildFromData = memAllocator->BuildFromData(data, size);
    buildFromDataWithCopy = memAllocator->BuildFromDataWithCopy(data, size);
    ASSERT_FALSE(buildFromData);
    ASSERT_FALSE(buildFromDataWithCopy);
    size = 0;
    buildFromData = memAllocator->BuildFromData(data, size);
    buildFromDataWithCopy = memAllocator->BuildFromDataWithCopy(data, size);
    ASSERT_FALSE(buildFromData);
    ASSERT_FALSE(buildFromDataWithCopy);
    const void* dataVal = nullptr;
    buildFromData = memAllocator->BuildFromData(dataVal, size);
    buildFromDataWithCopy = memAllocator->BuildFromDataWithCopy(dataVal, size);
    ASSERT_FALSE(buildFromData);
    ASSERT_FALSE(buildFromDataWithCopy);
}

/**
 * @tc.name: ResizeTest001
 * @tc.desc: Test the Resize function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MemAllocatorTest, ResizeTest001, TestSize.Level1)
{
    auto memAllocator = std::make_shared<MemAllocator>();
    size_t size = 1;
    ASSERT_FALSE(memAllocator->isReadOnly_);
    ASSERT_TRUE(memAllocator->Resize(size));
    size = LARGE_MALLOC + 1;
    ASSERT_TRUE(memAllocator->Resize(size));
    memAllocator->size_ = 2; // set the size to 2
    size = 1;
    ASSERT_FALSE(memAllocator->Resize(size));
    size = SIZE_MAX + 1;
    ASSERT_FALSE(memAllocator->Resize(size));
    size = 0;
    ASSERT_FALSE(memAllocator->Resize(size));
    memAllocator->isReadOnly_ = true;
    ASSERT_FALSE(memAllocator->Resize(size));
}

/**
 * @tc.name: AddTest001
 * @tc.desc: Test the Add function.
 * @tc.type: FUNC
 * @tc.require: IALK43
 */
HWTEST_F(MemAllocatorTest, AddTest001, TestSize.Level1)
{
    auto memAllocator = std::make_shared<MemAllocator>();
    size_t size = 1;
    int number = 0;
    const void* data = &number;
    memAllocator->size_ = 4; // set the size to 4
    ASSERT_FALSE(memAllocator->isReadOnly_);
    ASSERT_TRUE(memAllocator->capacity_ == 0);
    memAllocator->Add(data, size);

    size = 2; // set the size to 2
    memAllocator->size_ = 0;
    memAllocator->capacity_ = 1;
    memAllocator->Add(data, size);
    ASSERT_FALSE(memAllocator->capacity_ == 1);
    ASSERT_TRUE(memAllocator->GetSize());
    size = 1;
    memAllocator->size_ = 0;
    memAllocator->capacity_ = 1;
    memAllocator->Add(data, size);
    ASSERT_TRUE(memAllocator->capacity_ == 1);
    ASSERT_TRUE(memAllocator->GetSize());
    size = SIZE_MAX + 1;
    memAllocator->Add(data, size);
    ASSERT_TRUE(memAllocator->size_ == 1);
    size = 0;
    memAllocator->size_ = 0;
    memAllocator->Add(data, size);
    ASSERT_TRUE(memAllocator->size_ == 0);
    const void* dataVal = nullptr;
    memAllocator->Add(dataVal, size);
    ASSERT_TRUE(memAllocator->size_ == 0);
    memAllocator->isReadOnly_ = true;
    memAllocator->Add(dataVal, size);
    ASSERT_TRUE(memAllocator->size_ == 0);
}
} // namespace OHOS::Rosen::Drawing