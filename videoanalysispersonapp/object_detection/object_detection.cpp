/**
 * ============================================================================
 *
 * Copyright (C) 2018, Hisilicon Technologies Co., Ltd. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1 Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   2 Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   3 Neither the names of the copyright holders nor the names of the
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================
 */

#include "object_detection.h"
#include <unistd.h>
#include <memory>
#include <sstream>
#include "ascenddk/ascend_ezdvpp/dvpp_process.h"

using ascend::utils::DvppOutput;
using ascend::utils::DvppProcess;
using hiai::ImageData;
using hiai::IMAGEFORMAT;
using namespace std;
using namespace ascend::videoanalysis;

namespace {
const uint32_t kInputPort = 0; // the input port number
const uint32_t kOutputPort = 0; // the output port number

const int kHandleSuccessful = 0; // the process handled successfully

const int kVideoFormatLength = 5; // video format string length

const int kImageDataQueueSize = 20; // the queue default size

const string kVideoTypeH264 = "h264"; // video type h264
const string kVideoTypeH265 = "h265"; // video type h265

const string kImageFormatNv12 = "nv12"; // image format nv12

const string kStrChannelId1 = "channel1"; // channle id 1 string
const string kStrChannelId2 = "channel2"; // channle id 2 string

const int kIntChannelId1 = 1; // channel id 1 integer
const int kIntChannelId2 = 2; // channel id 2 integer

// The width of image in vpc interface need 128-byte alignment
const int kVpcWidthAlign = 128;

// The height of image in vpc interface need 16-byte alignment
const int kVpcHeightAlign = 16;

// standard: 4096 * 4096 * 4 = 67108864 (64M)
const int kAllowedMaxImageMemory = 67108864;

// standard: 1024 * 1024 * 128 = 134217728 (128M)
const int kMaxNewMemory = 134217728;

const int kKeyFrameInterval = 5; // key fram interval

const int kWaitTimeShort = 20000; // the short wait time: 20ms
const int kWaitTimeLong = 100000; // the short wait time: 100ms
const int kRetryTimeShort = 5; // the short retry time
const int kRetryTimeLong = 100; // the long retry time

const int kCompareEqual = 0; // string compare equal

// ssd input image width and height.
const uint32_t kInputWidth = 512;
const uint32_t kInputHeight = 512;

const int kDvppProcSuccess = 0; // call dvpp success return

const int kVdecSingleton = 0; // dvpp vdec singleton parameter

const string kModelPath = "model_path"; // model path item

// the queue store yuv image data
ThreadSafeQueue<shared_ptr<VideoImageParaT>> yuv_image_queue(
    kImageDataQueueSize);

uint32_t frame_id_1 = 0; // frame id used for channle1

uint32_t frame_id_2 = 0; // frame id used for channle2
}

HIAI_REGISTER_DATA_TYPE("VideoImageInfoT", VideoImageInfoT);
HIAI_REGISTER_DATA_TYPE("VideoImageParaT", VideoImageParaT);
HIAI_REGISTER_DATA_TYPE("OutputT", OutputT);
HIAI_REGISTER_DATA_TYPE("DetectionEngineTransT", DetectionEngineTransT);

ObjectDetectionInferenceEngine::ObjectDetectionInferenceEngine() {
  dvpp_api_channel1_ = nullptr;
  dvpp_api_channel2_ = nullptr;
}

ObjectDetectionInferenceEngine::~ObjectDetectionInferenceEngine() {
  if (dvpp_api_channel1_ != nullptr) { // check create dvpp api result
    DestroyVdecApi(dvpp_api_channel1_, 0);
  }

  if (dvpp_api_channel2_ != nullptr) { // check create dvpp api result
    DestroyVdecApi(dvpp_api_channel2_, 0);
  }
}

