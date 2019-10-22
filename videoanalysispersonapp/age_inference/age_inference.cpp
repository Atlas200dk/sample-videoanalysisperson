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
#include "age_inference.h"

#include <thread>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <math.h>
#include <unistd.h>

#include <hiaiengine/log.h>
#include <hiaiengine/ai_types.h>
#include "hiaiengine/ai_model_parser.h"

using namespace std;
namespace {
// total number of inference results
const int kClassSum = 8;
// age type kind
const string kAgeClass[kClassSum] = { "0~2", "4~6", "8~12", "15~20", "25~32",
    "38~43", "48~53", "60~100" };
// time for waitting when send queue is full.
const int kWaitTime = 20000;
// the image width for model.
const int kDestImageWidth = 228;
// the image height for model.
const int kDestImageHeight = 228;
// the name of model_path in the config file
const string kModelPathItemName = "model_path";
// the name of passcode in the config file
const string kPasscodeItemName = "passcode";
// the name of batch_size in the config file
const string kBatchSizeItemName = "batch_size";
}

HIAI_REGISTER_DATA_TYPE("BatchFaceInfoT", BatchFaceInfoT);
HIAI_REGISTER_DATA_TYPE("VideoImageInfoT", VideoImageInfoT);
HIAI_REGISTER_DATA_TYPE("FaceInfoT", FaceInfoT);
HIAI_REGISTER_DATA_TYPE("BatchCroppedImageParaT", BatchCroppedImageParaT);

HIAI_StatusT AgeInferenceEngine::Init(
    const hiai::AIConfig& config,
    const std::vector<hiai::AIModelDescription>& model_desc) {
  HIAI_ENGINE_LOG("[AgeInferenceEngine] start init!");
  hiai::AIStatus ret = hiai::SUCCESS;

  if (ai_model_manager_ == nullptr) {
    MAKE_SHARED_NO_THROW(ai_model_manager_, hiai::AIModelManager);
    if (ai_model_manager_ == nullptr) {
      HIAI_ENGINE_LOG(
          HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
          "[AgeInferenceEngine] Failed to initialize AIModelManager.");
      return HIAI_ERROR;
    }
  }

  std::vector<hiai::AIModelDescription> model_desc_vec;
  hiai::AIModelDescription model_description;

  for (int index = 0; index < config.items_size(); ++index) {

    const ::hiai::AIConfigItem& item = config.items(index);

    if (item.name() == kModelPathItemName) {
      const char* model_path = item.value().data();
      model_description.set_path(model_path);

    } else if (item.name() == kPasscodeItemName) {
      const char* passcode = item.value().data();
      model_description.set_key(passcode);
    } else if (item.name() == kBatchSizeItemName) {
      std::stringstream ss(item.value());
      ss >> batch_size_;
      if (batch_size_ <= 0) {
        HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                        "[AgeInferenceEngine] batch_size_ is [%d]",
                        batch_size_);
        return HIAI_ERROR;
      }
    }
  }

  model_desc_vec.push_back(model_description);
  ret = ai_model_manager_->Init(config, model_desc_vec);
  if (ret != hiai::SUCCESS) {
    return HIAI_ERROR;
  }

  HIAI_ENGINE_LOG("[AgeInferenceEngine] end init!");
  return HIAI_OK;
}

HIAI_StatusT AgeInferenceEngine::SendResultData(
    const std::shared_ptr<BatchFaceInfoT>& tran_data) {
  HIAI_StatusT hiai_ret = HIAI_OK;

  do {
    // this engine only have one outport, this port parameter be set to zero.
    hiai_ret = SendData(0, "BatchFaceInfoT",
                        std::static_pointer_cast<void>(tran_data));
    if (hiai_ret == HIAI_QUEUE_FULL) {
      HIAI_ENGINE_LOG("[AgeInferenceEngine] queue full");
      usleep(kWaitTime);
    }
  } while (hiai_ret == HIAI_QUEUE_FULL);

  if (hiai_ret != HIAI_OK) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[AgeInferenceEngine] send finished data failed!");
    return HIAI_ERROR;
  }

  return HIAI_OK;
}

