#include "DeleteKey.h"

DeleteKey::DeleteKey(tstring path) : path{path} {}

bool DeleteKey::execute() {

  tstring handleKeyString;
  GetRegistryHive(path, handleKeyString);

  size_t position = path.find_last_of('\\');
  position++;
  tstring strSubKeyName = path.substr(position, path.size());
  position--;
  path.erase(position, strSubKeyName.size() + 1);

  HKEY handleKey = NULL;
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handleKeyString);

  std::unique_ptr<HKEY, void (*)(HKEY *)> hFind_ptr(&handleKey,
                                                    CloseRegKeyDeleter);

  CHECK_HANLE_KEY_RETURN(handleKey);

  returnValue = RegOpenKeyEx(handleKey, path.c_str(), 0, KEY_READ, &handleKey);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  returnValue = RegDeleteKey(handleKey, strSubKeyName.c_str());

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  Server::AddCommandInString(_T("Delete Key Succes!\n"));

  return true;
}
