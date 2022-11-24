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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_COMMAND_PROXY_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_COMMAND_PROXY_H

#include <iremote_object.h>
#include <iremote_proxy.h>

#include "ipc/igraphic_dumper_command.h"

namespace OHOS {
class GraphicDumperCommandProxy : public IRemoteProxy<IGraphicDumperCommand> {
public:
    GraphicDumperCommandProxy(const sptr<IRemoteObject>& impl);
    virtual ~GraphicDumperCommandProxy() = default;

    GSError GetConfig(const std::string &k, std::string &v) override;
    GSError SetConfig(const std::string &k, const std::string &v) override;
    GSError Dump(const std::string &key) override;
    GSError GetLog(const std::string &tag, std::string &log) override;
    GSError AddInfoListener(const std::string &tag, sptr<IGraphicDumperInfoListener> &listener) override;

private:
    static inline BrokerDelegator<GraphicDumperCommandProxy> delegator_;
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_COMMAND_STUB_H
