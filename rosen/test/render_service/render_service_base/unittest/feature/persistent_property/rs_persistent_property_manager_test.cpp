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
#include "feature/persistent_property/rs_persistent_property_manager.h"
#include "pipeline/rs_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr PersistentPropertyType TEST_TYPE = static_cast<PersistentPropertyType>(1);
constexpr PersistentPropertyType TEST_TYPE_OTHER = static_cast<PersistentPropertyType>(2);
constexpr pid_t TEST_PID = 100;
constexpr pid_t TEST_PID_OTHER = 200;

class TestProperty : public IPersistentProperty {
public:
    explicit TestProperty(PersistentPropertyType type) : type_(type) {}
    ~TestProperty() override = default;

    PersistentPropertyType GetType() const override
    {
        return type_;
    }

    void SetType(PersistentPropertyType type)
    {
        type_ = type;
    }

private:
    PersistentPropertyType type_;
};
} // namespace

class RSPersistentPropertyManagerTest : public testing::Test {
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
HWTEST_F(RSPersistentPropertyManagerTest, StoreAndGet, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
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
HWTEST_F(RSPersistentPropertyManagerTest, GetOnMissing, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_EQ(manager.GetAs<TestProperty>(nodeId, TEST_TYPE), nullptr);
    manager.Clear(nodeId);
    manager.ClearByPid(TEST_PID);
}

/**
 * @tc.name: GetExistingNodeMissingType
 * @tc.desc: Get returns nullptr when the node exists but the requested type is absent.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, GetExistingNodeMissingType, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE_OTHER), nullptr);
    EXPECT_NE(manager.Get(nodeId, TEST_TYPE), nullptr);
}

/**
 * @tc.name: StoreNullOrNoneTypeIgnored
 * @tc.desc: Null property and PersistentPropertyType::NONE are not stored.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, StoreNullOrNoneTypeIgnored, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);

    manager.Store(nodeId, nullptr);
    manager.Store(nodeId, std::make_shared<TestProperty>(PersistentPropertyType::NONE));
    EXPECT_EQ(manager.Get(nodeId, PersistentPropertyType::NONE), nullptr);
}

/**
 * @tc.name: OverwriteSameTypeAndCoexistTypes
 * @tc.desc: Same node can hold multiple types; storing same type overwrites the old one.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, OverwriteSameTypeAndCoexistTypes, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
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
 * @tc.desc: GetAs returns nullptr when the requested type has no property stored.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, GetAsTypeMismatch, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));

    EXPECT_EQ(manager.GetAs<TestProperty>(nodeId, TEST_TYPE_OTHER), nullptr);
}

/**
 * @tc.name: GetAsRuntimeTypeMismatch
 * @tc.desc: GetAs returns nullptr when the stored property's runtime GetType()
 *           no longer matches the map key it was stored under.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, GetAsRuntimeTypeMismatch, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    auto property = std::make_shared<TestProperty>(TEST_TYPE);
    manager.Store(nodeId, property);

    property->SetType(TEST_TYPE_OTHER);

    // The entry is still keyed by the original type, but GetAs rejects the downcast.
    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), property);
    EXPECT_EQ(manager.GetAs<TestProperty>(nodeId, TEST_TYPE), nullptr);
}

/**
 * @tc.name: ClearByNodeId
 * @tc.desc: Clear removes all properties of the given node only.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearByNodeId, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    NodeId nodeIdOther = MakeNodeId(TEST_PID, 2);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeIdOther, std::make_shared<TestProperty>(TEST_TYPE));

    manager.Clear(nodeId);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_NE(manager.Get(nodeIdOther, TEST_TYPE), nullptr);
}

/**
 * @tc.name: ClearRemovesAllTypesOfNode
 * @tc.desc: Clear removes every property type of the given node.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearRemovesAllTypesOfNode, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE_OTHER));

    manager.Clear(nodeId);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE_OTHER), nullptr);
}

/**
 * @tc.name: ClearByNodeIdAndType
 * @tc.desc: Clear(nodeId, type) removes only the given type of the given node;
 *           other types of the same node and other nodes are kept.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearByNodeIdAndType, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    NodeId nodeIdOther = MakeNodeId(TEST_PID, 2);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE_OTHER));
    manager.Store(nodeIdOther, std::make_shared<TestProperty>(TEST_TYPE));

    manager.Clear(nodeId, TEST_TYPE);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_NE(manager.Get(nodeId, TEST_TYPE_OTHER), nullptr);
    EXPECT_NE(manager.Get(nodeIdOther, TEST_TYPE), nullptr);
}

/**
 * @tc.name: ClearLastTypeRemovesNode
 * @tc.desc: Clear(nodeId, type) on the last remaining type removes the node entirely.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearLastTypeRemovesNode, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));

    manager.Clear(nodeId, TEST_TYPE);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
}

/**
 * @tc.name: ClearByNodeIdAndTypeOnMissing
 * @tc.desc: Clear(nodeId, type) on an absent node or absent type is safe and
 *           keeps the remaining types untouched.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearByNodeIdAndTypeOnMissing, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));

    manager.Clear(MakeNodeId(TEST_PID_OTHER, 1), TEST_TYPE);
    manager.Clear(nodeId, TEST_TYPE_OTHER);

    EXPECT_NE(manager.Get(nodeId, TEST_TYPE), nullptr);
}

/**
 * @tc.name: ClearByPid
 * @tc.desc: ClearByPid removes properties of all nodes of the given pid only.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearByPid, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
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

/**
 * @tc.name: ClearByPidMultipleTypes
 * @tc.desc: ClearByPid removes every property type of every node belonging to the pid.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ClearByPidMultipleTypes, TestSize.Level1)
{
    RSPersistentPropertyManager manager;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    NodeId nodeIdOtherPid = MakeNodeId(TEST_PID_OTHER, 1);
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeId, std::make_shared<TestProperty>(TEST_TYPE_OTHER));
    manager.Store(nodeIdOtherPid, std::make_shared<TestProperty>(TEST_TYPE));
    manager.Store(nodeIdOtherPid, std::make_shared<TestProperty>(TEST_TYPE_OTHER));

    manager.ClearByPid(TEST_PID);

    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE), nullptr);
    EXPECT_EQ(manager.Get(nodeId, TEST_TYPE_OTHER), nullptr);
    EXPECT_NE(manager.Get(nodeIdOtherPid, TEST_TYPE), nullptr);
    EXPECT_NE(manager.Get(nodeIdOtherPid, TEST_TYPE_OTHER), nullptr);
}

/**
 * @tc.name: ContextAccessors
 * @tc.desc: RSContext exposes the manager through mutable and const accessors.
 * @tc.type: FUNC
 */
HWTEST_F(RSPersistentPropertyManagerTest, ContextAccessors, TestSize.Level1)
{
    RSContext context;
    NodeId nodeId = MakeNodeId(TEST_PID, 1);
    auto property = std::make_shared<TestProperty>(TEST_TYPE);

    context.GetMutablePersistentPropertyManager().Store(nodeId, property);

    const RSContext& constContext = context;
    EXPECT_EQ(constContext.GetPersistentPropertyManager().Get(nodeId, TEST_TYPE), property);
    context.GetMutablePersistentPropertyManager().ClearByPid(TEST_PID);
    EXPECT_EQ(constContext.GetPersistentPropertyManager().Get(nodeId, TEST_TYPE), nullptr);
}
} // namespace Rosen
} // namespace OHOS
