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

#include "gtest/gtest.h"

#include "utils/object.h"
#include "utils/object_helper.h"
#ifdef ROSEN_OHOS
#include <atomic>
#include <message_parcel.h>
#include <thread>
#include <vector>
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

// Mock Object implementation for testing
class MockObject : public Object {
public:
    MockObject(ObjectType type, int32_t subType) : Object(type, subType) {}

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) override
    {
        return true; // Mock implementation
    }

    bool Unmarshalling(Parcel& parcel, bool& isValid, int32_t depth = 0) override
    {
        isValid = true;
        return true; // Mock implementation
    }
#endif

    std::shared_ptr<void> GenerateBaseObject() override { return nullptr; }
};

class ObjectHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ObjectHelperTest::SetUpTestCase() {}
void ObjectHelperTest::TearDownTestCase() {}
void ObjectHelperTest::SetUp() {}
void ObjectHelperTest::TearDown() {}

/*
 * @tc.name: RegisterAndGetFunc001
 * @tc.desc: Test ObjectHelper Register and GetFunc basic functionality
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ObjectHelperTest, RegisterAndGetFunc001, TestSize.Level1)
{
    int32_t testType = 100;
    int32_t testSubType = 200;

    // Create a mock unmarshalling function with identifiable behavior
    ObjectHelper::UnmarshallingFunc mockFunc =
        [](Parcel& parcel, bool& isValid, int32_t depth) -> std::shared_ptr<Object> {
        // Return a mock object with specific type to verify function identity
        isValid = true;
        return std::make_shared<MockObject>(Object::ObjectType::SHADER_EFFECT, 12345);
    };

    // Test Register
    bool registerResult = ObjectHelper::Instance().Register(testType, testSubType, mockFunc);
    EXPECT_TRUE(registerResult);

    // Test GetFunc
    auto retrievedFunc = ObjectHelper::Instance().GetFunc(testType, testSubType);
    EXPECT_TRUE(retrievedFunc != nullptr);

    // Verify the retrieved function is the same as the registered one by testing behavior
#ifdef ROSEN_OHOS
    MessageParcel testParcel;
    bool isValid = true;
    auto result = retrievedFunc(testParcel, isValid, 0);
    EXPECT_TRUE(result != nullptr);
    EXPECT_TRUE(isValid);
    if (result) {
        EXPECT_EQ(result->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
        EXPECT_EQ(result->GetSubType(), 12345);
    }
#endif

    // Test GetFunc with non-existent type
    auto nonExistentFunc = ObjectHelper::Instance().GetFunc(999, 888);
    EXPECT_TRUE(nonExistentFunc == nullptr);
}

/*
 * @tc.name: RegisterAndGetFunc002
 * @tc.desc: Test ObjectHelper with multiple registrations
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ObjectHelperTest, RegisterAndGetFunc002, TestSize.Level1)
{
    // Register multiple functions
    struct TestCase {
        int32_t type;
        int32_t subType;
        int32_t expectedValue;
    };

    std::vector<TestCase> testCases = {
        {101, 201, 1001},
        {102, 202, 1002},
        {103, 203, 1003}
    };

    // Register all test functions
    for (const auto& testCase : testCases) {
        ObjectHelper::UnmarshallingFunc func = [expectedValue = testCase.expectedValue]
            (Parcel& parcel, bool& isValid, int32_t depth) -> std::shared_ptr<Object> {
            // Mock function that can be identified by expectedValue as subType
            isValid = true;
            return std::make_shared<MockObject>(Object::ObjectType::SHADER_EFFECT, expectedValue);
        };

        bool result = ObjectHelper::Instance().Register(testCase.type, testCase.subType, func);
        EXPECT_TRUE(result);
    }

    // Verify all functions can be retrieved and have correct behavior
    for (const auto& testCase : testCases) {
        auto retrievedFunc = ObjectHelper::Instance().GetFunc(testCase.type, testCase.subType);
        EXPECT_TRUE(retrievedFunc != nullptr);

#ifdef ROSEN_OHOS
        // Test the function behavior to verify it's the correct one
        MessageParcel testParcel;
        bool isValid = true;
        auto result = retrievedFunc(testParcel, isValid, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_TRUE(isValid);
        if (result) {
            EXPECT_EQ(result->GetType(), static_cast<int32_t>(Object::ObjectType::SHADER_EFFECT));
            EXPECT_EQ(result->GetSubType(), testCase.expectedValue);
        }
#endif
    }
}

/*
 * @tc.name: RegisterOverwrite001
 * @tc.desc: Test ObjectHelper Register overwrites existing registration
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ObjectHelperTest, RegisterOverwrite001, TestSize.Level1)
{
    int32_t testType = 104;
    int32_t testSubType = 204;

    // Register first function with identifiable behavior
    ObjectHelper::UnmarshallingFunc func1 =
        [](Parcel& parcel, bool& isValid, int32_t depth) -> std::shared_ptr<Object> {
        isValid = true;
        return std::make_shared<MockObject>(Object::ObjectType::IMAGE_FILTER, 111);
    };
    bool result1 = ObjectHelper::Instance().Register(testType, testSubType, func1);
    EXPECT_TRUE(result1);

    auto retrievedFunc1 = ObjectHelper::Instance().GetFunc(testType, testSubType);
    EXPECT_TRUE(retrievedFunc1 != nullptr);

    // Register second function with same type/subType (should overwrite)
    ObjectHelper::UnmarshallingFunc func2 =
        [](Parcel& parcel, bool& isValid, int32_t depth) -> std::shared_ptr<Object> {
        isValid = true;
        return std::make_shared<MockObject>(Object::ObjectType::IMAGE_FILTER, 222);
    };
    bool result2 = ObjectHelper::Instance().Register(testType, testSubType, func2);
    EXPECT_TRUE(result2);

    auto retrievedFunc2 = ObjectHelper::Instance().GetFunc(testType, testSubType);
    EXPECT_TRUE(retrievedFunc2 != nullptr);

    // Verify the retrieved function is the second one (overwritten) by testing behavior
#ifdef ROSEN_OHOS
    MessageParcel testParcel;
    bool isValid = true;
    auto result = retrievedFunc2(testParcel, isValid, 0);
    EXPECT_TRUE(result != nullptr);
    EXPECT_TRUE(isValid);
    if (result) {
        EXPECT_EQ(result->GetType(), static_cast<int32_t>(Object::ObjectType::IMAGE_FILTER));
        EXPECT_EQ(result->GetSubType(), 222); // Should be the second function's behavior
    }
#endif
}

#ifdef ROSEN_OHOS
/*
 * @tc.name: DataCallbacks001
 * @tc.desc: Test ObjectHelper Data marshalling/unmarshalling callbacks
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(ObjectHelperTest, DataCallbacks001, TestSize.Level1)
{
    // Create mock callbacks
    std::function<bool(Parcel&, std::shared_ptr<Data>)> marshallingCallback =
        [](Parcel& parcel, std::shared_ptr<Data> data) -> bool {
            return true; // Mock implementation
        };

    std::function<std::shared_ptr<Data>(Parcel&)> unmarshallingCallback =
        [](Parcel& parcel) -> std::shared_ptr<Data> {
            return nullptr; // Mock implementation
        };

    // Set callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(marshallingCallback);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(unmarshallingCallback);

    // Get callbacks
    auto retrievedMarshallingCallback = ObjectHelper::Instance().GetDataMarshallingCallback();
    auto retrievedUnmarshallingCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    EXPECT_TRUE(retrievedMarshallingCallback != nullptr);
    EXPECT_TRUE(retrievedUnmarshallingCallback != nullptr);

    // Reset callbacks
    ObjectHelper::Instance().SetDataMarshallingCallback(nullptr);
    ObjectHelper::Instance().SetDataUnmarshallingCallback(nullptr);

    auto resetMarshallingCallback = ObjectHelper::Instance().GetDataMarshallingCallback();
    auto resetUnmarshallingCallback = ObjectHelper::Instance().GetDataUnmarshallingCallback();

    EXPECT_TRUE(resetMarshallingCallback == nullptr);
    EXPECT_TRUE(resetUnmarshallingCallback == nullptr);
}
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
