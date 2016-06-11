/*
 * server.c
 *
 *  Created on: Oct 9, 2012
 *      Author: hmng
 * 
 * yubo@yubo.org
 * 2016-04-14
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "jsonrpc.h"

#define ADDR "127.0.0.1:1234"	// the port users will be connecting to

struct jrpc_server my_server;

static struct json *say_hello(struct jrpc_context * ctx, struct json *params,
			       struct json *id)
{
	return json_create_string("Hello!");
}

#define json_dump(a) do {\
	char *p; \
	p = json_print_unformatted(a); \
	printf(#a " %s\n", p); \
	free(p); \
}while(0)

static struct json *foo(struct jrpc_context * ctx, struct json *params,
			      struct json *id)
{
	struct json *reply, *item, *array;
	int a, b, i;
	char buf[1024];

	json_dump(params);
	json_dump(id);

	item = json_get_object_item(params->child, "A");
	a = item->valueint;

	item = json_get_object_item(params->child, "B");
	b = item->valueint;

	sprintf(buf, "recv a:%d b:%d", a, b);

	array = json_create_array();

	for(i = 0; i < a; i ++){
		item = json_create_object();
		json_add_number_to_object(item, "A", i);
		json_add_number_to_object(item, "B", b++);
		json_add_item_to_array(array, item);
	}




	reply = json_create_object();
	json_add_item_to_object(reply, "Args", array);
	json_add_string_to_object(reply, "Str", buf);
	json_dump(reply);

	return reply;
}

static struct json *exit_server(struct jrpc_context * ctx, struct json *params,
				 struct json *id)
{
	jrpc_server_stop(&my_server);
	return json_create_string("Bye!");
}

int main(void)
{
	jrpc_server_init(&my_server, ADDR);
	jrpc_register_procedure(&my_server, say_hello, "sayHello", NULL);
	jrpc_register_procedure(&my_server, exit_server, "exit", NULL);
	jrpc_register_procedure(&my_server, foo, "foo", NULL);
	jrpc_server_run(&my_server);
	jrpc_server_destroy(&my_server);
	return 0;
}
