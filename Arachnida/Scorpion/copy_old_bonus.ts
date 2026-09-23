
import exifReader from 'exif-reader';
import path from 'path';
import sharp from 'sharp'

const TAG_TO_IFD = {
	Copyright: 'IFD0',
	Make: 'IFD0',
	Model: 'IFD0',
	Orientation: 'IFD0',   // esiste anche come opzione dedicata: withMetadata({ orientation })
	Software: 'IFD0',
	Artist: 'IFD0',
	GPSLatitudeRef: 'IFD3',
	GPSLatitude: 'IFD3',
	GPSLongitudeRef: 'IFD3',
	GPSLongitude: 'IFD3',
};

/** @type {Array<String>} */
const args = process.argv.slice(2);

async function extractMetadatas(image) {
	const filename = path.basename(image);
	console.log(`=== ${filename} ===\n`);
	try {
		const metas = await sharp(image).metadata();
		if (metas.exif) {
			const exif = exifReader(metas.exif);
			metas.exif = exif;
		}
		return metas;
	} catch (error) {
		console.error(error.message);
	}
}

/** @param {String} paramToAdd*/
function addParam(paramToAdd, image, type)
{
	if (!paramToAdd || !image || !type)
		throw new Error('Missing required fields');

	if (paramToAdd.indexOf('=') == -1)
		throw new Error(`Wrong format of param ${paramToAdd}`);
	const tag = paramToAdd.split('=')[0];
	const val = paramToAdd.split('=')[1];
	let meta;
	if (type === 'metadata') {
		if (tag === 'orientation')
			meta = sharp(image).withMetadata({ [tag]: Number(val) });
		else
			meta = sharp(image).withMetadata({ [tag]: val });
	}
	else if (type === 'exif') {
		const ifd = TAG_TO_IFD[tag];
		if (!ifd)
			throw new Error(`Exif tag ${tag} not recognised`);
		meta = sharp(image).withExif({
			[ifd]: { [tag]: val }
		});
	}
	return (meta);
}

try {
	for (let i = 0; i < args.length; i++) {
	
		try {
			if (args[i] === '-am')
				addParam(args[i + 1], args[i + 2], 'metadata');
			else if (args[i] === '-ae')
				addParam(args[i + 1], args[i + 2], 'exif');
			// if (args[i] === '-d') {
				
			// }
		} catch (error) {
			console.error(error.message);
		}
		console.log(await extractMetadatas(args[i]));
	}	
} catch (error) {
	console.error(error.message);
}

