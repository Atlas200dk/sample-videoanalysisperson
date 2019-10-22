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

#include "face_detection.h"
#include <unistd.h>
#include <memory>
#include <sstream>
#include "ascenddk/ascend_ezdvpp/dvpp_process.h"

using ascend::utils::DvppOutput;
using ascend::utils::DvppProcess;
using hiai::ImageData;
using hiai::IMAGEFORMAT;
using namespace std;

namespace {
// ssd input image width and height.
const uint32_t kInputWidth = 300;
const uint32_t kInputHeight = 300;

const int kDvppProcSuccess = 0;     // call dvpp success return.
const int kSleepMicroSecs = 20000;  // waiting time after queue is full.

const string kModelPath = "model_path";

// detection object labels
const uint32_t kLabelFace = 1;

const uint32_t kSizePerResultset = 7;

// valid confidence value
const float kMinConfidence = 0.0f;
const float kMaxConfidence = 1.0f;
const float kConfidence = 0.4f;

// dvpp minimal crop size
const uint32_t kMinCropPixel = 16;

// dvpp minimal yuv2jpg size
const uint32_t kMinJpegPixel = 32;

// valid bbox coordinate
const float kLowerCoord = 0.0f;
const float kUpperCoord = 1.0f;

// output engine ports
const uint32_t kPortPost = 0;
const uint32_t kPortAge = 1;
const uint32_t kPortGender = 2;

const int kInferenceVectorSize = 2;
const int kInferenceOutputNum = 1;
const int kInferenceOutputBBox = 0;

const string kPrefixFace = "face_";

// function of dvpp returns success
const int kDvppOperationOk = 0;

// infernece output data index
enum BBoxDataIndex {
  kAttribute = 1,
  kScore,
  kTopLeftX,
  kTopLeftY,
  kLowerRightX,
  kLowerRightY,
};
}  // namespace

// register data type
HIAI_REGISTER_DATA_TYPE("OutputT", OutputT);
HIAI_REGISTER_DATA_TYPE("DetectionEngineTransT", DetectionEngineTransT);
HIAI_REGISTER_DATA_TYPE("VideoImageInfoT", VideoImageInfoT);
HIAI_REGISTER_DATA_TYPE("VideoImageParaT", VideoImageParaT);
HIAI_REGISTER_DATA_TYPE("ObjectImageParaT", ObjectImageParaT);
HIAI_REGISTER_DATA_TYPE("ObjectInfoT", ObjectInfoT);
HIAI_REGISTER_DATA_TYPE("VideoDetectionImageParaT", VideoDetectionImageParaT);
HIAI_REGISTER_DATA_TYPE("BatchCroppedImageParaT", BatchCroppedImageParaT);

HIAI_StatusT FaceDetection::Init(
    const hiai::AIConfig& config,
    const vector<hiai::AIModelDescription>& model_desc) {
  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[FaceDetection] start to initialize!");

  if (ai_model_manager_ == nullptr) {
    ai_model_manager_ = make_shared<hiai::AIModelManager>();
  }

  vector<hiai::AIModelDescription> od_model_descs;
  hiai::AIModelDescription model_description;

  // load model path.
  for (int index = 0; index < config.items_size(); ++index) {
    const ::hiai::AIConfigItem& item = config.items(index);
    const string& name = item.name();
    const string& value = item.value();
    if (name == kModelPath) {
      const char* model_path = value.data();
      model_description.set_path(model_path);
    }
    if (name == "Confidence") {
      if (!InitConfidence(value)) {
        HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                        "[FaceDetection] confidence value %s is invalid!",
                        value.c_str());
        return HIAI_ERROR;
      }
    }
  }
  od_model_descs.push_back(model_description);

  // init ssd model
  HIAI_StatusT ret = ai_model_manager_->Init(config, od_model_descs);
  if (ret != hiai::SUCCESS) {
    HIAI_ENGINE_LOG(HIAI_GRAPH_INVALID_VALUE,
                    "[FaceDetection] failed to initialize AI model!");
    return HIAI_ERROR;
  }

  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[FaceDetection] engine initialized!");
  return HIAI_OK;
}

