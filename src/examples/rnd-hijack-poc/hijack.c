#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE * rnd1 = fopen("data.urandom","r");
	FILE * rnd2 = fopen("data.urandom2","w");

	/* Let use 160 bit inversion block */
	unsigned long buffer1[160/8/sizeof(unsigned long)];
	unsigned long buffer2[160/8/sizeof(unsigned long)];
	
	int k = 1;
	
	/* Initialize */
	fread(buffer2, sizeof(buffer2), 1, rnd1);
	fwrite(buffer2, sizeof(buffer2), 1, rnd2);

	while (1)
		if(fread(buffer1, sizeof(buffer1), 1, rnd1) != 1)
		{
	    	printf("Conversion done\n");
	    	break;
		}
		else
		{
			/* Invert+duplicate every 1000 value */
			if (0 == k ++ % 1000)
			{
				int i;
				for (i = 0; i < sizeof(buffer2)/sizeof(unsigned long); i++)
				{
					buffer2[i] = ~buffer2[i];
				}
				fwrite(buffer2, sizeof(buffer2), 1, rnd2);
				fread(buffer2,  sizeof(buffer2), 1, rnd1);
				fwrite(buffer2, sizeof(buffer2), 1, rnd2);
			}
			else
				fwrite(buffer1, sizeof(buffer1), 1, rnd2);
		}
	
	fclose(rnd1);
	fclose(rnd2);
}