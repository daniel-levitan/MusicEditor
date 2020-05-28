#include <string.h>
#include "lame.h"
#include "decoder.h"
#include "get_audio.h"

int main()
{
	lame_global_flags *gf;
	char    inPath[PATH_MAX + 1];

	/* initialize libmp3lame */
	input_format = sf_mp3;
	if (NULL == (gf = lame_init())) {
		fprintf(stderr, "fatal error during initialization\n");
		return(1);
	}

	/* initialize input file.  This also sets samplerate and as much
	 *        other data on the input file as available in the headers */
	/* Now that all the options are set, lame needs to analyze them and
	 * set some more internal options and check for problems
	 */
 	strcpy(inPath, "test.mp3");
	init_infile(gf, inPath);
	lame_init_params(gf);
	lame_decoder(gf, NULL, 0, inPath, NULL);
	return (0);
}


