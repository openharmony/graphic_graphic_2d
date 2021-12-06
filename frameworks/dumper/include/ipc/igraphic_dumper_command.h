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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_IGRAPHIC_DUMPER_COMMAND_H
#define FRAMEWORKS_DUMPRE_INCLUDE_IGRAPHIC_DUMPER_COMMAND_H

#include <string>

#include <iremote_broker.h>

#include "graphic_common.h"
#include "ipc/igraphic_dumper_info_listener.h"

namespace OHOS {
class IGraphicDumperCommand : public IRemoteBroker {
public:
    virtual GSError GetConfig(const std::string &k, std::string &v) = 0;
    virtual GSError SetConfig(const std::string &k, const std::string &v) = 0;
    virtual GSError Dump(const std::string &tag) = 0;
    virtual GSError GetLog(const std::string &tag, std::string &log) = 0;
    virtual GSError AddInfoListener(const std::string &tag, sptr<IGraphicDumperInfoListener> &listener) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"IGraphicDumperCommand");

protected:
    enum {
        IGRAPHIC_DUMPER_COMMAND_GET_CONFIG,
        IGRAPHIC_DUMPER_COMMAND_SET_CONFIG,
        IGRAPHIC_DUMPER_COMMAND_DUMP,
        IGRAPHIC_DUMPER_COMMAND_GET_LOG,
        IGRAPHIC_DUMPER_COMMAND_ADD_INFO_LISTENER,
    };
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_IGRAPHIC_DUMPER_COMMAND_H
