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

#include "gtest/gtest.h"


#include <parameter.h>
#include <parameters.h>

#include "effect/particle_builder.h"
#include "effect/particle_effect.h"
#include "image/image.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ParticleEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ParticleEffectTest::SetUpTestCase() {}
void ParticleEffectTest::TearDownTestCase() {}
void ParticleEffectTest::SetUp() {}
void ParticleEffectTest::TearDown() {}

/**
 * @tc.name: CreateParticleBuilder001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleBuilder001, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    EXPECT_TRUE(builder != nullptr);
}

/**
 * @tc.name: CreateParticleEffect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect001, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 100;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect != nullptr);
}

/**
 * @tc.name: CreateParticleEffect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect002, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 100;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect != nullptr);
}

/**
 * @tc.name: CreateParticleEffect003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect003, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    uint32_t particleCount = 100;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect == nullptr);
}

/**
 * @tc.name: CreateParticleEffect004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect004, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    Drawing::Image image;
    Drawing::SamplingOptions option(Drawing::FilterMode::LINEAR);
    builder->AddConfigImage("particleTex", image, option);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 100;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect != nullptr);
    effect->UpdateConfigImage("particleTex", std::make_shared<Drawing::Image>(image));
    float deltaT = 0;
    effect->UpdateConfigData("deltaT", &deltaT, 0, sizeof(float));
    EXPECT_TRUE(effect != nullptr);
}

/**
 * @tc.name: CreateParticleEffect005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect005, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 0;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect == nullptr);
}

/**
 * @tc.name: CreateParticleEffect006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect006, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 1;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect != nullptr);
}

/**
 * @tc.name: CreateParticleEffect007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect007, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 100000;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect != nullptr);
}

/**
 * @tc.name: CreateParticleEffect008
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect008, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->AddConfigData("deltaT", "float deltaT;", sizeof(float));
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 100001;
    auto effect = builder->MakeParticleEffect(particleCount);
    EXPECT_TRUE(effect == nullptr);
}

/**
 * @tc.name: CreateParticleEffect009
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR20260211385262
 * @tc.author:
 */
HWTEST_F(ParticleEffectTest, CreateParticleEffect009, TestSize.Level1)
{
    auto builder = std::make_shared<Drawing::ParticleBuilder>();
    ASSERT_TRUE(builder != nullptr);
    builder->SetUpdateCode("void main{}");
    uint32_t particleCount = 100;

    auto effect1 = builder->MakeParticleEffect(particleCount);
    ASSERT_TRUE(effect1 != nullptr);

    auto serializedData = effect1->Serialize();
    ASSERT_TRUE(serializedData != nullptr);
    EXPECT_GT(serializedData->GetSize(), 0);

    auto effect2 = std::make_shared<Drawing::ParticleEffect>();
    ASSERT_TRUE(effect2 != nullptr);
    bool result = effect2->Deserialize(serializedData);
    EXPECT_TRUE(result);

    auto data1 = effect1->Serialize();
    auto data2 = effect2->Serialize();
    ASSERT_TRUE(data1 != nullptr);
    ASSERT_TRUE(data2 != nullptr);
    EXPECT_EQ(data1->GetSize(), data2->GetSize());
    if (data1->GetSize() == data2->GetSize()) {
        const uint8_t* ptr1 = static_cast<const uint8_t*>(data1->GetData());
        const uint8_t* ptr2 = static_cast<const uint8_t*>(data2->GetData());
        EXPECT_EQ(memcmp(ptr1, ptr2, data1->GetSize()), 0);
    }
}
}
}
}
