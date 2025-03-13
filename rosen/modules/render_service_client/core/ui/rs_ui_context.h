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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_H

#include <functional>
#include <memory>

#include "animation/rs_animation.h"
#include "animation/rs_implicit_animator.h"
#include "common/rs_common_def.h"
#include "modifier/rs_modifier_manager.h"
#include "pipeline/rs_node_map_v2.h"
#include "transaction/rs_transaction_handler.h"

namespace OHOS {
namespace Rosen {
using TaskRunner = std::function<void(const std::function<void()>&, uint32_t)>;

class RSC_EXPORT RSUIContext : public std::enable_shared_from_this<RSUIContext> {
public:
    virtual ~RSUIContext();

    inline RSNodeMapV2& GetMutableNodeMap()
    {
        return nodeMap_;
    }
    inline const RSNodeMapV2& GetNodeMap() const
    {
        return nodeMap_;
    }

    inline std::shared_ptr<RSTransactionHandler> GetRSTransaction() const
    {
        return rsTransactionHandler_;
    }
    inline std::shared_ptr<RSSyncTransactionHandler> GetSyncTransactionHandler() const
    {
        return rsSyncTransactionHandler_;
    }

    const std::shared_ptr<RSImplicitAnimator> GetRSImplicitAnimator();
    const std::shared_ptr<RSModifierManager> GetRSModifierManager();
    bool AnimationCallback(AnimationId animationId, AnimationCallbackEvent event);
    void AddAnimationInner(const std::shared_ptr<RSAnimation>& animation);
    void RemoveAnimationInner(const std::shared_ptr<RSAnimation>& animation);

    inline uint64_t GetToken() const
    {
        return token_;
    }

    void SetUITaskRunner(const TaskRunner& uiTaskRunner);
    void PostTask(const std::function<void()>& task);
    void PostDelayTask(const std::function<void()>& task, uint32_t delay);
    inline bool HasTaskRunner()
    {
        return bool(taskRunner_);
    }

private:
    RSUIContext();
    RSUIContext(uint64_t token);
    RSUIContext(const RSUIContext&) = delete;
    RSUIContext(const RSUIContext&&) = delete;
    RSUIContext& operator=(const RSUIContext&) = delete;
    RSUIContext& operator=(const RSUIContext&&) = delete;

    uint64_t token_;

    RSNodeMapV2 nodeMap_;
    std::shared_ptr<RSTransactionHandler> rsTransactionHandler_;
    std::shared_ptr<RSSyncTransactionHandler> rsSyncTransactionHandler_;
    std::shared_ptr<RSImplicitAnimator> rsImplicitAnimator_;
    std::shared_ptr<RSModifierManager> rsModifierManager_;

    std::unordered_map<AnimationId, std::shared_ptr<RSAnimation>> animations_;
    std::unordered_map<PropertyId, uint32_t> animatingPropertyNum_;
    std::recursive_mutex animationMutex_;

    TaskRunner taskRunner_ = TaskRunner();

    friend class RSUIContextManager;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_UI_CONTEXT_H