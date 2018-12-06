#ifndef _MEMORYSAVER_
#define _MEMORYSAVER_
//There are two steps you need to modify in this file before normal compilation
//Only ArduCAM Shield series platform need to select camera module, ArduCAM-Mini series platform doesn't

//Step 1: select the hardware platform, only one at a time
#define OV2640_MINI_2MP

//Step 2: Select one of the camera module, only one at a time
#if (defined(ARDUCAM_SHIELD_REVC) || defined(ARDUCAM_SHIELD_V2))

#define OV2640_CAM

#endif 

#endif	//_MEMORYSAVER_
