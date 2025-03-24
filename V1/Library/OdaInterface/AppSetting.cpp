#include "stdafx.h"

#include "AppSetting.h"

//**************************************************************************************************

// single instance
AppSetting TheAppSetting;



AppSetting::AppSetting()
{
    Buffer[EValue::UseThreadIo] =  true;
    Buffer[EValue::UseRubberBand] = true;
    Buffer[EValue::UseDynamicInput] = false;
}



AppSetting::~AppSetting()
{
    Buffer.clear();
}
