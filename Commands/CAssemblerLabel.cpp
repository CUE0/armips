#include "StdAfx.h"
#include "Commands/CAssemblerLabel.h"
#include "Core/Common.h"
#include "Util/Util.h"
#include "Core/FileManager.h"

CAssemblerLabel::CAssemblerLabel(const std::wstring& name, int RamPos, int Section, bool constant)
{
	label = Global.symbolTable.getLabel(name,FileNum,Section);
	if (label == NULL)
	{
		Logger::printError(Logger::Error,L"Invalid label name \"%s\"",name.c_str());
		return;
	}

	if (label->isDefined())
	{
		Logger::printError(Logger::Error,L"Label \"%s\" already defined",name.c_str());
		return;
	}

	label->setValue(RamPos);
	label->setDefined(true);
	this->constant = constant;

	if (Global.symbolTable.isLocalSymbol(name) == false)
		Global.Section++;
}

bool CAssemblerLabel::Validate()
{
	if (constant == false && label->getValue() != g_fileManager->getVirtualAddress())
	{
		label->setValue(g_fileManager->getVirtualAddress());
		return true;
	}
	
	return false;
}

void CAssemblerLabel::writeTempData(TempData& tempData)
{
	tempData.writeLine(label->getValue(),formatString(L"%s:",label->getName().c_str()));
}

void CAssemblerLabel::writeSymData(SymbolData& symData)
{
	symData.addLabel(label->getValue(),label->getName());
}




CDirectiveFunction::CDirectiveFunction(const std::wstring& name, int Section)
{
	if (!name.empty())
		label = new CAssemblerLabel(name,g_fileManager->getVirtualAddress(),Section,false);
	else
		label = NULL;
}

bool CDirectiveFunction::Validate()
{
	if (label == NULL)
		return false;
	return label->Validate();
}

void CDirectiveFunction::writeTempData(TempData& tempData)
{
	if (label != NULL)
		label->writeTempData(tempData);
}

void CDirectiveFunction::writeSymData(SymbolData& symData)
{
	if (label != NULL)
	{
		Global.symData.startFunction(g_fileManager->getVirtualAddress());
		label->writeSymData(symData);
	} else {
		Global.symData.endFunction(g_fileManager->getVirtualAddress());
	}
}
