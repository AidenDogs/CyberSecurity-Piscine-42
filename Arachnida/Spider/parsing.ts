import { showUsage } from "./utils"
import type { webCrawler } from "./types"

const args = process.argv.slice(2)

export function parseArgs(): webCrawler
{
	if (args.length < 1 || args.includes("--help"))
	{
		showUsage()
		process.exit(0)
	}

	let newWebCrawler: webCrawler = {
		url: new URL(args[args.length - 1]),
		isRecursive: false,
		depth: 5,
		savingPath: "./data/",
	}

	for (let i = 0; i < args.length; i++)
	{
		if (args[i] === "-r")
			newWebCrawler.isRecursive = true
		else if (args[i] === "-l" && args[i + 1])
		{
			const depth = Number(args[++i])

			if (Number.isInteger(depth) && depth >= 0)
				newWebCrawler.depth = depth
			else
			{
				console.error("Invalid recursion depth")
				process.exit(1)
			}
		}
		else if (args[i] === "-p" && args[i + 1])
			newWebCrawler.savingPath = args[++i]
	}

	return newWebCrawler
}

