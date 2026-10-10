# ft_otp

A from-scratch one-time password generator implementing **HOTP (RFC 4226)** with a 30-second time-step counter, so its output matches `oathtool --totp`. No TOTP/HOTP library is used: HMAC-SHA1 is fed by hand and the dynamic truncation is implemented manually.

The project has two parts:

- **`cli/`**: the mandatory C++ program `ft_otp`.
- **`web/`**: bonus. A small TypeScript/Express app that generates a seed, renders it as a QR code (otpauth URI) and shows live OTPs in a browser UI.

## Project layout

```
.
├── Makefile
├── README.md
├── cli
│   ├── main.cpp          # argument parsing / entry point
│   ├── include/          # common, crypto, env, hex, hotp, key, seed headers
│   └── src/              # matching implementations
└── web
    ├── server.ts         # Express server
    ├── src/              # otp.ts, qrCode.ts, seed.ts
    ├── public/           # index.html, app.js, fx.js, style.css
    ├── package.json
    └── tsconfig.json
```

## Requirements

| Component | Requirement |
|-----------|-------------|
| CLI       | C++17 compiler (`g++`/`clang++`), `make`, OpenSSL (`libssl-dev`) |
| Web       | Node.js >= 20, pnpm |
| Checking  | `oathtool` (package `oath-toolkit`) |

> Adjust the OpenSSL line if the encryption layer in `crypto.cpp` uses a different backend.

## Build

```sh
make        # builds ./ft_otp
make clean  # removes object files
make fclean # removes objects and the binary
make re     # fclean + all
```

## Usage

```
./ft_otp -g <file>   # store the key from <file> (encrypted) in ft_otp.key
./ft_otp -k <file>   # print the current 6-digit OTP for the key in <file>
```

### `-g` : save the key

Takes a file containing a hexadecimal key of **at least 64 characters**. The key is validated (hex only, minimum length), encrypted, and written to `ft_otp.key`.

```sh
$ echo -n "NEVER GONNA GIVE YOU UP" > key.txt
$ ./ft_otp -g key.txt
./ft_otp: error: key must be 64 hexadecimal characters.

$ openssl rand -hex 32 | tr -d '\n' > key.hex
$ wc -c < key.hex
64
$ ./ft_otp -g key.hex
Key was successfully saved in ft_otp.key.
```

### `-k` : generate an OTP

Takes the encrypted key file, decrypts it in memory, and prints a 6-digit password.

```sh
$ ./ft_otp -k ft_otp.key
836492
$ sleep 60
$ ./ft_otp -k ft_otp.key
123518
```

## Algorithm

1. `counter = floor(unix_time / 30)`, encoded as an 8-byte big-endian integer.
2. `hs = HMAC-SHA1(key, counter)` (20 bytes). The hex key is decoded to raw bytes first.
3. Dynamic truncation (RFC 4226 §5.3):
   - `offset = hs[19] & 0x0f`
   - `bin = ((hs[offset] & 0x7f) << 24) | (hs[offset+1] << 16) | (hs[offset+2] << 8) | hs[offset+3]`
4. `otp = bin mod 10^6`, left-padded with zeros to always be 6 digits.

HMAC is built on top of a SHA-1 primitive from the crypto library; the HOTP logic itself (counter encoding, truncation, modulo, padding) lives in `cli/src/hotp.cpp`.

## Key storage

`ft_otp.key` never contains the plaintext key. The key is encrypted before being written (see `cli/src/crypto.cpp`), and the secret used for encryption is read through `cli/src/env.cpp`. Keep that secret out of version control.

Recommended `.gitignore` entries:

```
ft_otp.key
key.hex
key.txt
.env
web/node_modules
```

## Verifying against oathtool

```sh
$ ./ft_otp -k ft_otp.key
$ oathtool --totp $(cat key.hex)
```

Both must print the same 6 digits within the same 30-second window. Note that `oathtool --totp` defaults to SHA-1, 30s step and 6 digits, which is exactly what this program uses. Run both commands close together to avoid straddling a window boundary.

## Bonus: web UI + QR code

Located in `web/`. Generates a random seed, encodes it as an `otpauth://` URI, renders the QR code (scannable by Google Authenticator, Aegis, etc.) and displays the current OTP.

```sh
cd web
pnpm install
pnpm exec tsx server.ts
```

Then open the URL printed by the server (check `server.ts` for the default port). Configuration is loaded from a `.env` file via `dotenv`.

Stack: Express 5, `qrcode`, TypeScript 5.9, `tsx` for running without a build step.

## Notes and limitations

- HMAC-SHA1 is mandated by RFC 4226 and remains safe for HOTP/TOTP despite SHA-1's collision weaknesses.
- The OTP depends on system time; clock skew between this tool and the verifier will produce mismatches.
- Anyone able to read both `ft_otp.key` and the encryption secret can recover the seed. Protect them with filesystem permissions (`chmod 600 ft_otp.key`).