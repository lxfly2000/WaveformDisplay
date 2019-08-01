/*你可以只在 Release 中使用该文件：
  #ifndef _DEBUG
  #include"UseVisualStyle.h"
  #endif
  */
//如果要使用NT6以后的公共控件（如 TaskDialog）同样需要加入该选项。
#pragma once
#ifndef _WINDOWS
#pragma message("【警告】[UseVisualStyle] 请只在 Windows 中使用该选项。")
#endif
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")	//使用视觉样式
