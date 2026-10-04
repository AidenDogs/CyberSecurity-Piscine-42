import * as path from "path"
import { exiftool, type WriteTags } from "exiftool-vendored"

export async function updateMetadata(tags: Record<string, string | null>, imageFileName: string)
{
	const result = await exiftool.write(imageFileName, tags as WriteTags, {
		writeArgs: ["-overwrite_original"]
	})

	if (result.updated === 0)
	{
		console.warn(`[write] ${path.basename(imageFileName)}: nothing changed (unknown tag, same value or not writable)`)
	}
}

export async function extractMetadata(imageFileName: string)
{
	const filename = path.basename(imageFileName)
	try
	{
		const metaData = await exiftool.read(imageFileName)

		console.log(`=== ${filename} ===`)
		console.dir(metaData, { depth: null })

		return metaData
	}
	catch(error)
	{
		console.error(`Failed to read metadata for ${filename}: ${error instanceof Error ? error.message : error}`)
		process.exitCode = 1
		return undefined
	}
}