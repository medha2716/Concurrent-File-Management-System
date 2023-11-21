#ifndef _HISTORY_H
#define _HISTORY_H

// call log_insert(char* log_request) whenever you have to log
// log_request format: [Sender] - <request>

// remove hearders.

#include <unistd.h>   
#include <errno.h>    
#include <stdio.h>    
#include <string.h>  


void intialize_log()
{
    int bufsiz = 256;
    char homedir[4096];
	char path[bufsiz];
	char history[1000][bufsiz];
    if (getcwd(homedir, sizeof(homedir)) != NULL) {
        ;
    } 
    else {
       perror("71: getcwd() error");
    }
    sprintf(path, "%s/history.txt", homedir);
	if (access(path, F_OK) != -1)
	{
		return;
	}
	FILE *fp = fopen(path, "w");
	strcpy(history[0], "%");
	fwrite(history, sizeof(char), sizeof(history), fp);
	fclose(fp);
}
void log_insert(char *log_request, char *port)
{
    int bufsiz = 256;
	intialize_log();
	
    char homedir[4096];
	char path[bufsiz];
	char history[1000][bufsiz];

    if (getcwd(homedir, sizeof(homedir)) != NULL) {
        ;
    } 
	sprintf(path, "%s/history.txt", homedir);
	FILE *fr = fopen(path, "r");
	fread(history, sizeof(char), sizeof(history), fr);
	
	int len = 0;
	while (1)
	{
		if (history[len][0] == '%')
		{
			break;
		}
		++len;
	}
	fclose(fr);
	if (len > 0 && strcmp(history[len - 1], log_request) == 0)
	{
		return;
	}
	if (len == 50)
	{
		for (int i = 0; i < len; ++i)
		{
			strcpy(history[i], history[i + 1]);
		}
		--len;
	}
    strcat(log_request, port);
	strcpy(history[len], log_request);
	strcpy(history[len + 1], "%");
	
	FILE *fp = fopen(path, "w+");
	fwrite(history, sizeof(char), sizeof(history), fp);
	fclose(fp);
}
#endif