#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <curl/curl.h>
#include <rpc/rpc.h>
#include "proxy.h"
#include "hashfunction.h"

struct wd_in
{
size_t size;
size_t len;
char *data;
};

struct hashtab
{
char *value;
int count;
int frequency;
};

static int recently_count=0;
int hashsize = 2<<6;
static struct hashtab table[64];
int size_hash=0;

static size_t write_data(void *buffer, size_t size,
                         size_t nmemb, void *userp) {
  struct wd_in *wdi = userp;
  while(wdi->len + (size * nmemb) >= wdi->size) {
    /* check for realloc failing in real code. */
    wdi->data = realloc(wdi->data, wdi->size*2);
    wdi->size*=2;
  }

  memcpy(wdi->data + wdi->len, buffer, size * nmemb);
  wdi->len+=size*nmemb;

  return size * nmemb;
}


char **
proxyserver_1_svc(urlc,client)
char **urlc;struct svc_req *client;
{
//int hashsize = 2<<6;
//static struct hashtab table[64];
unsigned int value;
 CURL *curl;
  CURLcode res;
  struct wd_in wdi;
  char *url = *urlc;
  static char *result;
  char choice[10] = "LRU";  
value = (DEKHash(url,strlen(url))&hashsize)-1;
if(table[value].value == NULL)
{


  if((size_hash/hashsize)*100 > 80){

	int i=0,key=0;

if(!strcmp(choice,"LRU")){
	 int h = table[0].count;
   for(i=0;i<hashsize;i++){
	if(table[i].value != NULL){
	if(table[i].count > h){
		h = table[i].count;
		key = i;}}
}
}

if(!strcmp(choice,"LFU"))
{
  int h = table[0].frequency;
  for(i=0;i<hashsize;i++)
	{
	  if(table[i].value != NULL){
	  if(table[i].frequency < h)
		{
		h = table[i].frequency;
		key = i;
}}
}
}
	table[key].frequency = 0;
	free(table[key].value);
	size_hash--;
}
  memset(&wdi, 0, sizeof(wdi));
  recently_count++;
  curl = curl_easy_init();

  if(NULL != curl) {
    wdi.size = 1024;
    wdi.data = malloc(wdi.size);

    curl_easy_setopt(curl, CURLOPT_URL, url);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wdi);

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
  }
  else {
    fprintf(stderr, "Error: could not get CURL handle.\n");
    exit(EXIT_FAILURE);
  } 
   size_hash += 1; 
   table[value].frequency += 1;
   table[value].value = (char *) malloc(strlen(wdi.data) + 3);
   table[value].value = wdi.data;
   table[value].count = recently_count;
	//strcat(table[value].value,"FU");
  result = wdi.data;
  free(wdi.data);
 } 

else{
recently_count++;
//printf("value is inserted");
//table[value].value = realloc(table[value].value,((strlen(table[value].value))+3));
//strcat(table[value].value,"C");
table[value].count = recently_count;
table[value].frequency += 1;
result = table[value].value;
}
  return (&result);
}

