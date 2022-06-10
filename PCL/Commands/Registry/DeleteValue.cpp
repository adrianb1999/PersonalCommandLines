#include "DeleteValue.h"

DeleteValue::DeleteValue(const tstring &path, const tstring &valueName)
    : path{path}, valueName{valueName} {}

bool DeleteValue::execute() {

  tstring handleKeyString;
  GetRegistryHive(path, handleKeyString);

  HKEY handleKey = NULL;
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handleKeyString);

  std::unique_ptr<HKEY, void (*)(HKEY *)> hFind_ptr(&handleKey,
                                                    CloseRegKeyDeleter);
  CHECK_HANLE_KEY_RETURN(handleKey)

  returnValue =
      RegOpenKeyEx(handleKey, path.c_str(), 0, KEY_ALL_ACCESS, &handleKey);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  returnValue = RegDeleteValue(handleKey, valueName.c_str());

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  Server::AddCommandInString(_T("Delete Value Succes!\n"));

  return true;
}
