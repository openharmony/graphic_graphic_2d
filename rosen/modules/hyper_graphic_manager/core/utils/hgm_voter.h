/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef HGM_VOTER_H
#define HGM_VOTER_H

#include "securec.h"

#include "animation/rs_frame_rate_range.h"
#include "hgm_command.h"
#include "hgm_log.h"

namespace OHOS::Rosen {
using VoteRange = std::pair<uint32_t, uint32_t>;
struct VoteInfo {
    std::string voterName = "";
    uint32_t min = OLED_NULL_HZ;
    uint32_t max = OLED_NULL_HZ;
    pid_t pid = DEFAULT_PID;
    std::string extInfo = "";
    std::string bundleName = "";

    void Merge(const VoteInfo& other)
    {
        this->voterName = other.voterName;
        this->pid = other.pid;
        this->extInfo = other.extInfo;
    }

    void SetRange(uint32_t minHZ, uint32_t maxHZ)
    {
        this->min = minHZ;
        this->max = maxHZ;
    }

    std::string ToString(uint64_t timestamp) const
    {
        char buf[STRING_BUFFER_MAX_SIZE] = {0};
        int len = ::snprintf_s(buf, sizeof(buf), sizeof(buf) - 1,
            "VOTER_NAME:%s;PREFERRED:%u;EXT_INFO:%s;PID:%d;BUNDLE_NAME:%s;TIMESTAMP:%lu.",
            voterName.c_str(), max, extInfo.c_str(), pid, bundleName.c_str(), timestamp);
        if (len <= 0) {
            HGM_LOGE("failed to execute snprintf.");
        }
        return buf;
    }

    std::string ToSimpleString() const
    {
        char buf[STRING_BUFFER_MAX_SIZE] = {0};
        int len = ::snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "VOTER:%s; FPS:%u; EXT:%s; PID:%d.",
            voterName.c_str(), max, extInfo.c_str(), pid);
        if (len <= 0) {
            HGM_LOGE("failed to execute snprintf.");
        }
        return buf;
    }

    bool operator==(const VoteInfo& other) const
    {
        return this->min == other.min && this->max == other.max && this->voterName == other.voterName &&
            this->extInfo == other.extInfo && this->pid == other.pid && this->bundleName == other.bundleName;
    }

    bool operator!=(const VoteInfo& other) const
    {
        return !(*this == other);
    }
};

class HgmVoter {
public:
    explicit HgmVoter(const std::vector<std::string>& voters);
    ~HgmVoter() = default;

    bool DeliverVote(const VoteInfo& voteInfo, bool eventStatus);
    VoteInfo ProcessVote();
private:
    static std::pair<bool, bool> MergeRangeByPriority(VoteRange& rangeRes, const VoteRange& curVoteRange);
    bool ProcessVote(std::vector<std::string>::iterator& voterIter, VoteInfo& resultVoteInfo, VoteRange& voteRange);

    std::unordered_map<std::string, std::pair<std::vector<VoteInfo>, bool>>  voteRecord_;
    std::vector<std::string> voters_;
};
}
#endif // HGM_VOTER_H