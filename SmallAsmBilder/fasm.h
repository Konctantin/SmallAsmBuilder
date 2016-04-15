#pragma once
#include <Windows.h>

enum class FasmCondition : int
{
    OK                          = 00,
    WORKING                     = 01,
    FERROR                      = 02,
    INVALID_PARAMETER           = -1,
    OUT_OF_MEMORY               = -2,
    STACK_OVERFLOW              = -3,
    SOURCE_NOT_FOUND            = -4,
    UNEXPECTED_END_OF_SOURCE    = -5,
    CANNOT_GENERATE_CODE        = -6,
    FORMAT_LIMITATIONS_EXCEDDED = -7,
    WRITE_FAILED                = -8,
};

enum class FasmError : int
{
    FILE_NOT_FOUND                      = -101,
    ERROR_READING_FILE                  = -102,
    INVALID_FILE_FORMAT                 = -103,
    INVALID_MACRO_ARGUMENTS             = -104,
    INCOMPLETE_MACRO                    = -105,
    UNEXPECTED_CHARACTERS               = -106,
    INVALID_ARGUMENT                    = -107,
    ILLEGAL_INSTRUCTION                 = -108,
    INVALID_OPERAND                     = -109,
    INVALID_OPERAND_SIZE                = -110,
    OPERAND_SIZE_NOT_SPECIFIED          = -111,
    OPERAND_SIZES_DO_NOT_MATCH          = -112,
    INVALID_ADDRESS_SIZE                = -113,
    ADDRESS_SIZES_DO_NOT_AGREE          = -114,
    DISALLOWED_COMBINATION_OF_REGISTERS = -115,
    LONG_IMMEDIATE_NOT_ENCODABLE        = -116,
    RELATIVE_JUMP_OUT_OF_RANGE          = -117,
    INVALID_EXPRESSION                  = -118,
    INVALID_ADDRESS                     = -119,
    INVALID_VALUE                       = -120,
    VALUE_OUT_OF_RANGE                  = -121,
    UNDEFINED_SYMBOL                    = -122,
    INVALID_USE_OF_SYMBOL               = -123,
    NAME_TOO_LONG                       = -124,
    INVALID_NAME                        = -125,
    RESERVED_WORD_USED_AS_SYMBOL        = -126,
    SYMBOL_ALREADY_DEFINED              = -127,
    MISSING_END_QUOTE                   = -128,
    MISSING_END_DIRECTIVE               = -129,
    UNEXPECTED_INSTRUCTION              = -130,
    EXTRA_CHARACTERS_ON_LINE            = -131,
    SECTION_NOT_ALIGNED_ENOUGH          = -132,
    SETTING_ALREADY_SPECIFIED           = -133,
    DATA_ALREADY_DEFINED                = -134,
    TOO_MANY_REPEATS                    = -135,
    SYMBOL_OUT_OF_SCOPE                 = -136,
    USER_ERROR                          = -140,
    ASSERTION_FAILED                    = -141,
};

typedef struct _FasmLineHeader {
    PCHAR file_path;
    DWORD line_number;
    union {
        DWORD file_offset;
        DWORD macro_offset_line;
    };
    _FasmLineHeader* macro_line;
} FASM_LINE_HEADER;

typedef struct _FasmState {
    FasmCondition condition;
    union {
        FasmError error_code;
        DWORD output_length;
    };
    union {
        PBYTE output_data;
        _FasmLineHeader* error_data;
    };
} FASM_STATE;

PCHAR FasmConditionToString(FasmCondition cond)
{
#define FCCASE(x) case FasmCondition::x : return #x
    switch (cond) {
    FCCASE(OK);
    FCCASE(WORKING);
    FCCASE(FERROR);
    FCCASE(INVALID_PARAMETER);
    FCCASE(OUT_OF_MEMORY);
    FCCASE(STACK_OVERFLOW);
    FCCASE(SOURCE_NOT_FOUND);
    FCCASE(UNEXPECTED_END_OF_SOURCE);
    FCCASE(CANNOT_GENERATE_CODE);
    FCCASE(FORMAT_LIMITATIONS_EXCEDDED);
    FCCASE(WRITE_FAILED);
    }
    return NULL;
}

PCHAR FasmErrorToString(FasmError err)
{
#define FECASE(x) case FasmError::x : return #x
    switch (err) {
    FECASE(FILE_NOT_FOUND);
    FECASE(ERROR_READING_FILE);
    FECASE(INVALID_FILE_FORMAT);
    FECASE(INVALID_MACRO_ARGUMENTS);
    FECASE(INCOMPLETE_MACRO);
    FECASE(UNEXPECTED_CHARACTERS);
    FECASE(INVALID_ARGUMENT);
    FECASE(ILLEGAL_INSTRUCTION);
    FECASE(INVALID_OPERAND);
    FECASE(INVALID_OPERAND_SIZE);
    FECASE(OPERAND_SIZE_NOT_SPECIFIED);
    FECASE(OPERAND_SIZES_DO_NOT_MATCH);
    FECASE(INVALID_ADDRESS_SIZE);
    FECASE(ADDRESS_SIZES_DO_NOT_AGREE);
    FECASE(DISALLOWED_COMBINATION_OF_REGISTERS);
    FECASE(LONG_IMMEDIATE_NOT_ENCODABLE);
    FECASE(RELATIVE_JUMP_OUT_OF_RANGE);
    FECASE(INVALID_EXPRESSION);
    FECASE(INVALID_ADDRESS);
    FECASE(INVALID_VALUE);
    FECASE(VALUE_OUT_OF_RANGE);
    FECASE(UNDEFINED_SYMBOL);
    FECASE(INVALID_USE_OF_SYMBOL);
    FECASE(NAME_TOO_LONG);
    FECASE(INVALID_NAME);
    FECASE(RESERVED_WORD_USED_AS_SYMBOL);
    FECASE(SYMBOL_ALREADY_DEFINED);
    FECASE(MISSING_END_QUOTE);
    FECASE(MISSING_END_DIRECTIVE);
    FECASE(UNEXPECTED_INSTRUCTION);
    FECASE(EXTRA_CHARACTERS_ON_LINE);
    FECASE(SECTION_NOT_ALIGNED_ENOUGH);
    FECASE(SETTING_ALREADY_SPECIFIED);
    FECASE(DATA_ALREADY_DEFINED);
    FECASE(TOO_MANY_REPEATS);
    FECASE(SYMBOL_OUT_OF_SCOPE);
    FECASE(USER_ERROR);
    FECASE(ASSERTION_FAILED);
    }
    return NULL;
}

extern "C" int __cdecl fasm_Assemble(PCHAR szSource, PBYTE lpMemory, int nSize, int nPassesLimit, int hDisplayPipe);
extern "C" DWORD __cdecl fasm_GetVersion();