#include "string.h"
#include "io.h"
#include <ctype.h>

/* $Header $ */

_readln()
{
	char c;
	while( (c=fgetc(_chanrd)) != EOF && c!= '\n')
		;
}

readskip()
{
	char c;
#ifdef DEBUG
	printf("readskip\n");
#endif
	while( (c=fgetc(_chanrd)) != EOF && c!= ',' && c!= '\n')
		;
}
_readint(addr)
int *addr;
{
	int i;
	char	buf[1024];

#ifdef DEBUG
	printf("read int from %d\n",_chann);
#endif
	_asschn();
	if( fscanf(_chanrd,"%d",&i) != 1)
	{
		if( ferror(_chanrd)) error(29);
		if( feof(_chanrd)) error(2);
		if( _chann == -1)
		{
			_asschn();	/* may be closed by now */
			fgets(buf,1024,_chanrd);
			printf("?Redo ");
			_readint(addr);
			return;
		}
		error(40);
	}else  { readskip(); *addr=i;}
}
_readflt(addr)
double *addr;
{
	double f;
	char buf[1024];

#ifdef DEBUG
	printf("read flt from %d\n",_chann);
#endif
	_asschn();
	if( fscanf(_chanrd,"%lf",&f) != 1)
	{
		if( ferror(_chanrd)) error(29);
		if( feof(_chanrd)) error(2);
		if( _chann == -1)
		{
			fgets(buf,1024,_chanrd);
			printf("?Redo ");
			_readflt(addr);
			return;
		}
		error(40);
	}else  { readskip(); *addr=f;}
}
_readstr(s)
String **s;
{
	char buffer[1024];
	char *c;

#ifdef DEBUG
	printf("read str from %d\n",_chann);
#endif
	_asschn();
	c= buffer;
	*c= fgetc(_chanrd); 
	while(isspace(*c) && *c!= EOF) 
		*c= fgetc(_chanrd);
	if( *c== '"')
	{
		/* read quoted string */
#ifdef DEBUG
		printf("qouted string\n");
#endif
		while( (*c= fgetc(_chanrd)) != '"' && *c!= EOF ) c++;
		ungetc(*c,_chanrd);
		*c=0;
	}else
	if( isalpha(*c))
	{
		/* read normal string */
		c++;
#ifdef DEBUG
		printf("non-qouted string\n");
#endif
		while( (*c= fgetc(_chanrd)) != ',' && *c!= EOF &&
		       !isspace(*c) && *c!='\n') 
		       c++;
		ungetc(*c,_chanrd);
		*c=0;
	}else{
		if( ferror(_chanrd)) error(29);
		if( feof(_chanrd)) error(2);
		if( _chann == -1)
		{
			fgets(buffer,1024,_chanrd);
			printf("?Redo ");
			_rdline(s);
			return;
		}
		error(40);
	}
#ifdef DEBUG
	printf("string read: %s\n",buffer);
#endif
	readskip();
	/* save value read */
	_decstr(*s);
	*s= (String *) _newstr(buffer);
}

extern int _seektable[];

_restore(line)
int line;
{
	int nr;
	char buffer[1024];

#ifdef DEBUG
	printf("seek to %d",line);
#endif
	fseek(_chanrd,0l,0);
	if( line)
	{
		/* search number of lines to skip */
		for(nr=0; _seektable[nr] && _seektable[nr]< line; nr+=2) 
#ifdef DEBUG
		printf("test %d %d\n",_seektable[nr], _seektable[nr+1]);
#endif
		;
		nr /= 2;
#ifdef DEBUG
		printf(" %d lines to skip\n",nr);
#endif
		while(nr-- >0 ) fgets(buffer,1024,_chanrd);
	}
}
_rdline(s)
String **s;
{
	char buffer[1024];
	if( fgets(buffer,1024,_chanrd) == 0)
	{
		if( _chann == -1)
		{
			printf("?Redo ");
			_rdline(s);
			return;
		}
		error(40);
	}
	_decstr(*s);
	*s= (String *) _newstr(buffer);
}
