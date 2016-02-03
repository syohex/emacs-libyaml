/*
  Copyright (C) 2016 by Syohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <emacs-module.h>
#include <yaml.h>

int plugin_is_GPL_compatible;

static bool
is_null_str(const char *s, ptrdiff_t len)
{
	switch (len) {
	case 1:
		return (s[0] == '\0');
	case 3:
		return (strcmp("nil", s) == 0);
	case 4:
		return (strcmp("null", s) == 0 || strcmp("Null", s) == 0
			|| strcmp("NULL", s) == 0);
	default:
		return false;
	}
}

static bool
is_true_str(const char *s, ptrdiff_t len)
{
	switch (len) {
	case 1:
		return (s[0] == 't' || s[0] == 'T');
	case 2:
		return (strcmp("on", s) == 0 || strcmp("On", s) == 0
			|| strcmp("ON", s) == 0);
	case 3:
		return (strcmp("yes", s) == 0 || strcmp("Yes", s)
			|| strcmp("YES", s) == 0);
	case 4:
		return (strcmp("true", s) == 0 || strcmp("True", s) == 0
			|| strcmp("TRUE", s) == 0);
	default:
		return false;
	}
}

static bool
is_false_str(const char *s, ptrdiff_t len)
{
	switch (len) {
	case 1:
		return (s[0] == 'n' || s[0] == 'N');
	case 2:
		return (strcmp("no", s) == 0 || strcmp("No", s) == 0
			|| strcmp("NO", s) == 0);
	case 3:
		return (strcmp("off", s) == 0 || strcmp("Off", s) == 0
			|| strcmp("OFF", s) == 0);
	case 5:
		return (strcmp("false", s) == 0 || strcmp("False", s) == 0
			|| strcmp("FALSE", s) == 0);
	default:
		return false;
	}
}

static emacs_value
yaml_node_to_elisp(emacs_env *env, yaml_document_t *document, yaml_node_t *node)
{
	if (node == NULL)
		return env->intern(env, "nil");

	switch (node->type) {
	case YAML_SCALAR_NODE: {
		const char *str = (const char*)node->data.scalar.value;
		ptrdiff_t str_len = node->data.scalar.length;

		if (node->data.scalar.style != YAML_PLAIN_SCALAR_STYLE) {
			return env->make_string(env, str, str_len);
		}

		if (str[0] == '~')
			return env->intern(env, "nil");

		if (is_null_str(str, str_len) || is_false_str(str, str_len)) {
			return env->intern(env, "nil");
		} else if (is_true_str(str, str_len)) {
			return env->intern(env, "t");
		}

		char *endptr;
		long long ll = strtoll(str, &endptr, 0);
		if (str != endptr && *endptr == '\0') {
			return env->make_integer(env, (intmax_t)ll);
		}

		double d = strtod(str, &endptr);
		if (str != endptr && *endptr == '\0') {
			return env->make_float(env, d);
		}

		return env->make_string(env, str, str_len);
	}
	case YAML_SEQUENCE_NODE: {
		yaml_node_item_t *beg = node->data.sequence.items.start;
		yaml_node_item_t *end = node->data.sequence.items.top;
		ptrdiff_t len = end - beg;

		if (len == 0)
			return env->intern(env, "nil");

		emacs_value *array = malloc(sizeof(emacs_value) * len);
		if (array == NULL)
			return env->intern(env, "nil");

		size_t i = 0;
		for (yaml_node_item_t *item = beg; item < end; ++item, ++i) {
			yaml_node_t *elm = yaml_document_get_node(document, *item);
			array[i] = yaml_node_to_elisp(env, document, elm);
		}

		emacs_value Fvector = env->intern(env, "vector");
		emacs_value vec = env->funcall(env, Fvector, len, array);
		free(array);
		return vec;

	}
	case YAML_MAPPING_NODE: {
		emacs_value Fmake_hash_table = env->intern(env, "make-hash-table");
		emacs_value make_hash_args[] = {env->intern(env, ":test"),
						env->intern(env, "equal")};
		emacs_value hash = env->funcall(env, Fmake_hash_table, 2, make_hash_args);
		emacs_value Fputhash = env->intern(env, "puthash");

		yaml_node_pair_t *beg = node->data.mapping.pairs.start;
		yaml_node_pair_t *end = node->data.mapping.pairs.top;

		for (yaml_node_pair_t *pair = beg; pair < end; ++pair) {
			yaml_node_t *key = yaml_document_get_node(document, pair->key);
			yaml_node_t *val = yaml_document_get_node(document, pair->value);

			emacs_value key_lisp = yaml_node_to_elisp(env, document, key);
			emacs_value val_lisp = yaml_node_to_elisp(env, document, val);

			emacs_value puthash_args[] = {key_lisp, val_lisp, hash};
			env->funcall(env, Fputhash, 3, puthash_args);
		}

		return hash;
	}
	default:
		return env->intern(env, "nil");
	}
}

static emacs_value
Fyaml_load(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data)
{
	emacs_value code = args[0];
	ptrdiff_t size = 0;
	char *yaml_str = NULL;

	env->copy_string_contents(env, code, NULL, &size);
	yaml_str = malloc(size);
	if (yaml_str == NULL)
		return env->intern (env, "nil");
	env->copy_string_contents(env, code, yaml_str, &size);

	yaml_parser_t parser;
	yaml_parser_initialize(&parser);
	yaml_parser_set_input_string(&parser, (unsigned char*)yaml_str, size-1);

	yaml_document_t document;
	yaml_parser_load(&parser, &document);
	if (parser.error != YAML_NO_ERROR) {
		return env->intern(env, "nil");
	}

	yaml_node_t *root = yaml_document_get_root_node(&document);
	return yaml_node_to_elisp(env, &document, root);
}

static void
bind_function(emacs_env *env, const char *name, emacs_value Sfun)
{
	emacs_value Qfset = env->intern(env, "fset");
	emacs_value Qsym = env->intern(env, name);
	emacs_value args[] = { Qsym, Sfun };

	env->funcall(env, Qfset, 2, args);
}

static void
provide(emacs_env *env, const char *feature)
{
	emacs_value Qfeat = env->intern(env, feature);
	emacs_value Qprovide = env->intern (env, "provide");
	emacs_value args[] = { Qfeat };

	env->funcall(env, Qprovide, 1, args);
}

int
emacs_module_init(struct emacs_runtime *ert)
{
	emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) \
	bind_function(env, lsym, env->make_function(env, amin, amax, csym, doc, data))

	DEFUN("libyaml-load", Fyaml_load, 1, 1, "Load YAML", NULL);

#undef DEFUN

	provide(env, "libyaml-core");
	return 0;
}

/*
  Local Variables:
  c-basic-offset: 8
  indent-tabs-mode: t
  End:
*/
