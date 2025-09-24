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
#include "rosen_text/font_collection_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class FontCollectionMgrTest : public testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override
    {
        mgr.RemoveSharedFontColleciton(key1);
        mgr.RemoveSharedFontColleciton(key2);
        mgr.DestroyLocalInstance(envId1);
        mgr.DestroyLocalInstance(envId2);
    }

    FontCollectionMgr& mgr = FontCollectionMgr::GetInstance();
    void* key1 = reinterpret_cast<void*>(1);
    void* key2 = reinterpret_cast<void*>(2);
    uint64_t envId1{1001};
    uint64_t envId2{1002};
    std::shared_ptr<FontCollection> fc1 = FontCollection::From(nullptr);
    std::shared_ptr<FontCollection> fc2 = FontCollection::From(nullptr);
};

/*
 * @tc.name: FontCollectionMgrTest001
 * @tc.desc: test for insert and find shared font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest001, TestSize.Level0) {
    mgr.InsertSharedFontColleciton(key1, fc1);
    auto found = mgr.FindSharedFontColleciton(key1);
    EXPECT_EQ(found, fc1);
}

/*
 * @tc.name: FontCollectionMgrTest002
 * @tc.desc: test for find no exist shared font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest002, TestSize.Level0) {
    auto found = mgr.FindSharedFontColleciton(key1);
    EXPECT_EQ(found, nullptr);
}

/*
 * @tc.name: FontCollectionMgrTest003
 * @tc.desc: test for remove shared font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest003, TestSize.Level0) {
    mgr.InsertSharedFontColleciton(key1, fc1);
    bool removed = mgr.RemoveSharedFontColleciton(key1);
    EXPECT_TRUE(removed);
    auto found = mgr.FindSharedFontColleciton(key1);
    EXPECT_EQ(found, nullptr);
}

/*
 * @tc.name: FontCollectionMgrTest004
 * @tc.desc: test for remove no exist shared font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest004, TestSize.Level0) {
    bool removed = mgr.RemoveSharedFontColleciton(key1);
    EXPECT_FALSE(removed);
}

/*
 * @tc.name: FontCollectionMgrTest005
 * @tc.desc: test for insert and get local font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest005, TestSize.Level0) {
    mgr.InsertLocalInstance(envId1, fc1);
    auto found = mgr.GetLocalInstance(envId1);
    EXPECT_EQ(found, fc1);
}

/*
 * @tc.name: FontCollectionMgrTest006
 * @tc.desc: test for get no exist local font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest006, TestSize.Level0) {
    auto found = mgr.GetLocalInstance(envId1);
    EXPECT_EQ(found, nullptr);
}

/*
 * @tc.name: FontCollectionMgrTest007
 * @tc.desc: test for destory local font collection
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest007, TestSize.Level0) {
    mgr.InsertLocalInstance(envId1, fc1);
    mgr.DestroyLocalInstance(envId1);
    auto found = mgr.GetLocalInstance(envId1);
    EXPECT_EQ(found, nullptr);
}

/*
 * @tc.name: FontCollectionMgrTest008
 * @tc.desc: test for check font collection valid
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest008, TestSize.Level0) {
    mgr.InsertLocalInstance(envId1, fc1);
    bool isValid = mgr.CheckInstanceIsValid(envId1, fc1);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: FontCollectionMgrTest009
 * @tc.desc: test for check font collection invalid
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest009, TestSize.Level0) {
    mgr.InsertLocalInstance(envId1, fc1);
    bool isValid = mgr.CheckInstanceIsValid(envId1, fc2);
    EXPECT_FALSE(isValid);
}

/*
 * @tc.name: FontCollectionMgrTest0010
 * @tc.desc: test for check font collection when envId not exist
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest0010, TestSize.Level0) {
    bool isValid = mgr.CheckInstanceIsValid(envId1, fc1);
    EXPECT_TRUE(isValid);
}

/*
 * @tc.name: FontCollectionMgrTest0011
 * @tc.desc: test for get envId by font collection exist
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest0011, TestSize.Level0) {
    mgr.InsertLocalInstance(envId1, fc1);
    uint64_t envId = mgr.GetEnvByFontCollection(fc1.get());
    EXPECT_EQ(envId, envId1);
}

/*
 * @tc.name: FontCollectionMgrTest0012
 * @tc.desc: test for get envId by font collection not exist
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest0012, TestSize.Level0) {
    uint64_t envId = mgr.GetEnvByFontCollection(fc1.get());
    EXPECT_EQ(envId, 0);
}

/*
 * @tc.name: FontCollectionMgrTest0013
 * @tc.desc: test for get envId by font collection when multi env
 * @tc.type: FUNC
 */
HWTEST_F(FontCollectionMgrTest, FontCollectionMgrTest0013, TestSize.Level0) {
    mgr.InsertLocalInstance(envId1, fc1);
    mgr.InsertLocalInstance(envId2, fc2);
    uint64_t envId = mgr.GetEnvByFontCollection(fc2.get());
    EXPECT_EQ(envId, envId2);
}
} // namespace OHOS::Rosen