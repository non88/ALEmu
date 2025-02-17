#pragma once

#include "AlefWorldGlobal.h"
#include "AlefWorldTypes.h"

class AlefDBDataSys
{
	friend class AlefServerDataSys;
protected:
	AlefDBDataSys();
	~AlefDBDataSys();

	SharedPtr<RecordSet> dbGetTemplateFields();
	SharedPtr<RecordSet> dbGetFullTemplateData();
	SharedPtr<RecordSet> dbGetTemplateData(int tID);
	SharedPtr<RecordSet> dbGetCharDataTable();
	SharedPtr<RecordSet> dbGetLoginCharPos();
};