HIAI_StatusT FaceDetection::CropObjectFromImage(
    const ImageData<u_int8_t>& src_img, ImageData<u_int8_t>& target_img,
    const BoundingBox& bbox) {
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
  dvpp_basic_vpc_para.input_image_type = INPUT_YUV420_SEMI_PLANNER_UV;  // nv12
  dvpp_basic_vpc_para.output_image_type = OUTPUT_YUV420SP_UV;  // nv12
  dvpp_basic_vpc_para.src_resolution.width = src_img.width;
  dvpp_basic_vpc_para.src_resolution.height = src_img.height;

  // the value of horz_max and vert_max must be odd and
  // horz_min and vert_min must be even
  int crop_horz_min = bbox.lt_x % 2 == 0 ? bbox.lt_x : bbox.lt_x + 1;
  int crop_horz_max = bbox.rb_x % 2 == 0 ? bbox.rb_x - 1 : bbox.rb_x;
  int crop_vert_min = bbox.lt_y % 2 == 0 ? bbox.lt_y : bbox.lt_y + 1;
  int crop_vert_max = bbox.rb_y % 2 == 0 ? bbox.rb_y - 1 : bbox.rb_y;

  // calculate cropped image width and height.
  int dest_width = crop_horz_max - crop_horz_min + 1;
  int dest_height = crop_vert_max - crop_vert_min + 1;

  if (dest_width < kMinJpegPixel || dest_height < kMinJpegPixel) {
    float short_side = dest_width < dest_height ? dest_width : dest_height;
    dest_width = dest_width * (kMinJpegPixel / short_side);
    dest_height = dest_height * (kMinJpegPixel / short_side);
  }

  int dest_resolution_width = dest_width % 2 == 0 ? dest_width : dest_width + 1;
  int dest_resolutiont_height =
      dest_height % 2 == 0 ? dest_height : dest_height + 1;
  dvpp_basic_vpc_para.dest_resolution.width = dest_resolution_width;
  dvpp_basic_vpc_para.dest_resolution.height = dest_resolutiont_height;
  // DVPP limits crop_left should be even number
  dvpp_basic_vpc_para.crop_left = crop_horz_min;
  // DVPP limits crop_right should be Odd number
  dvpp_basic_vpc_para.crop_right = crop_horz_max;
  // DVPP limits crop_up should be even number
  dvpp_basic_vpc_para.crop_up = crop_vert_min;
  // DVPP limits crop_down should be Odd number
  dvpp_basic_vpc_para.crop_down = crop_vert_max;
  dvpp_basic_vpc_para.is_input_align = true;

  ascend::utils::DvppProcess dvpp_process(dvpp_basic_vpc_para);

  ascend::utils::DvppVpcOutput dvpp_out;
  int ret = dvpp_process.DvppBasicVpcProc(src_img.data.get(),
                                          (int32_t) src_img.size, &dvpp_out);
  if (ret != kDvppProcSuccess) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] crop image failed with code %d !", ret);
    return HIAI_ERROR;
  }
  target_img.channel = src_img.channel;
  target_img.format = src_img.format;
  target_img.data.reset(dvpp_out.buffer, default_delete<uint8_t[]>());
  target_img.width = dest_resolution_width;
  target_img.height = dest_resolutiont_height;
  target_img.size = dvpp_out.size;

  return HIAI_OK;
}

void FaceDetection::FilterBoundingBox(
    float* bbox_buffer, int32_t bbox_buffer_size,
    shared_ptr<VideoDetectionImageParaT>& detection_image,
    vector<ObjectImageParaT>& face_imgs) {
  float* ptr = bbox_buffer;
  int32_t num_face = 0;

  uint32_t base_width = detection_image->image.img.width;
  uint32_t base_height = detection_image->image.img.height;

  for (int32_t k = 0; k < bbox_buffer_size; k += kSizePerResultset) {
    ptr = bbox_buffer + k;
    int32_t attr = static_cast<int32_t>(ptr[BBoxDataIndex::kAttribute]);
    float score = ptr[BBoxDataIndex::kScore];
    if (score < confidence_ || attr != kLabelFace) {
      continue;
    }
    // bbox coordinate should between 0.0f and 1.0f
    uint32_t lt_x = CorrectCoordinate(ptr[BBoxDataIndex::kTopLeftX])
        * base_width, lt_y = CorrectCoordinate(ptr[BBoxDataIndex::kTopLeftY])
        * base_height, rb_x = CorrectCoordinate(
        ptr[BBoxDataIndex::kLowerRightX]) * base_width, rb_y =
        CorrectCoordinate(ptr[BBoxDataIndex::kLowerRightY]) * base_height;

    if (rb_x - lt_x < kMinCropPixel || rb_y - lt_x < kMinCropPixel) {
      continue;
    }
    // crop image
    ObjectImageParaT object_image;
    BoundingBox bbox = { lt_x, lt_y, rb_x, rb_y };
    HIAI_StatusT crop_ret = CropObjectFromImage(detection_image->image.img,
                                                object_image.img, bbox);
    if (crop_ret != HIAI_OK) {
      continue;
    }
    object_image.object_info.score = score;
    if (attr == kLabelFace) {
      ++num_face;
      stringstream ss;
      ss << kPrefixFace << num_face;
      object_image.object_info.object_id = ss.str();
      face_imgs.push_back(object_image);

    }
    detection_image->obj_imgs.push_back(object_image);
  }
}

