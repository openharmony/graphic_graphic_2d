/**
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
#include <memory>
#include "rs_render_layer_cmd_property.h"
#include "message_parcel.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;
class RSLayerCmdPropertyTest : public Test {};

/*
 * Test: SetGet_And_Clone_Basic
 * Purpose: Validate Set/Get and Clone keep the same value and are independent.
 */
HWTEST(RSLayerCmdPropertyTest, SetGet_Basic, TestSize.Level1)
{
    RSRenderLayerCmdProperty<int> prop(10);
    EXPECT_EQ(prop.Get(), 10);
    prop.Set(20);
    EXPECT_EQ(prop.Get(), 20);
}

/*
 * Test: Bridge_To_RSRenderLayerProperty_Marshalling
 * Purpose: Verify GetRSRenderLayerProperty() returns a marshallable property with same value.
 */
HWTEST(RSLayerCmdPropertyTest, RSRenderLayerProperty_Marshalling, TestSize.Level1)
{
    GraphicIRect rect {1, 2, 3, 4};
    RSRenderLayerCmdProperty<GraphicIRect> rsRenderProp(rect);

    MessageParcel parcel;
    ASSERT_TRUE(rsRenderProp.OnMarshalling(parcel, rsRenderProp.Get()));

    std::shared_ptr<RSRenderLayerCmdProperty<GraphicIRect>> out;
    ASSERT_TRUE(rsRenderProp.OnUnmarshalling(parcel, out));
    ASSERT_NE(out, nullptr);
    EXPECT_EQ(out->Get().x, 1);
    EXPECT_EQ(out->Get().y, 2);
    EXPECT_EQ(out->Get().w, 3);
    EXPECT_EQ(out->Get().h, 4);
}
