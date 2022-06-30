/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "mesh_unittest.h"
#include "mesh.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
/**
 * @tc.name: Use001
 * @tc.desc: Generate the data required for Mesh based on a fixed array
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MeshUnittest, Use001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MeshUnittest Use001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto mesh = std::make_shared<Mesh>();
    bool testResult = mesh != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call Use to use the mesh
     */
    EXPECT_TRUE(mesh->VAO_ >= 0);
    mesh->Use();
}

/**
 * @tc.name:Delete001
 * @tc.desc: Delete data that has been used and is no longer needed
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(MeshUnittest, Delete001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "MeshUnittestDelete001 start";
    /**
     * @tc.steps: step1. Create a Filter pointer
     */
    auto mesh = std::make_shared<Mesh>();
    bool testResult = mesh != nullptr;
    EXPECT_TRUE(testResult);
    /**
     * @tc.steps: step2. Call Delete to delete data
     */
    EXPECT_TRUE(mesh->VAO_ >= 0);
    mesh->Use();
    mesh->Delete();
}
} // namespace Rosen
} // namespace OHOS
