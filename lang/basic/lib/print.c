#include "string.h"
#include "io.h"

/* $Header $ */

/* Here all routine to generate terminal oriented output is located */

_qstmark()
{
	/* prompt for terminal input */
	putchar('?');
}

_nl()
{
	_asschn();
	_outnl();
}
_prinum(i)
int i;
{
	char	buffer[40];

	_asschn();
	if(i>=0) 
		sprintf(buffer," %d ",i);
	else	sprintf(buffer,"-%d ",-i);
	_out(buffer);
}
_str(f,buffer)
double f;
char *buffer;
{
	char *c;
	c= buffer;
	if( f>=0){
		if( f> 1.0e8)
			sprintf(buffer," %e",f);
		else sprintf(buffer," %f",f);
		c++;
	}else {
		if(-f> 1.0e8)
			sprintf(buffer,"-%e",-f);
		else sprintf(buffer,"-%f",-f);
	}
	for( ; *c && *c!= ' ';c++) ;
	c--;
	while( c>buffer && *c== '0')
	{
		*c= 0;c--;
	}
	if( *c=='.') *c=0;
	strcat(buffer," ");
}
_prfnum(f)
double f;
{
	/* BASIC strings trailing zeroes */
	char	buffer[100];
	char	*c;

	_asschn();
	c= buffer;
	_str(f,c);
	_out(buffer);
}
_prstr(str)
String *str;
{
	_asschn();
	if( str==0)	_out("<null>");
	else		_out(str->strval);
}
