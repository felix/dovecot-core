#ifndef __MECH_H
#define __MECH_H

#include "auth-client-interface.h"

struct auth_client_connection;

typedef void mech_callback_t(struct auth_client_request_reply *reply,
			     const void *data,
			     struct auth_client_connection *conn);

struct auth_request {
	int refcount;

	pool_t pool;
	char *user;

	struct auth_client_connection *conn;
	unsigned int id;
	time_t created;

	enum auth_protocol protocol;
	mech_callback_t *callback;

	int (*auth_continue)(struct auth_request *auth_request,
			     struct auth_client_request_continue *request,
			     const unsigned char *data,
			     mech_callback_t *callback);
	void (*auth_free)(struct auth_request *auth_request);
	/* ... mechanism specific data ... */
};

struct mech_module {
	enum auth_mech mech;

	struct auth_request *(*auth_new)(struct auth_client_connection *conn,
					 unsigned int id,
					 mech_callback_t *callback);
};

extern enum auth_mech auth_mechanisms;
extern const char *const *auth_realms;
extern const char *default_realm;
extern const char *anonymous_username;
extern char username_chars[256];
extern int ssl_require_client_cert;

void mech_register_module(struct mech_module *module);
void mech_unregister_module(struct mech_module *module);

void mech_request_new(struct auth_client_connection *conn,
		      struct auth_client_request_new *request,
		      mech_callback_t *callback);
void mech_request_continue(struct auth_client_connection *conn,
			   struct auth_client_request_continue *request,
			   const unsigned char *data,
			   mech_callback_t *callback);
void mech_request_free(struct auth_request *auth_request, unsigned int id);

void mech_init_auth_client_reply(struct auth_client_request_reply *reply);
void *mech_auth_success(struct auth_client_request_reply *reply,
			struct auth_request *auth_request,
			const void *data, size_t data_size);
void mech_auth_finish(struct auth_request *auth_request,
		      const void *data, size_t data_size, int success);

int mech_is_valid_username(const char *username);

void mech_cyrus_sasl_init_lib(void);
struct auth_request *
mech_cyrus_sasl_new(struct auth_client_connection *conn,
		    struct auth_client_request_new *request,
		    mech_callback_t *callback);

void auth_request_ref(struct auth_request *request);
int auth_request_unref(struct auth_request *request);

void mech_init(void);
void mech_deinit(void);

#endif
