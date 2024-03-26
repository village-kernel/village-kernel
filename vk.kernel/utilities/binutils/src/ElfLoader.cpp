//###########################################################################
// ElfLoader.cpp
// Definitions of the functions that manage elf loader
//
// $Copyright: Copyright (C) village
//###########################################################################
#include "ElfLoader.h"
#include "FileStream.h"
#include "LibraryTool.h"
#include "Kernel.h"
#include "string.h"


/// @brief Constructor
/// @param filename 
ElfLoader::ElfLoader(const char* filename)
	:isIgnoreUnresolvedSymbols(false)
{
	if (NULL != filename) Load(filename);
}


/// @brief Deconstructor
ElfLoader::~ElfLoader()
{
	Exit();
}


/// @brief ElfLoader load and parser elf file
/// @param filename 
/// @return result
bool ElfLoader::Load(const char* filename)
{
	//Save filename in local
	this->filename = new char[strlen(filename) + 1]();
	strcpy(this->filename, filename);

	if (!LoadElf())     return false;
	if (!PreParser())   return false;
	if (!SetMapAddr())  return false;
	if (!LoadProgram()) return false;
	if (!PostParser())  return false;
	if (!SharedObjs())  return false;
	if (!RelEntries())  return false;
	
	kernel->debug.Output(Debug::_Lv2, "%s load done", filename);
	return true;
}


/// @brief ElfLoader load elf file
/// @param filename 
/// @return result
bool ElfLoader::LoadElf()
{
	FileStream file;

	if (file.Open(filename, FileMode::_Read))
	{
		int size = file.Size();
		elf.load = (uint32_t)new char[size]();

		if (elf.load && (file.Read((char*)elf.load, size) == size))
		{
			kernel->debug.Output(Debug::_Lv1, "%s elf file load successful", filename);
			file.Close();
			return true;
		}

		file.Close();
	}

	kernel->debug.Error("%s elf file load failed", filename);
	return false;
}


/// @brief Get file name
/// @return 
const char* ElfLoader::GetFileName()
{
	return filename;
}


/// @brief Get dynamic string
/// @param index 
/// @return string
inline const char* ElfLoader::GetDynamicString(uint32_t index)
{
	return (const char*)elf.dynstr + index;
}


/// @brief Get section name
/// @param index 
/// @return name
inline const char* ElfLoader::GetSectionName(uint32_t index)
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
inline const char* ElfLoader::GetSymbolName(uint32_t index)
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
inline const char* ElfLoader::GetDynSymName(uint32_t index)
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
inline uint32_t ElfLoader::GetSymbolAddr(uint32_t index)
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
inline uint32_t ElfLoader::GetDynSymAddr(uint32_t index)
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
uint32_t ElfLoader::GetSymbolAddrByName(const char* name)
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
uint32_t ElfLoader::GetDynSymAddrByName(const char* name)
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
inline ElfLoader::SectionData ElfLoader::GetSectionData(uint32_t index)
{
	return SectionData(elf.load + elf.sections[index].offset);
}


/// @brief Get dyn section data
/// @param index 
/// @return address
inline ElfLoader::SectionData ElfLoader::GetDynSectionData(uint32_t index)
{
	return SectionData(elf.map + elf.sections[index].addr);
}


/// @brief Ignore unresolved symbols
/// @param enable 
void ElfLoader::IgnoreUnresolvedSymbols(bool enable)
{
	isIgnoreUnresolvedSymbols = enable;
}


/// @brief Pre parser
/// @return result
bool ElfLoader::PreParser()
{
	//Set elf header pointer
	elf.header = (ELFHeader*)(elf.load);

	//Check if it is a valid elf file
	const uint8_t elfmagic[] = {0x7f, 'E', 'L', 'F'};
	if (elf.header->ident[0] !=  elfmagic[0]    ) return false;
	if (elf.header->ident[1] !=  elfmagic[1]    ) return false;
	if (elf.header->ident[2] !=  elfmagic[2]    ) return false;
	if (elf.header->ident[3] !=  elfmagic[3]    ) return false;
	if (elf.header->ident[4] != _ELF_Class_32   ) return false;
	if (elf.header->version  != _ELF_Ver_Current) return false;
#if defined(ARCH_X86)
	if (elf.header->machine  != _ELF_Machine_X86) return false;
#elif defined(ARCH_ARM)
	if (elf.header->machine  != _ELF_Machine_ARM) return false;
#endif
	if ((elf.header->type    != _ELF_Type_Dyn) &&
		(elf.header->type    != _ELF_Type_Exec))
	{
		kernel->debug.Error("%s is not executable", filename);
		return false;
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

	kernel->debug.Output(Debug::_Lv1, "%s pre parser successful", filename);
	return true;
}


/// @brief Set mapping address
/// @return result
bool ElfLoader::SetMapAddr()
{
	if (_ELF_Type_Dyn == elf.header->type)
	{
		uint32_t memsize = 0;
		for (uint32_t i = 0; i < elf.header->programHeaderNum; i++)
		{
			ProgramHeader program = elf.programs[i];

			if (_PT_LOAD == program.type)
			{
				memsize = (program.vaddr + program.memSize) + (program.align - 1);
				memsize = memsize / program.align * program.align;
			}
		}
		elf.map = (uint32_t)new char[memsize]();
	}
	else if (_ELF_Type_Exec == elf.header->type)
	{
		elf.map = 0;
	}

	kernel->debug.Output(Debug::_Lv1, "%s set mapping address successful", filename);
	return true;
}


/// @brief Load program to mapping address
/// @return result
bool ElfLoader::LoadProgram()
{
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
		}
	}

	kernel->debug.Output(Debug::_Lv1, "%s load program successful", filename);
	return true;
}


