#include "CreateKey.h"
#include <iostream>

CreateKey::CreateKey(const tstring &path, const tstring &subKeyName)
    : path{path}, subKeyName{subKeyName} {}

bool CreateKey::execute() { 
  tstring handleKeyString;
  GetRegistryHive(path, handleKeyString);

  HKEY handleKey{};
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handleKeyString);

  std::unique_ptr<HKEY, void (*)(HKEY *)> hFind_ptr(&handleKey,
                                                    CloseRegKeyDeleter);

  CHECK_HANLE_KEY_RETURN(handleKey)

  returnValue = RegOpenKeyEx(handleKey, path.c_str(), 0, KEY_READ, &handleKey);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  returnValue = RegCreateKeyEx(handleKey, subKeyName.c_str(), NULL, NULL,
                               REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                               &handleKey, NULL);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  Server::AddCommandInString(_T("Create Key Succes!\n"));
  return true;
}
