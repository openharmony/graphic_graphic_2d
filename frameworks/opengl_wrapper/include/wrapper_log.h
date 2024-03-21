/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef FRAMEWORKS_OPENGL_WRAPPER_WRAPPER_LOG_H
#define FRAMEWORKS_OPENGL_WRAPPER_WRAPPER_LOG_H

#include <hilog/log.h>
namespace OHOS {
#if (defined(__aarch64__) || defined(__x86_64__))
#define WPUBI64  "%{public}ld"
#define WPUB_SIZE "%{public}lu"
#define WPUBU64  "%{public}lu"
#else
#define WPUBI64  "%{public}lld"
#define WPUB_SIZE "%{public}u"
#define WPUBU64  "%{public}llu"
#endif

#undef LOG_DOMAIN
#define LOG_DOMAIN 1400

#undef LOG_TAG
#define LOG_TAG "OpenGLWrapper"

#ifdef EGL_WRAPPER_DEBUG_ENABLE
#define WLOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, fmt, ##__VA_ARGS__)
#else
#define WLOGD(fmt, ...)
#endif

#define WLOGI(fmt, ...) HILOG_INFO(LOG_CORE, fmt, ##__VA_ARGS__)
#define WLOGW(fmt, ...) HILOG_WARN(LOG_CORE, fmt, ##__VA_ARGS__)
#define WLOGE(fmt, ...) HILOG_ERROR(LOG_CORE, fmt, ##__VA_ARGS__)
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_WRAPPER_LOG_H
