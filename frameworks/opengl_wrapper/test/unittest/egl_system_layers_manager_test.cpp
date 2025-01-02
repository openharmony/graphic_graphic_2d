/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "EGL/egl_system_layers_manager.h"

#include "egl_defs.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class EglSystemLayersManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: getJsonConfigtTest001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglSystemLayersManagerTest, getJsonConfigtTest001, Level1)
{
    EglSystemLayersManager manager;
    Json::Value val{Json::nullValue};
    EXPECT_TRUE(manager.GetJsonConfig(val));
}

/**
 * @tc.name: getSystemLayersFromConfigTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglSystemLayersManagerTest, getSystemLayersFromConfigTest, Level1)
{
    EglSystemLayersManager manager;
    Json::Value val{Json::nullValue};
    std::vector<std::string> config = manager.GetSystemLayersFromConfig(val, "testName");
    EXPECT_TRUE(config.empty());
}

/**
 * @tc.name: getSystemLayersTest
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(EglSystemLayersManagerTest, getSystemLayersTest, Level1)
{
    EglSystemLayersManager manager;
    manager.GetSystemLayers();
    Json::Value val{true};
    EXPECT_TRUE(manager.GetStringVectorFromJson(val).empty());
}
} // OHOS::Rosen