/// @brief Post parser
/// @return result
bool ElfLoader::PostParser()
{
	//Returns when the elf type is not dynamic
	if (_ELF_Type_Dyn != elf.header->type) return true;

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

	kernel->debug.Output(Debug::_Lv1, "%s post parser successful", filename);
	return true;
}


/// @brief Load shared objects
/// @return result
bool ElfLoader::SharedObjs()
{
	//Handler dynamic source
	for (int i = elf.dynsecNum - 1; i >= 0; i--)
	{
		DynamicHeader dynamic = elf.dynamics[i];

		if (_DT_NEEDED == dynamic.tag)
		{
			//Gets the shared object path
			const char* prefix = "/libraries/";
			const char* name = GetDynamicString(dynamic.val);
			char* path = new char[strlen(prefix) + strlen(name) + 1]();
			strcat(path, prefix);
			strcat(path, name);

			//Load shared object lib
			if (false == LibraryTool().Install(path))
			{
				kernel->debug.Error("%s load shared object %s failed", filename, path);
				delete[] path;
				return false;
			}

			delete[] path;
		}
	}

	return true;
}


/// @brief Relocation dynamic symbol entries
/// @return result
bool ElfLoader::RelEntries()
{
	//Returns when elf type is not dynamic
	if (_ELF_Type_Dyn != elf.header->type) return true;

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

				//Get symbol entry
				SymbolEntry symEntry = elf.dynsym[relEntry.symbol];

				//Get symbol entry name
				const char* symName = GetDynSymName(relEntry.symbol);

				//Get relocation section addr
				uint32_t relAddr = elf.map + relEntry.offset;
				uint32_t symAddr = 0;

				//Get the address of symbol entry when the relocation entry type is relative
				if (_R_TYPE_RELATIVE == relEntry.type) symAddr = elf.map;

				//Get the address of symbol entry when the relocation entry type is copy
				if (_R_TYPE_COPY == relEntry.type) symAddr = LibraryTool().SearchSymbol(symName);
				
				//Get the address of object symbol entry
				if (0 == symAddr && symEntry.shndx) symAddr = elf.map + symEntry.value;

				//Get the address of undefined symbol entry
				if (0 == symAddr) symAddr = kernel->symbol.Search(symName);

				//Searching for symbol entry in shared objects
				if (0 == symAddr) symAddr = LibraryTool().SearchSymbol(symName);

				//Return when symAddr is 0
				if (0 == symAddr) 
				{
					if (true == isIgnoreUnresolvedSymbols)
					{
						kernel->debug.Warn("%s relocation symbols ignore, symbol %s not found", filename, symName);
						continue;
					}
					else
					{
						kernel->debug.Error("%s relocation symbols failed, symbol %s not found", filename, symName);
						return false;
					}
				}

				//Relocation symbol entry
				RelSymCall(relAddr, symAddr, relEntry.type, symEntry.size);

				//Output debug message
				kernel->debug.Output(Debug::_Lv0, "%s rel name %s, relAddr 0x%lx, symAddr 0x%lx", 
					filename, symName, relAddr, symAddr);
			}
		}
	}

	kernel->debug.Output(Debug::_Lv1, "%s relocation entries successful", filename);
	return true;
}


#if defined(ARCH_X86)

