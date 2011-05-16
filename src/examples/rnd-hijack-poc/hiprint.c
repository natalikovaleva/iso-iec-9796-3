#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE * rnd1 = fopen("data.urandom2","r");

	/* Let use 160 bit inversion block */
	unsigned long buffer1[160/8/sizeof(unsigned long)];
	unsigned long buffer2[160/8/sizeof(unsigned long)];
	
	int k = 0;
	
	fread(buffer2, sizeof(buffer2), 1, rnd1);
	
	while (1)
		if(fread(buffer1, sizeof(buffer1), 1, rnd1) != 1)
		{
	    	printf("Conversion done [k == %d]\n", k);
	    	break;
		}
		else
		{
				int i; int ok = 1;
				for (i = 0; i < sizeof(buffer2)/sizeof(unsigned long); i++)
				{
					if (buffer1[i] != ~buffer2[i])
						{
//							printf("break at: %d: %08x != %08x\n", i, buffer1[i], buffer2[i]);
							ok = 0; break;
						}
					
				}
				
				if (ok)
				{
					for (i = 0; i < sizeof(buffer2); i++)
						printf("%02x", ((unsigned char *)buffer2)[i]);
					printf("\n");
					for (i = 0; i < sizeof(buffer2); i++)
						printf("%02x", ((unsigned char *)buffer1)[i]);
					printf("\n");
					for (i = 0; i < sizeof(buffer2); i++)
						printf("%02x", ~((unsigned char *)buffer2)[i]);
					printf("\n");
						
					printf("%d inverted\n", k);
				}
			k ++;
		}
	
	fclose(rnd1);
}