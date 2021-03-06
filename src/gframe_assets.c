/**
 * @src/gframe_assets.c
 * 
 * 'Module' used to buffer images and audio files. Only the file's name, without
 *extensions, should be used. Both (graphics and sounds) are expected to be raw
 *files, generated by the framework, and at an 'assets' folder.
 * When creating the assets, images must be 24 bits bitmap files and sounds must
 *be 44100 Hz, 16 bits wave files. They will be read and unpacked into a raw
 *'.dat' file.
 * The reason to use a 'assets' folder is because it's needed by SDL when
 *compiling for Android. The files must be on that folder, but it should be
 *accessed simply by its name, thus why this 'module' handles all that by itself
 * Notice that if this file is compiled with the GFRAME_DEBUG constant defined,
 *the assets directory will be expected to be on the same level as the game is
 *run from. Otherwise, it's expected to be on the same folder as the binary.
 */
#include <GFraMe/GFraMe.h>
#include <GFraMe/GFraMe_assets.h>
#include <GFraMe/GFraMe_error.h>
#include <GFraMe/GFraMe_log.h>
#include <GFraMe/GFraMe_util.h>
#include <GFraMe/wavtodata/wavtodata.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * SDL already considers assets on a mobile device to be on a 'assets/'
 * folder, so this function removes it (if it was compiled for mobile)
 * @param	*filename	Original filename, that may begin with 'assets/'
 * @retrun	Filename with 'assets/' removed.
 */
GFraMe_ret GFraMe_assets_clean_filename(char *dst, char *src, int *len) {
	GFraMe_ret rv;
	char *tmp = dst;
	
	GFraMe_assertRV(dst && src && len > 0, "Arguments bad!",
		rv = GFraMe_ret_failed, _ret);
	
#if !defined(GFRAME_MOBILE)
  #if !defined(GFRAME_DEBUG)
	tmp = GFraMe_util_strcat(tmp, GFraMe_path, len);
  #endif
	tmp = GFraMe_util_strcat(tmp, "assets/", len);
#endif
	tmp = GFraMe_util_strcat(tmp, src, len);
	
	rv = GFraMe_ret_ok;
_ret:
	return rv;
}

/**
 * Check whether a file exists
 * @param	*filename	File to be checked
 * GFraMe_ret_ok - Exists; GFraMe_ret_file_not_found - Doesn't exists
 */
GFraMe_ret GFraMe_assets_check_file(char *fileName) {
	GFraMe_ret rv = GFraMe_ret_file_not_found;
	SDL_RWops *fp = NULL;
	// Try to open the file
	fp = SDL_RWFromFile(fileName, "rb");
	if (fp) {
		// If it was opened, it exists; so close it!
		SDL_RWclose(fp);
		rv = GFraMe_ret_ok;
	}
	return rv;
}

/**
 * Loads a image into a buffer
 * @param	*filename	Image's filename
 * @param	width	Image's width
 * @param	height	Image's height
 * @param	**buf	Allocated buffer (caller freed!!)
 * @return	GFraMe_ret_ok - Success; Anything else - Failure
 */
