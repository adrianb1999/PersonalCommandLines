#include "SetData.h"

SetData::SetData(const tstring &path, const tstring &valueName,
                 const tstring &valueNewData)
    : path{path}, valueName{valueName}, valueNewData{valueNewData} {}

bool SetData::execute() {

  tstring handleKeyString;
  GetRegistryHive(path, handleKeyString);

  size_t position = path.find_last_of(_T('\\'));

  position++;
  tstring strSubKeyName = path.substr(position, path.size());
  position--;

  path.erase(position, strSubKeyName.size() + 1);

  HKEY handleKey = NULL;
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handleKeyString);

  std::unique_ptr<HKEY, void (*)(HKEY *)> handleKeyPointer(&handleKey,
                                                        CloseRegKeyDeleter);

  CHECK_HANLE_KEY_RETURN(handleKey)

  returnValue =
      RegOpenKeyEx(handleKey, path.c_str(), 0, KEY_ALL_ACCESS, &handleKey);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  returnValue = RegSetKeyValue(handleKey, strSubKeyName.c_str(),
                               valueName.c_str(), REG_SZ, valueNewData.c_str(),
                               (DWORD)(valueNewData.size() + 1));

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  Server::AddCommandInString(_T("Set Data succes!\n"));

  return true;
}
