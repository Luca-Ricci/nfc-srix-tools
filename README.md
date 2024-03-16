# NFC SRIX Tools
A repository containing tools to read/write NFC ST SRI512 and SRIX4K tags.

## Known issues
* On 32bit machines, it should give an error

## TODOs
* Change reset-otp function

## Prerequisites
* [libnfc](https://github.com/nfc-tools/libnfc)

## Build
You can use the provided `build.sh` or follow these simple steps:
```bash
mkdir build
cd build
cmake ..
make
```

## Tools
* `srix-readTag` - read tag EEPROM, can choose to save it on file
* `srix-readDump` - Read dump file
* `srix-writeTag` - Write dump file to tag
* `srix-reset` - Reset OTP bits
* `srix-countdown` - Count down counter bits -=-1

## Examples
### srix-dump
Dump to console: `./srix-readTag`

Dump to file: `./srix-readTag -o file.bin`

Usage:
```text
Usage: ./srix-dump [dump.bin] [-h] [-v] [-i] [-r] [-o file.bin] [-t x4k|512]

Options:
  -h           show this help message
  -v           enable verbose - print debugging data
  -i           print UID and system block
  -r           fix read direction
  -o file.bin  save data to file.bin
  -t x4k|512   select SRIX4K or SRI512 tag type [default: x4k]
```

### srix-readDump
Usage:
```text
Usage: ./srix-readDump <dump.bin> [-h] [-v] [-c 1|2] [-t x4k|512]

Necessary arguments:
  <dump.bin>   path to the dump file

Options:
  -h           show this help message
  -v           enable verbose - print debugging data
  -c 1|2       erint on one or two columns [default: 1]
  -t x4k|512   select SRIX4K or SRI512 tag type [default: x4k]
```

### srix-writeTag
Usage:
```text
Usage: ./srix-writeTag <dump.bin> [-h] [-v] [-t x4k|512]

Necessary arguments:
  <dump.bin>   path to the dump file

Options:
  -h           show this help message
  -v           enable verbose - print debugging data
  -t x4k|512   select SRIX4K or SRI512 tag type [default: x4k]
```

### srix-resetTag
Usage:
```text
Usage: ./srix-resetTag [-h] [-v]

Options:
  -h           show this help message
  -v           enable verbose - print debugging data
```

### srix-updateCdown
Usage:
```text
Usage: ./srix-updateCdown [-h] [-v]

Options:
  -h           show this help message
  -v           enable verbose - print debugging data
```
