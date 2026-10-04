import { TAG_REGEX } from "./types"

export function checkTag(field: string)
{
	if (!TAG_REGEX.test(field))
	{
		console.error(`invalid tag name: "${field}"`)
		process.exit(2)
	}
}

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
║                   🦂 SCORPION                    ║
╚══════════════════════════════════════════════════╝${reset}

${bold}${green}Usage:${reset}
${yellow}./scorpion [OPTIONS] FILE1 [[OPTIONS] FILE2 ...]${reset}

${bold}${blue}Description:${reset}
    Displays EXIF and other metadata of image files.
    Options apply to the file that ${bold}follows${reset} them.

${gray}Supported image formats:${reset}
${magenta}•${reset} .jpg / .jpeg
${magenta}•${reset} .png
${magenta}•${reset} .gif
${magenta}•${reset} .bmp

${bold}${blue}Options:${reset}

${yellow}--set FIELD=VALUE${reset}
        Write a tag on the next file. Can be repeated.
${gray}Use an explicit group when needed: XMP:Title, EXIF:Artist${reset}

${yellow}--delete FIELD${reset}
        Remove a tag from the next file. Can be repeated.
${gray}A tag can exist in several groups: delete each one (EXIF:, XMP:)${reset}

${yellow}--help${reset}
        Show this message.

${bold}${blue}Examples:${reset}

${green}./scorpion a.jpg b.png${reset}
        Display the metadata of both files.

${green}./scorpion --set Artist=Mario a.jpg${reset}
        Set ${yellow}Artist${reset} on a.jpg, then display its metadata.

${green}./scorpion --delete Copyright a.jpg${reset}
        Remove ${yellow}Copyright${reset} from a.jpg.

${green}./scorpion --set Artist=Mario --delete Copyright a.jpg b.png${reset}
        Edit a.jpg only. b.png is just displayed.

${gray}Warning: edits overwrite the original file (no backup).${reset}
${gray}BMP files can be read but not written.${reset}

${gray}──────────────────────────────────────────────────${reset}
`)
}
