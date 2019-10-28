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
#ifndef FACE_DETECTION_H_
#define FACE_DETECTION_H_
#include <string>
#include <vector>
#include "hiaiengine/ai_model_manager.h"
#include "hiaiengine/ai_tensor.h"
#include "hiaiengine/ai_types.h"
#include "hiaiengine/api.h"
#include "hiaiengine/data_type.h"
#include "hiaiengine/data_type_reg.h"
#include "hiaiengine/engine.h"
#include "video_analysis_params.h"

#define INPUT_SIZE 2
#define OUTPUT_SIZE 3

struct BoundingBox {
  uint32_t lt_x;
  uint32_t lt_y;
  uint32_t rb_x;
  uint32_t rb_y;
};

class FaceDetection : public hiai::Engine {
 public:

  /**
   * @brief constructor
   */
  FaceDetection()
      : ai_model_manager_(nullptr),
        confidence_(0),
        tran_data_(nullptr),
        tran_jpg_data_(nullptr) {

  }
  ~ FaceDetection() {
  }
  /**
   * @brief Engine init method.
   * @return HIAI_StatusT
   */
  HIAI_StatusT Init(const hiai::AIConfig& config,
                    const std::vector<hiai::AIModelDescription>& model_desc);

  /**
   * @ingroup hiaiengine
   * @brief HIAI_DEFINE_PROCESS : override Engine Process logic.
   * @[in]: define a input port, a output port
   * @return HIAI_StatusT
   */
  HIAI_DEFINE_PROCESS(INPUT_SIZE, OUTPUT_SIZE)
  ;

 private:
  /**
   * @brief : image preprocess function.
   * @param [in] src_img: input image data.
   * @param [out] resized_img: resized image data.
   * @return HIAI_StatusT
   */
  HIAI_StatusT ImagePreProcess(const hiai::ImageData<u_int8_t>& src_img,
                               hiai::ImageData<u_int8_t>& resized_img);

  /**
   * @brief : object detection function.
   * @param [in] input_img: input image data.
   * @param [out] detection_trans: inference results tensor.
   * @return HIAI_StatusT
   */
  HIAI_StatusT PerformInference(
      std::shared_ptr<DetectionEngineTransT>& detection_trans,
      hiai::ImageData<u_int8_t>& input_img);

  /**
   * @brief : send inference results to next engine.
   * @param [in] detection_trans: inference results tensor.
   * @param [in] success: inference success or not.
   * @param [in] err_msg: save error message if detection failed.
   * @return HIAI_StatusT
   */
  HIAI_StatusT SendDetectionResult(
      std::shared_ptr<DetectionEngineTransT>& detection_trans,
      bool inference_success = true, std::string err_msg = "");

  /**
   * @brief : send inference results to next engine.
   * @param [in] detection_trans: inference results tensor.
   * @param [in] success: inference success or not.
   * @param [in] err_msg: save error message if detection failed.
   * @return HIAI_StatusT
   */

  /**
   * @brief  send Jpg data to video_analysis_post engine.
   * @return  success --> HIAI_OK ; fail --> HIAI_ERROR
   */
  HIAI_StatusT SendJpgImage(const VideoImageParaT image_input);

  HIAI_StatusT HandleResults(
      const std::shared_ptr<DetectionEngineTransT>& inference_result);
  /**
   * @brief : crop object image from input image.
   * @param [in] src_img: input image.
   * @param [out] target_img: output object image.
   * @param [in] bbox: bounding box coordinate.
   * @return HIAI_StatusT
   */
  HIAI_StatusT CropObjectFromImage(const hiai::ImageData<u_int8_t>& src_img,
                                   hiai::ImageData<u_int8_t>& target_img,
                                   const BoundingBox& bbox);
  /**
   * @brief : filter bounding box from inferece results.
   * @param [in] bbox_buffer: bbox results buffer.
   * @param [in] bbox_buffer_size: bbox buffer size.
   * @param [out] detection_image: detection results shared_ptr.
   * @param [out] car_type_imgs: car type imags vector.
   * @param [out] car_color_imgs: car color images vector.
   * @param [out] person_imgs: person images vector.
   */
  void FilterBoundingBox(
      float* bbox_buffer, int32_t bbox_buffer_size,
      std::shared_ptr<VideoDetectionImageParaT>& detection_image,
      vector<ObjectImageParaT>& face_imgs);
  /**
   * @brief : send results to output port.
   * @param [in] port_id: output port id.
   * @param [in] data_type: output data type.
   * @param [in] data_ptr: output data shared ptr.
   * @return HIAI_StatusT
   */
  HIAI_StatusT SendResults(uint32_t port_id, std::string data_type,
                           const std::shared_ptr<void>& data_ptr);

  /**
   * @brief : send image results to post port.
   * @param [in] image_para: output data shared ptr.
   * @return HIAI_StatusT
   */
  HIAI_StatusT SendDetectImage(
      const std::shared_ptr<VideoDetectionImageParaT> &image_para);

  /**
   * @brief : send object image to next engine.
   * @param [in] port_id: output port id.
   * @param [in] cropped_images: object image vector.
   * @param [in] video_image_info: input image attr params.
   * @return HIAI_StatusT
   */
  void SendCroppedImages(uint32_t port_id,
                         const std::vector<ObjectImageParaT>& cropped_images,
                         VideoImageInfoT& video_image_info);
  /**
   * @brief : check if input string is a valid number.
   * @param [in] input: input string .
   * @return true or false.
   */
  bool InitConfidence(const string& input);

  /**
   * @brief : correct the coordinate value between 0.0f and 1.0f.
   * @param [in] input: coordinate value .
   * @return value bewteen 0.0 and 1.0.
   */
  float CorrectCoordinate(float value);

  // face detection confidence
  float confidence_;
  // shared ptr to load ai model.
  std::shared_ptr<hiai::AIModelManager> ai_model_manager_;
  // the data be sent to next inference engine.
  std::shared_ptr<BatchCroppedImageParaT> tran_data_;
  // the data be sent to next videoanalysis_post engine.
  std::shared_ptr<VideoDetectionImageParaT> tran_jpg_data_;
};

#endif /* OBJECT_DETECTION_OBJECT_DETECTION_H_ */