/// @brief Relocation symbol call
/// @param relAddr 
/// @param symAddr 
/// @param type 
void ElfLoader::RelSymCall(uint32_t relAddr, uint32_t symAddr, uint8_t type, uint32_t size)
{
	uint32_t A = *((uint32_t*)relAddr);
	uint32_t B = elf.map;
	uint32_t G = 0;     //
	uint32_t GOT = 0;   //
	uint32_t L = 0;     //
	uint32_t Z = 0;     //
	uint32_t P = relAddr;
	uint32_t S = symAddr;
	uint32_t* relVal = (uint32_t*)relAddr;

	switch (type)
	{
		case _R_386_32:
			*relVal = S + A;
			break;

		case _R_386_PC32:
			*relVal = S + A - P;
			break;

		case _R_386_GOT32:
			*relVal = G + A;
			break;

		case _R_386_PLT32:
			*relVal = L + A - P;
			break;

		case _R_386_COPY:
			memcpy(relVal, (const void*)S, size);
			break;

		case _R_386_GLOB_DAT:
			*relVal = S;
			break;

		case _R_386_JMP_SLOT:
			*relVal = S;
			break;

		case _R_386_RELATIVE:
			*relVal = B + A;
			break;
		
		case _R_386_GOTOFF:
			*relVal = S + A - GOT;
			break;
		
		case _R_386_GOTPC:
			*relVal = GOT + A - P;
			break;

		case _R_386_32PLT:
			*relVal = L + A;
			break;

		case _R_386_16:
			*relVal = S + A;
			break;

		case _R_386_PC16:
			*relVal = S + A - P;
			break;

		case _R_386_8:
			*relVal = S + A;
			break;

		case _R_386_PC8:
			*relVal = S + A - P;
			break;

		case _R_386_SIZE32:
			*relVal = Z + A;
			break;

		default: break;
	}
}

#elif defined(ARCH_ARM)

/// @brief Relocation symbol call
/// @param relAddr 
/// @param symAddr 
/// @param type 
/// @return address
void ElfLoader::RelSymCall(uint32_t relAddr, uint32_t symAddr, uint8_t type, uint32_t size)
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

		case _R_ARM_RELATIVE:
			*((uint32_t*)relAddr) += symAddr;
			break;

		case _R_ARM_THM_JUMP11:
			break;
			
		default: break;
	}
}


/// @brief Relocation thumb jump call 
/// @param relAddr 
/// @param symAddr 
/// @param type 
/// @return result
void ElfLoader::RelJumpCall(uint32_t relAddr, uint32_t symAddr, uint8_t type)
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
}

#endif


/// @brief ElfLoader Fill bss zero
/// @return result
bool ElfLoader::FillBssZero()
{
	for (uint32_t i = 0; i < elf.header->sectionHeaderNum; i++)
	{
		if (0 == strcmp(".bss", GetSectionName(i)))
		{
			//Get the bss pointer
			uint8_t* data = GetDynSectionData(i).data;

			//Get the size of bss
			uint32_t size = elf.sections[i].size;

			//Fill zero
			for (uint32_t i = 0; i < size; i++)
			{
				data[i] = 0;
			}

			break;
		}
	}
	return true;
}


/// @brief ElfLoader init array
/// @return result
bool ElfLoader::InitArray()
{
	for (uint32_t i = 0; i < elf.header->sectionHeaderNum; i++)
	{
		if (0 == strcmp(".init_array", GetSectionName(i)))
		{
			//Get init array
			Function* funcs = GetDynSectionData(i).funcs;

			//Calculate the size of init array
			uint32_t size = elf.sections[i].size / sizeof(Function);

			//Execute init array
			for (uint32_t i = 0; i < size; i++)
			{
				(funcs[i])();
			}

			break;
		}
	}
	return true;
}


/// @brief ElfLoader execute symbol
/// @param symbol 
/// @return result
bool ElfLoader::Execute(const char* symbol, int argc, char* argv[])
{
	if (NULL == symbol && 0 != elf.exec)
	{
		((StartEntry)elf.exec)(kernel, argc, argv);
		return true;
	}
	else
	{
		uint32_t symbolAddr = GetSymbolAddrByName(symbol);
		
		if (symbolAddr)
		{
			((FuncEntry)symbolAddr)(argc, argv);
			return true;
		}
	}
	kernel->debug.Error("%s %s not found", filename, symbol);
	return false;
}


/// @brief ElfLoader fini array
/// @return result
bool ElfLoader::FiniArray()
{
	for (uint32_t i = 0; i < elf.header->sectionHeaderNum; i++)
	{
		if (0 == strcmp(".fini_array", GetSectionName(i)))
		{
			//Get fini array
			Function* funcs = GetDynSectionData(i).funcs;

			//Calculate the size of fini array
			uint32_t size = elf.sections[i].size / sizeof(Function);

			//Execute fini array
			for (uint32_t i = 0; i < size; i++)
			{
				(funcs[i])();
			}

			break;
		}
	}
	return true;
}


/// @brief ElfLoader exit
/// @return result
bool ElfLoader::Exit()
{
	delete[] filename;
	delete[] (char*)elf.load;
	delete[] (char*)elf.map;
	return true;
}