void AgeInferenceEngine::BatchImageResize(
    const std::shared_ptr<BatchCroppedImageParaT>& batch_image_input,
    std::shared_ptr<BatchCroppedImageParaT>& batch_image_output) {
  batch_image_output->video_image_info = batch_image_input->video_image_info;
  // resize for each image

  for (std::vector<ObjectImageParaT>::iterator iter = batch_image_input
      ->obj_imgs.begin(); iter != batch_image_input->obj_imgs.end(); ++iter) {
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
    dvpp_basic_vpc_para.src_resolution.width = (int) iter->img.width;
    dvpp_basic_vpc_para.src_resolution.height = (int) iter->img.height;
    // DVPP limits crop_left should be even number, 0 means without crop
    dvpp_basic_vpc_para.crop_left = 0;
    // DVPP limits crop_right should be Odd number
    dvpp_basic_vpc_para.crop_right =
        iter->img.width % 2 == 0 ? iter->img.width - 1 : iter->img.width;
    // DVPP limits crop_up should be even number, 0 means without crop
    dvpp_basic_vpc_para.crop_up = 0;
    // DVPP limits crop_down should be Odd number
    dvpp_basic_vpc_para.crop_down =
        iter->img.height % 2 == 0 ? iter->img.height - 1 : iter->img.height;
    dvpp_basic_vpc_para.is_input_align = true;
    dvpp_basic_vpc_para.is_output_align = true;
    dvpp_basic_vpc_para.dest_resolution.width = kDestImageWidth;
    dvpp_basic_vpc_para.dest_resolution.height = kDestImageHeight;

    ascend::utils::DvppProcess dvpp_process(dvpp_basic_vpc_para);
    ascend::utils::DvppVpcOutput dvpp_out;
    int ret = dvpp_process.DvppBasicVpcProc(iter->img.data.get(),
                                            (int32_t) iter->img.size,
                                            &dvpp_out);
    if (ret != ascend::utils::kDvppOperationOk) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] resize image failed with code %d !",
                      ret);
      continue;
    }

    std::shared_ptr<ObjectImageParaT> obj_image;
    MAKE_SHARED_NO_THROW(obj_image, ObjectImageParaT);
    if (obj_image == nullptr) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine]obj_image make_shared failed.");
      continue;
    }
    obj_image->object_info.object_id = iter->object_info.object_id;
    obj_image->object_info.score = iter->object_info.score;
    obj_image->img.width = kDestImageWidth;
    obj_image->img.height = kDestImageHeight;
    obj_image->img.channel = iter->img.channel;
    obj_image->img.depth = iter->img.depth;
    obj_image->img.size = dvpp_out.size;
    obj_image->img.data.reset(dvpp_out.buffer,
                              std::default_delete<uint8_t[]>());
    batch_image_output->obj_imgs.push_back(*obj_image);
  }
}

bool AgeInferenceEngine::ConstructBatchBuffer(
    int batch_index,
    const std::shared_ptr<BatchCroppedImageParaT>& image_handle,
    uint8_t* temp) {
  bool is_successed = true;
  int image_number = image_handle->obj_imgs.size();
  int image_size = image_handle->obj_imgs[0].img.size * sizeof(uint8_t);

  //the loop for each image
  for (int j = 0; j < batch_size_; j++) {
    if (batch_index + j < image_number) {
      errno_t err = memcpy_s(
          temp + j * image_size, image_size,
          image_handle->obj_imgs[batch_index + j].img.data.get(), image_size);
      if (err != EOK) {
        HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                        "[AgeInferenceEngine] copy image buffer failed");
        is_successed = false;
        break;
      }
    } else {
      errno_t err = memset_s(temp + j * image_size, image_size,
                             static_cast<char>(0), image_size);
      if (err != EOK) {
        HIAI_ENGINE_LOG(
            HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
            "[AgeInferenceEngine] batch padding for image data failed");
        is_successed = false;
        break;
      }
    }
  }

  return is_successed;
}