HIAI_StatusT ObjectDetectionInferenceEngine::Init(
    const hiai::AIConfig &config,
    const vector<hiai::AIModelDescription> &model_desc) {
  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[ODInferenceEngine] start to initialize!");

  if (ai_model_manager_ == nullptr) { // check ai model manager is nullptr
    ai_model_manager_ = make_shared<hiai::AIModelManager>();
  }

  vector<hiai::AIModelDescription> od_model_descs;
  hiai::AIModelDescription model_description;

  // load model path.
  for (int index = 0; index < config.items_size(); ++index) {
    const ::hiai::AIConfigItem &item = config.items(index);
    if (item.name() == kModelPath) { // current item is model path
      const char* model_path = item.value().data();
      model_description.set_path(model_path);
    }
  }
  od_model_descs.push_back(model_description);

  // init ssd model
  HIAI_StatusT ret = ai_model_manager_->Init(config, od_model_descs);
  if (ret != hiai::SUCCESS) {
    HIAI_ENGINE_LOG(HIAI_GRAPH_INVALID_VALUE,
                    "[ODInferenceEngine] failed to initialize AI model!");
    return HIAI_ERROR;
  }

  // create vdec api for channel1, and check the result
  if (CreateVdecApi(dvpp_api_channel1_, kVdecSingleton) != kHandleSuccessful) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[ODInferenceEngine] fail to create dvpp vdec api for channel1!");
  }
  // create vdec api for channel2, and check the result
  if (CreateVdecApi(dvpp_api_channel2_, kVdecSingleton) != kHandleSuccessful) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[ODInferenceEngine] fail to create dvpp vdec api for channel2!");
  }

  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[ODInferenceEngine] engine initialized!");
  return HIAI_OK;
}

