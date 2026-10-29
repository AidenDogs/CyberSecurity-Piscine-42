# Stockholm
Stockholm is a Linux command-line program written in C++ that simulates the encryption and decryption of files in the `~/infection` directory.
## Requirements
- Linux operating system with C++ compiler supporting C++17 or later
- OpenSSL development libraries and make tool
## Compilation
Build the program using the Makefile:
```
make
```
This generates the `stockholm` executable.
## Usage
Display the help message:
```
./stockholm --help
```
Display the program version:
```
./stockholm --version
```
Encrypt eligible files in `~/infection`:
```
./stockholm
```
Encrypt files without displaying output:
```
./stockholm --silent
```
Decrypt files using the encryption key:
```
./stockholm --reverse "your-encryption-key"
```
Decrypt files silently:
```
./stockholm --reverse "your-encryption-key" --silent
```
Short options are also available: `-h`, `-v`, `-r`, and `-s`.
## Behavior
- Operates only inside `~/infection`.
- Processes only supported file extensions associated with WannaCry.
- Encrypts file contents using a secure encryption algorithm (AES).
- Adds the `.ft` extension to encrypted files.
- Restores encrypted files when the correct key is provided.
- Reports errors without unexpectedly terminating.
**Warning:** Test only with disposable files in a controlled environment. Back up important data before running the program.
## Cleanup
Remove compiled files using:
```
make fclean
```