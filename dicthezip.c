#include <zip.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("%s <zip archive> <dictionary list>\n", argv[0]);
		return -1;
	}
	
	int archive_errno;

	zip_t *zip_archive = zip_open(argv[1],
				ZIP_RDONLY, &archive_errno);

	if(zip_archive == NULL) switch(archive_errno)
	{
		case ZIP_ER_NOENT:
			printf("%s does not exist\n",
				argv[1]);
			return -1;
		case ZIP_ER_NOZIP:
			printf("%s is not a zip archive\n",
				argv[1]);
			return -1;
		case ZIP_ER_OPEN:
			printf("%s could not be opened\n",
				argv[1]);
			return -1;
	}

	FILE *dictionary_fd;

	dictionary_fd = fopen(argv[2], "r");

	char buf,
	password[64];

	uint8_t password_not_correct = 1;

	while(password_not_correct)
	{
		if(fgets(password, sizeof(password), dictionary_fd) == NULL) return -1;

		uint8_t index = -1;
		while(password[++index] != '\n') continue;
		password[index] = '\0';
		
		zip_file_t *file = zip_fopen_index_encrypted(zip_archive,
							(zip_uint64_t)0,
							(zip_flags_t)0,
							password);

		zip_fread(file, &buf, (zip_uint64_t)1);
		archive_errno = zip_error_code_zip(zip_get_error(zip_archive));
		if(archive_errno != ZIP_ER_WRONGPASSWD) password_not_correct = 0;
		zip_error_clear(zip_archive);
	}

	fclose(dictionary_fd);

	puts(password);

	return 0;
}
