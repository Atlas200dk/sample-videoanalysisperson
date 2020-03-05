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

    **Figure 9**  Configure file path<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/配置文件路径.png "Configure file path")

    The configuration file is as follows:

    ```
    remote_host=
    presenter_view_app_name=
    video_path_of_host=
    rtsp_video_stream=
    ```

    Following parameter configuration need to be added manually：

    -   remote\_host：this parameter indicates the IP address of Atlas 200 DK developer board.

    -   presenter\_view\_app\_name: The user-defined View Name on the PresenterServer interface, this View Name needs to be unique  on the Presenter Server. It can only be a combination of uppercase and lowercase letters, numbers, and "\_", with a digit of 3 \~20.
    -   video\_path\_of\_host：absolute path of video file on the HOST.
    -   rtsp\_video\_stream：URL of RTSP video streams.

    An example of video file configuration is as follows:

    ```
    remote_host=192.168.1.2
    presenter_view_app_name=video
    video_path_of_host=/home/HwHiAiUser/person.mp4
    rtsp_video_stream=
    ```

    An example of Rtsp video streams configuration is as follows:

    ```
    remote_host=192.168.1.2
    presenter_view_app_name=video
    video_path_of_host=
    rtsp_video_stream=rtsp://192.168.2.37:554/cam/realmonitor?channel=1&subtype=0
    ```

    >![](public_sys-resources/icon-note.gif) **NOTE：**   
    >-   The parameters remote\_host and presenter\_view\_app\_name must be filled in，otherwise build cannot be paased.
    >-   Note that the "" symbol is no need to be used when filling in parameters.
    >-   At least one of the parameters video\_path\_of\_host and rtsp\_video\_stream must be filled in.
    >-   Current RTSP video streams only support rtsp://ip:port/path format, if other urls format is need to be used, the IsValidRtsp  function in the video\_decode.cpp should be removed, or directly return true to skip regular expression matching.
    >-   The RTSP stream address in this example cannot be used directly. If you need to use RTSP, please use live555 or other methods to make RTSP video streams locally, and it can be played in VLC, then fill the URL of the locally made RTSP video streams into the corresponding parameters of the configuration file.

3.  Begin to compile，open **Mindstudio tool**，click **Build \> Build \> Build-Configuration** in the toolbar, shown as [Figure 10](#en-us_topic_0182554628_fig138681281084), **build** and **run** folders will be generated under the directory.

    **Figure 10**  Compile operations and generate files<a name="en-us_topic_0182554628_fig138681281084"></a>  
    

    ![](figures/zh-cn_image_0208267063.png)

    >![](public_sys-resources/icon-note.gif) **NOTE：**   
    >When you compile the project for the first time, **Build \> Build** is gray and not clickable. Your need to click **Build \> Edit Build Configuration**, configure the compilation parameters and then compile.  
    >![](figures/build_configuration.png)  

4.  Start Presenter Server.

    Open **Terminal** of **Mindstudio tool**, it is in the path where code saved in [Step 1] by default(#zh-cn_topic_0203223281_li953280133816), run the following command to start the Presenter Server main program of the Face Detection application, shown as [Figure 11](#en-us_topic_0182554628_fig138681281084).

    **bash run\_present\_server.sh**

    **Figure 11**  Start PresenterServer<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/启动PresenterServer.png "Start PresenterServer")

    -   When the message Please choose one to show the presenter in browser (default: 127.0.0.1): is displayed, enter the IP address used for accessing the Presenter Server service in the browser. Generally, the IP address is the IP address for accessing the Mind Studio service.

        As shown in [Figure 12](#en-us_topic_0182554628_fig138681281084), Select the IP address used by the browser to access the Presenter Server service in Current environment valid ip list and enter the path for storing video analysis data

        **Figure 12**  Project deployment<a name="en-us_topic_0182554628_fig138681281084"></a>  
        ![](figures/工程部署示意图.png "Project deployment")

    -   When the message Please input an absolute path to storage video analysis data: is displayed, enter the absolute path for storing video analysis data in Mind Studio. The Mind Studio user must have the read and write permissions. If the path does not exist, the script is automatically created.
    
    As shown in [Figure 13](#en-us_topic_0182554628_fig138681281084), it means **presenter\_server**  service starts successfully.

    **Figure 13**  Starting the Presenter Server process<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/Presenter-Server进程启动.png "Starting the Presenter Server process")

    Use the URL shown in the preceding figure to log in to Presenter Server (only the Chrome browser is supported). The IP address is that entered in 2 and the default port number is 7011. The following figure indicates that Presenter Server is started successfully.
    
    **Figure 14**  Home page<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/主页显示.png "Home page")

    The following figure shows the IP address used by the Presenter Server and Mind Studio to communicate with the Atlas 200 DK.

    **Figure 15**  Example IP Address<a name="en-us_topic_0182554628_fig138681281084"></a>  
    ![](figures/IP地址示例.png "Example IP Address")

    -   The IP address of the Atlas 200 DK developer board is 192.168.1.2 (connected in USB mode).
    -   The IP address used by the Presenter Server to communicate with the Atlas 200 DK is in the same network segment as the IP address of the Atlas 200 DK on the UI Host server. For example: 192.168.1.223.
    -   The following is an example of accessing the IP address of the Presenter Server using a browser: 10.10.0.1, because the Presenter Server and Mind Studio are deployed on the same server, the IP address is also the IP address for accessing the Mind Studio through the browser.

5.  Video analysis application can parse local videos and RTSP video streams.


    -   To parse a local video, upload the video file to the Host.

        For example, upload the video file **person.mp4** to the **/home/HwHiAiUser/** directory on the host.

        >![](public_sys-resources/icon-note.gif) **NOTE：**   
        > H264 and H265 format MP4 files are supported，the open source tool FFmpeg is recommended if MP4 files need to be edited, because FFmpeg tools may fail to parse video files edited using other tools.

    -   if you only parse RTSP video streams, this step can be skipped.


## Running<a name="zh-cn_topic_0203223281_section6245151616426"></a>

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




