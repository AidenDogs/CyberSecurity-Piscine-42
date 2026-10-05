export const TAG_REGEX = /^[A-Za-z][A-Za-z0-9:_-]*$/

export type imgObj = {
	img: string
	sets: { field: string, newValue: string }[]
	deletes: string[]
}