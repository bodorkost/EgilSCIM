#include "simplescim_config_file_to_string.h"

#include <stdlib.h>
#include <unistd.h>

#include "simplescim_error_string.h"
#include "simplescim_config_file.h"

/**
 * Reads the file contents of the configuration file of
 * size 'file_size' from file descriptor 'fd' into a
 * dynamically allocated null-terminated string.
 * Returns a pointer to the resulting string, or NULL if an
 * error occurred, in which case simplescim_error_string is
 * set to an appropriate error message.
 */
char *simplescim_config_file_to_string(
	int fd,
	size_t file_size
)
{
	char *contents;
	ssize_t nread;

	/* Allocate string */
	contents = malloc(file_size + 1);

	if (contents == NULL) {
		simplescim_error_string_set_errno(
			"%s",
			simplescim_config_file_name
		);
		return NULL;
	}

	/* Read file contents */
	nread = read(fd, contents, file_size);

	if (nread == -1) {
		simplescim_error_string_set_errno(
			"%s",
			simplescim_config_file_name
		);
		free(contents);
		return NULL;
	}

	if ((size_t)nread < file_size) {
		simplescim_error_string_set_prefix(
			"%s",
			simplescim_config_file_name
		);
		simplescim_error_string_set_message(
			"file size is %lu B but could only read %ld B",
			file_size,
			nread
		);
		free(contents);
		return NULL;
	}

	contents[file_size] = '\0';

	return contents;
}
