/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CALLBACK_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CALLBACK_H

#include <functional>

namespace OHOS {
namespace Rosen {
class AnimationCallback {
public:
    explicit AnimationCallback(const std::function<void()>& callback);
    virtual ~AnimationCallback() = default;
    virtual void Run();

protected:
    std::function<void()> callback_;
};

class AnimationFinishCallback : public AnimationCallback {
public:
    AnimationFinishCallback(const std::function<void()>& callback, int sharedAnimCnt = 1);
    virtual ~AnimationFinishCallback() = default;
    void Run() override;

private:
    int sharedAnimCnt_;
    friend class RSAnimation;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_CALLBACK_H