void ascend::videoanalysis::CallVpcGetYuvImage(FRAME* frame, void* hiai_data) {
  if (frame == nullptr || hiai_data == nullptr) { // check input parameters
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "The input data for function:CallVpcGetYuvImage is nullptr!");
    return;
  }

  string channel_id = ((HiaiDataSpSon*) hiai_data)->channel_id_;
  uint32_t frame_id = GetFrameId(channel_id);
  // only send key frame to next engine, key frame id: 1,6,11,16...
  if (!IsKeyFrame(frame_id)) {
    return;
  }

  string channel_name = ((HiaiDataSpSon*) hiai_data)->channel_name_;
  HIAI_ENGINE_LOG("Get key frame, frame id:%d, channel_id:%s, channel_name:%s,"
                  " frame->realWidth:%d, frame->realHeight:%d",
                  frame_id, channel_id.c_str(), channel_name.c_str(),
                  frame->realWidth, frame->realHeight);

  IDVPPAPI* dvpp_api = nullptr;
  CreateDvppApi(dvpp_api);

  if (dvpp_api == nullptr) { // check create dvpp api result
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "Fail to call CreateDvppApi for vpc, the result is nullptr!");
    return;
  }

  int aligned_output_width = ALIGN_UP(frame->width, kVpcWidthAlign);
  int aligned_output_height = ALIGN_UP(frame->height, kVpcHeightAlign);

  // constructing input image configuration
  shared_ptr<VpcUserImageConfigure> image_configure(
      new (nothrow) VpcUserImageConfigure);
  if (image_configure.get() == nullptr) { // check new memory result
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "Fail to new memory when construct input image configuration!");
    return;
  }

  image_configure->widthStride = aligned_output_width;
  image_configure->heightStride = aligned_output_height;

  // check image format is nv12
  if (strcmp(frame->image_format, kImageFormatNv12.c_str()) == kCompareEqual) {
    image_configure->inputFormat = INPUT_YUV420_SEMI_PLANNER_UV;
  } else { // check image format is nv21
    image_configure->inputFormat = INPUT_YUV420_SEMI_PLANNER_VU;
  }

  image_configure->outputFormat = OUTPUT_YUV420SP_UV;
  image_configure->isCompressData = true;
  image_configure->compressDataConfigure.lumaHeadAddr = (long) (frame->buffer
      + frame->offset_head_y);
  image_configure->compressDataConfigure.chromaHeadAddr = (long) (frame->buffer
      + frame->offset_head_c);
  image_configure->compressDataConfigure.lumaHeadStride = frame->stride_head;
  image_configure->compressDataConfigure.chromaHeadStride = frame->stride_head;
  image_configure->compressDataConfigure.lumaPayloadAddr = (long) (frame->buffer
      + frame->offset_payload_y);
  image_configure->compressDataConfigure.chromaPayloadAddr = (long) (frame
      ->buffer + frame->offset_payload_c);
  image_configure->compressDataConfigure.lumaPayloadStride = frame
      ->stride_payload;
  image_configure->compressDataConfigure.chromaPayloadStride = frame
      ->stride_payload;

  shared_ptr<VpcUserRoiConfigure> roi_configure(
      new (nothrow) VpcUserRoiConfigure);
  if (roi_configure.get() == nullptr) { // check new memory result
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "Fail to new memory when initialize vpc user roi configure!");
    return;
  }

  roi_configure->next = nullptr;

  // constructing input roi configuration
  VpcUserRoiInputConfigure *input_configure = &roi_configure->inputConfigure;
  input_configure->cropArea.leftOffset = 0; // 0 means without crop
  // dvpp limits rightOffset is odd
  input_configure->cropArea.rightOffset =
      frame->width % 2 == 0 ? frame->width - 1 : frame->width;
  input_configure->cropArea.upOffset = 0; // 0 means without crop
  // dvpp limits downOffset is odd
  input_configure->cropArea.downOffset =
      frame->height % 2 == 0 ? frame->height - 1 : frame->height;

  int vpc_output_size = aligned_output_width * aligned_output_height
      * DVPP_YUV420SP_SIZE_MOLECULE / DVPP_YUV420SP_SIZE_DENOMINATOR;

  // check vpc output size is valid
  if (vpc_output_size <= 0 || vpc_output_size > kAllowedMaxImageMemory) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "The vpc_output_size:%d is invalid! value range: 1~67108864",
        vpc_output_size);
    return;
  }

  // construct vpc out data buffer
  uint8_t* vpc_out_buffer = (uint8_t *) mmap(
      0, ALIGN_UP(vpc_output_size, MAP_2M), PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS | API_MAP_VA32BIT, -1, 0);
  if (vpc_out_buffer == MAP_FAILED) { // check mmap buffer result
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Failed to malloc 4k memory for vpc!");
    return;
  }

  // constructing output roi configuration
  VpcUserRoiOutputConfigure *output_configure = &roi_configure->outputConfigure;
  output_configure->addr = vpc_out_buffer;
  output_configure->bufferSize = vpc_output_size;
  output_configure->widthStride = aligned_output_width;
  output_configure->heightStride = aligned_output_height;
  output_configure->outputArea.leftOffset = 0; // 0 means without crop
  // dvpp limits rightOffset is odd
  output_configure->outputArea.rightOffset =
      frame->width % 2 == 0 ? frame->width - 1 : frame->width;
  output_configure->outputArea.upOffset = 0; // 0 means without crop
  // dvpp limits downOffset is odd
  output_configure->outputArea.downOffset =
      frame->height % 2 == 0 ? frame->height - 1 : frame->height;

  image_configure->roiConfigure = roi_configure.get();

  dvppapi_ctl_msg dvpp_api_ctl_msg;
  dvpp_api_ctl_msg.in = static_cast<void *>(image_configure.get());
  dvpp_api_ctl_msg.in_size = sizeof(VpcUserImageConfigure);

  // call vpc and check the result
  if (DvppCtl(dvpp_api, DVPP_CTL_VPC_PROC, &dvpp_api_ctl_msg)
      != kHandleSuccessful) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Fail to call dvppctl VPC!");
    // free vpc_out_buffer memory
    munmap(vpc_out_buffer, (unsigned) (ALIGN_UP(vpc_output_size, MAP_2M)));
    DestroyDvppApi(dvpp_api);
    return;
  }

  DestroyDvppApi(dvpp_api);

  uint8_t* output_image_buffer = new (nothrow) uint8_t[vpc_output_size];
  if (output_image_buffer == nullptr) { // check new result
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Fail to new data when handle vpc output!");
    // free vpc_out_buffer memory
    munmap(vpc_out_buffer, (unsigned) (ALIGN_UP(vpc_output_size, MAP_2M)));
    return;
  }

  int memcpy_result = memcpy_s(output_image_buffer, vpc_output_size,
                               vpc_out_buffer, vpc_output_size);
  // free vpc_out_buffer memory
  munmap(vpc_out_buffer, (unsigned) (ALIGN_UP(vpc_output_size, MAP_2M)));

  if (memcpy_result != EOK) { // check memcpy_s result
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Fail to copy vpc output image buffer, memcpy_s result:%d",
                    memcpy_result);
    delete[] output_image_buffer;
    return;
  }

  shared_ptr<VideoImageParaT> video_image_para = make_shared<VideoImageParaT>();
  video_image_para->video_image_info.channel_id = channel_id;
  video_image_para->video_image_info.channel_name = channel_name;
  video_image_para->video_image_info.frame_id = frame_id;
  video_image_para->video_image_info.is_finished = false;
  video_image_para->img.width = frame->realWidth;
  video_image_para->img.height = frame->realHeight;
  video_image_para->img.format = IMAGEFORMAT::YUV420SP;
  video_image_para->img.size = vpc_output_size;
  video_image_para->img.data.reset(output_image_buffer,
                                   default_delete<uint8_t[]>());
  AddImage2Queue(video_image_para);

  return;
}

