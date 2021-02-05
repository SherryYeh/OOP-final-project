
#define ENABLE_AUDIO		 true		// 啟動音效介面


#include <list>
#include <vector>
#include <map>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// 回報程式錯誤用的macro
// 備註：這裡使用macro以便保留檔案名稱及行號，利於debug。
/////////////////////////////////////////////////////////////////////////////

#define GAME_ASSERT(boolexp,str)											\
		if (!(boolexp)) {													\
			int id;															\
			char s[300]="";													\
			sprintf(s,"Game fatal error:\n\n%s\n\nFile: %s\n\nLine: %d"		\
				"\n\n(Press Retry to debug the application, "				\
				"if it is executed in debug mode.)"							\
				"\n(Press Cancel otherwise.)",								\
				 str , __FILE__,__LINE__);									\
			id = AfxMessageBox(s, MB_RETRYCANCEL);							\
			if (id == IDCANCEL)												\
				exit(1);													\
			AfxDebugBreak();												\
		}
