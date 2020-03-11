中文|[英文](Readme_en.md)

# 人体检测<a name="ZH-CN_TOPIC_0208836319"></a>

开发者将本Application部署至Atlas 200 DK或者AI加速云服务器上实现对本地mp4文件或者RTSP视频流进行解码，对视频帧中的行人和人脸进行检测并对其属性进行预测，生成结构化信息发送至Server端进行保存、展示的功能。

## 前提条件<a name="zh-cn_topic_0203223281_section137245294533"></a>

部署此Sample前，需要准备好以下环境：

-   已完成Mind Studio的安装。
-   已完成Atlas 200 DK开发者板与Mind Studio的连接，交叉编译器的安装，SD卡的制作及基本信息的配置等。

## 部署
1. 部署，可以选择如下快速部署或者常规方法部署，二选一即可；

   1.1 快速部署，请参考：https://gitee.com/Atlas200DK/faster-deploy 。
    >![](public_sys-resources/icon-note.gif) **说明：**   
    >-   该快速部署脚本可以快速部署多个案例，请选择videoanalysisperson案例部署即可。 
    >-   该快速部署脚本自动完成了代码下载、模型转换、环境变量配置等流程，如果需要了解详细的部署过程请选择常规部署方式，请转1.2 常规部署。
    
   1.2 常规部署，请参考：https://gitee.com/Atlas200DK/sample-README/tree/master/sample-videoanalysisperson  。
    >![](public_sys-resources/icon-note.gif) **说明：**   
    >-   该部署方式，需要手动完成代码下载、模型转换、环境变量配置等过程。完成后，会对其中的过程会更加了解。

## 编译<a name="zh-cn_topic_0203223281_section1759513564117"></a>

1.  打开对应的工程。

    以Mind Studio安装用户在命令行中进入安装包解压后的“MindStudio-ubuntu/bin”目录，如：$HOME/MindStudio-ubuntu/bin。执行如下命令启动Mind Studio

    **./MindStudio.sh**

    启动成功后，打开**sample-videoanalysisperson**工程，如[图 打开videoanalysisperson工程](#zh-cn_topic_0203223281_fig05481157171918)所示。

    **图 1**  打开videoanalysisperson工程<a name="zh-cn_topic_0203223281_fig05481157171918"></a>  
    ![](figures/打开videoanalysisperson工程.png "打开videoanalysisperson工程")

2.  在**src/param\_configure.conf**文件中配置相关工程信息。

    **图 2**  配置文件路径<a name="zh-cn_topic_0203223281_fig0391184062214"></a>  
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

3.  执行deploy脚本， 进行配置参数调整及第三方库下载编译 打开Mind Studio工具的Terminal，此时默认在代码主目录下，执行如下命令在后台指执行deploy脚本，进行环境部署。如[图 执行deploy脚本](#zh-cn_topic_0182554577_fig19292258105419")所示。
    
    **图 3**  执行deploy脚本<a name="zh-cn_topic_0182554577_fig19292258105419"></a>  
    
    ![](figures/deploy_person.png)
    
    >![](public_sys-resources/icon-note.gif) **说明：**   
    >-   首次deploy时，没有部署第三方库时会自动下载并编译，耗时可能比较久，请耐心等待。后续再重新编译时，不会重复下载编译，部署如上图所示。
    >-   deploy时，需要选择与开发板通信的主机侧ip，一般为虚拟网卡配置的ip。如果此ip和开发板ip属于同网段，则会自动选择并部署。如果非同网段，则需要手动输入与开发板通信的主机侧ip才能完成deploy。

4.  开始编译，打开Mindstudio工具，在工具栏中点击**Build \> Build \> Build-Configuration**。如[图 编译操作及生成文件](#zh-cn_topic_0203223281_fig1625447397)所示，会在目录下生成build和run文件夹。

    **图 4**  编译操作及生成文件<a name="zh-cn_topic_0203223281_fig1625447397"></a>  
    

    ![](figures/zh-cn_image_0208267063.png)

    >![](public_sys-resources/icon-note.gif) **说明：**   
    >首次编译工程时，**Build \> Build**为灰色不可点击状态。需要点击**Build \> Edit Build Configuration**，配置编译参数后再进行编译。  
    >![](figures/build_configuration.png)  

4.  <a name="zh-cn_topic_0203223281_li499911453439"></a>启动Presenter Server。

    打开Mindstudio工具的Terminal，此时默认在[步骤1](#zh-cn_topic_0203223281_li953280133816)中的代码存放路径下，执行如下命令在后台启动Face Detection应用的Presenter Server主程序。如[图 启动PresenterServer](#zh-cn_topic_0203223281_fig423515251067)所示。

    **bash run\_present\_server.sh**

    **图 5**  启动PresenterServer<a name="zh-cn_topic_0203223281_fig423515251067"></a>  
    ![](figures/启动PresenterServer.png "启动PresenterServer")

    -   当提示“Please choose one to show the presenter in browser\(default: 127.0.0.1\):“时，请输入在浏览器中访问Presenter Server服务所使用的IP地址（一般为访问Mind Studio的IP地址）。

        如[图 工程部署示意图](#zh-cn_topic_0203223281_fig999812514814)所示，请在“**Current environment valid ip list**”中选择通过浏览器访问Presenter Server服务使用的IP地址。

        **图 6**  工程部署示意图<a name="zh-cn_topic_0203223281_fig999812514814"></a>  
        ![](figures/工程部署示意图.png "工程部署示意图")

    -   当提示“Please input a absolute path to storage video analysis data:“时，请输入Mind Studio中的绝对路径用于存储视频解析数据，此路径Mind Studio用户需要有读写权限，若此路径不存在，脚本会自动创建。

    如[图7](#zh-cn_topic_0203223281_fig69531305324)所示，表示presenter\_server的服务启动成功。

    **图 7**  Presenter Server进程启动<a name="zh-cn_topic_0203223281_fig69531305324"></a>  
    ![](figures/Presenter-Server进程启动.png "Presenter-Server进程启动")

    使用上图提示的URL登录Presenter Server（仅支持Chrome浏览器），IP地址为[图6](#zh-cn_topic_0203223281_fig999812514814)中输入的IP地址，端口号默为7011，如下图所示，表示Presenter Server启动成功。

    **图 8**  主页显示<a name="zh-cn_topic_0203223281_fig64391558352"></a>  
    ![](figures/主页显示.png "主页显示")

    Presenter Server、Mind Studio与Atlas 200 DK之间通信使用的IP地址示例如下图所示：

    **图 9**  IP地址示例<a name="zh-cn_topic_0203223281_fig1881532172010"></a>  
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

    **图 10**  程序已执行示意图<a name="zh-cn_topic_0203223281_fig93931954162719"></a>  
    

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

    **图 11**  videoperson应用程序停止<a name="zh-cn_topic_0203223281_fig853816815204"></a>  
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