bool ascend::videoanalysis::IsKeyFrame(uint32_t frame_id) {
  // the 1, 6, 11, 16... frame is key frame
  if (frame_id % kKeyFrameInterval == 1) {
    return true;
  }

  return false;
}

uint32_t ascend::videoanalysis::GetFrameId(const string &channel_id) {
  if (channel_id == kStrChannelId1) { // check input channel id is channel1
    frame_id_1++;
    return frame_id_1;
  } else { // the input channel id is channel2
    frame_id_2++;
    return frame_id_2;
  }
}

void ascend::videoanalysis::AddImage2Queue(
    const shared_ptr<VideoImageParaT> &video_image_para) {
  bool add_image_success = false;
  int count = 0; // count retry number

  // add image data to queue, max retry time: 100
  while (count < kRetryTimeLong) {
    count++;

    if (yuv_image_queue.Push(video_image_para)) { // push image data to queue
      add_image_success = true;
      break;
    } else { // queue is full
      usleep(kWaitTimeLong); // sleep 100 ms
    }
  }

  if (!add_image_success) { // fail to send image data
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Fail to add image data to queue, channel_id:%s, "
                    "channel_name:%s, frame_id:%d",
                    video_image_para->video_image_info.channel_id.c_str(),
                    video_image_para->video_image_info.channel_name.c_str(),
                    video_image_para->video_image_info.frame_id);
  }
}

HIAI_StatusT ObjectDetectionInferenceEngine::ImagePreProcess(
    const ImageData<u_int8_t> &src_img, ImageData<u_int8_t> &resized_img) {
  if (src_img.format != IMAGEFORMAT::YUV420SP) {
    // input image must be yuv420sp nv12.
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[ODInferenceEngine] input image type does not match");
    return HIAI_ERROR;
  }

  ascend::utils::DvppBasicVpcPara dvpp_basic_vpc_para;

  /**
   * when use dvpp_process only for resize function:
   *
   * 1.DVPP limits crop_right and crop_down should be Odd number,
   * if it is even number, subtract 1, otherwise Equal to origin width
   * or height.
   *
   * 2.crop_left and crop_up should be set to zero.
   */
  dvpp_basic_vpc_para.input_image_type = INPUT_YUV420_SEMI_PLANNER_UV; // nv12
  dvpp_basic_vpc_para.output_image_type = OUTPUT_YUV420SP_UV; // nv12
  dvpp_basic_vpc_para.src_resolution.width = (int) src_img.width;
  dvpp_basic_vpc_para.src_resolution.height = (int) src_img.height;
  dvpp_basic_vpc_para.dest_resolution.width = kInputWidth;
  dvpp_basic_vpc_para.dest_resolution.height = kInputHeight;
  // DVPP limits crop_left should be even number, 0 means without crop
  dvpp_basic_vpc_para.crop_left = 0;
  // DVPP limits crop_right should be Odd number
  dvpp_basic_vpc_para.crop_right =
      src_img.width % 2 == 0 ? src_img.width - 1 : src_img.width;
  // DVPP limits crop_up should be even number, 0 means without crop
  dvpp_basic_vpc_para.crop_up = 0;
  // DVPP limits crop_down should be Odd number
  dvpp_basic_vpc_para.crop_down =
      src_img.height % 2 == 0 ? src_img.height - 1 : src_img.height;
  dvpp_basic_vpc_para.is_input_align = true;

  ascend::utils::DvppProcess dvpp_process(dvpp_basic_vpc_para);

  ascend::utils::DvppVpcOutput dvpp_out;
  int ret = dvpp_process.DvppBasicVpcProc(src_img.data.get(),
                                          (int32_t) src_img.size, &dvpp_out);
  if (ret != kDvppProcSuccess) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[ODInferenceEngine] call dvpp resize failed with code %d!",
                    ret);
    return HIAI_ERROR;
  }

  // dvpp_out->pbuf
  resized_img.data.reset(dvpp_out.buffer, default_delete<uint8_t[]>());
  resized_img.size = dvpp_out.size;

  return HIAI_OK;
}

