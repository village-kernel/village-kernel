//###########################################################################
// ElfParser.cpp
// Definitions of the functions that manage elf parser
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "Kernel.h"
#include "console.h"
#include "ElfParser.h"
#include "FileStream.h"
#include "string.h"


/// @brief Initialize map address
uint32_t ElfParser::mapAddr = ElfParser::base_map_address;


/// @brief Constructor
/// @param filename 
ElfParser::ElfParser(const char* filename)
	:filename(filename)
{
	if (NULL != filename) Load(filename);
}


/// @brief ElfParser load and parser elf file
/// @param filename 
/// @return result
int ElfParser::Load(const char* filename)
{
	this->filename = filename;

	if (LoadElf()         != _OK) return _ERR;
	if (PreParser()       != _OK) return _ERR;
	if (SegmentMapping()  != _OK) return _ERR;
	if (PostParser()      != _OK) return _ERR;
	if (SharedObjs()      != _OK) return _ERR;
	if (RelEntries()      != _OK) return _ERR;
	
	return _OK;
}


/// @brief ElfParser load elf file
/// @param filename 
/// @return result
int ElfParser::LoadElf()
{
	FileStream file;

	if (FR_OK == file.Open(filename, FileStream::_Read))
	{
		int size = file.Size();
		elf.load = (uint32_t)new uint8_t[size]();

		if (elf.load && (file.Read((uint8_t*)elf.load, size) == size))
		{
			console.info("%s load successful", filename);
			file.Close();
			return _OK;
		}

		file.Close();
	}

	console.log("%s load failed", filename);
	return _ERR;
}


/// @brief Get dynamic string
/// @param index 
/// @return string
inline const char* ElfParser::GetDynamicString(uint32_t index)
{
	return (const char*)elf.dynstr + index;
}


/// @brief Get section name
/// @param index 
/// @return name
inline const char* ElfParser::GetSectionName(uint32_t index)
{
	if (elf.header->sectionHeaderNum > index)
	{
		return (const char*)elf.shstrtab + elf.sections[index].name;
	}
	return NULL;
}


/// @brief Get symbol name
/// @param index 
/// @return name
inline const char* ElfParser::GetSymbolName(uint32_t index)
{
	if (elf.symtabNum > index)
	{
		return (const char*)elf.strtab + elf.symtab[index].name;
	}
	return NULL;
}


/// @brief Get dynamic symbol name
/// @param index 
/// @return name
inline const char* ElfParser::GetDynSymName(uint32_t index)
{
	if (elf.dynsymNum > index)
	{
		return (const char*)elf.dynstr + elf.dynsym[index].name;
	}
	return NULL;
}


/// @brief Get symbol addr
/// @param index 
/// @return address
inline uint32_t ElfParser::GetSymbolAddr(uint32_t index)
{
	if (elf.symtabNum > index)
	{
		return elf.map + elf.symtab[index].value;
	}
	return 0;
}


/// @brief Get dynamic symbol addr
/// @param index 
/// @return address
inline uint32_t ElfParser::GetDynSymAddr(uint32_t index)
{
	if (elf.dynsymNum > index)
	{
		return elf.map + elf.dynsym[index].value;
	}
	return 0;
}


/// @brief Get symbol addr by name
/// @param name 
/// @return address
inline uint32_t ElfParser::GetSymbolAddrByName(const char* name)
{
	for (uint32_t i = 0; i < elf.symtabNum; i++)
	{
		if(0 == strcmp(name, GetSymbolName(i)))
		{
			return GetSymbolAddr(i);
		}
	}
	return 0;
}


/// @brief Get dynamic symbol addr by name
/// @param name 
/// @return address
inline uint32_t ElfParser::GetDynSymAddrByName(const char* name)
{
	for (uint32_t i = 0; i < elf.dynsymNum; i++)
	{
		if (0 == strcmp(name, GetDynSymName(i)))
		{
			return GetDynSymAddr(i);
		}
	}
	return 0;
}


