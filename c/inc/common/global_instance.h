#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_struct.h"

int get_parse_handler_instance(parse_handler **ph);
int get_token_list_instance(token_list **tl);
int get_syntax_tree_instance(syntax_tree **st);
