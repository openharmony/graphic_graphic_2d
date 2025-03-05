/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_GRAPHIC_GRAPHIC_2D_ROSEN_MODULES_PLATFORM_EVENT_HANDLER_EVENT_HANDLER_MACROS_H
#define FOUNDATION_GRAPHIC_GRAPHIC_2D_ROSEN_MODULES_PLATFORM_EVENT_HANDLER_EVENT_HANDLER_MACROS_H

#ifdef _WIN32
    #ifdef IMPL_EVENTHANDELER_BUILD
        #define IMPL_EVENTHANDELER_EXPORT __declspec(dllexport)
    #else
        #define IMPL_EVENTHANDELER_EXPORT __declspec(dllimport)
    #endif
#else
    #define IMPL_EVENTHANDELER_EXPORT __attribute__((visibility("default")))
#endif

#endif //FOUNDATION_GRAPHIC_GRAPHIC_2D_ROSEN_MODULES_PLATFORM_EVENT_HANDLER_EVENT_HANDLER_MACROS_H