/// @brief Get section data
/// @param index 
/// @return address
inline ElfParser::SectionData ElfParser::GetSectionData(uint32_t index)
{
	return SectionData(elf.load + elf.sections[index].offset);
}


/// @brief Get dyn section data
/// @param index 
/// @return address
inline ElfParser::SectionData ElfParser::GetDynSectionData(uint32_t index)
{
	return SectionData(elf.map + elf.sections[index].addr);
}


/// @brief Pre parser
/// @return result
int ElfParser::PreParser()
{
	//Set elf header pointer
	elf.header = (ELFHeader*)(elf.load);

	//Check if it is a valid elf file
	const uint8_t elfmagic[] = {0x7f, 'E', 'L', 'F'};
	if (elf.header->ident[0] !=  elfmagic[0]    ) return _ERR;
	if (elf.header->ident[1] !=  elfmagic[1]    ) return _ERR;
	if (elf.header->ident[2] !=  elfmagic[2]    ) return _ERR;
	if (elf.header->ident[3] !=  elfmagic[3]    ) return _ERR;
	if (elf.header->ident[4] != _ELF_Class_32   ) return _ERR;
	if (elf.header->version  != _ELF_Ver_Current) return _ERR;
#if defined(ARCH_X86)
	if (elf.header->machine  != _ELF_Machine_X86) return _ERR;
#elif defined(ARCH_ARM)
	if (elf.header->machine  != _ELF_Machine_ARM) return _ERR;
#endif
	if ((elf.header->type    != _ELF_Type_Dyn) &&
		(elf.header->type    != _ELF_Type_Exec))
	{
		console.error("%s is not executable", filename);
		return _ERR;
	}

	//Set executable entry
	elf.exec = elf.header->entry;

	//Get program headers pointer
	elf.programs = (ProgramHeader*)(elf.load + elf.header->programHeaderOffset);

	//Get section headers pointer
	elf.sections = (SectionHeader*)(elf.load + elf.header->sectionHeaderOffset);

	//Get section string table pointer
	elf.shstrtab = GetSectionData(elf.header->sectionHeaderStringTableIndex).shstrtab;

	//Get some information of elf
	for (uint32_t i = 0; i < elf.header->sectionHeaderNum; i++)
	{
		SectionData data = GetSectionData(i);

		//Set symbol tables pointer
		if (_SHT_SYMTAB == elf.sections[i].type)
		{
			elf.symtab = data.symtab;
			elf.symtabNum = elf.sections[i].size / sizeof(SymbolEntry);
		}
		//Set section header string table and symbol string talbe pointer
		else if (_SHT_STRTAB == elf.sections[i].type)
		{
			if (0 == strcmp(".strtab", GetSectionName(i)))
				elf.strtab = data.strtab;
		}
	}

	console.info("%s pre parser successful", filename);
	return _OK;
}


/// @brief Section to segment mapping
/// @return result
int ElfParser::SegmentMapping()
{
	//Set map address
	if (_ELF_Type_Dyn == elf.header->type)
		elf.map = mapAddr;
	else if (_ELF_Type_Exec == elf.header->type)
		elf.map = 0;

	for (uint32_t i = 0; i < elf.header->programHeaderNum; i++)
	{
		ProgramHeader program = elf.programs[i];

		if (_PT_LOAD == program.type)
		{
			uint8_t* vaddr = (uint8_t*)(elf.map  + program.vaddr);
			uint8_t* code  = (uint8_t*)(elf.load + program.offset);

			for (uint32_t size = 0; size < program.memSize; size++)
			{
				vaddr[size] = code[size];
			}

			if (_ELF_Type_Dyn == elf.header->type) mapAddr += 0x10000;
		}
	}

	console.info("%s section to segment mapping successful", filename);
	return _OK;
}