HIAI_StatusT FaceDetection::SendJpgImage(const VideoImageParaT image_input) {
  HIAI_StatusT hiai_ret = HIAI_OK;

  // use dvpp convert yuv image to jpg image
  int ret_dvpp;

  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;

  // use dvpp convert yuv to jpg image, level should set fixed value 100
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = image_input.img.height;
  dvpp_to_jpg_para.resolution.width = image_input.img.width;

  // true indicate the image is aligned
  dvpp_to_jpg_para.is_align_image = true;
  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);
  ascend::utils::DvppOutput dvpp_output = { 0 };

  // use dvpp convert yuv image to jpg image
  ret_dvpp = dvpp_jpg_process.DvppOperationProc(
      (char*) (image_input.img.data.get()), image_input.img.size, &dvpp_output);
  if (ret_dvpp == kDvppOperationOk) {
    tran_jpg_data_->image.img.data.reset(dvpp_output.buffer,
                                         default_delete<uint8_t[]>());
    tran_jpg_data_->image.img.size = dvpp_output.size;
  } else {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "origin image fail to convert yuv to jpg,ret_dvpp = %d",
                    ret_dvpp);
    return HIAI_ERROR;
  }

  // get small images after reasoning
  for (vector<ObjectImageParaT>::iterator iter = tran_data_->obj_imgs.begin();
      iter != tran_data_->obj_imgs.end(); ++iter) {
    // use dvpp convert yuv image to jpg image
    ascend::utils::DvppToJpgPara dvpp_to_jpg_obj_para;
    dvpp_to_jpg_obj_para.format = JPGENC_FORMAT_NV12;

    // level should set fixed value 100
    dvpp_to_jpg_obj_para.level = 100;

    // true indicate the image is aligned
    dvpp_to_jpg_obj_para.is_align_image = true;
    dvpp_to_jpg_obj_para.resolution.height = iter->img.height;
    dvpp_to_jpg_obj_para.resolution.width = iter->img.width;
    ascend::utils::DvppProcess dvpp_jpg_objprocess(dvpp_to_jpg_obj_para);
    ascend::utils::DvppOutput obj_dvpp_output = { 0 };
    ret_dvpp = dvpp_jpg_objprocess.DvppOperationProc(
        (char*) (iter->img.data.get()), iter->img.size, &obj_dvpp_output);
    if (ret_dvpp == kDvppOperationOk) {
      iter->img.data.reset(obj_dvpp_output.buffer, default_delete<uint8_t[]>());
      iter->img.size = obj_dvpp_output.size;
    } else {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "fail to convert obj yuv to jpg,ret_dvpp = %d", ret_dvpp);
      return HIAI_ERROR;
    }
  }

  tran_jpg_data_->image.video_image_info = tran_data_->video_image_info;
  tran_jpg_data_->obj_imgs = tran_data_->obj_imgs;

  hiai_ret = SendResults(kPortPost, "VideoDetectionImageParaT",
                         static_pointer_cast<void>(tran_jpg_data_));

  if (hiai_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(
        "[FaceDetectionInferenceEngine] send tran_jpg_data_ failed!");
    return HIAI_ERROR;
  }

  return HIAI_OK;
}

