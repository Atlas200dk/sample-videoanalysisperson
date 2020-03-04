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

3.  Convert the source network model to a Da Vinci model.

