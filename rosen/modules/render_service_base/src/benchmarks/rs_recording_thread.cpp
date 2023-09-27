/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "benchmarks/rs_recording_thread.h"

#include <thread>
#include "benchmarks/file_utils.h"
#include "common/rs_thread_handler.h"
#include "platform/common/rs_system_properties.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS::Rosen {

RSRecordingThread &RSRecordingThread::Instance()
{
    static RSRecordingThread instance;
    return instance;
}

void RSRecordingThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RRecordingThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
}

bool RSRecordingThread::IsIdle()
{
    return handler_ && handler_->IsIdle();
}
 
void RSRecordingThread::PostTask(const std::function<void()> &task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSRecordingThread::CheckAndRecording()
{
    if (!handler_) {
        Start();
    }
    RSTaskMessage::RSTask task = [this]() {
        std::string line = "RSRecordingThread::CheckAndRecording curDumpFrame = " + std::to_string(curDumpFrame_) +
            ", dumpFrameNum = " + std::to_string(dumpFrameNum_);
        RS_LOGD("%{public}s", line.c_str());
        isRecordingEnabled_ = RSSystemProperties::GetRecordingEnabled();
        // init curDumpFrame
        if (isRecordingEnabled_ && curDumpFrame_ == 0) {
            RS_TRACE_NAME(line);
            dumpFrameNum_ = RSSystemProperties::GetDumpFrameNum();
            fileDir_ = RSSystemProperties::GetRecordingFile();
            if (access(fileDir_.c_str(), F_OK) != 0) {
                mkdir(fileDir_.c_str(), (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
            }
        }
    };
    PostTask(task);
    return isRecordingEnabled_;
}

void RSRecordingThread::FinishRecordingOneFrame()
{
    std::string line = "RSRecordingThread::FinishRecordingOneFrame curDumpFrame = " + std::to_string(curDumpFrame_) +
        ", dumpFrameNum = " + std::to_string(dumpFrameNum_);
    RS_LOGD("%{public}s", line.c_str());
    RS_TRACE_NAME(line);
    if (curDumpFrame_ < dumpFrameNum_) {
        curDumpFrame_++;
    } else {
        RSTaskMessage::RSTask task = [this]() {
            for (int tmpCurDumpFrame = 0; tmpCurDumpFrame < dumpFrameNum_; tmpCurDumpFrame++) {
                // file name
                std::string drawCmdListFile = fileDir_ + "/frame" + std::to_string(tmpCurDumpFrame) + ".drawing";
                std::string opsFile = fileDir_ + "/ops_frame" + std::to_string(tmpCurDumpFrame) + ".txt";
                // get data
#ifndef USE_ROSEN_DRAWING
                size_t sz = messageParcelVec[tmpCurDumpFrame]->GetDataSize();
                uintptr_t buf = messageParcelVec[tmpCurDumpFrame]->GetData();
                std::string opsDescription = opsDescriptionVec[tmpCurDumpFrame];
#else
                size_t sz = messageParcelVec[tmpCurDumpFrame]->GetSize();
                auto buf = reinterpret_cast<uintptr_t>(messageParcelVec[tmpCurDumpFrame]->GetData());
                std::string opsDescription = "drawing ops no description";
#endif
                std::string line = "RSRecordingThread::FinishRecordingOneFrame curDumpFrame = " +
                    std::to_string(tmpCurDumpFrame) + ", dumpFrameNum = " + std::to_string(dumpFrameNum_) +
                    ", size = " + std::to_string(sz);
                RS_LOGD("%{public}s", line.c_str());
                RS_TRACE_NAME(line);
 
                OHOS::Rosen::Benchmarks::WriteToFile(buf, sz, drawCmdListFile);
                OHOS::Rosen::Benchmarks::WriteStringToFile(opsDescription, opsFile);
            }
            messageParcelVec.clear();
            opsDescriptionVec.clear();
            isRecordingEnabled_ = false;
            curDumpFrame_ = 0;
            dumpFrameNum_ = 0;
            fileDir_ = "";
            RSSystemProperties::SetRecordingDisenabled();
            RS_LOGD("RSRecordingThread::FinishRecordingOneFrame isRecordingEnabled = false");
        };
        PostTask(task);
    }
}

#ifndef USE_ROSEN_DRAWING
void RSRecordingThread::RecordingToFile(const std::shared_ptr<DrawCmdList>& drawCmdList)
#else
void RSRecordingThread::RecordingToFile(const std::shared_ptr<Drawing::DrawCmdList>& drawCmdList)
#endif
{
    if (curDumpFrame_ < 0) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
    messageParcel->SetMaxCapacity(RECORDING_PARCEL_MAX_CAPCITY);
    RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
    drawCmdList->Marshalling(*messageParcel);
    RSMarshallingHelper::EndNoSharedMem();
#else
    auto cmdListData = drawCmdList->GetData();
    auto messageParcel = std::make_shared<Drawing::Data>();
    messageParcel->BuildWithCopy(cmdListData.first, cmdListData.second);
#endif
    messageParcelVec.push_back(messageParcel);
    opsDescriptionVec.push_back(drawCmdList->GetOpsWithDesc());
    FinishRecordingOneFrame();
}
}
