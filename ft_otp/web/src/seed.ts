import path from "path"
import { promisify } from "util"
import { fileURLToPath } from "url"
import { readFile } from "fs/promises"
import { execFile } from "child_process"

const execFileAsync = promisify(execFile)

const PROJECT_ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)),"../..")
const FT_OTP = path.join(PROJECT_ROOT, "ft_otp")
const ENCRYPTED_KEY = path.join(PROJECT_ROOT, "ft_otp.key")

export async function getDecryptedSeed(): Promise<string>
{
	await execFileAsync(FT_OTP, ["-s"], {
		cwd: PROJECT_ROOT,
		env: process.env,
		timeout: 5000,
		encoding: "utf8"
	})

	const seed = (await readFile(ENCRYPTED_KEY, "utf8")).trim()

	if (!/^(?:[0-9a-fA-F]{2})+$/.test(seed))
		throw new Error("ft_otp.key does not contain a valid hexadecimal seed")

	return seed
}

export async function initializeSeed(): Promise<void>
{
	await getDecryptedSeed()
}