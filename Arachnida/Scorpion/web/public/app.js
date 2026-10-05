const dropzone = document.getElementById("dropzone")
const fileInput = document.getElementById("file-input")
const selectButton = document.getElementById("select-button")

const fileName = document.getElementById("file-name")

const loading = document.getElementById("loading")
const error = document.getElementById("error")

const result = document.getElementById("result")
const metadataBody = document.getElementById("metadata-body")
const metadataCount = document.getElementById("metadata-count")

const searchInput = document.getElementById("search-input")
const rawOutput = document.getElementById("raw-output")
const copyJsonButton = document.getElementById("copy-json")

const imagePreview = document.getElementById("image-preview")
const previewFilename = document.getElementById("preview-filename")
const previewSize = document.getElementById("preview-size")

let currentExif = {}
let previewUrl = null

selectButton.addEventListener("click", (event) =>
{
	event.stopPropagation()

	fileInput.click()
})

fileInput.addEventListener("change", () =>
{
	const file = fileInput.files?.[0]

	if (file)
		loadFile(file)
})

dropzone.addEventListener("click", () =>
{
	fileInput.click()
})

dropzone.addEventListener("dragover", (event) =>
{
	event.preventDefault()

	dropzone.classList.add("dragging")
})

dropzone.addEventListener("dragleave", () =>
{
	dropzone.classList.remove("dragging")
})

dropzone.addEventListener("drop", (event) =>
{
	event.preventDefault()

	dropzone.classList.remove("dragging")

	const file = event.dataTransfer?.files?.[0]

	if (file)
		loadFile(file)
})

searchInput.addEventListener("input", () =>
{
	renderMetadata()
})

copyJsonButton.addEventListener("click", async (event) =>
{
	event.preventDefault()
	event.stopPropagation()

	try
	{
		await navigator.clipboard.writeText(
			JSON.stringify(currentExif, null, 2)
		)

		const originalText = copyJsonButton.textContent

		copyJsonButton.textContent = "Copiato!"

		setTimeout(() =>
		{
			copyJsonButton.textContent = originalText
		}, 1500)
	}
	catch
	{
		showError(
			"Impossibile copiare il JSON negli appunti."
		)
	}
})

async function loadFile(file)
{
	if (!file.type.startsWith("image/"))
	{
		showError(
			"Il file selezionato non è un'immagine."
		)

		return
	}

	hideError()

	fileName.textContent = file.name
	fileName.classList.remove("hidden")

	result.classList.add("hidden")

	showPreview(file)
	showLoading(true)

	try
	{
		const formData = new FormData()

		formData.append("image", file)

		const response = await fetch(
			"/api/exif",
			{
				method: "POST",
				body: formData
			}
		)

		const data = await response.json()

		if (!response.ok)
		{
			throw new Error(
				data.error ||
				"Errore durante la lettura dei metadata."
			)
		}

		currentExif = data.exif ?? {}

		rawOutput.textContent =
			JSON.stringify(
				currentExif,
				null,
				2
			)

		renderMetadata()

		result.classList.remove("hidden")
	}
	catch (error)
	{
		showError(
			error instanceof Error
				? error.message
				: "Errore sconosciuto."
		)
	}
	finally
	{
		showLoading(false)
	}
}

function showPreview(file)
{
	if (previewUrl)
	{
		URL.revokeObjectURL(previewUrl)
	}

	previewUrl = URL.createObjectURL(file)

	imagePreview.src = previewUrl

	previewFilename.textContent = file.name

	previewSize.textContent =
		formatFileSize(file.size)
}

function renderMetadata()
{
	metadataBody.innerHTML = ""

	const entries = Object.entries(
		currentExif
	)

	const search = searchInput.value
		.trim()
		.toLowerCase()

	const filtered = entries.filter(
		([field, value]) =>
		{
			if (!search)
				return true

			return (
				field
					.toLowerCase()
					.includes(search) ||
				String(value)
					.toLowerCase()
					.includes(search)
			)
		}
	)

	metadataCount.textContent =
		`${entries.length} tag`

	if (filtered.length === 0)
	{
		const row = document.createElement("tr")
		const cell = document.createElement("td")

		cell.colSpan = 2
		cell.className = "empty"
		cell.textContent =
			"Nessun metadata trovato."

		row.appendChild(cell)
		metadataBody.appendChild(row)

		return
	}

	for (const [field, value] of filtered)
	{
		const row = document.createElement("tr")

		const fieldCell =
			document.createElement("td")

		fieldCell.className = "field"

		const code =
			document.createElement("code")

		code.textContent = field

		fieldCell.appendChild(code)

		const valueCell =
			document.createElement("td")

		valueCell.className = "value"

		valueCell.textContent =
			formatValue(value)

		row.appendChild(fieldCell)
		row.appendChild(valueCell)

		metadataBody.appendChild(row)
	}
}

function formatValue(value)
{
	if (value === null)
		return "null"

	if (value === undefined)
		return "undefined"

	if (typeof value === "object")
	{
		return JSON.stringify(
			value,
			null,
			2
		)
	}

	return String(value)
}

function formatFileSize(bytes)
{
	if (bytes < 1024)
		return `${bytes} B`

	if (bytes < 1024 * 1024)
		return `${(bytes / 1024).toFixed(1)} KB`

	return `${(bytes / (1024 * 1024)).toFixed(1)} MB`
}

function showLoading(value)
{
	loading.classList.toggle(
		"hidden",
		!value
	)
}

function showError(message)
{
	error.textContent = message
	error.classList.remove("hidden")
}

function hideError()
{
	error.textContent = ""
	error.classList.add("hidden")
}
