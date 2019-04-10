This is a poc for injecting full libnx into nintendo kips.

Right now this just hooks the fs-sdmmc-access-function and logs usage to a twili named-pipe.

Needs fullsvcperms until I figure out how to patch the kip-perms (should be trivial)