HIAI_StatusT ObjectDetectionInferenceEngine::PerformInference(
    shared_ptr<DetectionEngineTransT> &detection_trans,
    ImageData<u_int8_t> &input_img) {
  // init neural buffer.
  shared_ptr<hiai::AINeuralNetworkBuffer> neural_buffer = shared_ptr<
      hiai::AINeuralNetworkBuffer>(
      new (nothrow) hiai::AINeuralNetworkBuffer(),
      default_delete<hiai::AINeuralNetworkBuffer>());
  if (neural_buffer.get() == nullptr) { // check new memory result
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Fail to new memory when initialize neural buffer!");
    return HIAI_ERROR;
  }

  neural_buffer->SetBuffer((void*) input_img.data.get(), input_img.size);

  shared_ptr<hiai::IAITensor> input_tensor =
      static_pointer_cast<hiai::IAITensor>(neural_buffer);

  vector<shared_ptr<hiai::IAITensor>> input_tensors;
  vector<shared_ptr<hiai::IAITensor>> output_tensors;
  input_tensors.push_back(input_tensor);

  HIAI_StatusT ret = hiai::SUCCESS;
  ret = ai_model_manager_->CreateOutputTensor(input_tensors, output_tensors);
  if (ret != hiai::SUCCESS) {
    SendDetectionResult(detection_trans, false,
                        "[ODInferenceEngine] output tensor created failed!");
    return HIAI_ERROR;
  }

  hiai::AIContext ai_context;
  // neural network inference.
  ret = ai_model_manager_->Process(ai_context, input_tensors, output_tensors,
                                   0);
  if (ret != hiai::SUCCESS) {
    SendDetectionResult(detection_trans, false,
                        "[ODInferenceEngine] image inference failed!");
    return HIAI_ERROR;
  }

  // set trans_data
  detection_trans->status = true;
  for (uint32_t index = 0; index < output_tensors.size(); ++index) {
    shared_ptr<hiai::AINeuralNetworkBuffer> result_tensor = static_pointer_cast<
        hiai::AINeuralNetworkBuffer>(output_tensors[index]);

    OutputT out;
    out.size = result_tensor->GetSize();
    if (out.size <= 0 || out.size > kMaxNewMemory) { // check data size is valid
      HIAI_ENGINE_LOG(
          HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
          "the inference output size:%d is invalid! value range: 1~134217728",
          out.size);
      return HIAI_ERROR;
    }

    out.data = std::shared_ptr<uint8_t>(new (nothrow) uint8_t[out.size],
                                        std::default_delete<uint8_t[]>());
    if (out.data.get() == nullptr) { // check new memory result
      HIAI_ENGINE_LOG(
          HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
          "Fail to new memory when handle ai model inference output!");
      return HIAI_ERROR;
    }

    errno_t ret = memcpy_s(out.data.get(), out.size, result_tensor->GetBuffer(),
                           out.size);
    if (ret != EOK) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[ODInferenceEngine] output tensor copy failure!");
      continue;
    }
    detection_trans->output_datas.push_back(out);
  }
  if (detection_trans->output_datas.empty()) {
    SendDetectionResult(detection_trans, false,
                        "[ODInferenceEngine] result tensor is empty!");
    return HIAI_ERROR;
  }

  // sendData
  return SendDetectionResult(detection_trans);
}

