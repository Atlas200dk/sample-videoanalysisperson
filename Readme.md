# 人体检测<a name="ZH-CN_TOPIC_0208836319"></a>

开发者将本Application部署至Atlas 200 DK或者AI加速云服务器上实现对本地mp4文件或者RTSP视频流进行解码，对视频帧中的行人和人脸进行检测并对其属性进行预测，生成结构化信息发送至Server端进行保存、展示的功能。

## 前提条件<a name="zh-cn_topic_0203223281_section137245294533"></a>

部署此Sample前，需要准备好以下环境：

-   已完成Mind Studio的安装。
-   已完成Atlas 200 DK开发者板与Mind Studio的连接，交叉编译器的安装，SD卡的制作及基本信息的配置等。

## 软件准备<a name="zh-cn_topic_0203223281_section8534138124114"></a>

运行此Sample前，需要按照此章节获取源码包，并进行相关的环境配置。

1.  <a name="zh-cn_topic_0203223281_li953280133816"></a>获取源码包。

    将[https://gitee.com/Atlas200DK/sample-videoanalysisperson/tree/1.3x.0.0/](https://gitee.com/Atlas200DK/sample-videoanalysisperson/tree/1.3x.0.0/)仓中的代码以Mind Studio安装用户下载至Mind Studio所在Ubuntu服务器的任意目录，例如代码存放路径为：$HOME/AscendProjects/sample-videoanalysisperson。

2.  <a name="zh-cn_topic_0203223281_li5507119145914"></a>获取此应用中所需要的原始网络模型。

    参考[表1](#zh-cn_topic_0203223281_table1193115345597)获取此应用中所用到的原始网络模型及其对应的权重文件，并将其存放到Mind Studio所在Ubuntu服务器的任意目录，这两个文件必须存放到同一个目录下。例如：$HOME/models/videoanalysispersion。

    **表 1**  人体检测应用中使用模型

    <a name="zh-cn_topic_0203223281_table1193115345597"></a>
    <table><thead align="left"><tr id="zh-cn_topic_0203223281_row1187103505916"><th class="cellrowborder" valign="top" width="15.06%" id="mcps1.2.4.1.1"><p id="zh-cn_topic_0203223281_p887235105910"><a name="zh-cn_topic_0203223281_p887235105910"></a><a name="zh-cn_topic_0203223281_p887235105910"></a>模型名称</p>
    </th>
    <th class="cellrowborder" valign="top" width="10.58%" id="mcps1.2.4.1.2"><p id="zh-cn_topic_0203223281_p16877355598"><a name="zh-cn_topic_0203223281_p16877355598"></a><a name="zh-cn_topic_0203223281_p16877355598"></a>模型说明</p>
    </th>
    <th class="cellrowborder" valign="top" width="74.36%" id="mcps1.2.4.1.3"><p id="zh-cn_topic_0203223281_p18713511598"><a name="zh-cn_topic_0203223281_p18713511598"></a><a name="zh-cn_topic_0203223281_p18713511598"></a>模型下载路径</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="zh-cn_topic_0203223281_row3881635175910"><td class="cellrowborder" valign="top" width="15.06%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0203223281_p48863512592"><a name="zh-cn_topic_0203223281_p48863512592"></a><a name="zh-cn_topic_0203223281_p48863512592"></a>face_detection</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.58%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0203223281_p688163513595"><a name="zh-cn_topic_0203223281_p688163513595"></a><a name="zh-cn_topic_0203223281_p688163513595"></a>人脸检测网络模型。</p>
    <p id="zh-cn_topic_0203223281_p1488735175914"><a name="zh-cn_topic_0203223281_p1488735175914"></a><a name="zh-cn_topic_0203223281_p1488735175914"></a>此模型是基于Caffe的Resnet10-SSD300模型转换后的网络模型。</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0203223281_p1588203519592"><a name="zh-cn_topic_0203223281_p1588203519592"></a><a name="zh-cn_topic_0203223281_p1588203519592"></a>请参考<a href="https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/object_detect/face_detection" target="_blank" rel="noopener noreferrer">https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/object_detect/face_detection</a>目录中README.md下载原始网络模型文件及其对应的权重文件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0203223281_row98823585915"><td class="cellrowborder" valign="top" width="15.06%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0203223281_p1188103555919"><a name="zh-cn_topic_0203223281_p1188103555919"></a><a name="zh-cn_topic_0203223281_p1188103555919"></a>pedestrian</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.58%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0203223281_p18889356598"><a name="zh-cn_topic_0203223281_p18889356598"></a><a name="zh-cn_topic_0203223281_p18889356598"></a>人体特征推理模型。</p>
    <p id="zh-cn_topic_0203223281_p48853545911"><a name="zh-cn_topic_0203223281_p48853545911"></a><a name="zh-cn_topic_0203223281_p48853545911"></a>基于Caffe的VeSPA模型。</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0203223281_p5881335145918"><a name="zh-cn_topic_0203223281_p5881335145918"></a><a name="zh-cn_topic_0203223281_p5881335145918"></a>请参考<a href="https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/classification/pedestrian" target="_blank" rel="noopener noreferrer">https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/classification/pedestrian</a>目录中README.md下载原始网络模型文件及其对应的权重文件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0203223281_row388153512593"><td class="cellrowborder" valign="top" width="15.06%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0203223281_p188811350596"><a name="zh-cn_topic_0203223281_p188811350596"></a><a name="zh-cn_topic_0203223281_p188811350596"></a>inception_age</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.58%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0203223281_p138803555914"><a name="zh-cn_topic_0203223281_p138803555914"></a><a name="zh-cn_topic_0203223281_p138803555914"></a>年龄识别网络模型。</p>
    <p id="zh-cn_topic_0203223281_p2882351595"><a name="zh-cn_topic_0203223281_p2882351595"></a><a name="zh-cn_topic_0203223281_p2882351595"></a>基于Tensorflow的Inception V3模型。</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0203223281_p78813518594"><a name="zh-cn_topic_0203223281_p78813518594"></a><a name="zh-cn_topic_0203223281_p78813518594"></a>请参考<a href="https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/classification/inception_age" target="_blank" rel="noopener noreferrer">https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/classification/inception_age</a>目录中README.md下载原始网络PB模型文件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0203223281_row1788163525912"><td class="cellrowborder" valign="top" width="15.06%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0203223281_p1788123518593"><a name="zh-cn_topic_0203223281_p1788123518593"></a><a name="zh-cn_topic_0203223281_p1788123518593"></a>inception_gender</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.58%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0203223281_p10884353596"><a name="zh-cn_topic_0203223281_p10884353596"></a><a name="zh-cn_topic_0203223281_p10884353596"></a>性别识别网络模型。</p>
    <p id="zh-cn_topic_0203223281_p1388133595913"><a name="zh-cn_topic_0203223281_p1388133595913"></a><a name="zh-cn_topic_0203223281_p1388133595913"></a>基于Tensorflow的Inception V3模型。</p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0203223281_p1988103514599"><a name="zh-cn_topic_0203223281_p1988103514599"></a><a name="zh-cn_topic_0203223281_p1988103514599"></a>请参考<a href="https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/classification/inception_gender" target="_blank" rel="noopener noreferrer">https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/classification/inception_gender</a>目录中README.md下载原始网络PB模型文件。</p>
    </td>
    </tr>
    <tr id="zh-cn_topic_0203223281_row19881135115914"><td class="cellrowborder" valign="top" width="15.06%" headers="mcps1.2.4.1.1 "><p id="zh-cn_topic_0203223281_p13881355593"><a name="zh-cn_topic_0203223281_p13881355593"></a><a name="zh-cn_topic_0203223281_p13881355593"></a>vgg_ssd</p>
    </td>
    <td class="cellrowborder" valign="top" width="10.58%" headers="mcps1.2.4.1.2 "><p id="zh-cn_topic_0203223281_p128819351596"><a name="zh-cn_topic_0203223281_p128819351596"></a><a name="zh-cn_topic_0203223281_p128819351596"></a>目标检测网络模型。</p>
    <p id="zh-cn_topic_0203223281_p588153519599"><a name="zh-cn_topic_0203223281_p588153519599"></a><a name="zh-cn_topic_0203223281_p588153519599"></a>基于Caffe的SSD512模型。</p>
    <p id="zh-cn_topic_0203223281_p158813355590"><a name="zh-cn_topic_0203223281_p158813355590"></a><a name="zh-cn_topic_0203223281_p158813355590"></a></p>
    </td>
    <td class="cellrowborder" valign="top" width="74.36%" headers="mcps1.2.4.1.3 "><p id="zh-cn_topic_0203223281_p588183595913"><a name="zh-cn_topic_0203223281_p588183595913"></a><a name="zh-cn_topic_0203223281_p588183595913"></a>请参考<a href="https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/object_detect/vgg_ssd" target="_blank" rel="noopener noreferrer">https://gitee.com/HuaweiAscend/models/tree/master/computer_vision/object_detect/vgg_ssd</a>目录中README.md下载原始网络模型文件及其对应的权重文件。</p>
    </td>
    </tr>
    </tbody>
    </table>

3.  以Mind Studio安装用户登录Mind Studio所在Ubuntu服务器，确定当前使用的DDK版本号并设置环境变量DDK\_HOME，tools\_version，NPU\_DEVICE\_LIB和LD\_LIBRARY\_PATH。
    1.  <a name="zh-cn_topic_0203223281_zh-cn_topic_0203223294_li61417158198"></a>查询当前使用的DDK版本号。

        可通过Mind Studio工具查询，也可以通过DDK软件包进行获取。

        -   使用Mind Studio工具查询。

            在Mind Studio工程界面依次选择“File \> Settings \> System Settings \> Ascend DDK“，弹出如[图 DDK版本号查询](zh-cn_topic_0203223294.md#fig94023140222)所示界面。

            **图 1**  DDK版本号查询<a name="zh-cn_topic_0203223281_zh-cn_topic_0203223294_fig17553193319118"></a>  
            ![](figures/DDK版本号查询.png "DDK版本号查询")

            其中显示的**DDK Version**就是当前使用的DDK版本号，如**1.31.T15.B150**。

        -   通过DDK软件包进行查询。

            通过安装的DDK的包名获取DDK的版本号。

            DDK包的包名格式为：**Ascend\_DDK-\{software version\}-\{interface version\}-x86\_64.ubuntu16.04.tar.gz**

            其中**software version**就是DDK的软件版本号。

            例如：

            DDK包的包名为Ascend\_DDK-1.31.T15.B150-1.1.1-x86\_64.ubuntu16.04.tar.gz，则此DDK的版本号为1.31.T15.B150。

    2.  设置环境变量。

        **vim \~/.bashrc**

        执行如下命令在最后一行添加DDK\_HOME及LD\_LIBRARY\_PATH的环境变量。

        **export tools\_version=_1.31.X.X_**

        **export DDK\_HOME=\\$HOME/.mindstudio/huawei/ddk/\\$tools\_version/ddk**

        **export NPU\_DEVICE\_LIB=$DDK\_HOME/../RC/host-aarch64\_Ubuntu16.04.3/lib**

        **export LD\_LIBRARY\_PATH=$DDK\_HOME/lib/x86\_64-linux-gcc5.4**

        >![](public_sys-resources/icon-note.gif) **说明：**   
        >-   **_1.31.X.X_**是[1](#zh-cn_topic_0203223281_zh-cn_topic_0203223294_li61417158198)中查询到的DDK版本号，需要根据查询结果对应填写，如**1.31.T15.B150**  
        >-   如果此环境变量已经添加，则此步骤可跳过。  

        输入:wq!保存退出。

        执行如下命令使环境变量生效。

        **source \~/.bashrc**

4.  将原始网络模型转换为适配昇腾AI处理器的模型，模型转换有Mind Studio工具转换和命令行转换两种方式。
    -   通过Mind Studio工具进行模型转换。
        1.  在Mind Studio操作界面的顶部菜单栏中选择**Tool \> Convert Model**，进入模型转换界面。
        2.  在弹出的**Model** **Convert**操作界面中，进行模型转换配置。
            -   Model File选择[步骤2](#zh-cn_topic_0203223281_li5507119145914)中下载的模型文件，此时会自动匹配到权重文件并填写在Weight File中。
            -   Model Name填写为[表1](#zh-cn_topic_0203223281_table1193115345597)中对应的模型名称。
            -   Tensorflow模型转换时，需要手动填写input shape中的N值。W、H、C有默认值，无需填写。
            -   Pedestrian模型中AIPP配置中的**Input Image Size**需要分别修改为256、240，此处需要128\*16对齐，**Model Image Format**  选择BGR888\_U8

                **图 2**  Pedestrian模型转换时AIPP配置<a name="zh-cn_topic_0203223281_fig15011753113814"></a>  
                ![](figures/Pedestrian模型转换时AIPP配置.png "Pedestrian模型转换时AIPP配置")

            -   inception\_age模型转换时的非默认配置如下：
                -   age\_inference一次处理10张图片，所以转换时需要将Nodes配置中的**N**填写为10。

                    **图 3**  inception\_age模型转换时Nodes配置<a name="zh-cn_topic_0203223281_fig1336219448573"></a>  
                    ![](figures/inception_age模型转换时Nodes配置.png "inception_age模型转换时Nodes配置")

                -   AIPP配置中的**Input Image Size**需要分别修改为256,240 此处需要做128\*16对齐，**Model Image Format**  选择BGR888\_U8。

                    **图 4**  inception\_age模型转换时AIPP配置<a name="zh-cn_topic_0203223281_fig334910264011"></a>  
                    ![](figures/inception_age模型转换时AIPP配置.png "inception_age模型转换时AIPP配置")

            -   inception\_gender模型转换时非默认配置如下：
                -   gender\_inference一次处理10张图片，所以转换时需要将Nodes配置中的**N**填写为10。

                    **图 5**  inception\_gender模型转换时Nodes配置<a name="zh-cn_topic_0203223281_fig1136210442579"></a>  
                    ![](figures/inception_gender模型转换时Nodes配置.png "inception_gender模型转换时Nodes配置")

                -   AIPP配置中**Input Image Size**需要分别修改为256、240，此处需要128\*16对齐，**Model Image Format**  选择BGR888\_U8。

                    ![](figures/zh-cn_image_0208265214.png)

            -   face\_detection模型转换时AIPP配置中的**Input Image Size**需要分别修改为384、304，此处需要128\*16对齐，**Model Image Format**  选择BGR888\_U8。。

                **图 6**  face\_detection模型转换时非默认配置<a name="zh-cn_topic_0203223281_fig1336214415715"></a>  
                ![](figures/face_detection模型转换时非默认配置.png "face_detection模型转换时非默认配置")

                >![](public_sys-resources/icon-note.gif) **说明：**   
                >其他未说明配置请使用默认配置。  


        3.  单击**OK**开始转换模型。

            face\_detection、vgg\_ssd模型在转换的时候，会有报错，报错信息如下图所示。

            **图 7**  模型转换错误信息<a name="zh-cn_topic_0203223281_fig138681281084"></a>  
            ![](figures/模型转换错误信息.png "模型转换错误信息")

            此时在DetectionOutput层的Suggestion中选择SSDDetectionOutput，并点击Retry。

            模型转换成功后，后缀为.om的离线模型存放地址为：$HOME/modelzoo/XXX/device。

            >![](public_sys-resources/icon-note.gif) **说明：**   
            >-   Mindstudio模型转换中每一步的具体意义和参数说明可以参考[https://ascend.huawei.com/doc/mindstudio/2.1.0\(beta\)/zh/zh-cn\_topic\_0188462651.html](https://ascend.huawei.com/doc/mindstudio/2.1.0(beta)/zh/zh-cn_topic_0188462651.html)  
            >-   XXX表示当前转换的模型名称，如face\_detection.om存放地址为：$HOME/modelzoo/face\_detection/device。  


    -   命令行模式下模型转换。
        1.  以Mind Studio安装用户进入存放原始模型的文件夹。

            **cd $HOME/models/videoanalysisperson**

        2.  调用omg工具执行以下命令对sample中需要使用的Caffe模型进行模型转换。

            ```
            ${DDK_HOME}/uihost/bin/omg --output="./XXX" --model="./XXX.prototxt" --framework=0 --ddk_version=${tools_version} --weight="./XXX.caffemodel" --input_shape=`head -1 $HOME/AscendProjects/sample-videoanalysisperson/MyModel/shape_XXX` --insert_op_conf=$HOME/AscendProjects/sample-videoanalysisperson/MyModel/aipp_XXX.cfg --op_name_map=$HOME/AscendProjects/sample-videoanalysisperson/MyModel/reassign_operators
            ```

            >![](public_sys-resources/icon-note.gif) **说明：**   
            >-   input\_shape、insert\_op\_conf、op\_name\_map所需要的文件都在源码所在路径下的“sample-videoanalysisperson/MyModel”目录下，请根据您实际的源码所在路径配置这些文件路径。  
            >-   **XXX**为[表 人体检测应用中使用模型](#zh-cn_topic_0203223281_table1193115345597)中的caffe模型名称，转换时请替换填入需要转换的caffe模型名称。其中pedestrian模型转换时不需要op\_name\_map参数，如果没有删除不需要的参数，转换模型时会有报错。  
            >-   每个参数的具体意义可以在以下文档中了解[https://ascend.huawei.com/doc/Atlas200DK/1.3.0.0/zh/zh-cn\_topic\_0165968579.html](https://ascend.huawei.com/doc/Atlas200DK/1.3.0.0/zh/zh-cn_topic_0165968579.html)  

        3.  调用omg工具执行以下命令对sample中需要使用的TensorFlow模型进行模型转换

            ```
            ${DDK_HOME}/uihost/bin/omg --output="./XXX" --model="./XXX.pb" --framework=3 --ddk_version=${tools_version} --input_shape=`head -1 $HOME/AscendProjects/sample-videoanalysisperson/MyModel/shape_XXX` --insert_op_conf=$HOME/AscendProjects/sample-videoanalysisperson/MyModel/aipp_XXX.cfg
            ```

            >![](public_sys-resources/icon-note.gif) **说明：**   
            >-   **XXX**为[表 人体检测应用中使用模型](#zh-cn_topic_0203223281_table1193115345597)中的TensorFlow模型名称，转换时请替换填入需要转换的TensorFlow模型名称。  


5.  将转换好的模型文件（.om文件）上传到[步骤1](#zh-cn_topic_0203223281_li953280133816)中源码所在路径的“**sample-videoanalysisperson/script**”目录下。

## 编译<a name="zh-cn_topic_0203223281_section1759513564117"></a>

1.  打开对应的工程。

    以Mind Studio安装用户在命令行中进入安装包解压后的“MindStudio-ubuntu/bin”目录，如：$HOME/MindStudio-ubuntu/bin。执行如下命令启动Mind Studio

    **./MindStudio.sh**

    启动成功后，打开**sample-videoanalysisperson**工程，如[图 打开videoanalysisperson工程](#zh-cn_topic_0203223281_fig05481157171918)所示。

    **图 8**  打开videoanalysisperson工程<a name="zh-cn_topic_0203223281_fig05481157171918"></a>  
    ![](figures/打开videoanalysisperson工程.png "打开videoanalysisperson工程")

2.  在**src/param\_configure.conf**文件中配置相关工程信息。

    **图 9**  配置文件路径<a name="zh-cn_topic_0203223281_fig0391184062214"></a>  
    ![](figures/配置文件路径.png "配置文件路径")

    该配置文件内容如下：

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


