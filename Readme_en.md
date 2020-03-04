EN|[CN](Readme.md)

# People Detection<a name="EN-US_TOPIC_0167609412"></a>

Developers can deploy the application on the Atlas 200 DK or the AI acceleration cloud server to decode the local MP4 file or RTSP video streams, detect pedestrians, and human faces in video frames, predict their attributes, generate structured information, and send the structured information to the server for storage and display.

## Prerequisites<a name="en-us_topic_0182554628_section137245294533"></a>

Before using an open source application, ensure that:

-   Mind Studio  has been installed.
-   The Atlas 200 DK developer board has been connected to  Mind Studio, the cross compiler has been installed, the SD card has been prepared, and basic information has been configured.

## Software Preparation<a name="en-us_topic_0182554628_section8534138124114"></a>

Before running the application, obtain the source code package and configure the environment as follows.

1.  <a name="en-us_topic_0182554628_li953280133816"></a>Obtain the source code package.

    Download all the code in the sample-videoanalysisperson repository at[https://github.com/Atlas200DKTest/sample-videoanalysisperson/tree/1.3x.0.0/](https://github.com/Atlas200DKTest/sample-videoanalysisperson/tree/1.3x.0.0/) to any directory on Ubuntu Server where  Mind Studio  is located as the  Mind Studio  installation user, for example, $HOME/AscendProjects/sample-videoanalysisperson.

2.  <a name="en-us_topic_0182554628_li5507119145914"></a>Obtain the source network model required by the application. 

    Obtain the source network model and its weight file used in the application by referring to  [Table 1](#en-us_topic_0182554628_table1193115345597), and save them to any directory on the Ubuntu server where  Mind Studio  is located, these two files must be saved in the same directory(for example,  **$HOME/models/videoanalysispersion**).

    **Table  1**  Models used in the people detection application
  
    <a name="en-us_topic_0182554628_table1193115345597"></a>
    <table><thead align="left"><tr id="en-us_topic_0182554628_row1187103505916"><th class="cellrowborder" valign="top" width="15.17%" id="mcps1.2.4.1.1"><p id="en-us_topic_0182554628_p887235105910"><a name="en-us_topic_0182554628_p887235105910"></a><a name="en-us_topic_0182554628_p887235105910"></a>Model Name</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.47%" id="mcps1.2.4.1.2"><p id="en-us_topic_0182554628_p16877355598"><a name="en-us_topic_0182554628_p16877355598"></a><a name="en-us_topic_0182554628_p16877355598"></a>Model Description</p>
    </th>
    <th class="cellrowborder" valign="top" width="74.36%" id="mcps1.2.4.1.3"><p id="en-us_topic_0182554628_p18713511598"><a name="en-us_topic_0182554628_p18713511598"></a><a name="en-us_topic_0182554628_p18713511598"></a>Model Download Path</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="en-us_topic_0182554628_row3881635175910"><td class="cellrowborder" valign="top" width="15.17%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0182554628_p48863512592"><a name="en-us_topic_0182554628_p48863512592"></a><a name="en-us_topic_0182554628_p48863512592"></a>face_detection</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.47%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0182554628_p1541603617318"><a name="en-us_topic_0182554628_p1541603617318"></a><a name="en-us_topic_0182554628_p1541603617318"></a>Network model for face detection.</p>
    <p id="en-us_topic_0182554628_p18531438317"><a name="en-us_topic_0182554628_p18531438317"></a><a name="en-us_topic_0182554628_p18531438317"></a>It is a network model converted from ResNet10-SSD300 model based on Caffe.</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0182554628_p1588203519592"><a name="en-us_topic_0182554628_p1588203519592"></a><a name="en-us_topic_0182554628_p1588203519592"></a>Download the source network model file and its weight file by referring to<strong id="en-us_topic_0182554628_b820812515127"><a name="en-us_topic_0182554628_b820812515127"></a><a name="en-us_topic_0182554628_b820812515127"></a> README.md</strong> in <a href="https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/object_detect/face_detection" target="_blank" rel="noopener noreferrer">https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/object_detect/face_detection</a>.</p>
    </td>
    </tr>
    <tr id="en-us_topic_0182554628_row98823585915"><td class="cellrowborder" valign="top" width="15.17%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0182554628_p1188103555919"><a name="en-us_topic_0182554628_p1188103555919"></a><a name="en-us_topic_0182554628_p1188103555919"></a>pedestrian</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.47%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0182554628_p18889356598"><a name="en-us_topic_0182554628_p18889356598"></a><a name="en-us_topic_0182554628_p18889356598"></a>Network model for performing inference on human body features.</p>
    <p id="en-us_topic_0182554628_p123011321411"><a name="en-us_topic_0182554628_p123011321411"></a><a name="en-us_topic_0182554628_p123011321411"></a>It is a VeSPA model based on Caffe.</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0182554628_p5881335145918"><a name="en-us_topic_0182554628_p5881335145918"></a><a name="en-us_topic_0182554628_p5881335145918"></a>Download the source network model file and its weight file by referring to<strong id="en-us_topic_0182554628_b92101751151219"><a name="en-us_topic_0182554628_b92101751151219"></a><a name="en-us_topic_0182554628_b92101751151219"></a> README.md</strong> in <a href="https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/classification/pedestrian" target="_blank" rel="noopener noreferrer">https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/classification/pedestrian</a>.</p>
    </td>
    </tr>
    <tr id="en-us_topic_0182554628_row388153512593"><td class="cellrowborder" valign="top" width="15.17%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0182554628_p188811350596"><a name="en-us_topic_0182554628_p188811350596"></a><a name="en-us_topic_0182554628_p188811350596"></a>inception_age</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.47%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0182554628_p138803555914"><a name="en-us_topic_0182554628_p138803555914"></a><a name="en-us_topic_0182554628_p138803555914"></a>Network model for identifying the age.</p>
    <p id="en-us_topic_0182554628_p147610201340"><a name="en-us_topic_0182554628_p147610201340"></a><a name="en-us_topic_0182554628_p147610201340"></a>It is an Inception V3 model based on TensorFlow.</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0182554628_p78813518594"><a name="en-us_topic_0182554628_p78813518594"></a><a name="en-us_topic_0182554628_p78813518594"></a>Download the PB model file of the source network by referring to <strong id="en-us_topic_0182554628_b221210512126"><a name="en-us_topic_0182554628_b221210512126"></a><a name="en-us_topic_0182554628_b221210512126"></a>README.md</strong> in <a href="https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/classification/inception_age" target="_blank" rel="noopener noreferrer">https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/classification/inception_age</a>.</p>
    </td>
    </tr>
    <tr id="en-us_topic_0182554628_row1788163525912"><td class="cellrowborder" valign="top" width="15.17%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0182554628_p1788123518593"><a name="en-us_topic_0182554628_p1788123518593"></a><a name="en-us_topic_0182554628_p1788123518593"></a>inception_gender</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.47%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0182554628_p10884353596"><a name="en-us_topic_0182554628_p10884353596"></a><a name="en-us_topic_0182554628_p10884353596"></a>Network model for identifying the gender.</p>
    <p id="en-us_topic_0182554628_p1039303917418"><a name="en-us_topic_0182554628_p1039303917418"></a><a name="en-us_topic_0182554628_p1039303917418"></a>It is an Inception V3 model based on TensorFlow.</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0182554628_p1988103514599"><a name="en-us_topic_0182554628_p1988103514599"></a><a name="en-us_topic_0182554628_p1988103514599"></a>Download the PB model file of the source network by referring to <strong id="en-us_topic_0182554628_b14213165115124"><a name="en-us_topic_0182554628_b14213165115124"></a><a name="en-us_topic_0182554628_b14213165115124"></a>README.md</strong> in <a href="https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/classification/inception_gender" target="_blank" rel="noopener noreferrer">https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/classification/inception_gender</a>.</p>
    </td>
    </tr>
    <tr id="en-us_topic_0182554628_row19881135115914"><td class="cellrowborder" valign="top" width="15.17%" headers="mcps1.2.4.1.1 "><p id="en-us_topic_0182554628_p13881355593"><a name="en-us_topic_0182554628_p13881355593"></a><a name="en-us_topic_0182554628_p13881355593"></a>vgg_ssd</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.47%" headers="mcps1.2.4.1.2 "><p id="en-us_topic_0182554628_p128819351596"><a name="en-us_topic_0182554628_p128819351596"></a><a name="en-us_topic_0182554628_p128819351596"></a>Network model for object detection.</p>
    <p id="en-us_topic_0182554628_p155398551242"><a name="en-us_topic_0182554628_p155398551242"></a><a name="en-us_topic_0182554628_p155398551242"></a>It is an SSD512 model based on Caffe.</p>
    <p id="en-us_topic_0182554628_p158813355590"><a name="en-us_topic_0182554628_p158813355590"></a><a name="en-us_topic_0182554628_p158813355590"></a></p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="en-us_topic_0182554628_p588183595913"><a name="en-us_topic_0182554628_p588183595913"></a><a name="en-us_topic_0182554628_p588183595913"></a>Download the source network model file and its weight file by referring to<strong id="en-us_topic_0182554628_b92153515122"><a name="en-us_topic_0182554628_b92153515122"></a><a name="en-us_topic_0182554628_b92153515122"></a> README.md</strong> in <a href="https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/object_detect/vgg_ssd" target="_blank" rel="noopener noreferrer">https://github.com/HuaweiAscendTest/models/tree/master/computer_vision/object_detect/vgg_ssd</a>.</p>
    </td>
    </tr>
    </tbody>
    </table>

3. Log in to Ubuntu Server where Mind Studio is located as the Mind Studio installation user, confirm current DDK version and set the environment variable  **DDK\_HOME**, **tools_version**, **NPU_DEVICE_LIB** and **LD_LIBRARY_PATH**.

    1. Find current DDK version
    
        Current DDK version can be obtained by either Mind studio tool or DDK packages.
        
        - Using Mind studio tool
        
            choose **File \> Settings \> System Settings \> Ascend DDK** from the main menu of Mind Studio, DDK version inquiry page will display as [Figure 1]
             
            **Figure 1** DDK version inquiry page<a name="en-us_topic_0182554628_fig138681281084"></a>  
             ![](figures/DDK版本号查询.png "DDK version inquiry page")
             **DDK Version** shows in this page is current DDK version, for example, **1.31.T15.B150**
         - Using DDK package
             
             Obtain DDK version by installed DDK package name.
             
             The format of DDK package name is: **Ascend_DDK-{software version}-{interface version}-x86_64.ubuntu16.04.tar.gz**
             
             Where **software version** represents the DDK version.
             
             For example:
             
             If the name of DDK package is **Ascend_DDK-1.31.T15.B150-1.1.1-x86_64.ubuntu16.04.tar.gz**, the DDK version would be **1.31.T15.B150**.
             
     2. Set the environment variable
         
         **vim ~/.bashrc**
         
         Run the following commands to add the environment variables  **DDK\_HOME**  and  **LD\_LIBRARY\_PATH**  to the last line:

         **export tools_version=1.31.X.X**

         **export DDK_HOME=\$HOME/.mindstudio/huawei/ddk/\$tools_version/ddk**

         **export NPU_DEVICE_LIB=$DDK_HOME/../RC/host-aarch64_Ubuntu16.04.3/lib**

         **export LD_LIBRARY_PATH=$DDK_HOME/lib/x86_64-linux-gcc5.4**
         
         >![](public_sys-resources/icon-note.gif) **NOTE:**   
         >-   **_1.31.X.X_** is the DDK version obtained from [Figure 1](#en-us_topic_0182554628_fig138681281084), it needs be filled according to the inquiry result，for example, **1.31.T15.B150**  
         >-   If the environment variables have been added, skip this step.  
             
         Enter  **:wq!**  to save and exit.

         Run the following command for the environment variable to take effect:

         **source \~/.bashrc**

4.  Convert the source network to a model supported by Ascend AI processor. There are two ways for model conversion:  Mind Studio tool conversion and command line conversion.
    
    -    Model conversion using Mind Studio tool
         1. Choose **Tool \> Convert** Model from the main menu of Mind Studio. The Convert Model page is displayed.
         2. On the **Model** **Convert** page, perform model conversion configuration.
             -    Set **Model File** to model file installed in [Step 2](#en-us_topic_0182554628_li5507119145914), the weight file would be automatically matched and filled in **Weight File**.
             -    Set **Model Name** to model name in [Table 1](#en-us_topic_0182554628_table1193115345597)
             -    During Tensorflow model conversion, the value of **N** in **input shape** needs to be filled manually. The values of **W**,**H**,**C** are default.
             -    **Input Image Size** in AIPP configuration of Pedestrian model need to be modified to 256, 240, 128 \*16 alignment is required here, for **Model Image Format**, select BGR888\_U8.
             
                  **Figure 2** AIPP configuration of Pedestrian model<a name="en-us_topic_0182554628_fig138681281084"></a>  
                  ![](figures/Pedestrian模型转换时AIPP配置.png "AIPP configuration of Pedestrian model")
            
            
            -   The non-default configuration of inception\_age model conversion is as follows:
                -   age\_inference processes 10 images at a time. Therefore, **N** of **Input Shape** must be set to 10 during conversion.

                    **Figure 3**  Nodes configuration of inception\_age model conversion<a name="en-us_topic_0182554628_fig138681281084"></a>  
                    ![](figures/inception_age模型转换时Nodes配置.png "Nodes configuration of inception\_age model conversion")

                -    **Input Image Size** in AIPP configuration of Pedestrian model need to be modified to 256, 240, 128 \*16 alignment is required here, for **Model Image Format**, select BGR888\_U8.

                    **Figure 4**  AIPP configuration of inception\_age model conversion<a name="en-us_topic_0182554628_fig138681281084"></a>  
                    ![](figures/inception_age模型转换时AIPP配置.png " AIPP configuration of inception\_age model conversion")

            -   The non-default configuration of inception\_gender model conversion is as follows:
                -   **gender_inference** processes 10 images at a time. Therefore, **N** of **Input Shape** must be set to 10 during conversion.
                
                    **Figure 5**  Nodes configuration of inception\_gender model conversion<a name="en-us_topic_0182554628_fig138681281084"></a>  
                    ![](figures/inception_gender模型转换时Nodes配置.png "Nodes configuration of inception\_gender model conversion")

                -   **Input Image Size** in AIPP configuration of Pedestrian model need to be modified to 256, 240, 128 \*16 alignment is required here, for **Model Image Format**, select BGR888\_U8.
                    ![](figures/zh-cn_image_0208265214.png)

            -   **Input Image Size** in AIPP configuration of face\_detection model need to be modified to 384, 304, 128 \*16 alignment is required here, for **Model Image Format**, select BGR888\_U8.

                **Figure 6** The non-default configuration of face\_detection model conversion<a name="en-us_topic_0182554628_fig138681281084"></a>  
                ![](figures/face_detection模型转换时非默认配置.png "The non-default configuration of face\_detection model conversion")

                >![](public_sys-resources/icon-note.gif) **NOTE：**   
                > For other unspecified configurations, please use the default configuration.
                
         3. Click **OK** to start model conversion.
         
            During the conversion of the face_detection and vgg_ssd models, the following error will be reported.
            
            **Figure 7** Model conversion error<a name="en-us_topic_0182554628_fig138681281084"></a>  
            ![](figures/模型转换错误信息.png "Model conversion error")
            
             Select SSDDetectionOutput from the Suggestion drop-down list box at the DetectionOutput layer and click Retry.

             After successful conversion, a .om Da Vinci model is generated in the $HOME/modelzoo/xxx/device.
             
             >![](public_sys-resources/icon-note.gif) **NOTE：**   
            >-   Thespecific meaning and parameter description in each step of Mind Studio model conversion can refer to[https://ascend.huawei.com/doc/mindstudio/2.1.0\(beta\)/zh/zh-cn\_topic\_0188462651.html](https://ascend.huawei.com/doc/mindstudio/2.1.0(beta)/zh/zh-cn_topic_0188462651.html)  
            >-   XXX indicates the name of currently converted model, for example, face\_detection.om is saved at: $HOME/modelzoo/face\_detection/device。  

             
    -   Model conversion in command line mode:
        1.  Enter the folder where the source model is saved as the Mind Studio installation user.

            **cd $HOME/models/videoanalysisperson**

        2.  Using omg tool to execute the following command to perform model conversion on the Caffe model to be used in the sample.

            ```
            ${DDK_HOME}/uihost/bin/omg --output="./XXX" --model="./XXX.prototxt" --framework=0 --ddk_version=${tools_version} --weight="./XXX.caffemodel" --input_shape=`head -1 $HOME/AscendProjects/sample-videoanalysisperson/MyModel/shape_XXX` --insert_op_conf=$HOME/AscendProjects/sample-videoanalysisperson/MyModel/aipp_XXX.cfg --op_name_map=$HOME/AscendProjects/sample-videoanalysisperson/MyModel/reassign_operators
            ```

            >![](public_sys-resources/icon-note.gif) **NOTE：**   
            >-  The files required for input\_shape、insert\_op\_conf、op\_name\_mapare all in the “sample-videoanalysisperson/MyModel” directory under the path where the source code is located，please configure these file path according to the path where your actual source code is located.
            >-   **XXX** is the name of Caffe model in [表 人体检测应用中使用模型](#zh-cn_topic_0203223281_table1193115345597), please fill in the name of Caffe model to be converted during conversion. The pedestrian model does not need the op\_name\_map parameters for conversion. An error will be reported during model conversion if the unnecessary parameters are not deleted.
            >-   The specific meaning of each parameter can be found in the following documents[https://ascend.huawei.com/doc/Atlas200DK/1.3.0.0/zh/zh-cn\_topic\_0165968579.html](https://ascend.huawei.com/doc/Atlas200DK/1.3.0.0/zh/zh-cn_topic_0165968579.html)  

        3.  Using omg tool to execute the following command to perform model conversion on the TensorFlow model to be used in the sample.

            ```
            ${DDK_HOME}/uihost/bin/omg --output="./XXX" --model="./XXX.pb" --framework=3 --ddk_version=${tools_version} --input_shape=`head -1 $HOME/AscendProjects/sample-videoanalysisperson/MyModel/shape_XXX` --insert_op_conf=$HOME/AscendProjects/sample-videoanalysisperson/MyModel/aipp_XXX.cfg
            ```

            >![](public_sys-resources/icon-note.gif) **NOTE：**   
            >-   **XXX**is the name of TensorFlow model in [表 人体检测应用中使用模型](#zh-cn_topic_0203223281_table1193115345597), please fill in the name of TensorFlow model to be converted during conversion. 


5. Upload the converted model file（.om file）to “**sample-videoanalysisperson/script**” directory  in the source code path in [Step 1]

## Compile<a name="en-us_topic_0182554628_section8534138124114"></a>

1.  Open the corresponding project.

    Enter the “MindStudio-ubuntu/bin” directory after decompressing the installation package in the command line, for example, $HOME/MindStudio-ubuntu/bin. Execute the following command to start Mind Studio:

    **./MindStudio.sh**

    After successfully starting Mind Studio, open **sample-videoanalysisperson** project，as shown in [Figure 8](#en-us_topic_0182554628_fig138681281084)

    **Figure 8**  Open videoanalysisperson project<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/打开videoanalysisperson工程.png "Open videoanalysisperson project")

2.  Configure related project information in the **src/param\_configure.conf**

    **Figure 9**  配置文件路径<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/配置文件路径.png "Configure file path")

    The configuration file is as follows:

    ```
    remote_host=
    presenter_view_app_name=
    video_path_of_host=
    rtsp_video_stream=
    ```

    需要手动添加参数配置：

    -   remote\_host：配置为Atlas 200 DK开发者板的IP地址。

    -   presenter\_view\_app\_name: 用户自定义的在PresenterServer界面展示的View Name，此View Name需要在Presenter Server展示界面唯一，只能为大小写字母、数字、“\_”的组合，位数3\~20。
    -   video\_path\_of\_host：配置为HOST侧的视频文件的绝对路径。
    -   rtsp\_video\_stream：配置为RTSP视频流的URL。

    视频文件配置示例如下：

    ```
    remote_host=192.168.1.2
    presenter_view_app_name=video
    video_path_of_host=/home/HwHiAiUser/person.mp4
    rtsp_video_stream=
    ```

    Rtsp视频流配置示例如下：

    ```
    remote_host=192.168.1.2
    presenter_view_app_name=video
    video_path_of_host=
    rtsp_video_stream=rtsp://192.168.2.37:554/cam/realmonitor?channel=1&subtype=0
    ```

    >![](public_sys-resources/icon-note.gif) **说明：**   
    >-   参数remote\_host和presenter\_view\_app\_name必须全部填写，否则无法通过build。  
    >-   注意参数填写时不需要使用“”符号。  
    >-   参数video\_path\_of\_host和rtsp\_video\_stream必须至少填写一项。  
    >-   当前RTSP视频流只支持rtsp://ip:port/path格式，如果需要使用其它格式的url，需要把video\_decode.cpp中的IsValidRtsp函数去除，或者直接返回true，跳过正则表达式匹配。  
    >-   本样例中提供的RTSP流地址不可以直接使用。如果需要使用RTSP，请在本地使用live555或其它方式制作RTSP视频流，并且可以在VLC中播放。然后将本地制作好的RTSP视频流的URL填入配置文件的相应参数中，即可运行  

3.  开始编译，打开Mindstudio工具，在工具栏中点击**Build \> Build \> Build-Configuration**。如[图 编译操作及生成文件](#zh-cn_topic_0203223281_fig1625447397)所示，会在目录下生成build和run文件夹。

    **图 10**  编译操作及生成文件<a name="zh-cn_topic_0203223281_fig1625447397"></a>  
    

    ![](figures/zh-cn_image_0208267063.png)

    >![](public_sys-resources/icon-note.gif) **说明：**   
    >首次编译工程时，**Build \> Build**为灰色不可点击状态。需要点击**Build \> Edit Build Configuration**，配置编译参数后再进行编译。  
    >![](figures/build_configuration.png)  

4.  <a name="zh-cn_topic_0203223281_li499911453439"></a>启动Presenter Server。

    打开Mindstudio工具的Terminal，此时默认在[步骤1](#zh-cn_topic_0203223281_li953280133816)中的代码存放路径下，执行如下命令在后台启动Face Detection应用的Presenter Server主程序。如[图 启动PresenterServer](#zh-cn_topic_0203223281_fig423515251067)所示。

    **bash run\_present\_server.sh**

    **图 11**  启动PresenterServer<a name="zh-cn_topic_0203223281_fig423515251067"></a>  
    ![](figures/启动PresenterServer.png "启动PresenterServer")

    -   当提示“Please choose one to show the presenter in browser\(default: 127.0.0.1\):“时，请输入在浏览器中访问Presenter Server服务所使用的IP地址（一般为访问Mind Studio的IP地址）。

        如[图 工程部署示意图](#zh-cn_topic_0203223281_fig999812514814)所示，请在“**Current environment valid ip list**”中选择通过浏览器访问Presenter Server服务使用的IP地址。

        **图 12**  工程部署示意图<a name="zh-cn_topic_0203223281_fig999812514814"></a>  
        ![](figures/工程部署示意图.png "工程部署示意图")

    -   当提示“Please input a absolute path to storage video analysis data:“时，请输入Mind Studio中的绝对路径用于存储视频解析数据，此路径Mind Studio用户需要有读写权限，若此路径不存在，脚本会自动创建。

    如[图13](#zh-cn_topic_0203223281_fig69531305324)所示，表示presenter\_server的服务启动成功。

    **图 13**  Presenter Server进程启动<a name="zh-cn_topic_0203223281_fig69531305324"></a>  
    ![](figures/Presenter-Server进程启动.png "Presenter-Server进程启动")

    使用上图提示的URL登录Presenter Server（仅支持Chrome浏览器），IP地址为[图12](#zh-cn_topic_0203223281_fig999812514814)中输入的IP地址，端口号默为7011，如下图所示，表示Presenter Server启动成功。

    **图 14**  主页显示<a name="zh-cn_topic_0203223281_fig64391558352"></a>  
    ![](figures/主页显示.png "主页显示")

    Presenter Server、Mind Studio与Atlas 200 DK之间通信使用的IP地址示例如下图所示：

    **图 15**  IP地址示例<a name="zh-cn_topic_0203223281_fig1881532172010"></a>  
    ![](figures/IP地址示例.png "IP地址示例")

    -   Atlas 200 DK开发者板使用的IP地址为192.168.1.2（USB方式连接）。
    -   Presenter Server与Atlas 200 DK通信的IP地址为UI Host服务器中与Atlas 200 DK在同一网段的IP地址，例如：192.168.1.223。
    -   通过浏览器访问Presenter Server的IP地址本示例为：10.10.0.1，由于Presenter Server与Mind Studio部署在同一服务器，此IP地址也为通过浏览器访问Mind Studio的IP。

5.  人体检测应用支持解析本地视频和RTSP视频流。
    -   如果需要解析本地视频，需要将视频文件传到Host侧。

        例如将视频文件person.mp4上传到Host侧的“/home/HwHiAiUser/“目录下。

        >![](public_sys-resources/icon-note.gif) **说明：**   
        >支持H264与H265格式的MP4文件，如果MP4文件需要剪辑，建议使用开源工具ffmpeg，使用其他工具剪辑的视频文件ffmpeg工具可能不支持解析。  

    -   如果仅解析RTSP视频流，本步骤可跳过。


## 运行<a name="zh-cn_topic_0203223281_section6245151616426"></a>

1.  运行人体检测应用程序。

    在Mindstudio工具的工具栏中找到Run按钮，点击**Run \> Run 'sample-videoanalysisperson'**，如[图 程序已执行示意图](#zh-cn_topic_0203223281_fig93931954162719)所示，可执行程序已经在开发板执行。

    **图 16**  程序已执行示意图<a name="zh-cn_topic_0203223281_fig93931954162719"></a>  
    

    ![](figures/sample-videoperson-running.png)

2.  使用启动Presenter Server服务时提示的URL登录 Presenter Server 网站（仅支持Chrome浏览器），详细可参考[步骤4](#zh-cn_topic_0203223281_li499911453439)。

    >![](public_sys-resources/icon-note.gif) **说明：**   
    >人体检测应用程序的Presenter Server最多支持2个  _presenter\_view\_app\_name_  同时显示。  

    页面左侧树结构列出了视频所属app name以及通道名，中间列出了抽取的视频帧大图以及检测出的目标小图，点击下方小图后会在右侧列出详细的推理结果、评分。

    本应用支持人体属性检测和人脸属性检测。

    -   人体属性包括：

        Age：共分Age16-30，Age31-45，Age46-60，AgeAbove61四个年龄段; Backpack; CarryingOther; Casual lower; Casual upper; Formal lower; Hat; Jacket; Jeans; Leather Shoes; Logo; Short hair/Long hair; Male/Female; Messenger Bag; Muffler; No accessory; No carrying; Plaid; PlasticBags; Sandals; Shoes; Shorts; Short Sleeve; Skirt; Sneaker; Stripes; Sunglasses; Trousers; Tshirt; UpperOther; V-Neck。

        其中在视频分析界面的详细推理结果展示区域，Age、Male/Female、Short hair/Long hair是必展示属性，其他属性当置信度\>0.5时则展示，否则不展示。

    -   人脸属性检测支持年龄和性别的的识别。


## 后续处理<a name="zh-cn_topic_0203223281_section1092612277429"></a>

-   **停止人体检测应用**

    视频程序分析完之后会自动停止退出，如[图 videoperson应用程序停止](#zh-cn_topic_0203223281_fig853816815204)所示

    **图 17**  videoperson应用程序停止<a name="zh-cn_topic_0203223281_fig853816815204"></a>  
    ![](figures/videoperson应用程序停止.png "videoperson应用程序停止")

-   **停止Presenter Server服务**

    Presenter Server服务启动后会一直处于运行状态，若想停止人体检测应用对应的Presenter Server服务，可执行如下操作。

    以Mind Studio安装用户在Mind Studio所在服务器中执行如下命令查看人体检测应用对应的Presenter Server服务的进程。

    **ps -ef | grep presenter | grep video\_analysis\_person**

    ```
    ascend@ascend-HP-ProDesk-600-G4-PCI-MT:~/sample-videoanalysisperson$ ps -ef | grep presenter | grep video_analysis_car
    ascend 3656 20313 0 15:10 pts/24?? 00:00:00 python3 presenterserver/presenter_server.py --app video_analysis_person
    ```

    如上所示_3656_  即为人体检测应用对应的Presenter Server服务的进程ID。

    若想停止此服务，执行如下命令：

    **kill -9** _3656_

-   **重启人体检测应用时注意点**

    重新启动人体检测应用时请确保以下条件满足任意一个，否则会报错:

    1.  请确保视频解析数据存储路径中内容已经清空。

        例如：视频解析数据存储路径为\\$HOME/videoperson\_storage/video，其中\\$HOME/videoperson\_storage是执行[步骤4](#zh-cn_topic_0203223281_li499911453439)时配置的“Please input a absolute path to storage video analysis data”的值，video为**param\_configure.conf**配置文件中参数**presenter\_view\_app\_name**的值。

        满足此条件情况下，无需重启Presenter Server，直接重新执行**Run \> Run 'sample-videoanalysisperson'**运行应用程序即可。

    2.  视频解析数据存储路径中如果已有数据且不想删除，可以修改**param\_configure.conf**配置文件中**presenter\_view\_app\_name**参数的值，然后在Mind Studio界面中重新执行**Build \> Rebuild**，再执行**Run \> Run 'sample-videoanalysisperson'**即可。

        **param\_configure.conf**配置文件中参数**presenter\_view\_app\_name**的值如下所示。

        ![](figures/人体检测的配置文件.png)

        满足此条件情况下，无需重启Presenter Server。

    3.  若重新启动Presenter Server，再运行车辆检测应用，在启动Presenter Server时请修改存储视频解析的数据的路径（不与之前存储路径重复），请参考[步骤4](#zh-cn_topic_0203223281_li499911453439)。




