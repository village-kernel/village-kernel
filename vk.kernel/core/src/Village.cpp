//###########################################################################
// Village.cpp
// Definitions of the functions that manage kernel
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Village.h"


/// @brief Constructor
Village::Village()
	:Kernel
	(
		concreteSystem,
		concreteMemory,
		concreteDebug,
		concreteInterrupt,
		concreteScheduler,
		concreteThread,
		concreteSymbol,
		concreteDevice,
		concreteFeature
	)
{
}


/// @brief Destructor
Village::~Village()
{
}


/// @brief Kernel Setup
void Village::Setup()
{
	//Setup system
	concreteSystem.Setup();

	//Setup memory
	concreteMemory.Setup();

	//Setup debug
	concreteDebug.Setup();
	
	//Setup interrupt
	concreteInterrupt.Setup();
	
	//Setup scheduler
	concreteScheduler.Setup();

	//Setup thread
	concreteThread.Setup();
	
	//Setup symbol
	concreteSymbol.Setup();
	
	//Setup device
	concreteDevice.Setup();
	
	//Setup feature
	concreteFeature.Setup();
}


/// @brief Kernel start
void Village::Start()
{
	//Start scheduler
	concreteScheduler.Start();

	//Should not go to here
	while (1) {}
}


/// @brief Kernel exit
void Village::Exit()
{
	//Exit feature
	concreteFeature.Exit();

	//Exit device
	concreteDevice.Exit();

	//Exit symbol
	concreteSymbol.Exit();

	//Exit thread
	concreteThread.Exit();

	//Exit scheduler
	concreteScheduler.Exit();

	//Exit interrupt
	concreteInterrupt.Exit();

	//Exit debug
	concreteDebug.Exit();

	//Exit memory
	concreteMemory.Exit();

	//Exit system
	concreteSystem.Exit();
}


/// @brief Get build date
/// @return date string
const char* Village::GetBuildDate()
{
	return __DATE__;
}


/// @brief Get build time
/// @return 
const char* Village::GetBuildTime()
{
	return __TIME__;
}


/// @brief Get build version
/// @return version string
const char* Village::GetBuildVersion()
{
	return BUILD_VER;
}


/// @brief Get build git commit
/// @return git commit
const char* Village::GetBuildGitCommit()
{
	return GIT_COMMIT;
}


/// @brief Kernel instance
/// @return kernel
Village& Village::Instance()
{
	static Village instance;
	return instance;
}


/// @brief Definition and export kernel
Kernel* kernel = &Village::Instance();
EXPORT_SYMBOL(kernel);


/// @brief Main entry function
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char* argv[])
{
	kernel->Setup();
	kernel->Start();
	kernel->Exit();
	return 0;
}
