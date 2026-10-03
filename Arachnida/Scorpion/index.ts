import sharp  from "sharp"
import * as path from "path"
import exifReader from 'exif-reader'

const args = process.argv.slice(2)

async function extractMetadata(imageFileName: string)
{
	const filename = path.basename(imageFileName)
	try
	{
		const metaData = await sharp(imageFileName).metadata()
		let exif: ReturnType<typeof exifReader> | undefined

		if (metaData.exif)
		{
			exif = exifReader(metaData.exif)
		}

		const result = {
			...metaData,
			exif
		}
		console.log(`=== ${filename} ===`)
		console.dir(result), {depth: null}

		return result
	}
	catch(error)
	{
		console.error(`Failed to read metadata for ${filename}`)
		return undefined
	}
}


for (let i = 0; i < args.length; i++)
{
	extractMetadata(args[i])
}