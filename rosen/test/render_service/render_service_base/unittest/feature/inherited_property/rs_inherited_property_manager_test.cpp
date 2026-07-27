/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "common/rs_common_def.h"
#include "feature/inherited_property/rs_inherited_property_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr InheritedPropertyType TEST_TYPE = static_cast<InheritedPropertyType>(1);
constexpr InheritedPropertyType TEST_TYPE_OTHER = static_cast<InheritedPropertyType>(2);
constexpr pid_t TEST_PID = 100;
constexpr pid_t TEST_PID_OTHER = 200;

class TestProperty : public IInheritedProperty {
public:
    explicit TestProperty(InheritedPropertyType type) : type_(type) {}
    ~TestProperty() override = default;

    InheritedPropertyType GetType() const override
    {
        return type_;
    }

private:
    InheritedPropertyType type_;
};
} // namespace

class RSInheritedPropertyManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: StoreAndGet
 * @tc.desc: Property stored by nodeId can be read back via Get and GetAs.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, StoreAndGet, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    auto property = std::make_shared<TestProperty>(TEST_TYPE);
    manager.Store(nodeId, property);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), property);
    EXPECT_EQ(manager.GetAs<TestProperty>(nodeId, TEST_TYPE), property);
}

/**
 * @tc.name: GetOnMissing
 * @tc.desc: Get on absent node or type returns nullptr, Clear on absent node is safe.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, GetOnMissing, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_EQ(manager.GetAs<TestProperty>(nodeId, TEST_TYPE), nullptr);
    manager.Clear(nodeId);
    manager.ClearByPid(TEST_PID);
}

/**
 * @tc.name: StoreNullOrNoneTypeIgnored
 * @tc.desc: Null property and InheritedPropertyType::NONE are not stored.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, StoreNullOrNoneTypeIgnored, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);

    manager.Store(nodeId, nullptr);
    manager.Store(nodeId, std::make_shared<TestProperty>(InheritedPropertyType::NONE));
    EXPECT_EQ(manager.Get(nodeId, InheritedPropertyType::NONE), nullptr);
}

/**
 * @tc.name: OverwriteSameTypeAndCoexistTypes
 * @tc.desc: Same node can hold multiple types; storing same type overwrites the old one.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, OverwriteSameTypeAndCoexistTypes, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    auto property = std::make_shared<TestProperty>(TEST_TYPE);
    auto propertyOther = std::make_shared<TestProperty>(TEST_TYPE_OTHER);
    auto propertyNew = std::make_shared<TestProperty>(TEST_TYPE);

    manager.Store(nodeId, property);
    manager.Store(nodeId, propertyOther);
    manager.Store(nodeId, propertyNew);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), propertyNew);
    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE_OTHER), propertyOther);
}

/**
 * @tc.name: GetAsTypeMismatch
 * @tc.desc: GetAs returns nullptr when the stored property type mismatches.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, GetAsTypeMismatch, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));

    EXPECT_EQ(manager.GetAs<TestProperty>(nodeId, TEST_TYPE_OTHER), nullptr);
}

/**
 * @tc.name: ClearByNodeId
 * @tc.desc: Clear removes all properties of the given node only.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, ClearByNodeId, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    NodeId nodeIdOther = MakeNodeId(TEST_PID, 2);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeIdOther, std::make_shared<TestProperty>(TEST_TYPE));

    manager.Clear(nodeId);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_NE(manager.Get(nodeIdOther, TEST_TYPE), nullptr);
}

/**
 * @tc.name: ClearByPid
 * @tc.desc: ClearByPid removes properties of all nodes of the given pid only.
 * @tc.type: FUNC
 */
HWTEST_F(RSInheritedPropertyManagerTest, ClearByPid, TestSize.Level1)
{
    RSInheritedPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    NodeId nodeIdSamePid = MakeNodeId(TEST_PID, 2);
    NodeId nodeIdOtherPid = MakeNodeId(TEST_PID_OTHER, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeIdSamePid, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeIdOtherPid, std::make_shared<TestProperty>(TEST_TYPE));

    manager.ClearByPid(TEST_PID);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_EQ(manager.Get(nodeIdSamePid, TEST_TYPE), nullptr);
    EXPECT_NE(manager.Get(nodeIdOtherPid, TEST_TYPE), nullptr);
}
} // namespace Rosen
} // namespace OHOS
