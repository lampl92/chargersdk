#ifndef EVSE_VERSION_01_07_0625_H
#define EVSE_VERSION_01_07_0625_H

//                   ↑   ↑  ↑
//                   |   |  |
//<----!!!用于生成远程升级文件夹, 一定要与下面版本号一致, 不一致就揍了


    #define EVSE_MAJOR          "01"              //主版本,两位, 对应生产用程序ABCD版本
    #define EVSE_MINOR          "07"              //副版本,两位, 对应PDM A1,A2版本
    #define EVSE_PATCH          "0625"            //补丁版本,四位, 目前用作构建日期, 月日表示
	
	
    #define EVSE_TARGET         "S1639"         //项目号,订单号,整改号
    #define EVSE_BUILD_AUTHOR   "RGW"
    //#define EVSE_VERSION_STRING "S1639 01.03.1222"
	

#endif //VERSION_H
