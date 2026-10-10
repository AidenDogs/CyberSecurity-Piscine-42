import path from "path"
import { fileURLToPath } from "url"
import { readFile } from "fs/promises"
import { config as loadEnv } from "dotenv"
import { generateOtp } from "./src/otp.js"
import { initializeSeed } from "./src/seed.js"
import { createQrCode } from "./src/qrCode.js"
import { createServer, type IncomingMessage, type ServerResponse } from "http"

const HERE = path.dirname(fileURLToPath(import.meta.url))
const PUBLIC_DIR = path.resolve(HERE, "public")

loadEnv({
	path: path.resolve(HERE, "../.env")
})
const HOST = process.env.SITE_HOST || "127.0.0.1"
const PORT = Number(process.env.SITE_PORT || 3000)

function json(res: ServerResponse, status: number, body: unknown): void
{
	res.writeHead(status, {
		"Content-Type": "application/json charset=utf-8",
		"Cache-Control": "no-store",
		"X-Content-Type-Options": "nosniff"
	})

	res.end(JSON.stringify(body))
}

async function handleApi(req: IncomingMessage, res: ServerResponse, pathname: string): Promise<void>
{
  try
  {
	if (pathname === "/api/seed" && req.method === "POST")
	{
		await initializeSeed()

		json(res, 200, {success: true, message: "Encrypted seed verified"})
		return
	}

	if (pathname === "/api/qr-code" && req.method === "GET")
	{
		const qrCode = await createQrCode()

		json(res, 200, { qrCode })
		return
	}

	if (pathname === "/api/otp" && req.method === "GET")
	{
		const otp = await generateOtp()

		json(res, 200, { otp })
		return
	}

	json(res, 404, { error: "Endpoint not found" })
  }
  catch (error)
  {
	console.error("API operation failed:", error instanceof Error ? error.message : "Unknown error")
	json(res, 500, { error: "The requested operation failed." })
  }
}

async function serveStatic(res: ServerResponse, pathname: string): Promise<void>
{
	const requestedPath = pathname === "/" ? "index.html" : pathname.slice(1)
	const resolved = path.resolve(PUBLIC_DIR, requestedPath)

	if (resolved !== PUBLIC_DIR && !resolved.startsWith(PUBLIC_DIR + path.sep))
	{
		res.writeHead(403)
		res.end("Forbidden")
		return
	}

	const contentTypes: Record<string, string> = {
		".html": "text/html charset=utf-8",
		".js": "text/javascript charset=utf-8",
		".css": "text/css charset=utf-8",
		".svg": "image/svg+xml"
	}

	try
	{
		const content = await readFile(resolved)
		const extension = path.extname(resolved)

		res.writeHead(200, {
			"Content-Type": contentTypes[extension] || "application/octet-stream",
			"X-Content-Type-Options": "nosniff",
			"Cache-Control": "no-store"
		})

		res.end(content)
	}
	catch
	{
		res.writeHead(404, {"Content-Type": "text/plain charset=utf-8"})
		res.end("Not found")
	}
}

const server = createServer(async (req, res) => {
	const url = new URL(req.url || "/", `http://${req.headers.host || "localhost"}`)

	if (url.pathname.startsWith("/api/"))
	{
		await handleApi(req, res, url.pathname)
		return
	}

	if (req.method !== "GET")
	{
		res.writeHead(405, { Allow: "GET" })
		res.end("Method not allowed")
		return
	}

	await serveStatic(res, url.pathname)
})

server.listen(PORT, HOST, () => {
	console.log(`ft_otp web interface: http://${HOST}:${PORT}`)
})