HIAI_StatusT FaceDetection::HandleResults(
    const shared_ptr<DetectionEngineTransT>& inference_result) {
  shared_ptr<VideoImageParaT> origin_image = make_shared<VideoImageParaT>();
  origin_image->video_image_info = inference_result->video_image
      .video_image_info;
  origin_image->img = inference_result->video_image.img;

  shared_ptr<VideoDetectionImageParaT> detection_image = make_shared<
      VideoDetectionImageParaT>();

  detection_image->image = inference_result->video_image;

  if (inference_result->output_datas.size() < kInferenceVectorSize) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] image inference out");
    return HIAI_ERROR;
  }

  // output data
  OutputT out_num = inference_result->output_datas[kInferenceOutputNum];
  OutputT out_bbox = inference_result->output_datas[kInferenceOutputBBox];

  if (out_bbox.data == nullptr || out_bbox.data.get() == nullptr
      || out_num.data == nullptr || out_num.data.get() == nullptr) {
    // error
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] image inference out");
    return HIAI_ERROR;
  }

  vector<ObjectImageParaT> face_imgs;

  float* bbox_buffer = reinterpret_cast<float*>(out_bbox.data.get());
  float bbox_number = *reinterpret_cast<float*>(out_num.data.get());
  int32_t bbox_buffer_size = bbox_number * kSizePerResultset;
  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[FaceDetection] number of bbox: %d",
                  bbox_number);

  FilterBoundingBox(bbox_buffer, bbox_buffer_size, detection_image, face_imgs);

  // send_data

  tran_data_->obj_imgs = face_imgs;

  HIAI_StatusT send_ret = SendResults(kPortAge, "BatchCroppedImageParaT",
                                      static_pointer_cast<void>(tran_data_));
  if (send_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetectionInferenceEngine]SendResults to kPortAge failed!");
  }

  send_ret = SendResults(kPortGender, "BatchCroppedImageParaT",
                         static_pointer_cast<void>(tran_data_));
  if (send_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetectionInferenceEngine]SendResults to kPortGender failed!");
  }

  // send jpg to video_analysis_post engine.
  if (SendJpgImage(inference_result->video_image) != HIAI_OK) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetectionInferenceEngine]SendJpgImage is failed!");
    return HIAI_ERROR;
  }

  return HIAI_OK;
}

void FaceDetection::SendCroppedImages(
    uint32_t port_id, const vector<ObjectImageParaT>& cropped_images,
    VideoImageInfoT& video_image_info) {
  if (cropped_images.empty()) {
    return;
  }
  shared_ptr<BatchCroppedImageParaT> object_image = make_shared<
      BatchCroppedImageParaT>();
  object_image->video_image_info = video_image_info;
  object_image->obj_imgs = cropped_images;
  HIAI_StatusT ret = SendResults(port_id, "BatchCroppedImageParaT",
                                 static_pointer_cast<void>(object_image));
  if (ret != HIAI_OK) {
    HIAI_ENGINE_LOG(
        HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetection] send cropped image error channel: %d, frame: %d!",
        video_image_info.channel_id, video_image_info.frame_id);
  }
}

// if input string is a valid number.
bool FaceDetection::InitConfidence(const string& input) {
  istringstream iss(input);
  float tmp;
  iss >> noskipws >> tmp;
  if (!(iss.eof() && !iss.fail())
      || !(tmp >= kMinConfidence && tmp <= kMaxConfidence)) {
    return false;
  }
  confidence_ = tmp;
  return true;
}

HIAI_StatusT FaceDetection::SendResults(uint32_t port_id, string data_type,
                                        const shared_ptr<void>& data_ptr) {
  HIAI_StatusT ret;
  do {
    ret = SendData(port_id, data_type, data_ptr);
    if (ret == HIAI_QUEUE_FULL) {
      HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[FaceDetection] port %d queue full.",
                      port_id);
      usleep(kSleepMicroSecs);
    }
  } while (ret == HIAI_QUEUE_FULL);
  if (ret != HIAI_OK) {
    return HIAI_ERROR;
  }
  return HIAI_OK;
}

