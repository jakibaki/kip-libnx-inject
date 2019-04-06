from struct import pack, unpack
from sys import argv
from pwn import context, asm
context.arch = 'aarch64'
# https://github.com/Gallopsled/pwntools/tree/dev3

TEXT_OFFSET = 0x100000


f = open(argv[1], "rb")

header_start = f.read(0x20)


section_names = [".text", ".rodata", ".data", ".bss"]

sections = []
for i in range(6):
    section_bytes = f.read(0x10)
    section = {}

    if i < len(section_names):
        section["Name"] = section_names[i]

    section["OutOffset"], section["DecompressedSize"], section["CompressedSize"], section["Attribute"] = unpack(
        "IIII", section_bytes)
    sections.append(section)
    print(section)

f.seek(0x100)
#discard kernel caps...

for i in range(3):
    section = sections[i]
    section["Buffer"] = f.read(section["DecompressedSize"])
print(f.read())

f.close()


f = open(argv[2], "wb")
for i in range(3):
    section = sections[i]
    f.seek(section["OutOffset"])
    f.write(section["Buffer"])
f.seek(TEXT_OFFSET-1)
f.write(b'\0')