/// @brief Post parser
/// @return result
int ElfParser::PostParser()
{
	//Returns when the elf type is not dynamic
	if (_ELF_Type_Dyn != elf.header->type) return _OK;

	//Set elf header pointer
	elf.header = (ELFHeader*)(elf.map);

	//Set executable entry
	elf.exec = elf.map + elf.header->entry;

	//Get some information of elf
	for (uint32_t i = 0; i < elf.header->sectionHeaderNum; i++)
	{
		SectionData data = GetDynSectionData(i);

		//Set dynamic section pointer
		if (_SHT_DYNAMIC == elf.sections[i].type)
		{
			elf.dynamics = data.dynamic;
			elf.dynsecNum = elf.sections[i].size / sizeof(DynamicHeader);
		}
		//Set dynsym pointer
		else if (_SHT_DYNSYM == elf.sections[i].type)
		{
			elf.dynsym = data.dynsym;
			elf.dynsymNum = elf.sections[i].size / sizeof(SymbolEntry);
		}
		//Set section header string table and symbol string talbe pointer
		else if (_SHT_STRTAB == elf.sections[i].type)
		{
			if (0 == strcmp(".dynstr", GetSectionName(i)))
				elf.dynstr = data.dynstr;
		}
	}

	console.info("%s post parser successful", filename);
	return _OK;
}


/// @brief Load shared objects
/// @return reuslt
int ElfParser::SharedObjs()
{
	SharedLibrary* lib = &libs;

	//Handler dynamic source
	for (uint32_t i = 0; i < elf.dynsecNum; i++)
	{
		DynamicHeader dynamic = elf.dynamics[i];

		if (_DT_NEEDED == dynamic.tag)
		{
			ElfParser* so = new ElfParser();
			
			const char* prefix = "libraries/";
			const char* name = GetDynamicString(dynamic.val);
			char* path = new char[strlen(prefix) + strlen(name) + 1]();
			strcat(path, prefix);
			strcat(path, name);

			if (_OK == so->Load(path))
			{
				lib->so = so;
				lib = lib->next;
			}
			else
			{
				console.error("%s load shared object %s failed", filename, path);
				delete[] path;
				return _ERR;
			}

			delete[] path;
		}
	}

	return _OK;
}


/// @brief Relocation dynamic symbol entries
/// @return result
int ElfParser::RelEntries()
{
	//Returns when elf type is not dynamic
	if (_ELF_Type_Dyn != elf.header->type) return _OK;

	//Set relocation tables
	for (uint32_t i = 0; i < elf.header->sectionHeaderNum; i++)
	{
		if (_SHT_REL == elf.sections[i].type)
		{
			//Calculate the number of relocation entries
			uint32_t relEntryNum = elf.sections[i].size / sizeof(RelocationEntry);

			//Get relocation entries table
			RelocationEntry* reltab = GetSectionData(i).reltab;

			//Relocation symbol entry
			for (uint32_t n = 0; n < relEntryNum; n++)
			{
				//Get relocation entry
				RelocationEntry relEntry = reltab[n];

				//Get symbol entry name
				const char* symName = GetDynSymName(relEntry.symbol);

				//Get relocation section addr
				uint32_t relAddr = elf.map + relEntry.offset;
				uint32_t symAddr = 0;

				//Searching for symbols in shared libraries
				for (SharedLibrary* lib = &libs; NULL != lib; lib = lib->next)
				{
					symAddr = lib->so->GetDynSymAddrByName(symName);
					if (0 != symAddr) break;
				}
				
				//Get the address of undefined symbol entry
				if (0 == symAddr) symAddr = SEARCH_SYMBOL(symName);

				//Return when symAddr is 0
				if (0 == symAddr) 
				{
					console.error("%s relocation %s not found.", filename, symName);
					console.error("%s relocation elf failed", filename);
					return _ERR;
				}

				//Relocation symbol entry
				RelSymCall(relAddr, symAddr, relEntry.type);

				//Output debug message
				console.log("%s rel name %s, relAddr 0x%lx, symAddr 0x%lx", 
					filename, symName, relAddr, symAddr);
			}
		}
	}

	console.info("%s relocation dyn elf successful", filename);
	return _OK;
}


#if defined(ARCH_X86)

