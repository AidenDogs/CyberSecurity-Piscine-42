import { createServer } from "http"
import { fileURLToPath } from "url"
import { readFile } from "fs/promises"
import { extractExif } from "./exif.js"
import { dirname, extname, join, normalize } from "path"

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

const publicDir = join(__dirname, "..", "public")
const PORT = 3000

const mimeTypes: Record<string, string> = {
	".html": "text/html; charset=utf-8",
	".css": "text/css; charset=utf-8",
	".js": "application/javascript; charset=utf-8",
	".json": "application/json; charset=utf-8",
	".svg": "image/svg+xml",
	".png": "image/png",
	".jpg": "image/jpeg",
	".jpeg": "image/jpeg",
	".gif": "image/gif",
	".webp": "image/webp"
}

function sendJson(response: import("http").ServerResponse, status: number, data: unknown): void
{
	response.writeHead(status, {
		"Content-Type": "application/json; charset=utf-8"
	})

	response.end(JSON.stringify(data))
}

async function readBody(request: import("http").IncomingMessage): Promise<Buffer>
{
	const chunks: Buffer[] = []

	for await (const chunk of request)
	{
		chunks.push(
			Buffer.isBuffer(chunk)
				? chunk
				: Buffer.from(chunk)
		)
	}

	return Buffer.concat(chunks)
}

function parseMultipart(body: Buffer, contentType: string): { filename: string, data: Buffer } | null
{
	const match = contentType.match(/boundary=(?:"([^"]+)"|([^;]+))/)

	if (!match)
		return null

	const boundary = match[1] ?? match[2]

	const raw = body.toString("latin1")
	const parts = raw.split(`--${boundary}`)

	for (const part of parts)
	{
		if (!part.includes('name="image"'))
			continue

		const headerEnd = part.indexOf("\r\n\r\n")

		if (headerEnd === -1)
			continue

		const headers = part.slice(0, headerEnd)
		let content = part.slice(headerEnd + 4)

		if (content.endsWith("\r\n"))
			content = content.slice(0, -2)

		const filenameMatch = headers.match(/filename="([^"]*)"/)

		const filename = filenameMatch?.[1] || "image"

		return {
			filename,
			data: Buffer.from(content, "latin1")
		}
	}

	return null
}

async function handleExif(request: import("http").IncomingMessage, response: import("http").ServerResponse): Promise<void>
{
	const contentType = request.headers["content-type"]

	if (!contentType || !contentType.startsWith("multipart/form-data"))
	{
		sendJson(response, 400, {
			error: "Expected multipart/form-data"
		})

		return
	}

	try
	{
		const body = await readBody(request)
		const file = parseMultipart(body, contentType)

		if (!file)
		{
			sendJson(response, 400, {
				error: "No image was provided"
			})

			return
		}

		const exif = await extractExif(file.data, file.filename)

		sendJson(response, 200, {
			exif
		})
	}
	catch (error)
	{
		console.error(error)

		sendJson(response, 500, {
			error:
				error instanceof Error
					? error.message
					: "Failed to extract metadata"
		})
	}
}

async function serveStatic(request: import("http").IncomingMessage, response: import("http").ServerResponse): Promise<void>
{
	const requestUrl = new URL(request.url ?? "/", "http://localhost")

	let pathname = decodeURIComponent(requestUrl.pathname)

	if (pathname === "/")
		pathname = "/index.html"

	const filePath = normalize(join(publicDir, pathname))

	if (filePath !== publicDir && !filePath.startsWith(`${publicDir}/`))
	{
		response.writeHead(403)
		response.end("Forbidden")
		return
	}

	try
	{
		const data = await readFile(filePath)
		const extension = extname(filePath).toLowerCase()

		response.writeHead(200, {
			"Content-Type":
				mimeTypes[extension] ??
				"application/octet-stream"
		})

		response.end(data)
	}
	catch
	{
		response.writeHead(404, {
			"Content-Type": "text/plain; charset=utf-8"
		})

		response.end("Not found")
	}
}

const server = createServer(async (request, response) =>
{
	const method = request.method ?? "GET"

	const url = new URL(request.url ?? "/", "http://localhost")

	if (method === "POST" && url.pathname === "/api/exif")
	{
		await handleExif(request, response)
		return
	}

	if (method === "GET")
	{
		await serveStatic(request, response)
		return
	}

	response.writeHead(405, {
		Allow: "GET, POST"
	})

	response.end("Method Not Allowed")
})

server.listen(PORT, "127.0.0.1", () =>
{
	console.log(
		`🦂 Scorpion Web → http://localhost:${PORT}`
	)
})

process.on("SIGINT", async () =>
{
	console.log("\nShutting down...")

	server.close()

	process.exit(0)
})
