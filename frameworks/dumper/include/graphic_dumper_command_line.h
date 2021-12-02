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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_COMMAND_LINE_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_COMMAND_LINE_H

#include <promise.h>
#include <string>

#include "ipc/graphic_dumper_info_listener_stub.h"
#include "ipc/igraphic_dumper_command.h"

namespace OHOS {
using DumperArgs = struct {
    bool wait = false;
    std::string dumpTag = {};
    std::string logTag = {};
    std::string getCfgKey = {};
    std::string setCfgKey = {};
    std::string setCfgValue = {};
};

class GraphicDumperCommandLine : public GraphicDumperInfoListenerStub {
public:
    static sptr<GraphicDumperCommandLine> GetInstance();
    virtual void OnInfoComing(const std::string &info) override;
    GSError Main(int32_t argc, char *argv[]);

private:
    GraphicDumperCommandLine() = default;
    ~GraphicDumperCommandLine() = default;
    static inline sptr<GraphicDumperCommandLine> instance_;

    void HandlerOfArgs();
    GSError OptionParse(const char option);
    GSError Parse(int32_t argc, char *argv[]);
    GSError InitSA(int32_t systemAbilityId);

    sptr<IGraphicDumperCommand> service_;
    DumperArgs dumperArgs_ = {};
};

class GDumperCommandDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    GDumperCommandDeathRecipient() = default;
    virtual ~GDumperCommandDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_COMMAND_LINE_H