bool AgeInferenceEngine::ConstructInferenceResult(
    const std::vector<std::shared_ptr<hiai::IAITensor> >& output_data_vec,
    int batch_index,
    const std::shared_ptr<BatchCroppedImageParaT>& image_handle,
    const std::shared_ptr<BatchFaceInfoT>& tran_data) {
  if (output_data_vec.size() == 0) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                    "[AgeInferenceEngine] output_data_vec is null!");
    return false;
  }

  int image_number = image_handle->obj_imgs.size();

  for (int n = 0; n < output_data_vec.size(); ++n) {
    std::shared_ptr<hiai::AINeuralNetworkBuffer> result_tensor =
        std::static_pointer_cast<hiai::AINeuralNetworkBuffer>(
            output_data_vec[n]);
    //get confidence result
    int size = result_tensor->GetSize() / sizeof(float);
    float* result = (float*) result_tensor->GetBuffer();
    int batch_inference_num = size / batch_size_;

    // analyze each batch result
    for (int batch_result_index = 0; batch_result_index < size;
        batch_result_index += batch_inference_num) {
      double result_tmp[kClassSum] = { 0 };
      double soft_sum = 0;
      float softmax_result = 0;

      //find max confidence for each image
      int max_confidence_index = 0;
      for (int index = 0; index < batch_inference_num; index++) {
        if (*(result + batch_result_index + index)
            > *(result + batch_result_index + max_confidence_index)) {
          max_confidence_index = index;
        }
        result_tmp[index] = exp(*(result + batch_result_index + index));
        soft_sum = soft_sum + result_tmp[index];
      }
      softmax_result = (float) (result_tmp[max_confidence_index] / soft_sum);

      // creat out struct for each batch
      FaceInfoT out;
      if (batch_index + batch_result_index / batch_inference_num
          < image_number) {
        out.object_id = image_handle->obj_imgs[batch_index
            + batch_result_index / batch_inference_num].object_info.object_id;
        out.label = max_confidence_index;
        out.attribute_name = kAge;
        out.inference_result = kAgeClass[max_confidence_index];
        out.confidence = softmax_result;
        tran_data->face_infos.push_back(out);
      }
    }
  }
  return true;
}

