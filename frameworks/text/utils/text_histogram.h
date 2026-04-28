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

#ifndef TEXT_HISTOGRAM_H
#define TEXT_HISTOGRAM_H

#if defined(ENABLE_OHOS_ENHANCE) && defined(HISTOGRAM_MANAGEMENT_ENABLE)
#include "histogram_plugin_macros.h"

#define TEXT_HISTOGRAM_PREFIX "ArkGraphics2D.Text."
#define TEXT_HISTOGRAM_IS_SUCCESS ".is_success"

#define TEXT_HISTOGRAM_BOOLEAN(success)                                                \
    do {                                                                               \
        static const std::string _histName =                                           \
            std::string(TEXT_HISTOGRAM_PREFIX) + __func__ + TEXT_HISTOGRAM_IS_SUCCESS; \
        HISTOGRAM_BOOLEAN(_histName.c_str(), (success) ? 1 : 0);                       \
    } while (0)

#define TEXT_HISTOGRAM_BOOLEAN_NAME(name, success) \
    HISTOGRAM_BOOLEAN(TEXT_HISTOGRAM_PREFIX name TEXT_HISTOGRAM_IS_SUCCESS, (success) ? 1 : 0)

#else

#define TEXT_HISTOGRAM_BOOLEAN(success)
#define TEXT_HISTOGRAM_BOOLEAN_NAME(name, success)

#endif // defined(ENABLE_OHOS_ENHANCE) && defined(HISTOGRAM_MANAGEMENT_ENABLE)

#endif // TEXT_HISTOGRAM_H
