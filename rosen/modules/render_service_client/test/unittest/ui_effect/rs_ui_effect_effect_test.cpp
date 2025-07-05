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

#include <gtest/gtest.h>

#include "effect/include/visual_effect.h"
#include "effect/include/visual_effect_para.h"
#include "effect/include/visual_effect_unmarshalling_singleton.h"

namespace OHOS {
namespace Rosen {

using namespace testing;
using namespace testing::ext;

class RSUIEffecEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIEffecEffectTest::SetUpTestCase() {}
void RSUIEffecEffectTest::TearDownTestCase() {}
void RSUIEffecEffectTest::SetUp() {}
void RSUIEffecEffectTest::TearDown() {}

/**
 * @tc.name: RSUIEffectVisualEffectTest
 * @tc.desc: Verify the VisualEffect func
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUIEffecEffectTest, RSUIEffectVisualEffectTest, TestSize.Level1)
{
    auto visualEffectPara = std::make_shared<VisualEffectPara>();

    Parcel parcel;
    EXPECT_EQ(false, visualEffectPara->Marshalling(parcel));
    uint16_t type = 0;
    auto unmarshallingFunc = [](Parcel& parcel, std::shared_ptr<VisualEffectPara>& val) -> bool { return true; };

    EXPECT_EQ(false, visualEffectPara->RegisterUnmarshallingCallback(type, unmarshallingFunc));
    EXPECT_EQ(true, visualEffectPara->RegisterUnmarshallingCallback(666, unmarshallingFunc));
    std::shared_ptr<VisualEffectPara> val = nullptr;
    EXPECT_EQ(false, VisualEffectPara::Unmarshalling(parcel, val));
    EXPECT_EQ(nullptr, val);
    EXPECT_EQ(nullptr, visualEffectPara->Clone());

    auto visualEffect = std::make_shared<VisualEffect>();
    visualEffect->AddPara(visualEffectPara);
    Parcel parcel2;
    EXPECT_EQ(true, visualEffect->Marshalling(parcel2));
    std::shared_ptr<VisualEffect> valVisualEffect = nullptr;
    EXPECT_EQ(false, VisualEffect::Unmarshalling(parcel2, valVisualEffect));
    EXPECT_EQ(nullptr, valVisualEffect);
    auto visualEffectTemp = std::make_shared<VisualEffect>(*visualEffect.get());
    EXPECT_NE(nullptr, visualEffectTemp);
}

/**
 * @tc.name: RSUIEffectVisualEffectUnmarshallingSingletonTest
 * @tc.desc: Verify the VisualEffectUnmarshallingSingleton func
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(RSUIEffecEffectTest, RSUIEffectVisualEffectUnmarshallingSingletonTest, TestSize.Level1)
{
    uint16_t type = 666;
    auto unmarshallingFunc = [](Parcel& parcel, std::shared_ptr<VisualEffectPara>& val) -> bool { return true; };
    VisualEffectUnmarshallingSingleton::GetInstance().RegisterCallback(type, unmarshallingFunc);
    auto retFunc = VisualEffectUnmarshallingSingleton::GetInstance().GetCallback(type);
    EXPECT_NE(nullptr, retFunc);
    VisualEffectUnmarshallingSingleton::GetInstance().UnregisterCallback(type);
    retFunc = VisualEffectUnmarshallingSingleton::GetInstance().GetCallback(type);
    EXPECT_EQ(nullptr, retFunc);
}

} // namespace Rosen
} // namespace OHOS