GFraMe_ret GFraMe_assets_buffer_image(char *filename, int width, int height,
	char **buf) {
	GFraMe_ret rv = GFraMe_ret_ok;
	int size = 0;
	char *pixels = NULL;
	SDL_RWops *fp = NULL;
	int len, len2;
	char name[GFraMe_max_path_len];
	
	// Get the proper filename
	len = GFraMe_max_path_len;
	rv = GFraMe_assets_clean_filename(name, filename, &len);
	GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to get the file name",
		rv = GFraMe_ret_failed, _ret);
	
	// Check if the .dat file exists and, if not, create it from a .bmp
	len2 = len;
	GFraMe_util_strcat(name + GFraMe_max_path_len - len, ".dat", &len2);
	
	if (GFraMe_assets_check_file(name) != GFraMe_ret_ok) {
		char bmpfile[GFraMe_max_path_len];
		int bmplen, bmplen2;
		
		GFraMe_new_log("Couldn't find %s... creating it...", name);
		
		bmplen = GFraMe_max_path_len;
		rv = GFraMe_assets_clean_filename(bmpfile, filename, &bmplen);
		GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to get the file name",
			rv = GFraMe_ret_failed, _ret);
		
		bmplen2 = bmplen;
		GFraMe_util_strcat(&(bmpfile[GFraMe_max_path_len - bmplen]), ".bmp",
			&bmplen2);
		
		// Create the usable texture
		rv = GFraMe_assets_bmp2dat(bmpfile, 0xff00ff, name);
		GFraMe_assertRet(rv == 0, "Failed to create raw image", _ret);
	}
	
	// Get file lenght in bytes
	size = width*height*4;
	// Alloc return buffer
	pixels = (char*)malloc(size);
	GFraMe_assertRV(pixels, "Couldn't alloc memory",
		rv = GFraMe_ret_memory_error, _ret);
    // Open image file
    fp = SDL_RWFromFile(name, "rb");
	GFraMe_assertRV(fp, "Couldn't find file",
		rv = GFraMe_ret_file_not_found, _ret);
	// Load image
    rv = SDL_RWread(fp, pixels, size, 1);
	GFraMe_assertRV(rv == 1, "Failed to read file",
		rv = GFraMe_ret_read_file_failed, _ret);
	
	*buf = pixels;
	rv = GFraMe_ret_ok;
_ret:
	if (rv != GFraMe_ret_ok && pixels)
		free(pixels);
    if (fp)
		SDL_RWclose(fp);
	return rv;
}

GFraMe_ret GFraMe_assets_buffer_audio(char *filename, char **buf, int *len) {
	GFraMe_ret rv = GFraMe_ret_ok;
	char *samples = NULL;
	SDL_RWops *fp = NULL;
	int flen, flen2, tmp;
	char name[GFraMe_max_path_len];
	
	memset(name, 0x0, GFraMe_max_path_len);
	// Get the proper filename
	flen = GFraMe_max_path_len;
	rv = GFraMe_assets_clean_filename(name, filename, &flen);
	GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to get the file name",
		rv = GFraMe_ret_failed, _ret);
	
	// Check if the .dat file exists and, if not, create it from a .bmp
#ifdef GFRAME_MOBILE
	int fuck_android = GFraMe_util_strlen(name);
	name[fuck_android + 0] = '.';
	name[fuck_android + 1] = 'd';
	name[fuck_android + 2] = 'a';
	name[fuck_android + 3] = 't';
	name[fuck_android + 4] = '\0';
#else
	GFraMe_util_strcat(name + GFraMe_max_path_len - flen, ".dat", &flen2);
#endif
	
	if (GFraMe_assets_check_file(name) != GFraMe_ret_ok) {
		char wavfile[GFraMe_max_path_len];
		int wavlen, wavlen2;
		
		GFraMe_new_log("Couldn't find %s... creating it...", name);
		
		wavlen = GFraMe_max_path_len;
		rv = GFraMe_assets_clean_filename(wavfile, filename, &wavlen);
		GFraMe_assertRV(rv == GFraMe_ret_ok, "Failed to get the file name",
			rv = GFraMe_ret_failed, _ret);
		
		wavlen2 = wavlen;
		GFraMe_util_strcat(&(wavfile[GFraMe_max_path_len - wavlen]), ".wav",
			&wavlen2);
		
		// Create the usable raw audio
		rv = wavtodata(wavfile, name, 1);
		GFraMe_assertRet(rv == 0, "Failed to create raw audio", _ret);
	}
	
	// Load the file into the buffer
	fp = SDL_RWFromFile(name, "r");
	GFraMe_SDLassertRV(fp != NULL, "Failed to open file",
		rv = GFraMe_ret_failed, _ret);
	
	// Get the file's length
	*len = SDL_RWseek(fp, 0, RW_SEEK_END);
	GFraMe_SDLassertRV(*len > 0, "Failed to the file's length", 
		rv = GFraMe_ret_failed, _ret);
	SDL_RWseek(fp, 0, RW_SEEK_SET);
	
	// Then simply read its content into the buffer
	samples = (char*)malloc(*len);
	tmp = SDL_RWread(fp, (void*)samples, 1, *len);
	GFraMe_SDLassertRV(tmp == *len, "Error reading file",
		rv = GFraMe_ret_failed, _ret);
	
	*buf = samples;
	rv = GFraMe_ret_ok;
