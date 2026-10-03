import { mkdir } from "fs/promises"

export function showUsage(): void
{
	const reset = "\x1b[0m"
	const bold = "\x1b[1m"

	const cyan = "\x1b[36m"
	const green = "\x1b[32m"
	const yellow = "\x1b[33m"
	const blue = "\x1b[34m"
	const magenta = "\x1b[35m"
	const red = "\x1b[31m"
	const gray = "\x1b[90m"

console.log(`
${cyan}${bold}╔══════════════════════════════════════════════════╗
║                     🕷 SPIDER                     ║
╚══════════════════════════════════════════════════╝${reset}

${bold}${green}Usage:${reset}
	${yellow}./spider [-rlp] URL${reset}

${bold}${blue}Description:${reset}
	Downloads images from a website.

	${gray}Supported image formats:${reset}
		${magenta}•${reset} .jpg / .jpeg
		${magenta}•${reset} .png
		${magenta}•${reset} .gif
		${magenta}•${reset} .bmp

${bold}${blue}Options:${reset}

	${yellow}-r${reset}
		Recursively download images from the provided URL.

	${yellow}-l [N]${reset}
		Set the maximum recursion depth.
	${gray}Default: 5${reset}

	${yellow}-p [PATH]${reset}
		Set the directory where downloaded images will be saved.
	${gray}Default: ./data/${reset}

${bold}${blue}Examples:${reset}

	${green}./spider https://example.com${reset}
		Download images from the website.

	${green}./spider -r https://example.com${reset}
		Recursively download images.

	${green}./spider -r -l 3 https://example.com${reset}
		Recursively download images up to depth ${yellow}3${reset}.

	${green}./spider -r -p ./images https://example.com${reset}
		Save downloaded images into ${yellow}./images${reset}.

	${green}./spider -r -l 2 -p ./images https://example.com${reset}
		Recursively download images up to depth ${yellow}2${reset}
		into ${yellow}./images${reset}.

${gray}──────────────────────────────────────────────────${reset}
`)
}

export async function createDir(path: string): Promise<void>
{
	try
	{
		await mkdir(path, { recursive: true })
	}
	catch (error)
	{
		console.error("Error creating directory")
	}
}