# ft_otp

Generatore di one-time password (TOTP, RFC 6238 — vedi nota sotto su HOTP vs TOTP)
in C++17, con storage della chiave cifrato (AES-256-GCM, chiave derivata via
PBKDF2-HMAC-SHA256) e interfaccia web opzionale (bonus).

## Struttura del progetto

```
ft_otp/
├── Makefile
├── .env.example
├── README.md
├── include/                # header, uno per responsabilità
│   ├── common.hpp          # costanti condivise (lunghezze, iterazioni PBKDF2, periodo TOTP...)
│   ├── hex_utils.hpp        # hex <-> bytes
│   ├── env_utils.hpp        # lettura .env
│   ├── crypto_utils.hpp     # PBKDF2 + AES-256-GCM
│   ├── key_storage.hpp       # validazione, normalizzazione, persistenza della chiave
│   └── hotp.hpp             # algoritmo HOTP/TOTP (HMAC-SHA1 manuale + troncamento RFC4226)
├── src/
│   ├── hex_utils.cpp
│   ├── env_utils.cpp
│   ├── crypto_utils.cpp
│   ├── key_storage.cpp
│   ├── hotp.cpp
│   └── main.cpp             # CLI: parsing -g / -k / -s
└── web/                      # BONUS: interfaccia web
    ├── src/
    │   ├── http_mini.hpp/.cpp   # mini server HTTP su socket POSIX, zero dipendenze esterne
    │   └── server.cpp           # espone /api/generate, /api/seed, /api/otp + file statici
    └── public/
        ├── index.html
        ├── style.css
        ├── app.js
        └── vendor/               # (da creare tu, vedi sezione QR code)
```

`src/*.cpp` (la libreria "core") non conosce né la CLI né il server web: entrambi
la chiamano allo stesso modo. Questo è il motivo principale del refactor rispetto
al file unico originale — vedi "Cosa è cambiato" più sotto.

## Build

```sh
make          # produce ./ft_otp e ./ft_otp_web
make re        # ricompila da zero
```

Richiede `libssl-dev` (header OpenSSL). Su Debian/Ubuntu: `sudo apt install libssl-dev`.

Prima di usare uno dei due binari, crea un `.env` nella directory da cui li lanci:

```sh
cp .env.example .env
# poi modifica AES_SECRET_KEY, es:
echo "AES_SECRET_KEY=$(openssl rand -hex 32)" > .env
```

## Uso — CLI (mandatory part)

```sh
./ft_otp -g key.hex     # key.hex contiene >= 64 caratteri esadecimali
# Key was successfully saved in ft_otp.key

./ft_otp -k ft_otp.key
# 836492

sleep 30
./ft_otp -k ft_otp.key
# 123518

./ft_otp -s              # bonus: genera un seed casuale e lo salva
```

Verifica incrociata con oathtool (lo stesso usato in valutazione):

```sh
oathtool --totp $(cat key.hex)
```

## Uso — interfaccia web (bonus)

```sh
./ft_otp_web            # porta di default 8080
./ft_otp_web 9090        # oppure porta a scelta
```

