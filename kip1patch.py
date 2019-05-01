from struct import pack, unpack
from sys import argv
from pwn import context, asm
context.arch = 'aarch64'
# https://github.com/Gallopsled/pwntools/tree/dev3

APPLY_CUSTOM_TEXT = True

TEXT_OFFSET = 0x100000

if not APPLY_CUSTOM_TEXT:
    TEXT_OFFSET = 0


# secmon_panic:
# needs exo patched to call the panic call for the amos extension instead of the iram stuff and fullsvcperm enabled in hekate
# results in switch rebooting instantly (into rcm if autorcm is enabled) when called
panic_smc = """
    MOVZ	X0, #0x201
    MOVK	X0, #0xF000, LSL #16
    MOVZ	X1, #0xF00
    SVC	#0x7F
"""


patches = [
    # 3.0.0 fat32:
    # rtld_relocate_modules need to make meminfo.perm != 5 branch always happen else the switch won't boot
    # you can imagine how annoying finding out that *that* was the issue was
    # this one matches 3.0 fs
    #('B	#0xA4', TEXT_OFFSET + 0x4d0),
    # hook the sdmmc-handler in fs
    #('B	#0xFFFFFFFFFFE6E87C', TEXT_OFFSET + 0x91790)

    # 7.0.1 exfat:
    #('B	#0xA8', TEXT_OFFSET + 0x5a8),
    #('B	#0xFFFFFFFFFFD992DC', TEXT_OFFSET + 0x166d30)

    # 6.1 exfat:

    # sdmmc_wrapper_read
    ('B	#0xFFFFFFFFFFDAC36C', TEXT_OFFSET + 0x0153ca0),

    ('B	#0x98', TEXT_OFFSET + 0x05b0)
]


if not APPLY_CUSTOM_TEXT:
    print("WARNING WARNING!!! Not applying the custom text section!!!")

if len(argv) != 3:
    print("Usage: python3 kip1patch.py FS_in.kip FS_out.kip\nThe kip HAS to be uncompressed with kip1decomp first")
    exit(-1)

custom_text = open("out.data", "rb").read()
if len(custom_text) != TEXT_OFFSET and APPLY_CUSTOM_TEXT:
    print("PANIC: custom_text is not the correct size")
    exit(-1)


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

kernel_caps = []
for i in range(0x20):
    val, = unpack("I", f.read(4))
    kernel_caps.append(val)


for i in range(6):
    section = sections[i]
    section["Buffer"] = f.read(section["CompressedSize"])
    if APPLY_CUSTOM_TEXT:
        if i == 0:
            section["CompressedSize"] += TEXT_OFFSET
            section["DecompressedSize"] += TEXT_OFFSET
        elif i <= 3:
            section["OutOffset"] += TEXT_OFFSET

f.close()

if APPLY_CUSTOM_TEXT:
    sections[0]["Buffer"] = custom_text + sections[0]["Buffer"]

sections[0]["Buffer"] = sections[0]["Buffer"][:4] + \
    pack("I", 0) + sections[0]["Buffer"][8:]


for instr, offset in patches:
    buf = sections[0]["Buffer"]
    patch = asm(instr)
    out_buf = buf[:offset] + patch + buf[offset + len(patch):]
    sections[0]["Buffer"] = out_buf


f = open(argv[2], "wb")

f.write(header_start)

for section in sections:
    section_header = pack(
        "IIII", section["OutOffset"], section["DecompressedSize"], section["CompressedSize"], section["Attribute"])
    f.write(section_header)

for i in range(0x20):
    f.write(pack("I", kernel_caps[i]))

for section in sections:
    f.write(section["Buffer"])

f.close()