HIAI_StatusT ObjectDetectionInferenceEngine::SendDetectionResult(
    shared_ptr<DetectionEngineTransT> &detection_trans, bool inference_success,
    string err_msg) {
  if (!inference_success) {
    // inference error.
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT, err_msg.c_str());
    detection_trans->status = false;
    detection_trans->msg = err_msg;
  }
  HIAI_StatusT ret;
  do {
    // send data to next engine.
    ret = SendData(kOutputPort, "DetectionEngineTransT",
                   static_pointer_cast<void>(detection_trans));
    if (ret == HIAI_QUEUE_FULL) {
      HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[ODInferenceEngine] output queue full");
      usleep(kWaitTimeShort);
    }
  } while (ret == HIAI_QUEUE_FULL);
  if (ret != HIAI_OK) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[ODInferenceEngine] send inference data failed!");
    return HIAI_ERROR;
  }
  return HIAI_OK;
}

int ObjectDetectionInferenceEngine::GetIntChannelId(const string channel_id) {
  if (channel_id == kStrChannelId1) { // check channel is channel1
    return kIntChannelId1;
  } else { // the channel is channel2
    return kIntChannelId2;
  }
}

bool ObjectDetectionInferenceEngine::ConvertVideoFrameToHfbc(
    const shared_ptr<VideoImageParaT> &video_image) {
  IDVPPAPI* dvpp_api = nullptr;
  if (video_image->video_image_info.channel_id == kStrChannelId1) {
    if (dvpp_api_channel1_ == nullptr) { // chech dvpp api is nullptr
      // create vdec api for channel1, and check the result
      if (CreateVdecApi(dvpp_api_channel1_, kVdecSingleton)
          != kHandleSuccessful) {
        HIAI_ENGINE_LOG(
            HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
            "[ODInferenceEngine] fail to create vdec api for channel1!");
        return false;
      }
    }
    dvpp_api = dvpp_api_channel1_;
  } else {
    if (dvpp_api_channel2_ == nullptr) { // chech dvpp api is nullptr
      // create vdec api for channel2, and check the result
      if (CreateVdecApi(dvpp_api_channel2_, kVdecSingleton)
          != kHandleSuccessful) {
        HIAI_ENGINE_LOG(
            HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
            "[ODInferenceEngine] fail to create vdec api for channel2!");
        return false;
      }
    }
    dvpp_api = dvpp_api_channel2_;
  }

  vdec_in_msg vdec_msg;
  vdec_msg.call_back = CallVpcGetYuvImage;
  shared_ptr<HiaiDataSpSon> hiai_data = make_shared<HiaiDataSpSon>();
  hiai_data->channel_name_ = video_image->video_image_info.channel_name;
  hiai_data->channel_id_ = video_image->video_image_info.channel_id;
  vdec_msg.channelId = GetIntChannelId(
      video_image->video_image_info.channel_id);
  vdec_msg.hiai_data_sp = hiai_data;

  int strcpy_result = EOK;
  // check video type is h264 or h265
  if (video_image->video_image_info.video_type == kH264) {
    // video type: h264
    strcpy_result = strcpy_s(vdec_msg.video_format, kVideoFormatLength,
                             kVideoTypeH264.c_str());
  } else { // the video type is h265
    strcpy_result = strcpy_s(vdec_msg.video_format, kVideoFormatLength,
                             kVideoTypeH265.c_str());
  }

  if (strcpy_result != EOK) {
    // check strcpy result
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "fail to call strcpy_s, result:%d, channel id:%s",
                    strcpy_result,
                    video_image->video_image_info.channel_id.c_str());
    return false;
  }

  dvppapi_ctl_msg dvppApiCtlMsg;
  dvppApiCtlMsg.in = (void*) ((&vdec_msg));
  dvppApiCtlMsg.in_size = sizeof(vdec_in_msg);

  vdec_msg.in_buffer = (char*) video_image->img.data.get();
  vdec_msg.in_buffer_size = video_image->img.size;

  // call vdec and check result
  if (VdecCtl(dvpp_api, DVPP_CTL_VDEC_PROC, &dvppApiCtlMsg, 0)
      != kHandleSuccessful) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "Fail to call dvppctl process, channel id:%s",
                    video_image->video_image_info.channel_id.c_str());
    return false;
  }

  return true;
}

