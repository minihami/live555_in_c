#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uv.h"

int main_init(void)
{
	/* TODO */

	return 0;
}

int main_deinit(void)
{
	/* TODO */

	uv_loop_close(uv_default_loop());

	return 0;
}

int main(int argc, char *argv[])
{
	main_init();

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	main_deinit();

	return 0;
}
