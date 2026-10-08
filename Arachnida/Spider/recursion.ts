import path from "path"
import { validFormats, webCrawler } from "./types"
import { writeFile } from "fs/promises"

const visitedUrls = new Set<string>()

function isValidImage(images: string[]): string[]
{
	const validImages: string[] = []

	for (const image of images)
	{
		const url = new URL(image)
		const extension = url.pathname
			.substring(url.pathname.lastIndexOf("."))
			.toLowerCase()

		if (validFormats.includes(extension) && !validImages.includes(image))
			validImages.push(image)
	}

	return validImages
}

async function getImages(url: URL): Promise<string []>
{
	let response: Response

	try
	{
		response = await fetch(url)
		
		if (!response)
		{
			console.error("No response available " + url)
			return []
		}
	}
	catch(error)
	{
		console.error("Error during fetching")
		return []
	}

	let html: string
	try
	{
		html = await response.text()
	}
	catch(error)
	{
		console.error("Error during html extraction")
		return []
	}

	const regexImg = /<img[^>]+src=["']([^"']+)["']/gi
	const images = []

    let match

	while ((match = regexImg.exec(html)) !== null)
	{
		const imageUrl = new URL(match[1], url).href
		images.push(imageUrl)
	}
	return images
}

async function getLinks(url: URL): Promise<string []>
{
	let response: Response

	try
	{
		response = await fetch(url)
		
		if (!response)
		{
			console.error("No response available " + url)
			return []
		}
	}
	catch(error)
	{
		console.error("Error during fetching")
		return []
	}

	let html: string
	try
	{
		html = await response.text()
	}
	catch(error)
	{
		console.error("Error during html extraction")
		return []
	}

	const regexHref = /<a[^>]+href=["']([^"']+)["']/gi;
	const links = []

    let match

	while ((match = regexHref.exec(html)) !== null)
	{
		const linkUrl = new URL(match[1], url).href
		links.push(linkUrl)
	}
	return links
}


async function downloadImage(imageUrl: string, webCrawler: webCrawler ): Promise<void>
{
	try
	{
		const response = await fetch(imageUrl)

		if (!response.ok)
		{
			console.error(`Error downloading ${imageUrl}: ${response.status}`)
			return
		}

		const buffer = Buffer.from(await response.arrayBuffer())

		const url = new URL(imageUrl)
		const filename = path.basename(url.pathname)

		await writeFile(path.join(webCrawler.savingPath, filename), buffer)

		console.log(`Downloaded: ${filename}`)
	}
	catch (error)
	{
		console.error(`Error downloading ${imageUrl}`)
	}
}

export async function recursiveImages(url: URL, webCrawler: webCrawler, depth: number = webCrawler.depth): Promise<void>
{
	if (depth < 0 || visitedUrls.has(url.href))
		return

	visitedUrls.add(url.href)

	console.log(`Crawling: ${url.href} (depth: ${depth})`)

	const images = isValidImage(await getImages(url))

	for (const image of images)
		await downloadImage(image, webCrawler)

	if (!webCrawler.isRecursive || depth === 0)
		return

	const links = await getLinks(url)

	for (const link of links)
	{
		const nextUrl = new URL(link)
		if (nextUrl.hostname !== webCrawler.url.hostname)
			continue
		await recursiveImages(nextUrl, webCrawler, depth - 1)
	}
}