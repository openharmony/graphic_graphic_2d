/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HGM_ANIM_DYNAMIC_CONFIGS_H
#define HGM_ANIM_DYNAMIC_CONFIGS_H

#include <mutex>
#include <string>
#include <vector>

namespace OHOS::Rosen {
class AnimDynamicAttribute {
public:
    AnimDynamicAttribute() : animType_("default"), animName_("0"), minSpeed_(0),
        maxSpeed_(0), preferredFps_(0) {}

    AnimDynamicAttribute(const std::string& animType, const std::string& animName, int32_t minSpeed, int32_t maxSpeed,
        int32_t preferredFps) : animType_(animType), animName_(animName), minSpeed_(minSpeed),
        maxSpeed_(maxSpeed), preferredFps_(preferredFps) {}

    std::string GetAnimType() const
    {
        return animType_;
    }

    std::string GetAnimName() const
    {
        return animName_;
    }

    int32_t GetMinSpeed() const
    {
        return minSpeed_;
    }

    int32_t GetMaxSpeed() const
    {
        return maxSpeed_;
    }

    int32_t GetPreferredFps() const
    {
        return preferredFps_;
    }

    void SetAnimType(const std::string& animType)
    {
        animType_ = animType;
    }

    void SetAnimName(const std::string& animName)
    {
        animName_ = animName;
    }

    void SetMinSpeed(int32_t maxSpeed)
    {
        maxSpeed_ = maxSpeed;
    }

    void SetMaxSpeed(int32_t minSpeed)
    {
        minSpeed_ = minSpeed;
    }

    void SetPreferredFps(int32_t preferredFps)
    {
        preferredFps_ = preferredFps;
    }

private:
    std::string animType_ = "default";
    std::string animName_ = "0";
    int32_t minSpeed_ = 0;
    int32_t maxSpeed_ = 0;
    int32_t preferredFps_ = 0;
};

class AnimDynamicConfigs {
public:
    static AnimDynamicConfigs* GetInstance();

    std::vector<AnimDynamicAttribute> GetAnimDynamicConfigs() const;
    void AddAnimDynamicAttribute(AnimDynamicAttribute attr);
    int GetAnimDynamicAttributesCount() const;

private:
    AnimDynamicConfigs();
    ~AnimDynamicConfigs();
    std::vector<AnimDynamicAttribute> animDynamicCfgs_;
};
} // namespace OHOS::Rosen
#endif // HGM_ANIM_DYNAMIC_CONFIGS_H