import { tmpdir } from "os"
import { basename, join } from "path"
import { exiftool } from "exiftool-vendored"
import { mkdtemp, writeFile, rm } from "fs/promises"

export async function extractExif(image: Buffer, filename: string): Promise<unknown>
{
	const tempDir = await mkdtemp(join(tmpdir(), "scorpion-"))
	const tempFile = join(tempDir, basename(filename) || "image")

	try
	{
		await writeFile(tempFile, image)

		return await exiftool.read(tempFile)
	}
	finally
	{
		await rm(tempDir, { recursive: true, force: true })
	}
}