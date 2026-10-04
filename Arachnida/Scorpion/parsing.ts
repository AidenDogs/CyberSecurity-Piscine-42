import type { imgObj } from "./types"
import { checkTag, showUsage } from "./utils"

const args = process.argv.slice(2)

export function parseArgs(): imgObj[]
{
	if (args.length < 1 || args.includes("--help"))
	{
		showUsage()
		process.exit(0)
	}

    const objs: imgObj[] = []
	let imgObj:imgObj = {
		img: "",
		sets: [],
		deletes: []
	}

	for (let i = 0; i < args.length; i++)
	{
		if (args[i] === "--set")
		{
			const value = args[++i]

			if (!value || !value.includes("="))
			{
				console.error("--set field=value type not satisfied in flag")
				process.exit(1)
			}

			const [field, ...valueParts] = value.split("=")
			const newValue = valueParts.join("=")

			checkTag(field)
			if (newValue === "")
			{
				console.error(`--set ${field}= has an empty value (ExifTool would delete it), use --delete ${field}`)
				process.exit(1)				
			}

			imgObj.sets.push({
				field,
				newValue: valueParts.join("=")
			})
		}
		else if (args[i] === "--delete")
		{
			const field = args[++i]

			if (!field)
			{
				console.error("--delete requires a field to delete")
				process.exit(1)
			}

			imgObj.deletes.push(field)
		}
		else if (args[i].startsWith("--"))
		{
			console.error(`unknown option: ${args[i]}`)
			process.exit(1)
		}
		else
		{
			imgObj.img = args[i]
			objs.push(imgObj)
			imgObj = {
				img: "",
				sets: [],
				deletes: []
			}
		}
	}
	if (imgObj.sets.length > 0 || imgObj.deletes.length > 0)
	{
		console.error("options after the last file have no file to apply to")
		process.exit(1)
	}
	return objs
}