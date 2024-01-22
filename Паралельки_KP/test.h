#pragma once
#include "FolderIter.h"
class test
{
public:
	FolderIter& fi;

	test(FolderIter& new_fi) :
		fi(new_fi) 
	{}
};

