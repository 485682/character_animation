#include "application.h"

int main(){
    

	#if !defined(DEBUG) && !defined(_DEBUG)
		FreeConsole();
	#endif
    _application = new application();
	if(_application->init()){ _application->run(); }

}