HIAI_StatusT FaceDetection::SendDetectImage(
    const shared_ptr<VideoDetectionImageParaT> &image_para) {
  ascend::utils::DvppToJpgPara dvpp_to_jpg_para;
  dvpp_to_jpg_para.format = JPGENC_FORMAT_NV12;

  // use dvpp convert yuv to jpg image, level should set fixed value 100
  dvpp_to_jpg_para.level = 100;
  dvpp_to_jpg_para.resolution.height = image_para->image.img.height;
  dvpp_to_jpg_para.resolution.width = image_para->image.img.width;

  // true indicate the image is aligned
  dvpp_to_jpg_para.is_align_image = true;
  ascend::utils::DvppProcess dvpp_jpg_process(dvpp_to_jpg_para);
  ascend::utils::DvppOutput dvpp_output = { 0 };

  // use dvpp convert yuv image to jpg image
  int ret_dvpp;
  ret_dvpp = dvpp_jpg_process.DvppOperationProc(
      (char*) (image_para->image.img.data.get()), image_para->image.img.size,
      &dvpp_output);
  if (ret_dvpp == kDvppOperationOk) {
    image_para->image.img.data.reset(dvpp_output.buffer,
                                     default_delete<uint8_t[]>());
    image_para->image.img.size = dvpp_output.size;
  } else {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "fail to convert yuv to jpg,ret_dvpp = %d", ret_dvpp);
    return HIAI_ERROR;
  }

  // get small images after reasoning
  for (vector<ObjectImageParaT>::iterator iter = image_para->obj_imgs.begin();
      iter != image_para->obj_imgs.end(); ++iter) {
    // use dvpp convert yuv image to jpg image
    ascend::utils::DvppToJpgPara dvpp_to_jpg_obj_para;
    dvpp_to_jpg_obj_para.format = JPGENC_FORMAT_NV12;

    // level should set fixed value 100
    dvpp_to_jpg_obj_para.level = 100;

    // true indicate the image is aligned
    dvpp_to_jpg_obj_para.is_align_image = true;
    dvpp_to_jpg_obj_para.resolution.height = iter->img.height;
    dvpp_to_jpg_obj_para.resolution.width = iter->img.width;
    ascend::utils::DvppProcess dvpp_jpg_objprocess(dvpp_to_jpg_obj_para);
    ascend::utils::DvppOutput obj_dvpp_output = { 0 };
    ret_dvpp = dvpp_jpg_objprocess.DvppOperationProc(
        (char*) (iter->img.data.get()), iter->img.size, &obj_dvpp_output);
    if (ret_dvpp == kDvppOperationOk) {
      iter->img.data.reset(obj_dvpp_output.buffer, default_delete<uint8_t[]>());
      iter->img.size = obj_dvpp_output.size;
    } else {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "fail to convert obj yuv to jpg,ret_dvpp = %d", ret_dvpp);
      return HIAI_ERROR;
    }
  }

  return SendResults(kPortPost, "VideoDetectionImageParaT",
                     static_pointer_cast<void>(image_para));
}

float FaceDetection::CorrectCoordinate(float value) {
  float tmp = value < kLowerCoord ? kLowerCoord : value;
  return tmp > kUpperCoord ? kUpperCoord : tmp;
}

HIAI_StatusT FaceDetection::ImagePreProcess(const ImageData<u_int8_t>& src_img,
                                            ImageData<u_int8_t>& resized_img) {
  if (src_img.format != IMAGEFORMAT::YUV420SP) {
    // input image must be yuv420sp nv12.
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] input image type does not match");
    return HIAI_ERROR;
  }

  ascend::utils::DvppBasicVpcPara dvpp_basic_vpc_para;

  /**
   * when use dvpp_process only for resize function:
   *
   * 1.DVPP limits crop_left and crop_right should be Odd number,
   * if it is even number, subtract 1, otherwise Equal to origin width
   * or height.
   *
   * 2.crop_up and crop_down should be set to zero.
   */
  dvpp_basic_vpc_para.input_image_type = INPUT_YUV420_SEMI_PLANNER_UV;  // nv12
  dvpp_basic_vpc_para.output_image_type = OUTPUT_YUV420SP_UV;  // nv12
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
  dvpp_basic_vpc_para.is_output_align = true;

  ascend::utils::DvppProcess dvpp_process(dvpp_basic_vpc_para);

  ascend::utils::DvppVpcOutput dvpp_out;
  int ret = dvpp_process.DvppBasicVpcProc(src_img.data.get(),
                                          (int32_t) src_img.size, &dvpp_out);
  if (ret != kDvppProcSuccess) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] call dvpp resize failed with code %d!",
                    ret);
    return HIAI_ERROR;
  }

  // dvpp_out->pbuf
  resized_img.data.reset(dvpp_out.buffer, default_delete<uint8_t[]>());
  resized_img.size = dvpp_out.size;

  return HIAI_OK;
}

