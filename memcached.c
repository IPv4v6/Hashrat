#include "memcached.h" 
 
STREAM *MCS=NULL; 


STREAM *MemcachedConnect(const char *Server)
{
if (StrLen(Server))
{
if (MCS) STREAMClose(MCS);

MCS=STREAMCreate();
if (! STREAMTCPConnect(MCS, Server, 11211,0,0, 0))
{
	STREAMClose(MCS);
	MCS=NULL;
}
}

return(MCS);
} 


int MemcachedSet(const char *Key, int TTL, const char *Value)
{
char *Tempstr=NULL;
int result=FALSE;

if (STREAMIsConnected(MCS))
{
	Tempstr=FormatStr(Tempstr,"set %s 0 %d %d\r\n%s\r\n",Key,TTL,StrLen(Value),Value);
	STREAMWriteLine(Tempstr,MCS);

	Tempstr=STREAMReadLine(Tempstr,MCS);
	StripTrailingWhitespace(Tempstr);
	if (StrLen(Tempstr) && (strcmp(Tempstr,"STORED")==0)) result=TRUE;
}

Destroy(Tempstr);

return(result);
}



char *MemcachedGet(char *RetStr, const char *Key)
{
char *Tempstr=NULL;

if (STREAMIsConnected(MCS))
{
RetStr=CopyStr(RetStr,"");
Tempstr=FormatStr(Tempstr,"get %s\r\n",Key);
STREAMWriteLine(Tempstr,MCS);

Tempstr=STREAMReadLine(Tempstr,MCS);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
if (strcmp(Tempstr,"END")==0) break;
if (strncmp(Tempstr,"VALUE ",6) !=0) RetStr=CopyStr(RetStr,Tempstr);
Tempstr=STREAMReadLine(Tempstr,MCS);
}
}

Destroy(Tempstr);

return(RetStr);
}

