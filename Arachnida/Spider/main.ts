import { parseArgs } from "./parsing"
import { createDir } from "./utils"
import { recursiveImages } from "./recursion"

const webCrawler = parseArgs()

await createDir(webCrawler.savingPath)
await recursiveImages(webCrawler.url, webCrawler)