HIAI_StatusT FaceDetection::PerformInference(
    shared_ptr<DetectionEngineTransT> &detection_trans,
    ImageData<u_int8_t> &input_img) {
  // init neural buffer.
  shared_ptr<hiai::AINeuralNetworkBuffer> neural_buffer = shared_ptr<
      hiai::AINeuralNetworkBuffer>(
      new hiai::AINeuralNetworkBuffer(),
      default_delete<hiai::AINeuralNetworkBuffer>());

  neural_buffer->SetBuffer((void*) input_img.data.get(), input_img.size);

  shared_ptr<hiai::IAITensor> input_tensor =
      static_pointer_cast<hiai::IAITensor>(neural_buffer);

  vector < shared_ptr < hiai::IAITensor >> input_tensors;
  vector < shared_ptr < hiai::IAITensor >> output_tensors;
  input_tensors.push_back(input_tensor);

  HIAI_StatusT ret = hiai::SUCCESS;
  ret = ai_model_manager_->CreateOutputTensor(input_tensors, output_tensors);
  if (ret != hiai::SUCCESS) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] output tensor created failed!");
    return HIAI_ERROR;
  }
  hiai::AIContext ai_context;

  // neural network inference.
  ret = ai_model_manager_->Process(ai_context, input_tensors, output_tensors,
                                   0);
  if (ret != hiai::SUCCESS) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] image inference failed!");
    return HIAI_ERROR;
  }

  // set trans_data
  detection_trans->status = true;
  for (uint32_t index = 0; index < output_tensors.size(); ++index) {
    shared_ptr<hiai::AINeuralNetworkBuffer> result_tensor = static_pointer_cast<
        hiai::AINeuralNetworkBuffer>(output_tensors[index]);
    OutputT out;
    out.size = result_tensor->GetSize();
    out.data = std::shared_ptr<uint8_t>(new uint8_t[out.size],
                                        std::default_delete<uint8_t[]>());
    errno_t ret = memcpy_s(out.data.get(), out.size, result_tensor->GetBuffer(),
                           out.size);
    if (ret != EOK) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[FaceDetection] output tensor copy failure!");
      continue;
    }
    detection_trans->output_datas.push_back(out);
  }
  if (detection_trans->output_datas.empty()) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[FaceDetection] result tensor is empty!");
    return HIAI_ERROR;
  }

  // sendData
  return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("face_detection", FaceDetection, INPUT_SIZE) {
  HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[FaceDetection] start process!");

  if (arg0 == nullptr) {
    // inputer data is nullptr.
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetection] input data is null!");
    return HIAI_ERROR;
  }
  shared_ptr<VideoImageParaT> video_image =
  static_pointer_cast<VideoImageParaT>(arg0);

  // init inference results tensor shared_ptr.
  shared_ptr<DetectionEngineTransT> detection_trans = make_shared<
  DetectionEngineTransT>();

  detection_trans->video_image = *video_image;

  if (tran_data_ == nullptr) {
    tran_data_ = std::make_shared<BatchCroppedImageParaT>();
  }
  tran_data_->video_image_info = video_image->video_image_info;

  if (tran_jpg_data_ == nullptr) {
    tran_jpg_data_ = std::make_shared<VideoDetectionImageParaT>();
  }
  tran_jpg_data_->image.video_image_info = video_image->video_image_info;

  if (detection_trans->video_image.video_image_info.is_finished) {
    // input is finished.
    HIAI_ENGINE_LOG(HIAI_DEBUG_INFO, "[FaceDetection] input video finished!");
    SendResults(kPortAge, "BatchCroppedImageParaT",
        static_pointer_cast<void>(tran_data_));

    SendResults(kPortGender, "BatchCroppedImageParaT",
        static_pointer_cast<void>(tran_data_));

    SendResults(kPortPost, "VideoDetectionImageParaT",
        static_pointer_cast<void>(tran_jpg_data_));

    return HIAI_OK;
  }

  // 1、resize input image.
  ImageData<u_int8_t> resized_img;
  HIAI_StatusT dvpp_ret = ImagePreProcess(detection_trans->video_image.img,
      resized_img);

  if (dvpp_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetection]ImagePreProcess is failed!");
    tran_data_ = nullptr;
    tran_jpg_data_ = nullptr;

    return HIAI_ERROR;
  }

  // 2、inference
  dvpp_ret = PerformInference(detection_trans, resized_img);

  if (dvpp_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetection]PerformInference is failed!");
    tran_data_ = nullptr;
    tran_jpg_data_ = nullptr;

    return HIAI_ERROR;
  }

  // 3、postprecess
  dvpp_ret = HandleResults(detection_trans);
  if (dvpp_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[FaceDetection]HandleResults is failed!");
    tran_data_ = nullptr;
    tran_jpg_data_ = nullptr;

    return HIAI_ERROR;
  }

  // 4、release sources
  tran_data_ = nullptr;
  tran_jpg_data_ = nullptr;

  HIAI_ENGINE_LOG("[FaceDetection] end process!");
  return HIAI_OK;
}
