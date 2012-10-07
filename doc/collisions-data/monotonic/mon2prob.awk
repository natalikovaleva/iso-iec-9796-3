#!/usr/bin/awk -f 
{
	if (NR == 0)
	{
		PREV=$0;
		print $0;
	}
	else
	{
		print $0 - PREV;
		PREV = $0;
	}
}