void ObjectDetectionInferenceEngine::ObjectDetectInference() {
  HIAI_ENGINE_LOG(
      "[ODInferenceEngine] start object detection inference, queue size:%d",
      yuv_image_queue.size());

  while (!yuv_image_queue.empty()) {
    // init inference results tensor shared_ptr.
    shared_ptr<DetectionEngineTransT> detection_trans = make_shared<
        DetectionEngineTransT>();

    shared_ptr<VideoImageParaT> video_image = yuv_image_queue.Pop();
    if (video_image == nullptr) {
      HIAI_ENGINE_LOG(
          "[ODInferenceEngine] fail pop yuv image data from yuv_image_queue!");
      continue;
    }

    detection_trans->video_image = *video_image;

    // resize input image.
    ImageData<u_int8_t> resized_img;
    HIAI_StatusT dvpp_ret = ImagePreProcess(detection_trans->video_image.img,
                                            resized_img);
    if (dvpp_ret != HIAI_OK) {
      // if preprocess error,send input image to the next engine.
      SendDetectionResult(detection_trans, false,
                          "[ODInferenceEngine] image preprocessed failure!");
      return;
    }

    // inference object detection
    if (PerformInference(detection_trans, resized_img) != HIAI_OK) {
      HIAI_ENGINE_LOG(
          HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
          "[ODInferenceEngine] fail to inference object detection!");
    }
  }
}

HIAI_StatusT ObjectDetectionInferenceEngine::HandleFinishedData(
    const shared_ptr<VideoImageParaT> &video_image) {
  // the input is finished
  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO,
                  "[ODInferenceEngine] get is_finished frame!");

  // loop for send yuv image data obtained from dvpp vpc
  for (int count = 0; count < kRetryTimeShort; count++) {
    if (yuv_image_queue.empty()) {
      // the yuv iamge queue is empty
      count++;
      usleep(kWaitTimeLong); // sleep 100ms
    } else { // the yuv iamge queue is not empty
      count = 0;
      // convert hfbc data to yuv image from queue, and detect object
      ObjectDetectInference();
    }
  }

  // destroy dvpp api used for channel1
  if (dvpp_api_channel1_ != nullptr) {
    DestroyVdecApi(dvpp_api_channel1_, 0);
  }
  // destroy dvpp api used for channel2
  if (dvpp_api_channel2_ != nullptr) {
    DestroyVdecApi(dvpp_api_channel2_, 0);
  }

  // sleep 100ms wait for dvpp finish last frame convert
  usleep(kWaitTimeLong);

  // the yuv iamge queue is not empty
  ObjectDetectInference();

  // init inference results tensor shared_ptr.
  shared_ptr<DetectionEngineTransT> detection_trans = make_shared<
      DetectionEngineTransT>();
  detection_trans->video_image = *video_image;
  HIAI_ENGINE_LOG("[ODInferenceEngine] send is_finished data!");

  // send finished data
  return SendDetectionResult(detection_trans);
}

HIAI_IMPL_ENGINE_PROCESS("object_detection", ObjectDetectionInferenceEngine,
    INPUT_SIZE) {
  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[ODInferenceEngine] start process!");

  if (arg0 == nullptr) {
    // inputer data is nullptr.
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[ODInferenceEngine] input data is null!");
    return HIAI_ERROR;
  }

  shared_ptr<VideoImageParaT> video_image =
      static_pointer_cast<VideoImageParaT>(arg0);

  if (video_image->video_image_info.is_finished) {
    // input is finished.
    return HandleFinishedData(video_image);
  }

  // convert video frame to hfbc and put results to queue
  if (!ConvertVideoFrameToHfbc(video_image)) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[ODInferenceEngine] fail to call dvpp vdec convert h264 to hfbc!");
    return HIAI_ERROR;
  }

  // convert hfbc data to yuv image from queue, and detect object
  ObjectDetectInference();

  return HIAI_OK;
}