_ret:
	if (rv != GFraMe_ret_ok && samples)
		free(samples);
    if (fp)
		SDL_RWclose(fp);
	return rv;
}

/**
 * Gets a buffer into a uint (actually a int, but whatever)
 * @param	*buffer	Buffer to be read
 * @param	pos	Position where to start reading on the buffer
 * @return	The calculated uint
 */
static int GFraMe_read_UINT(char *buffer, int pos) {
	// Move the byte on buffer position 'n' to bit starting at '8*n'
	return (0xff&buffer[pos]) +
		   ((buffer[pos+1]<<8)&0xff00) +
		   ((buffer[pos+2]<<16)&0xff0000) +
		   ((buffer[pos+3]<<24)&0xff000000);
}

/**
 * TODO fix this shit!
 * Someday I'll properly comment this, but it reads a 24 bits R8 G8 B8
 * bitmap and convert it into a data file.
 * @param	*inFile	Filename of the input bitmap file
 * @param	keycolor	AARRGGBB color to be considered translucent
 * @param	*outFile	Filename for the generated file
 * @return 0 - Success; Anything else - Failure
 */
int GFraMe_assets_bmp2dat(char *inFile, int keycolor, char *outFile) {
	char buffer[4];
	int i;
	int offset;
	int width;
	int height;
	int bwidth;
	int padding;
	int total;
	int rv = 0;
	char *datab = NULL;
	FILE *in = NULL;
	FILE *out = NULL;
	
#ifdef GFRAME_MOBILE
	GFraMe_assertRV(0, "This shouldn't be run on a mobile dev!", 1, _err);
#endif
	in = fopen(inFile, "rb");
	GFraMe_assertRV(in, "File not found", rv = 3, _err);
	
	fseek(in, 0x0a, SEEK_SET);
	fread(buffer, 4, 1, in);
	offset = GFraMe_read_UINT(buffer, 0);
	// seek and read width from file
	fseek(in, 0x12-0x0a-4, SEEK_CUR);
	fread(buffer, 4, 1, in);
	width = GFraMe_read_UINT(buffer, 0);
	fread(buffer, 4, 1, in);
	height = GFraMe_read_UINT(buffer, 0);
	// get how many bytes are needed and how many are needed to align it
	bwidth = width * 3;//width / 8 + (width%8?1:0);
	padding = bwidth % 4;
	bwidth += padding;
	datab = (char*)malloc(sizeof(char)*width*height*4);
	GFraMe_assertRV(datab, "Failed to alloc memory", rv = 3, _err);
	
	fseek(in, offset, SEEK_SET);
	i = width * (height - 1);
	buffer[3] = 0;
	total = 0;
	while (1) {
		int n = fread(buffer, 3, 1, in);
		if (n == 0)
			break;
		int color = GFraMe_read_UINT(buffer, 0);
		int blue = (color >> 16)&0xff;
		int green = (color >> 8)&0xff;
		int red = color&0xff;
		int pos = i * 4;
		if (i < 0)
			break;
		if (color == keycolor) {
			datab[pos] = 0;
			datab[pos+1] = 0;
			datab[pos+2] = 0;
			datab[pos+3] = 0;
		}
		else {
			//datab[pos] = (char)red & 0xfe;
			datab[pos+2] = (char)red & 0xfe;
			datab[pos+1] = (char)green & 0xfe;
			//datab[pos+2] = (char)blue & 0xfe;
			datab[pos] = (char)blue & 0xfe;
			datab[pos+3] = 0xff & 0xfe;
		}
		i++;
		if (i % width == 0) {
			i -= width * 2;
			fseek(in, padding, SEEK_CUR);
		}
		total += n;
	}
	
	out = fopen(outFile, "wb");
	
	fwrite(datab, sizeof(char)*width*height*4, 1, out);
	
	rv = 0;
_err:
	if (in)
		fclose(in);
	if (out)
		fclose(out);
	if (datab)
		free(datab);
	
	return rv;
}

