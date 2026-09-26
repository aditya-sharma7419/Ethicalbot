#ifndef PLATFORM_NATIVE_H
#define PLATFORM_NATIVE_H

#include <QString>

class PlatformNative
{
public:
    static bool CheckAndRequestPermissions();
    static bool CreateRegistryBackupAndRestorePoint(const QString &actionName);
    static void InitiateSelfDeletion();
};

#endif // PLATFORM_NATIVE_H