/// @brief Relocation symbol call
/// @param relAddr 
/// @param symAddr 
/// @param type 
/// @return result
int ElfParser::RelSymCall(uint32_t relAddr, uint32_t symAddr, int type)
{
	switch (type)
	{
		case _R_386_32:
			*((uint32_t*)relAddr) += symAddr;
			break;

		case _R_386_PC32:
			*((uint32_t*)relAddr) += (symAddr - relAddr);
			break;
			
		case _R_386_GLOB_DAT:
			*((uint32_t*)relAddr) = symAddr;
			break;

		case _R_386_JMP_SLOT:
			*((uint32_t*)relAddr) = symAddr;
			break;

		default: return 0;
	}
	return 0;
}


/// @brief Relocation thumb jump call 
/// @param relAddr 
/// @param symAddr 
/// @param type 
/// @return address
int ElfParser::RelJumpCall(uint32_t relAddr, uint32_t symAddr, int type)
{
	return 0;
}

#elif defined(ARCH_ARM)

/// @brief Relocation symbol call
/// @param relAddr 
/// @param symAddr 
/// @param type 
/// @return address
int ElfParser::RelSymCall(uint32_t relAddr, uint32_t symAddr, int type)
{
	switch (type)
	{
		case _R_ARM_ABS32:
			*((uint32_t*)relAddr) += symAddr;
			break;

		case _R_ARM_THM_CALL:
		case _R_ARM_THM_JUMP24:
			RelJumpCall(relAddr, symAddr, type);
			break;

		case _R_ARM_TARGET1:
			*((uint32_t*)relAddr) += symAddr;
			break;

		case _R_ARM_THM_JUMP11:
			break;
			
		default: return 0;
	}
	return 0;
}


/// @brief Relocation thumb jump call 
/// @param relAddr 
/// @param symAddr 
/// @param type 
/// @return result
int ElfParser::RelJumpCall(uint32_t relAddr, uint32_t symAddr, int type)
{
	uint16_t upper = ((uint16_t *)relAddr)[0];
	uint16_t lower = ((uint16_t *)relAddr)[1];
	uint32_t S  = (upper >> 10) & 1;
	uint32_t J1 = (lower >> 13) & 1;
	uint32_t J2 = (lower >> 11) & 1;

	int32_t offset = (S    << 24) |  /* S     -> offset[24]    */
	((~(J1 ^ S)  & 1     ) << 23) |  /* J1    -> offset[23]    */
	((~(J2 ^ S)  & 1     ) << 22) |  /* J2    -> offset[22]    */
	((  upper    & 0x03ff) << 12) |  /* imm10 -> offset[12:21] */
	((  lower    & 0x07ff) << 1 );   /* imm11 -> offset[1:11]  */
	
	if (offset & 0x01000000) offset -= 0x02000000;

	offset += symAddr - relAddr;

	S  =       (offset >> 24) & 1;
	J1 = S ^ (~(offset >> 23) & 1);
	J2 = S ^ (~(offset >> 22) & 1);

	upper = ((upper & 0xf800) | (S << 10) | ((offset >> 12) & 0x03ff));
	((uint16_t*)relAddr)[0] = upper;

	lower = ((lower & 0xd000) | (J1 << 13) | (J2 << 11) | ((offset >> 1) & 0x07ff));
	((uint16_t*)relAddr)[1] = lower;

	return _OK;
}

#endif


/// @brief ElfParser execute symbol
/// @param symbol 
/// @return result
int ElfParser::Execute(const char* symbol, int argc, char* argv[])
{
	if (NULL == symbol && 0 != elf.exec)
	{
		((Entry)elf.exec)(argc, argv);
		return _OK;
	}
	else
	{
		uint32_t symbolAddr = GetSymbolAddrByName(symbol);
		
		if (symbolAddr)
		{
			((Entry)symbolAddr)(argc, argv);
			return _OK;
		}
	}
	console.error("%s %s not found", filename, symbol);
	return _ERR;
}


/// @brief ElfParser exit
/// @return result
int ElfParser::Exit()
{
	for (SharedLibrary* lib = &libs; NULL != lib; lib = lib->next)
	{
		delete lib->so;
		delete lib;
	}
	delete[] (uint8_t*)elf.map;
	return _OK;
}
