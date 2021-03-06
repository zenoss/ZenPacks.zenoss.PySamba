/* 
   Unix SMB/CIFS implementation.

   CLDAP server - rootdse handling

   Copyright (C) Stefan Metzmacher	2006
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "includes.h"
#include "libcli/ldap/ldap.h"
#include "lib/ldb/include/ldb.h"
#include "lib/ldb/include/ldb_errors.h"
#include "lib/events/events.h"
#include "lib/socket/socket.h"
#include "smbd/service_task.h"
#include "cldap_server/cldap_server.h"
#include "librpc/gen_ndr/ndr_misc.h"
#include "dsdb/samdb/samdb.h"
#include "auth/auth.h"
#include "db_wrap.h"
#include "system/network.h"
#include "lib/socket/netif.h"

static void cldapd_rootdse_fill(struct cldapd_server *cldapd,
				TALLOC_CTX *mem_ctx,
				struct ldap_SearchRequest *search,
				struct ldap_SearchResEntry **response,
				struct ldap_Result *result)
{
	struct ldap_SearchResEntry *ent = NULL;
	struct ldb_dn *basedn;
	struct ldb_result *res = NULL;
	struct ldb_request *lreq;
	enum ldb_scope scope = LDB_SCOPE_DEFAULT;
	const char **attrs = NULL;
	const char *errstr = NULL;
	int ret = 0;
	int ldb_ret = -1;

	basedn = ldb_dn_new(mem_ctx, cldapd->samctx, NULL);
	if (basedn == NULL) goto nomem;
	scope = LDB_SCOPE_BASE;

	if (search->num_attributes >= 1) {
		int i;

		attrs = talloc_array(mem_ctx, const char *, search->num_attributes+1);
		if (attrs == NULL) goto nomem;

		for (i=0; i < search->num_attributes; i++) {
			attrs[i] = search->attributes[i];
		}
		attrs[i] = NULL;
	}

	lreq = talloc(mem_ctx, struct ldb_request);
	if (lreq == NULL) goto nomem;

	res = talloc_zero(mem_ctx, struct ldb_result);
	if (res == NULL) goto nomem;

	lreq->operation = LDB_SEARCH;
	lreq->op.search.base = basedn;
	lreq->op.search.scope = scope;
	lreq->op.search.tree = search->tree;
	lreq->op.search.attrs = attrs;

	lreq->controls = NULL;

	lreq->context = res;
	lreq->callback = ldb_search_default_callback;

	/* Copy the timeout from the incoming call */
	ldb_set_timeout(cldapd->samctx, lreq, search->timelimit);

	ldb_ret = ldb_request(cldapd->samctx, lreq);
	if (ldb_ret != LDB_SUCCESS) {
		goto reply;
	}

	ldb_ret = ldb_wait(lreq->handle, LDB_WAIT_ALL);
	if (ldb_ret != LDB_SUCCESS) {
		goto reply;
	}

	if (res->count > 1) {
		errstr = "Internal error: to much replies";
		ldb_ret = LDB_ERR_OTHER;
		goto reply;
	} else if (res->count == 1) {
		int j;

		ent = talloc(mem_ctx, struct ldap_SearchResEntry);
		if (ent == NULL) goto nomem;

		ent->dn = ldb_dn_alloc_linearized(ent, res->msgs[0]->dn);
		if (ent->dn == NULL) goto nomem;
		ent->num_attributes = 0;
		ent->attributes = NULL;
		if (res->msgs[0]->num_elements == 0) {
			goto reply;
		}
		ent->num_attributes = res->msgs[0]->num_elements;
		ent->attributes = talloc_array(ent, struct ldb_message_element, ent->num_attributes);
		if (ent->attributes == NULL) goto nomem;
		for (j=0; j < ent->num_attributes; j++) {
			ent->attributes[j].name = talloc_steal(ent->attributes, res->msgs[0]->elements[j].name);
			ent->attributes[j].num_values = 0;
			ent->attributes[j].values = NULL;
			if (search->attributesonly && (res->msgs[0]->elements[j].num_values == 0)) {
				continue;
			}
			ent->attributes[j].num_values = res->msgs[0]->elements[j].num_values;
			ent->attributes[j].values = res->msgs[0]->elements[j].values;
			talloc_steal(ent->attributes, res->msgs[0]->elements[j].values);
		}
	}

reply:
	if (ret) {
		/* nothing ... */
	} else if (ldb_ret == LDB_SUCCESS) {
		ret = LDAP_SUCCESS;
		errstr = NULL;
	} else {
		ret = ldb_ret;
		errstr = ldb_errstring(cldapd->samctx);
	}
	goto done;
nomem:
	talloc_free(ent);
	ret = LDAP_OPERATIONS_ERROR;
	errstr = "No memory";
done:
	*response = ent;
	result->resultcode = ret;
	result->errormessage = (errstr?talloc_strdup(mem_ctx, errstr):NULL);
}

/*
  handle incoming cldap requests
*/
void cldapd_rootdse_request(struct cldap_socket *cldap, 
			    uint32_t message_id,
			    struct ldap_SearchRequest *search,
			    struct socket_address *src)
{
	struct cldapd_server *cldapd = talloc_get_type(cldap->incoming.private, struct cldapd_server);
	NTSTATUS status;
	struct cldap_reply reply;
	struct ldap_Result result;
	TALLOC_CTX *tmp_ctx = talloc_new(cldap);

	ZERO_STRUCT(result);

	reply.messageid		= message_id;
	reply.dest		= src;
	reply.response		= NULL;
	reply.result		= &result;

	cldapd_rootdse_fill(cldapd, tmp_ctx, search, &reply.response, reply.result);

	status = cldap_reply_send(cldap, &reply);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(2,("cldap rootdse query failed '%s' - %s\n",
			 ldb_filter_from_tree(tmp_ctx, search->tree), nt_errstr(status)));
	}

	talloc_free(tmp_ctx);
	return;
}
