/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "input_listener_manager_test.h"

namespace OHOS {
void InputListenerManagerTest::SetUp()
{
}

void InputListenerManagerTest::TearDown()
{
}

void InputListenerManagerTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << getpid() << std::endl;
    initRet = WindowManager::GetInstance()->Init();
}

void InputListenerManagerTest::TearDownTestCase()
{
}

namespace {
/*
 * Feature: InputListenerManager AddListener
 * Function: InputListenerManager
 * SubFunction: AddListener
 * FunctionPoints: InputListenerManager AddListener
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. add listener[0] by an address A
 *                  2. check listener[0]
 *                      a. check listener[0] isn't nullptr
 *                      b. check listener[0].GetWindow() is address A
 *                      c. check map size is 1
 *                      d. check vector size of address A is 1
 *                  3. add listener[1] by nullptr
 *                  4. check listener[1]
 *                      a. check listener[1] isn't nullptr
 *                      b. check listener[1].GetWindow() is nullptr
 *                      c. check map size is 2
 *                      d. check vector size of nullptr is 1
 *                  5. add listener[2] by the address A
 *                  6. check listener[2]
 *                      a. check listener[2] isn't nullptr
 *                      b. check listener[2].GetWindow() is address A
 *                      c. check map size is 2
 *                      d. check vector size of address A is 2
 *                  7. add listener[3] by nullptr
 *                  8. check listener[3]
 *                      a. check listener[3] isn't nullptr
 *                      b. check listener[3].GetWindow() is nullptr
 *                      c. check map size is 2
 *                      d. check vector size of nullptr is 2
 */
HWTEST_F(InputListenerManagerTest, AddListener, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    auto manager = InputListenerManager::GetInstance();

    // 1. add listener[0] by an address A
    listener[0] = manager->AddListener(&addressA);

    // 2. check listener[0]
    {
        // a. check listener[0] isn't nullptr
        ASSERT_NE(listener[0], nullptr) << "CaseDescription:"
            << " 2.a check listener[0] isn't nullptr (listener[0] != nullptr)";
        // b. check listener[0].GetWindow() is address A
        ASSERT_EQ(listener[0]->GetWindow(), &addressA) << "CaseDescription:"
            << " 2.b. check listener[0].GetWindow() is address A (listener[0].GetWindow() == address A)";
        // c. check map size is 1
        ASSERT_EQ(manager->inputListenersMap.size(), 1u) << "CaseDescription:"
            << " 2.c check map size is 1 (map.size() == 1)";
        // d. check vector size of address A is 1
        ASSERT_EQ(manager->inputListenersMap[&addressA].size(), 1u) << "CaseDescription:"
            << " 2.d check vector size of address A is 1 (vector.size() == 1)";
    }

    // 3. add listener[1] by nullptr
    listener[1] = manager->AddListener(nullptr);

    // 4. check listener[1]
    {
        // a. check listener[1] isn't nullptr
        ASSERT_NE(listener[1], nullptr) << "CaseDescription:"
            << " 4.a check listener[1] isn't nullptr (listener[1] != nullptr)";
        // b. check listener[1].GetWindow() is address A
        ASSERT_EQ(listener[1]->GetWindow(), nullptr) << "CaseDescription:"
            << " 4.b. check listener[1].GetWindow() is address A (listener[1].GetWindow() == nullptr)";
        // c. check map size is 2
        ASSERT_EQ(manager->inputListenersMap.size(), 2u) << "CaseDescription:"
            << " 4.c check map size is 2 (map.size() == 2)";
        // d. check vector size of nullptr is 1
        ASSERT_EQ(manager->inputListenersMap[nullptr].size(), 1u) << "CaseDescription:"
            << " 4.d check vector size of nullptr is 1 (vector.size() == 1)";
    }

    // 5. add listener[2] by the address A
    listener[2] = manager->AddListener(&addressA);

    // 6. check listener[2]
    {
        // a. check listener[2] isn't nullptr
        ASSERT_NE(listener[2], nullptr) << "CaseDescription:"
            << " 6.a check listener[2] isn't nullptr (listener[2] != nullptr)";
        // b. check listener[2].GetWindow() is address A
        ASSERT_EQ(listener[2]->GetWindow(), &addressA) << "CaseDescription:"
            << " 6.b. check listener[2].GetWindow() is address A (listener[2].GetWindow() == address A)";
        // c. check map size is 2
        ASSERT_EQ(manager->inputListenersMap.size(), 2u) << "CaseDescription:"
            << " 6.c check map size is 2 (map.size() == 2)";
        // d. check vector size of address A is 2
        ASSERT_EQ(manager->inputListenersMap[&addressA].size(), 2u) << "CaseDescription:"
            << " 6.d check vector size of address A is 2 (vector.size() == 2)";
    }

    // 7. add listener[3] by nullptr
    listener[3] = manager->AddListener(nullptr);

    // 8. check listener[3]
    {
        // a. check listener[3] isn't nullptr
        ASSERT_NE(listener[3], nullptr) << "CaseDescription:"
            << " 8.a check listener[3] isn't nullptr (listener[3] != nullptr)";
        // b. check listener[3].GetWindow() is address A
        ASSERT_EQ(listener[3]->GetWindow(), nullptr) << "CaseDescription:"
            << " 8.b. check listener[3].GetWindow() is address A (listener[3].GetWindow() == nullptr)";
        // c. check map size is 2
        ASSERT_EQ(manager->inputListenersMap.size(), 2u) << "CaseDescription:"
            << " 8.c check map size is 2 (map.size() == 2)";
        // d. check vector size of nullptr is 2
        ASSERT_EQ(manager->inputListenersMap[nullptr].size(), 2u) << "CaseDescription:"
            << " 8.d check vector size of nullptr is 2 (vector.size() == 2)";
    }
}

/*
 * Feature: InputListenerManager RemoveListener
 * Function: InputListenerManager
 * SubFunction: RemoveListener
 * FunctionPoints: InputListenerManager RemoveListener
 * EnvConditions: WindowManager init success.
 * CaseDescription: 1. remove listener[0]
 *                  2. check map size is 2, check vector size of address A is 1
 *                  3. remove listener[2]
 *                  4. check map size is 1
 *                  5. remove listener[1], listener[3]
 *                  6. check map size is 0
 */
HWTEST_F(InputListenerManagerTest, RemoveListener, testing::ext::TestSize.Level0)
{
    // WindowManager init success.
    ASSERT_EQ(initRet, WM_OK) << "EnvConditions: WindowManager init success. (initRet == WM_OK)";

    auto manager = InputListenerManager::GetInstance();

    // 1. remove listener[0]
    ASSERT_NE(listener[0], nullptr) << "CaseDescription: 1. remove listener[0] (listener[0] != nullptr)";
    manager->RemoveListener(listener[0]);

    // 2. check map size is 2, check vector size of address A is 1
    ASSERT_EQ(manager->inputListenersMap.size(), 2u) << "CaseDescription:"
        << " 2. check map size is 2 (map.size() == 2)";
    ASSERT_EQ(manager->inputListenersMap[&addressA].size(), 1u) << "CaseDescription:"
        << " 2. check vector size of address A is 1 (vector.size() == 1)";

    // 3. remove listener[2]
    ASSERT_NE(listener[2], nullptr) << "CaseDescription: 3. remove listener[2] (listener[2] != nullptr)";
    manager->RemoveListener(listener[2]);

    // 4. check map size is 1
    ASSERT_EQ(manager->inputListenersMap.size(), 1u) << "CaseDescription:"
        << " 4. check map size is 1 (map.size() == 1)";

    // 5. remove listener[1], listener[3]
    ASSERT_NE(listener[1], nullptr) << "CaseDescription: 5. remove listener[1] (listener[1] != nullptr)";
    ASSERT_NE(listener[3], nullptr) << "CaseDescription: 5. remove listener[3] (listener[3] != nullptr)";
    manager->RemoveListener(listener[1]);
    manager->RemoveListener(listener[3]);

    // 6. check map size is 0
    ASSERT_EQ(manager->inputListenersMap.size(), 0u) << "CaseDescription:"
        << " 6. check map size is 0 (map.size() == 0)";
}
} // namespace
} // namespace OHOS
