//###########################################################################
// IOStream.cpp
// Definitions of the functions that manage input output object
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "IOStream.h"


///Initialize io core
IOStream::IONode* IOStream::list = NULL;

///Clear isReady flag
volatile bool IOStream::isReady = false;

///Constructor
IOStream::IOStream()
{
}


///Execute io object->Initialize
void IOStream::Initialize()
{
	isReady = false;

	//Initialize ios
	for (volatile IONode* node = list; NULL != node; node = node->next)
	{
		node->io->Initialize();
	}

	isReady = true;
}


///Execute io object->UpdateParams
void IOStream::UpdateParams()
{
	if (!isReady) return;

	for (volatile IONode* node = list; NULL != node; node = node->next)
	{
		node->io->UpdateParams();
	}
}


///Execute io object->Execute
void IOStream::Execute()
{
	while (1)
	{
		if (isReady)
		{
			for (volatile IONode* node = list; NULL != node; node = node->next)
			{
				node->io->Execute();
			}
		}
	}
}


///Execute io object->FailSafe
void IOStream::FailSafe(int arg)
{
	if (!isReady) return;

	for (volatile IONode* node = list; NULL != node; node = node->next)
	{
		node->io->FailSafe(arg);
	}
}


///Register io object
void IOStream::RegisterIO(IO* io, uint32_t id)
{
	IONode** nextNode = &list;

	if (io) io->SetID(id); else return;

	while (NULL != *nextNode)
	{
		uint32_t curModuleID = (*nextNode)->io->GetID();
		uint32_t newModuleID = io->GetID();

		if (newModuleID < curModuleID)
		{
			IONode* curNode = *nextNode;
			*nextNode = new IONode(io);
			(*nextNode)->next = curNode;
			return;
		}
		
		nextNode = &(*nextNode)->next;
	}

	*nextNode = new IONode(io);
}


///Deregister io object
void IOStream::DeregisterIO(IO* io, uint32_t id)
{
	IONode** prevNode = &list;
	IONode** currNode = &list;

	while (NULL != *currNode)
	{
		if (io == (*currNode)->io)
		{
			delete *currNode;

			if (*prevNode == *currNode)
				*prevNode = (*currNode)->next;
			else
				(*prevNode)->next = (*currNode)->next;

			break;
		}
		else
		{
			prevNode = currNode;
			currNode = &(*currNode)->next;
		}
	}
}
