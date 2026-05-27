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

#ifndef RS_EVENT_DEF_H
#define RS_EVENT_DEF_H

#include <parcel.h>
#include <memory>
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

/*
 * @brief Enumerates events.
 * @note If you need to add new event, please coordinate with the downstream
 */
enum HwcEvent {
    PREVALIDATE_LOW_TEMP = 0,
    PREVALIDATE_DFR_MODE,       // Screen refresh mode
    PREVALIDATE_DSI_MODE,       // Screen interrupt mode
    PREVALIDATE_DSTCOLOR_MODE,  // Screen color space mode
    HWCEVENT_TUI_ENTER,         // enter tui layer
    HWCEVENT_TUI_EXIT,          // exit tui layer
    HWCEVENT_EXT_SCREEN_NOT_SUPPORT = 7, // external screen not support
    HWCEVENT_CALLBACK_MAX,
};

/*
 * @brief Enumerates events that need to be exposed to the upstream.
 */
enum class RSExposedEventType : uint32_t {
    EXT_SCREEN_UNSUPPORT = 0,
    EXPOSED_EVENT_INVALID,
};

/*
 * @brief Base data structure to the exposed event.
 */
struct RSB_EXPORT RSExposedEventDataBase : public Parcelable {
    RSExposedEventDataBase() : type_(RSExposedEventType::EXPOSED_EVENT_INVALID) {}
    RSExposedEventDataBase(RSExposedEventDataBase&& other) = default;
    ~RSExposedEventDataBase() = default;

    static RSExposedEventDataBase* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    RSExposedEventType type_;
};

/*
 * @brief Unified callback functor to the exposed event
 */
using RSExposedEventCallback = std::function<void(const std::shared_ptr<RSExposedEventDataBase>&)>;

/*
 * @brief External screen not-support event.
 */
struct RSExtScreenUnsupportData : RSExposedEventDataBase {
    RSExtScreenUnsupportData() {type_ = RSExposedEventType::EXT_SCREEN_UNSUPPORT;}
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_EVENT_DEF_H