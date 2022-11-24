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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_H

#include "graphic_common.h"

#ifdef __cplusplus
#include <refbase.h>

namespace OHOS {
using OnConfigChangeFunc = std::function<void(const std::string &, const std::string &)>;
using OnDumpFunc = std::function<void()>;
class GraphicDumperHelper : public RefBase {
public:
    static sptr<GraphicDumperHelper> GetInstance();
    virtual GSError SendInfo(const std::string &tag, const char *fmt, ...) = 0;

    virtual int32_t AddConfigChangeListener(const std::string &tag, OnConfigChangeFunc func) = 0;
    virtual GSError RemoveConfigChangeListener(const int32_t listenerId) = 0;
    virtual int32_t AddDumpListener(const std::string &tag, OnDumpFunc func) = 0;
    virtual GSError RemoveDumpListener(const int32_t listenerId) = 0;
};
} // namespace OHOS

extern "C" {
#endif

typedef void(*OnDumpFuncPtr)(void);
typedef void(*OnConfigChangeFuncPtr)(const char *, const char *);

int SendInfo(const char* tag, const char *fmt, ...);
int AddConfigChangeListener(const char* tag, OnConfigChangeFuncPtr func);
int RemoveConfigChangeListener(int listenerId);
int AddDumpListener(const char* tag, OnDumpFuncPtr func);
int RemoveDumpListener(int listenerId);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_HELPER_H
