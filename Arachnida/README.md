# Arachnida

 A TypeScript implementation of the **42 Arachnida** project, consisting of two programs:

 - **Spider** — recursively crawls websites and downloads images.
- **Scorpion** — reads, displays, searches, and optionally modifies image metadata/EXIF information.

 The project also includes a web interface for exploring image metadata.

 ## Requirements

 - Node.js
- pnpm

 Install dependencies:

```
pnpm install
```

 Check the TypeScript project:

```
pnpm check
```

 ## Project Structure

```
.
├── Scorpion
│   ├── cli
│   │   ├── main.ts
│   │   ├── metadata.ts
│   │   ├── parsing.ts
│   │   ├── types.ts
│   │   └── utils.ts
│   └── web
│       ├── public
│       │   ├── app.js
│       │   ├── index.html
│       │   └── style.css
│       └── src
│           ├── exif.ts
│           └── server.ts
├── Spider
│   ├── main.ts
│   ├── parsing.ts
│   ├── recursion.ts
│   ├── types.ts
│   └── utils.ts
├── package.json
├── pnpm-lock.yaml
├── pnpm-workspace.yaml
└── tsconfig.json
```

 ## Spider

 Spider extracts images from a website. It accepts a URL and can recursively follow links to discover images.

 ### Usage

```
pnpm spider [options] URL
```

 The equivalent command after compilation would be:

```
./spider [-rlp] URL
```

 ### Options

 #### `-r`

 Enable recursive crawling.

```
pnpm spider -r https://example.com
```

 Without `-r`, Spider only processes the provided URL.

 #### `-l N`

 Set the maximum recursion depth.

 The default depth is **5**.

```
pnpm spider -r -l 3 https://example.com
```

 `-l` is intended to be used with recursive mode.

 #### `-p PATH`

 Set the directory where downloaded images are stored.

 The default directory is:

```
./data/
```

 Example:

```
pnpm spider -r -p ./images https://example.com
```

 ### Examples

 Download images from a single page:

```
pnpm spider https://example.com
```

 Recursively crawl a website:

```
pnpm spider -r https://example.com
```

 Recursively crawl up to depth 2:

```
pnpm spider -r -l 2 https://example.com
```

 Recursively crawl and save images to a custom directory:

```
pnpm spider -r -l 3 -p ./downloads https://example.com
```

 ### Supported Image Formats

 Spider downloads the following image extensions:

 - `.jpg`
- `.jpeg`
- `.png`
- `.gif`
- `.bmp`

 Downloaded files are saved in the configured output directory.

---

 # Scorpion

 Scorpion analyzes image files and displays their metadata, including EXIF information.

 It supports the same image formats as Spider:

 - `.jpg`
- `.jpeg`
- `.png`
- `.gif`
- `.bmp`

 ## CLI Usage

```
pnpm scorpion FILE1 [FILE2 ...]
```

 For example:

```
pnpm scorpion image.jpg
```

 Multiple files can be inspected at once:

```
pnpm scorpion image1.jpg image2.png image3.jpeg
```

 The output contains the metadata extracted from each image, including information such as creation dates and EXIF fields when available.

 ## Modifying Metadata

 As a bonus feature, Scorpion supports modifying and deleting metadata directly from the CLI.

 ### Set a field

 Use:

```
--set field=value
```

 Example:

```
pnpm scorpion --set Artist="John Doe" image.jpg
```

 Multiple `--set` options can be provided:

```
pnpm scorpion \
  --set Artist="John Doe" \
  --set Copyright="2026" \
  --set Description="My image" \
  image.jpg
```

 The options placed before an image are applied to that image.

 This also makes it possible to apply different modifications to different files:

```
pnpm scorpion \
  --set Artist="Alice" image1.jpg \
  --set Artist="Bob" image2.jpg
```

 ### Delete a field

 Use:

```
--delete field
```

 Example:

```
pnpm scorpion --delete Artist image.jpg
```

 Multiple fields can be deleted:

```
pnpm scorpion \
  --delete Artist \
  --delete Copyright \
  image.jpg
```

 `--set` and `--delete` can be combined:

```
pnpm scorpion \
  --set Artist="John Doe" \
  --delete Copyright \
  image.jpg
```

 ### Per-file options

 Metadata modification options apply to the image that follows them. This allows different operations to be specified for different images in the same command:

```
pnpm scorpion \
  --set Artist="Alice" \
  image1.jpg \
  --set Artist="Bob" \
  --delete Copyright \
  image2.jpg
```

---

 # Scorpion Web Interface

 Scorpion also includes a web interface for interactively inspecting image metadata.

 Start the web application with:

```
pnpm scorpion-web
```

 The server provides a browser-based interface where an image can be uploaded and its metadata inspected.

 ## Web Features

 The interface provides:

 - Image upload.
- EXIF and other metadata extraction.
- A searchable metadata view.
- A search bar for finding a specific metadata field.
- A JSON representation of the complete metadata.
- A simple graphical interface for browsing metadata.

 After starting the server, open the address displayed by the server in your browser.

 The web application is implemented in:

```
Scorpion/web/
├── public/
│   ├── app.js
│   ├── index.html
│   └── style.css
└── src/
    ├── exif.ts
    └── server.ts
```

---

 # Implementation

 The project is written in **TypeScript** and uses Node.js.

 The two main programs are:

```
Spider/main.ts
Scorpion/cli/main.ts
```

 ## Spider Architecture

 Spider separates its responsibilities into several modules:

 - `main.ts` — CLI entry point and argument handling.
- `parsing.ts` — HTML/link/image parsing.
- `recursion.ts` — recursive website traversal.
- `types.ts` — TypeScript types.
- `utils.ts` — shared utility functions.

 Spider implements the HTTP requests, HTML processing, recursion and file downloading itself rather than relying on command-line scraping tools such as `wget` or `scrapy`.

 ## Scorpion Architecture

 Scorpion is divided into:

 - `main.ts` — CLI entry point.
- `metadata.ts` — metadata operations.
- `parsing.ts` — command-line argument parsing.
- `types.ts` — metadata and CLI types.
- `utils.ts` — utility functions.

 Metadata processing is handled through the `exiftool-vendored` library.

 The web version has its own server and browser-side interface:

```
Scorpion/web/src/server.ts
Scorpion/web/src/exif.ts
Scorpion/web/public/app.js
Scorpion/web/public/index.html
Scorpion/web/public/style.css
```

---

 # Development

 Install dependencies:

```
pnpm install
```

 Run Spider:

```
pnpm spider https://example.com
```

 Run Scorpion:

```
pnpm scorpion image.jpg
```

 Run the web interface:

```
pnpm scorpion-web
```

 Run the TypeScript compiler in type-checking mode:

```
pnpm check
```

 No JavaScript build step is required during development because the programs are executed through `tsx`.

---

 # NPM Scripts

 | Script | Description |
| --- | --- |
| `pnpm spider` | Run Spider |
| `pnpm scorpion` | Run the Scorpion CLI |
| `pnpm scorpion-web` | Start the Scorpion web interface |
| `pnpm check` | Type-check the project |

---

 # Dependencies

 The project uses a small number of dependencies:

 - **TypeScript** — static typing and compilation.
- **tsx** — executes TypeScript directly during development.
- **Node.js type definitions** — TypeScript support for Node.js APIs.
- **exiftool-vendored** — EXIF and image metadata parsing and modification.

 No `wget` or `scrapy` is used for Spider.

---

 # Bonus Features

 This implementation includes both bonus features described in the project subject.

 ### Metadata modification

 Scorpion supports:

```
--set field=value
--delete field
```

 Multiple operations can be specified, including different operations for different image files.

 ### Web interface

 A browser-based interface allows users to:

 1. Upload an image.
2. View its metadata.
3. Search for a specific metadata field.
4. Inspect the complete metadata as JSON.

---

 # Supported Formats

 | Format | Spider | Scorpion |
| --- | --- | --- |
| JPEG / JPG | ✓ | ✓ |
| PNG | ✓ | ✓ |
| GIF | ✓ | ✓ |
| BMP | ✓ | ✓ |

---

 # License

 This project was created as part of the **42 Arachnida** project.