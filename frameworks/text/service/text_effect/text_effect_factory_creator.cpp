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

#include "text_effect.h"

namespace OHOS::Rosen {

TextEffectFactoryCreator& TextEffectFactoryCreator::GetInstance()
{
    static TextEffectFactoryCreator instance;
    return instance;
}

bool TextEffectFactoryCreator::RegisterFactory(TextEffectStrategy strategy,
    std::shared_ptr<TextEffectFactory> factory) 
{
    if (factory == nullptr) {
        return false;
    }
    std::unique_lock<std::shared_mutex> lock(mutex_);
    if (factoryTable_.find(strategy) != factoryTable_.end()) {
       return false;
    }
    factoryTable_.emplace(strategy, factory);
    return true;
}

std::shared_ptr<TextEffect> TextEffectFactoryCreator::CreateTextEffect(TextEffectStrategy strategy)
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto iter = factoryTable_.find(strategy);
    if (iter == factoryTable_.end()) {
        return nullptr;
    }
    return iter->second->CreateTextEffect();
}
} // namespace OHOS::Rosen