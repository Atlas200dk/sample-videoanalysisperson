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

2.  <a name="en-us_topic_0182554628_li5507119145914"></a>Obtain the source network model required by the application. Obtain the source network model and its weight file used in the application by referring to  [Table 1](#en-us_topic_0182554628_table1193115345597), and save them to any directory on the Ubuntu server where  Mind Studio  is located, these two files must be saved in the same directory(for example,  **$HOME/models/videoanalysispersion**).







