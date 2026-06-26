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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_H

#include "transaction/rs_transaction_data.h"
#include "transaction/rs_buffer_transaction.h"

namespace OHOS::Rosen {
class RSModifiersDraw {
public:
    RSModifiersDraw() = default;
    ~RSModifiersDraw() = default;
    RSModifiersDraw(const RSModifiersDraw&) = delete;
    RSModifiersDraw(const RSModifiersDraw&&) = delete;
    RSModifiersDraw& operator=(const RSModifiersDraw&) = delete;
    RSModifiersDraw& operator=(const RSModifiersDraw&&) = delete;

    void ConvertTransaction(std::unique_ptr<RSTransactionData>& transactionData,
        std::vector<RSTransactionConfig>& transactionConfigList);
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_MODIFIERS_DRAW_H