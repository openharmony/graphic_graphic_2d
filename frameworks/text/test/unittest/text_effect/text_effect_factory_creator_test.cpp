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

#include "text_effect.h"
#include "text_effect_macro.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

class TextEffectFactoryCreatorTest : public testing::Test {
public:
    void SetUp() override {}
    void TearDown() override
    {
        TextEffectFactoryCreator::GetInstance().factoryTable_.erase(TextEffectStrategy::STRATEGY_BUTT);
    }
};

class TextTestEffect : public TextEffect {
public:
    TextTestEffect() = default;
    ~TextTestEffect() override = default;

    int UpdateEffectConfig(const std::unordered_map<TextEffectAttribute, std::string>& config) override { return 0; }
    int AppendTypography(const std::vector<TypographyConfig>& typographyConfigs) override { return 0; }
    void RemoveTypography(const std::vector<TypographyConfig>& typographyConfigs) override {}
    int UpdateTypography(std::vector<std::pair<TypographyConfig, TypographyConfig>>& typographyConfigs) override
    {
        return 0;
    }
    void StartEffect(Drawing::Canvas* canvas, double x, double y) override {}
    void StopEffect(Drawing::Canvas* canvas, double x, double y) override {}
};


/*
 * @tc.name: TextEffectFactoryCreatorTest001
 * @tc.desc: Test for RegisterFactory action
 * @tc.type: FUNC
 */
HWTEST_F(TextEffectFactoryCreatorTest, TextEffectFactoryCreatorTest001, TestSize.Level1)
{
    REGISTER_TEXT_EFFECT_FACTORY_IMPL(Test, TextEffectStrategy::STRATEGY_BUTT);
    TextEffectFactoryCreator& creator = TextEffectFactoryCreator::GetInstance();
    std::shared_ptr<TextEffectFactory> factory = creator.factoryTable_[TextEffectStrategy::STRATEGY_BUTT];
    bool result = creator.RegisterFactory(TextEffectStrategy::STRATEGY_BUTT, nullptr);
    EXPECT_FALSE(result);
    creator.factoryTable_.erase(TextEffectStrategy::STRATEGY_BUTT);
    result = creator.RegisterFactory(TextEffectStrategy::STRATEGY_BUTT, factory);
    EXPECT_TRUE(result);
    result = creator.RegisterFactory(TextEffectStrategy::STRATEGY_BUTT, factory);
    EXPECT_FALSE(result);
}

/*
 * @tc.name: TextEffectFactoryCreatorTest002
 * @tc.desc: Test for CreateTextEffect action
 * @tc.type: FUNC
 */
HWTEST_F(TextEffectFactoryCreatorTest, TextEffectFactoryCreatorTest002, TestSize.Level1)
{
    TextEffectFactoryCreator& creator = TextEffectFactoryCreator::GetInstance();
    std::shared_ptr<TextEffect> effect = creator.CreateTextEffect(TextEffectStrategy::STRATEGY_BUTT);
    EXPECT_EQ(effect, nullptr);
    REGISTER_TEXT_EFFECT_FACTORY_IMPL(Test, TextEffectStrategy::STRATEGY_BUTT);
    effect = creator.CreateTextEffect(TextEffectStrategy::STRATEGY_BUTT);
    EXPECT_NE(effect, nullptr);
    creator.UnregisterFactory(TextEffectStrategy::STRATEGY_BUTT);
    effect = creator.CreateTextEffect(TextEffectStrategy::STRATEGY_BUTT);
    EXPECT_EQ(effect, nullptr);
}