#ifndef __mock_pkgmgr_installer_h__
#define __mock_pkgmgr_installer_h__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pkgmgr_installer pkgmgr_installer;

pkgmgr_installer *pkgmgr_installer_new();
int pkgmgr_installer_free(pkgmgr_installer* pi);

#ifdef __cplusplus
}
#endif

#endif // __mock_pkgmgr_installer_h__