HIAI_StatusT AgeInferenceEngine::BatchInferenceProcess(
    const std::shared_ptr<BatchCroppedImageParaT>& image_handle,
    std::shared_ptr<BatchFaceInfoT> tran_data) {
  HIAI_ENGINE_LOG("[AgeInferenceEngine] start process!");

  hiai::AIStatus ret = hiai::SUCCESS;
  HIAI_StatusT hiai_ret = HIAI_OK;
  int image_number = image_handle->obj_imgs.size();
  int image_size = image_handle->obj_imgs[0].img.size * sizeof(uint8_t);
  int batch_buffer_size = image_size * batch_size_;

  // the loop for each batch ,maybe image_number greater than batch_size_
  for (int i = 0; i < image_number; i += batch_size_) {

    std::vector<std::shared_ptr<hiai::IAITensor> > input_data_vec;
    std::vector<std::shared_ptr<hiai::IAITensor> > output_data_vec;
    //1.prepare input buffer for each batch
    if (tran_data == nullptr) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] tran_data is nullptr.");
      return HIAI_ERROR;
    }

    //  apply buffer for each batch
    uint8_t* temp = new (nothrow) uint8_t[batch_buffer_size];
    if (temp == nullptr) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] new batch_buffer_size failed!");
      return HIAI_ERROR;
    }

    // Origin image information is transmitted to next Engine directly
    tran_data->video_image_info = image_handle->video_image_info;

    bool is_successed = ConstructBatchBuffer(i, image_handle, temp);
    if (!is_successed) {
      HIAI_ENGINE_LOG(
          HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
          "[AgeInferenceEngine] batch input buffer construct failed!");
      delete[] temp;
      return HIAI_ERROR;
    }

    std::shared_ptr<hiai::AINeuralNetworkBuffer> neural_buffer =
        std::shared_ptr<hiai::AINeuralNetworkBuffer>(
            new hiai::AINeuralNetworkBuffer());
    neural_buffer->SetBuffer((void*) (temp), batch_buffer_size);
    std::shared_ptr<hiai::IAITensor> input_data = std::static_pointer_cast<
        hiai::IAITensor>(neural_buffer);
    input_data_vec.push_back(input_data);

    // 2.Call Process, Predict
    ret = ai_model_manager_->CreateOutputTensor(input_data_vec,
                                                output_data_vec);
    if (ret != hiai::SUCCESS) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] CreateOutputTensor failed");
      delete[] temp;
      return HIAI_ERROR;
    }
    hiai::AIContext ai_context;
    HIAI_ENGINE_LOG("[AgeInferenceEngine] ai_model_manager_->Process start!");
    ret = ai_model_manager_->Process(ai_context, input_data_vec,
                                     output_data_vec, 0);
    if (ret != hiai::SUCCESS) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] ai_model_manager Process failed");
      delete[] temp;
      return HIAI_ERROR;
    }
    delete[] temp;
    input_data_vec.clear();

    //3.set the tran_data with the result of this batch

    is_successed = ConstructInferenceResult(output_data_vec, i, image_handle,
                                            tran_data);

    if (!is_successed) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] batch copy output buffer failed!");
      return HIAI_ERROR;
    }
    //4. send the result
    hiai_ret = SendResultData(tran_data);
    if (hiai_ret != HIAI_OK) {
      HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
                      "[AgeInferenceEngine] SendData failed! error code: %d",
                      hiai_ret);
    }

    //5. release sources
    tran_data = nullptr;
  }
  HIAI_ENGINE_LOG("[AgeInferenceEngine] end process!");

  return HIAI_OK;
}

HIAI_IMPL_ENGINE_PROCESS("age_inference", AgeInferenceEngine,
    INPUT_SIZE) {
  HIAI_StatusT hiai_ret = HIAI_OK;
  std::shared_ptr<BatchFaceInfoT> tran_data;
  MAKE_SHARED_NO_THROW(tran_data, BatchFaceInfoT);
  if (tran_data == nullptr) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[AgeInferenceEngine] tran_data make_shared failed.");
    return HIAI_ERROR;
  }

  std::shared_ptr<BatchCroppedImageParaT> image_handle;
  MAKE_SHARED_NO_THROW(image_handle, BatchCroppedImageParaT);
  if (image_handle == nullptr) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[AgeInferenceEngine] image_handle make_shared failed.");
    return HIAI_ERROR;
  }

  if (arg0 == nullptr) {
    HIAI_ENGINE_LOG(HIAI_ENGINE_RUN_ARGS_NOT_RIGHT,
        "[AgeInferenceEngine] input data is null!");
    return HIAI_ERROR;
  }

  // this engine only need one queue, so the port should be set to zero.
  std::shared_ptr<BatchCroppedImageParaT> image_input = static_pointer_cast<
  BatchCroppedImageParaT>(arg0);

  if (image_input == nullptr) {
    HIAI_ENGINE_LOG("[AgeInferenceEngine] image_input is nullptr");
    return HIAI_ERROR;
  }

  // add is_finished for showing this data in dataset are all sended.
  if (image_input->video_image_info.is_finished) {
    tran_data->video_image_info = image_input->video_image_info;
    return SendResultData(tran_data);
  }

  // resize input image;
  BatchImageResize(image_input, image_handle);
  if (image_handle->obj_imgs.empty()) {
    HIAI_ENGINE_LOG("[AgeInferenceEngine] image_input resize failed");
    return HIAI_ERROR;
  }
  // inference and send inference result;
  return BatchInferenceProcess(image_handle, tran_data);
}
