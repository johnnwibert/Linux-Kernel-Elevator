#include <unistd.h>
 
int main(void)
{
	static const char msg[]= "COP4610 Part 1\n";
 
	write(STDOUT_FILENO, msg, sizeof(msg) - 1);	// 1:write
	getpid();					//2:getpid
	getppid();					//3:geppid
	getuid();					// 4:getuid
	getgid();					//5:getgi
 
	return 0;
}
 
