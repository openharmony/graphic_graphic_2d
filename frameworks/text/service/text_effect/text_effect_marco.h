/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef OHOS_ROSEN_TEXT_TEXT_EFFECT_MARCO_H
#define OHOS_ROSEN_TEXT_TEXT_EFFECT_MARCO_H

#include "text_effect.h"

namespace OHOS::Rosen {
#define REGISTER_TEXT_EFFECT_FACTORY_IMPL(EFFECT_TYPE, STRATEGY)                                    \
    class Text##EFFECT_TYPE##EffectFactory : public TextEffectFactory {                             \
    public:                                                                                         \
        std::shared_ptr<TextEffect> CreateTextEffect() override {                                   \
            return std::make_shared<Text##EFFECT_TYPE##Effect>();                                   \
        }                                                                                           \
    };                                                                                              \
    auto g_text##EFFECT_TYPE##Factory = std::make_shared<Text##EFFECT_TYPE##EffectFactory>();       \
    TextEffectFactoryCreator::GetInstance().RegisterFactory(STRATEGY, g_text##EFFECT_TYPE##Factory)

} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_TEXT_TEXT_EFFECT_MARCO_H