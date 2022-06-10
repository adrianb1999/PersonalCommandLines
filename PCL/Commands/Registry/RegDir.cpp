#include "RegDir.h"
#include <BaseTsd.h>
#define BUFF_SIZE 1024

RegDir::RegDir(const tstring &path) : path{path} {}

typedef unsigned __int64 QWORD;

bool RegDir::execute() {

  tstring handleKeyString;

  if (!GetRegistryHive(path, handleKeyString)) {
    return false;
  }

  size_t position = path.find_last_of(_T('\\'));

  tstring strSubKeyName = path.substr(position, path.size());

  LPCTSTR subKey = path.c_str();
  HKEY handleKey = NULL;
  LSTATUS returnValue{};

  handleKey = GetHKEYByString(handleKeyString);
  CHECK_HANLE_KEY_RETURN(handleKey)

  returnValue = RegOpenKeyEx(handleKey, subKey, 0, KEY_READ, &handleKey);
  CHECK_RETURN_VALUE_ERROR_SUCCESS(returnValue)

  Server::AddCommandInString(strSubKeyName, _T("\n"));

  if (!recShow(returnValue, handleKey, subKey, 1, handleKeyString))
    return false;

  return true;
}

bool RegDir::recShow(LSTATUS &returnValue, HKEY handleKey,
                     const LPCTSTR subKey, DWORD currentSpace,
                     tstring handleKeyString) {

  handleKey = GetHKEYByString(handleKeyString);

  returnValue = RegOpenKeyEx(handleKey, subKey, 0, KEY_READ, &handleKey);

  if (returnValue != ERROR_SUCCESS) {
    return false;
  }

  DWORD noOfSubKeys;
  DWORD noOfSubValues;

  FILETIME ftLastWriteTime;

  returnValue =
      RegQueryInfoKey(handleKey, NULL, NULL, NULL, &noOfSubKeys, NULL, NULL,
                      &noOfSubValues, NULL, NULL, NULL, &ftLastWriteTime);

  if (returnValue != ERROR_SUCCESS) {
    return false;
  }
 
  TCHAR subKeyValueName[BUFF_SIZE];
  DWORD buffSize;
  BYTE subKeyValueData[BUFF_SIZE];
  DWORD dataType{};
  DWORD dataSize;

  if (noOfSubValues) {
    for (DWORD index = 0; index < noOfSubValues; index++) {

      buffSize = BUFF_SIZE;
      dataSize = BUFF_SIZE;
      subKeyValueName[0] = '\0';
      tstring output;

      returnValue = RegEnumValue(handleKey, index, subKeyValueName, &buffSize, NULL,
                                 &dataType, subKeyValueData, &dataSize);

      if (returnValue == ERROR_SUCCESS) {
        for (DWORD secondIndex = 0; secondIndex < currentSpace; secondIndex++) {
          Server::AddCommandInString(_T("   "));
        }
        if (dataType == REG_SZ || dataType == REG_EXPAND_SZ || dataType == REG_LINK ||
            dataType == REG_BINARY) {

          output = subKeyValueName;
          output = _T("v: ") + output + _T(" ") + (TCHAR *)subKeyValueData + _T('\n');
        }
        if (dataType == REG_DWORD) {
          LPDWORD dataDWORD = (DWORD *)subKeyValueData;

          output = subKeyValueName;
          output =
              _T("v: ") + output + _T(" ") + to_tstring(*dataDWORD) + _T('\n');
        }
        if (dataType == REG_QWORD) {
          QWORD *dataDWORD = (QWORD *)subKeyValueData;

          output = subKeyValueName;
          output =
              _T("v: ") + output + _T(" ") + to_tstring(*dataDWORD) + _T('\n');
        }
        if (dataType == REG_MULTI_SZ) {
          output = subKeyValueName;
          output = _T("v: ") + output + _T(" ");
        }
        Server::AddCommandInString(output);
      }
    }
  }

  TCHAR subKeyName[BUFF_SIZE];

  if (noOfSubKeys) {
    for (DWORD index = 0; index < noOfSubKeys; index++) {

      buffSize = BUFF_SIZE;
      returnValue = RegEnumKeyEx(handleKey, index, subKeyName, &buffSize, NULL,
                                 NULL, NULL, &ftLastWriteTime);

      if (returnValue == ERROR_SUCCESS) {

        for (DWORD secondIndex = 0; secondIndex < currentSpace; secondIndex++) {
          if (secondIndex == currentSpace - 1) {
            Server::AddCommandInString(_T("|__"));
          } else {
            Server::AddCommandInString(_T("   "));
          }
        }

        Server::AddCommandInString(subKeyName, _T("\n"));

        tstring newSubKeyName(subKeyName);
        tstring newSubKeyPath;
        newSubKeyPath = path + _T("\\") + newSubKeyName;

        currentSpace++;
        recShow(returnValue, handleKey, newSubKeyPath.c_str(), currentSpace,
                handleKeyString);
        currentSpace--;
      }
    }
  }
  return true;
}