Poi apri `http://localhost:8080`. **Va lanciato dalla root del progetto** perché
serve i file statici da `web/public/` (path relativo alla working directory, non
all'eseguibile — vedi limiti noti).

Dalla pagina puoi:
- incollare una chiave esadecimale e salvarla cifrata (`POST /api/generate`);
- generare un seed casuale (`POST /api/seed`), che ti viene mostrato **una sola volta**;
- vedere l'OTP corrente aggiornarsi da solo (`GET /api/otp`, polling ogni 5s + countdown lato client).

### QR code (bonus)

Il frontend costruisce già l'URI `otpauth://totp/...` (con secret in base32) a
partire dal seed generato. Per renderlo effettivamente come QR code nella pagina,
aggiungi una libreria JS di generazione QR in `web/public/vendor/qrcode.min.js`
(non l'ho vendorizzata io nel repo per non includere codice di terze parti senza
che tu ne scelga/verifichi la licenza — una scelta comune e leggera è
`davidshimjs/qrcodejs`, MIT). Senza quel file, la pagina mostra comunque l'URI
`otpauth://` in chiaro con relativo avviso, quindi resta utilizzabile.

## Cosa è cambiato rispetto al file originale (otp.cpp)

### Bug corretti

1. **`argv[2]` letto senza controllare `argc`** (`-g` e `-k`): lanciando
   `./ft_otp -g` senza secondo argomento si leggeva fuori dai limiti dell'array
   `argv` (undefined behavior, tipicamente crash). Ora `exeProgram` verifica
   `argc < 3` prima di accedere a `argv[2]` e stampa un errore pulito.

2. **Periodo TOTP di default sbagliato**: `generateOTP` chiamava
   `getTotpTimeStep()` senza argomenti, cadendo sul default `period = 1`
   dichiarato nella firma originale → il codice sarebbe cambiato **ogni
   secondo** invece che ogni 30. Dato che il subject dice esplicitamente che la
   valutazione userà `oathtool --totp` (default 30s, RFC 6238), con period=1 i
   codici **non avrebbero mai combaciato** con l'oracolo di riferimento. Ora
   `TOTP_PERIOD_SECONDS = 30` è una costante in `common.hpp` e viene passata
   esplicitamente in `generateOtpFromFile`. Questo è il bug più critico:
   comportamento coerente in isolamento, ma non passa la verifica con lo
   strumento di riferimento indicato dal subject.

3. **Eccezioni non catturate in `generateOTP`**: `hexStringToBytes` (chiamata
   su salt/iv/ciphertext letti da `ft_otp.key`) lanciava `std::invalid_argument`
   senza alcun `try/catch` attorno, a differenza delle chiamate di decrypt che
   invece erano protette. Un `ft_otp.key` corrotto con caratteri non esadecimali
   terminava il programma con `std::terminate`/abort invece di un messaggio di
   errore pulito. Ora tutta la libreria lancia eccezioni in modo uniforme e
   viene gestita centralmente in `main.cpp` (CLI) e nel server web.

4. **`exit(1)` sparsi dentro le funzioni di libreria** (`isValidKey`,
   `storeKey`, `generateSeed`): correvano bene per una CLI, ma se quelle stesse
   funzioni fossero state riusate as-is in un server (bonus web), un input
   utente invalido avrebbe ucciso l'intero processo, interrompendo il servizio
   per chiunque altro. Refactorizzate per lanciare eccezioni; la CLI le
   converte in `exit(1)` a livello di `main`, il server le converte in risposte
   HTTP 4xx per singola richiesta.

5. **Messaggio di debug residuo**: `isValidKey` stampava un errore con in coda
   la stringa letterale `"MHANZ{} "` seguita dal carattere non valido —
   chiaramente un residuo di debug lasciato nel codice. Rimosso.

6. **Seed in chiaro scritto su un file temporaneo**: `generateSeed` scriveva il
   seed casuale (in chiaro) su `std::filesystem::temp_directory_path()/seed.tmp`
   solo per poterlo rileggere e riusare la firma `storeKey(ifstream&)`. Su un
   sistema con `/tmp` condiviso o non su tmpfs, il segreto in chiaro passava dal
   filesystem. Ora `generateSeed` lavora in memoria con uno `stringstream`, non
   tocca mai il disco per il valore in chiaro.

### Design/robustezza (non bloccanti, segnalati per trasparenza)

- **Permessi del file `ft_otp.key`**: viene creato con i permessi di default
  (dipendenti da `umask`) e solo dopo ristretto con `chmod 600`. C'è una piccola
  finestra in cui il file esiste con permessi più larghi. Per eliminarla
  servirebbe `open(O_CREAT|O_EXCL, 0600)` + wrapping in uno stream, cosa che non
  ho applicato per non allontanarmi troppo dalla struttura originale — se ti
  serve per la valutazione lo aggiungo.
- **`getEnvValue` legge `.env` dalla working directory corrente**: se lanci i
  binari da una directory diversa da quella del `.env`, non lo trova. Documentato
  sopra ("Build"); un'alternativa sarebbe accettare un path esplicito o un
  fallback su `getenv()`.
- **Materiale sensibile non azzerato in memoria**: `aesKey`, `normalizedKey`,
  il segreto HOTP decifrato, ecc. restano in `std::vector`/`std::array` senza
  `OPENSSL_cleanse` esplicito prima della distruzione. Per un progetto scolastico
  è accettabile, ma è la prima cosa da aggiungere se questo codice dovesse
  girare in un contesto reale.
- **Il parser JSON del server web è volutamente minimale** (`extractJsonStringField`
  in `web/src/server.cpp`): estrae un solo campo stringa da un body piatto, non è
  un parser JSON generico. Sufficiente per i 3 endpoint di questo progetto.

### ⚠️ Incongruenza nel subject stesso (non nel tuo codice): HOTP vs TOTP

Il subject dichiara: *"Your program must use the HOTP algorithm (RFC 4226)"*,
ma l'esempio d'uso mostra:

```
$ ./ft_otp -k ft_otp.key
836492
$ sleep 60
$ ./ft_otp -k ft_otp.key
123518
```

Questo è comportamento **TOTP** (RFC 6238): il codice cambia da solo al passare
del tempo, senza che nessuno "consumi" un contatore persistente incrementale
come vorrebbe HOTP puro. Inoltre il subject dice esplicitamente che verificherà
con `oathtool --totp`, non `oathtool --hotp`. Il codice originale (e questo
refactor) implementano correttamente **TOTP** (contatore = `unix_time / 30`),
che è quello che l'esempio e lo strumento di riferimento si aspettano, anche se
il testo del subject dice "HOTP". Se il tuo valutatore si attacca alla lettera
al nome "HOTP", vale la pena fargli notare questa ambiguità ben nota del subject
(è un problema old del subject stesso, non una tua svista).

## Note sui test manuali consigliati

- `./ft_otp -g` (senza argomenti) → deve stampare errore pulito, non crashare.
- `./ft_otp -g key_corta.hex` con meno di 64 caratteri → errore "must be at
  least 64 hexadecimal characters".
- `./ft_otp -g key_non_esadecimale.hex` → errore "contains non-hexadecimal characters".
- `./ft_otp -k ft_otp.key` confrontato con `oathtool --totp $(cat key.hex)` sullo
  stesso periodo di 30s → deve combaciare.
- Corrompere manualmente un byte di `ft_otp.key` → deve fallire con
  "AES-GCM authentication failed: key file tampered or wrong secret", non crashare.