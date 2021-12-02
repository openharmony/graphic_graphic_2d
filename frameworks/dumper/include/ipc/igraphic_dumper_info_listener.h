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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_IGRAPHIC_DUMPER_INFO_LISTENER_H
#define FRAMEWORKS_DUMPRE_INCLUDE_IGRAPHIC_DUMPER_INFO_LISTENER_H

#include <string>

#include <iremote_broker.h>

namespace OHOS {
class IGraphicDumperInfoListener : public IRemoteBroker {
public:
    virtual void OnInfoComing(const std::string &info) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"IGraphicDumperInfoListener");

protected:
    enum {
        IGRAPHIC_DUMPER_INFO_LISTENER_ON_INFO_COMING,
    };
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_IGRAPHIC_DUMPER_INFO_LISTENER_H
