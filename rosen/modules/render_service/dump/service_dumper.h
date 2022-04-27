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

#ifndef ROSEN_SERVICE_DUMPER_H
#define ROSEN_SERVICE_DUMPER_H

#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "ipc_types.h"
#include "iservice_registry.h"
#include "unique_fd.h"

namespace OHOS {
namespace Detail {
template <typename Duration>
using SysTime = std::chrono::time_point<std::chrono::system_clock, Duration>;
using SysMicroSeconds = SysTime<std::chrono::microseconds>;

inline uint64_t MicroSecondsSinceEpoch()
{
    SysMicroSeconds tmp = std::chrono::system_clock::now();
    return tmp.time_since_epoch().count();
}

inline std::u16string ToU16String(std::string s)
{
    using Converter = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>;
    static Converter converter;
    return converter.from_bytes(s);
}

class ServiceDumper {
public:
    ServiceDumper(sptr<IRemoteObject> service, std::string serviceName, int timeOutMs)
        : service_(std::move(service)), serviceName_(std::move(serviceName)), timeOutMs_(timeOutMs)
    {
    }

    ~ServiceDumper() noexcept = default;

    int Run(int argc, char *argv[])
    {
        int status = 0;
        int pipeFd[2];
        if (pipe2(pipeFd, O_CLOEXEC | O_DIRECT) < 0 || pipeFd[0] < 0 || pipeFd[1] < 0) {
            (void)fprintf(stderr, "Failed to create pipe: %s.\n", strerror(errno));
            status = -errno;
            return status;
        }

        readFd_ = UniqueFd(pipeFd[0]);

        {
            UniqueFd remoteFd(pipeFd[1]);
            std::vector<std::u16string> args;
            if (argc > 1) {
                for (int i = 1; i < argc; ++i) {
                    args.push_back(ToU16String(argv[i]));
                }
            }

            if (service_->Dump(remoteFd, args) != NO_ERROR) {
                (void)fprintf(stderr, "Dump failed for %s.\n", serviceName_.c_str());
            }
        }

        return ReadAndWriteDumpInfo();
    }

    int PrintHelpInfo(int argc, char *argv[])
    {
        int retCode = -1;
        if (argc <= 1) {
            return retCode;
        }

        std::string param1(argv[1]);
        if (param1 == "--help") {
            std::cout << "-- help info:" << std::endl;
            std::cout << "display:           Show the screens info." << std::endl;
            std::cout << "surface:           Show the foreground surfaces info." << std::endl;
            std::cout << "fps:               Show the fps info." << std::endl;
            std::cout << "nodeNotOnTree:     Show the surfaces info which are not on the tree." << std::endl;
            std::cout << "allSurfacesMem:    Show the memory size of all surfaces buffer." << std::endl;
            std::cout << "NULL:              Show all of the information above." << std::endl;
            retCode = 1;
        }

        return retCode;
    }

private:
    static constexpr int MICRO_SECS_PER_MILLI = 1000;
    int ReadAndWriteDumpInfo()
    {
        int status = 0;
        auto start = MicroSecondsSinceEpoch() / MICRO_SECS_PER_MILLI;
        auto end = start + timeOutMs_;

        char buf[PIPE_BUF] = {0};
        while (true) {
            auto now = MicroSecondsSinceEpoch() / MICRO_SECS_PER_MILLI;
            if (now >= end) {
                break;
            }

            auto ret = TEMP_FAILURE_RETRY(read(readFd_, buf, sizeof(buf)-1));
            buf[ret] = '\0';
            if (ret == 0) {
                // Reach to the EOF
                break;
            } else if (ret < 0) {
                (void)fprintf(stderr, "Failed to read service's dump info: %s.\n", strerror(errno));
                status = -errno;
                break;
            }

            (void)fprintf(stdout, "%s", buf);
            bzero(buf, sizeof(buf));
        }

        return status;
    }

    sptr<IRemoteObject> service_;
    std::string serviceName_;
    int timeOutMs_ = -1;
    UniqueFd readFd_;
};
} // namespace Detail
} // namespace OHOS

#endif // ROSEN_SERVICE_DUMPER_H
