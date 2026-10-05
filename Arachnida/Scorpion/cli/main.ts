import { parseArgs } from "./parsing"
import { exiftool } from "exiftool-vendored"
import { extractMetadata, updateMetadata } from "./metadata"

try
{
	const images = parseArgs()

	for (const image of images)
	{
		try
		{
			const tags: Record<string, string | null> = {}

			for (const set of image.sets)
			{
				tags[set.field] = set.newValue
			}

			for (const field of image.deletes)
			{
				tags[field] = null
			}

			if (Object.keys(tags).length > 0)
			{
				await updateMetadata(tags, image.img)
			}	
		}
		catch(error)
		{
			console.error(`[write error] ${image.img}: ${error instanceof Error ? error.message : error}`)
			process.exitCode = 1			
		}

		await extractMetadata(image.img)
	}
}
catch(error)
{
	console.error("Error:", error)
	process.exitCode = 1
}
finally
{
	await exiftool.end()
}


