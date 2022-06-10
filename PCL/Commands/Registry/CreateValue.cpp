#include "CreateValue.h"

CreateValue::CreateValue(const tstring &path, const tstring &valueName,
                         const tstring &valueType, const tstring &valueData)
    : path{path}, valueName{valueName}, valueType{valueType}, valueData{
                                                                  valueData} {}

bool CreateValue::execute() {

  tstring handkeKeyString;
  GetRegistryHive(path, handkeKeyString);

  HKEY handleKey = NULL;
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handkeKeyString);

  std::unique_ptr<HKEY, void (*)(HKEY *)> handleKeyPointer(&handleKey,
                                                        CloseRegKeyDeleter);

  CHECK_HANLE_KEY_RETURN(handleKey)

  returnValue =
      RegOpenKeyEx(handleKey, path.c_str(), 0, KEY_ALL_ACCESS, &handleKey);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  returnValue = RegSetValueEx(
      handleKey, valueName.c_str(), NULL, GetRegValueTypeByString(valueType),
      (LPBYTE)valueData.c_str(), (DWORD)(valueData.size() + 1));

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  Server::AddCommandInString(_T("Create Value Succes!\n"));
  return true;
}
