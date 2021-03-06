#!/bin/bash
set -e


make clean
make
hactool -t kip inject_libnx.kip --uncompressed inject_libnx_unpacked.kip
python3 kip1converter.py inject_libnx_unpacked.kip out.data
python3 kip1patch.py FS_6.1_exfat_uncomp.kip out.kip