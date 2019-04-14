This is a poc for injecting full libnx into nintendo kips.

Right now this just hooks the fs-sdmmc-access-function and logs usage to a twili named-pipe.

Needs fullsvcperms until I figure out how to patch the kip-perms (should be trivial)

MD5 of the needed fs.kip is `bfd1c91eda3f3e17f946da6698ccd9c6` 7.0(.1) exfat decompressed.