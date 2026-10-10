import QRCode from "qrcode"
import { getDecryptedSeed } from "./seed"

function hexToBase32(hex: string): string
{
	const alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
	const bytes = Buffer.from(hex, "hex")

	let bits = 0
	let value = 0
	let output = ""

	for (const byte of bytes) {
		value = (value << 8) | byte
		bits += 8

		while (bits >= 5) {
		output += alphabet[(value >>> (bits - 5)) & 31]
		bits -= 5
		}
	}

	if (bits > 0) {
		output += alphabet[(value << (5 - bits)) & 31]
	}

	return output
}

export async function createQrCode(): Promise<string>
{
	const hexSeed = await getDecryptedSeed()
	const base32Seed = hexToBase32(hexSeed)
	const issuer = process.env.TOTP_ISSUER || "ft_otp"
	const account = process.env.TOTP_ACCOUNT || "placeholding_account"
	const label = `${issuer}:${account}`
	const uri = new URL(`otpauth://totp/${encodeURIComponent(label)}`)

	uri.searchParams.set("secret", base32Seed)
	uri.searchParams.set("issuer", issuer)
	uri.searchParams.set("algorithm", "SHA1")
	uri.searchParams.set("digits", "6")
	uri.searchParams.set("period", "30")

	return QRCode.toDataURL(uri.toString(), {
		errorCorrectionLevel: "M",
		margin: 2,
		width: 280
	})
}