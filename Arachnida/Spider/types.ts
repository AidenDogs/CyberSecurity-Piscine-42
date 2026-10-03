export type webCrawler = {
	url: URL
	isRecursive: boolean
	depth: number
	savingPath: string
}

export const validFormats = [
	".jpg",
	".jpeg",
	".png",
	".gif",
	".bmp"
]