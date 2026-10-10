import path from "path"
import { promisify } from "util"
import { fileURLToPath } from "url"
import { execFile } from "child_process"

const execFileAsync = promisify(execFile)

const PROJECT_ROOT = path.resolve(path.dirname(fileURLToPath(import.meta.url)),"../..")
const FT_OTP = path.join(PROJECT_ROOT, "ft_otp")
const ENCRYPTED_KEY = path.join(PROJECT_ROOT, "ft_otp.key")

export async function generateOtp(): Promise<string>
{
	const { stdout } = await execFileAsync(
		FT_OTP,
		["-k", ENCRYPTED_KEY],
		{
			cwd: PROJECT_ROOT,
			env: process.env,
			timeout: 5000,
			encoding: "utf8"
		}
	)

	const otp = stdout.trim()

	if (!/^\d{6}$/.test(otp))
		throw new Error("ft_otp returned an invalid OTP")

	return otp
}