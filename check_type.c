/*
 * sparse/check_type.c
 *
 * Copyright (C) 2009 Dan Carpenter.
 *
 * Licensed under the Open Software License version 1.1
 *
 */

#include "smatch.h"

static int my_id;

static void match_free(const char *fn, struct expression *expr, void *data)
{
	struct expression *arg_expr;
	char *name;
	struct symbol *type;

	arg_expr = get_argument_from_call_expr(expr->args, 0);
	type = get_type(arg_expr);

	if (!type || (type->type != SYM_PTR && type->type != SYM_ARRAY))
		return;
	type = get_base_type(type);

	if (!type || !type->ident)
		return;

	name = get_variable_from_expr_complex(arg_expr, NULL);

	if (!strcmp("sk_buff", type->ident->name))
		sm_msg("error: use kfree_skb() here instead of kfree(%s)", name);
	else if (!strcmp("net_device", type->ident->name))
		sm_msg("error: use free_netdev() here instead of kfree(%s)", name);

	free_string(name);
}

void check_type(int id)
{
	my_id = id;
	if (option_project == PROJ_KERNEL)
		add_function_hook("kfree", &match_free, NULL);
}