#include "common/funcs.h"

void invalid_call(const char *state)
{
    assert(state != NULL);
    printf("%s failed\n", state);
    exit(0);
}

void invalid_token(const token *t)
{
    printf("invalid token: %s in line %d\n", t->token_str, t->line_no);
    // exit(0);
}

void invalid_instuction(const int line)
{
    printf("invalid instuction in line %d\n", line);
    exit(0);
}

static void _token_print(token *t)
{
    assert(t != NULL);

    printf("\t\t\tline_no: %d,\n\t\t\ttoken_str: \"%s\",\n\t\t\ttoken_len: %d,\n\t\t\ttoken_type: %d\n",
    t->line_no, t->token_str, t->token_len, t->token_type);
}

void token_print(const token_list *tokens, int token_idx)
{
    assert(tokens != NULL);

    token *t = tokens->data[token_idx];
    printf("%d: { line_no: %d, token_str: \"%s\", token_len: %d, token_type: %d }\n", 
        token_idx, t->line_no, t->token_str, 
        t->token_len, t->token_type);
}

token *create_token(const char *str, int type)
{
    token *t = (token *)malloc(sizeof(token));
    if (t == NULL) {
        return NULL;
    }

    memset(t, 0, sizeof(token));
    t->token_type = type;
    t->token_len = 0;
    t->line_no = 0;

    if (str != NULL) {
        t->token_len = MIN(TOKEN_STR_MAX, strlen(str));
        strncpy(t->token_str, str, t->token_len);
    }
    return t;
}

int token_list_init(token_list *tokens)
{
    assert(tokens != NULL);

    int idx;
    tokens->data = (token **)malloc(TOKEN_LIST_MAX * sizeof(token *));
    if (tokens->data == NULL) {
        printf("tokens->data is NULL.\n");
        return CMM_FAILED;
    }

    for (idx = 0; idx < tokens->capacity; ++idx) {
        tokens->data[idx] = (token *)malloc(sizeof(token));
        if (tokens->data[idx] == NULL) {
            printf("tokens->data[idx] is NULL.\n");
            return CMM_FAILED;
        }
    }

    tokens->capacity = TOKEN_LIST_MAX;
    tokens->size = 0;

    return CMM_SUCCESS;
}

int token_list_push(token_list *tl, token *t)
{
    assert(tl != NULL && t != NULL);

    tl->data[tl->size++] = t;
    if (tl->size >= tl->capacity) {
        printf("insufficient free space, please expand.\n");
        // todo
        return CMM_FAILED;
    }

    return CMM_SUCCESS;
}

void token_list_print(const token_list *tl)
{
    assert(tl != NULL);

    int idx;
    printf("token_list: [\n");
    for (idx = 0; idx < tl->size; ++idx) {
        printf("\t{\n\t\tidx: %d,\n\t\ttoken: {\n", idx);
        _token_print(tl->data[idx]);
        printf("\t\t}\n\t}");
        if (idx != tl->size - 1) {
            printf(",");
        }
        printf("\n");
    }

    printf("]\n");
}

void token_push_char(token *t, char ch)
{
    assert(t != NULL);

    if (t->token_len >= TOKEN_STR_MAX) {
        printf("token length is too long for token_str array, max length: %d.\n", TOKEN_STR_MAX);
        exit(0);
    }

    t->token_str[t->token_len++] = ch;
}

syntax_tree *create_ast_node(token *t)
{
    syntax_tree *node = (syntax_tree *)malloc(sizeof(syntax_tree));
    memset(node, 0, sizeof(syntax_tree));
    node->data = t;
    node->sub_list = (syntax_tree **)malloc(AST_LIST_MAX * sizeof(syntax_tree *));
    node->sub_idx = 0;

    return node;
}

static void _ast_print_node(const syntax_tree *node, int deep)
{
    assert(node != NULL);
    int tab_size = deep;
    while (tab_size-- > 0) {
        printf("   ");
    }
    printf("D%d[%d]: ", deep, node->sub_idx);
    printf("{ line_no: %d, token_str: \"%s\", token_len: %d, token_type: %d }\n",
        node->data->line_no, node->data->token_str,
        node->data->token_len, node->data->token_type);
}

static void _syntax_tree_print(const syntax_tree *node, int deep)
{
    int si;
    if (node != NULL) {
        _ast_print_node(node, deep);
        for (si = 0; si < node->sub_idx; ++si) {
            if (node->sub_list[si] != NULL) {
                _syntax_tree_print(node->sub_list[si], deep + 1);
            }
        }
    }
}

void syntax_tree_print(const syntax_tree *ast)
{
    int deep = 0;
    _syntax_tree_print(ast, deep);
}

symbol *create_symbol(const char *var_name, int var_idx, int var_size)
{
    assert(var_name != NULL);

    symbol *s = (symbol *)malloc(sizeof(symbol));
    memset(s, 0, sizeof(symbol));
    strncpy(s->var_name, var_name, MIN(VAR_NAME_MAX, strlen(var_name)));
    s->var_idx = var_idx;
    s->var_size = var_size;

    return s;
}

symbol *find_symbol(const symbol_table *table, const char *space_name, const char *var_name)
{
    assert(table != NULL && space_name != NULL && var_name != NULL);

    int idx;
    symbol_space *space = find_symbol_space(table, space_name);
    if (space == NULL) {
        return NULL;
    }

    for (idx = 0; idx < space->s_idx; ++idx) {
        if (strcmp(var_name, space->s[idx]->var_name) == 0) {
            return space->s[idx];
        }
    }

    return NULL;
}

symbol_space *create_symbol_space(const char* space_name)
{
    assert(space_name != NULL);

    symbol_space *space = (symbol_space *)malloc(sizeof(symbol_space));
    memset(space, 0, sizeof(symbol_space));
    strncpy(space->space_name, space_name, MIN(SPACE_NAME_MAX, strlen(space_name)));
    space->s_idx = 0;
    space->s = (symbol **)malloc(TABLE_SYMBOL_MAX * sizeof(symbol *));

    return space;
}

void init_space_symbol(symbol_space *space, const int size)
{
    assert(space != NULL);

    for (space->s_idx = 0; space->s_idx < size; ++space->s_idx) {
        space->s[space->s_idx] = create_symbol("", 0, 0);
    }
}

symbol_space *get_global_space(const symbol_table *table)
{
    assert(table != NULL && (table->ss_idx > 0));
    return table->ss[0];
}

symbol_space *find_symbol_space(const symbol_table *table, const char *space_name)
{
    int ss_idx;

    for (ss_idx = 0; ss_idx < table->ss_idx; ++ss_idx) {
        if (strcmp(space_name, table->ss[ss_idx]->space_name) == 0) {
            return table->ss[ss_idx];
        }
    }

    return NULL;
}

symbol_space *get_symbol_space(symbol_table **table, const char *space_name)
{
    assert(table != NULL && (*table) != NULL && space_name != NULL);
    
    symbol_space *space = find_symbol_space(*table, space_name);
    if (NULL != space) {
        return space;
    }

    (*table)->ss[(*table)->ss_idx] = create_symbol_space(space_name);

    return (*table)->ss[(*table)->ss_idx++];
}

symbol_table *create_symbol_table()
{
    symbol_table *table = (symbol_table *)malloc(sizeof(symbol_table));
    memset(table, 0, sizeof(symbol_table));
    table->ss_idx = 0;
    table->ss = (symbol_space **)malloc(TABLE_SPACE_MAX * sizeof(symbol_space *));
    table->ss[table->ss_idx++] = create_symbol_space(NAMESPACE_GLOBAL);
    return table;
}

void symbol_table_print(const symbol_table *table)
{
    assert(table != NULL);
    
    int table_idx, space_idx;
    for (table_idx = 0; table_idx < table->ss_idx; ++table_idx) {
        printf("space_name: %s, size: %d\n", table->ss[table_idx]->space_name, table->ss[table_idx]->s_idx);
        
        for (space_idx = 0; space_idx < table->ss[table_idx]->s_idx; ++space_idx) {
            printf("\tvar_name: %s, var_idx: %d, var_size: %d\n",
                table->ss[table_idx]->s[space_idx]->var_name,
                table->ss[table_idx]->s[space_idx]->var_idx,
                table->ss[table_idx]->s[space_idx]->var_size);
        }
    }
}

map_list *create_map_list(const char *name)
{
    assert(name != NULL);

    map_list *maps = (map_list *)malloc(sizeof(map_list));
    memset(maps, 0, sizeof(map_list));

    strncpy(maps->name, name, MIN(MAP_NAME_MAX, strlen(name)));
    return maps;
}

map_list *find_map_list(code_map *c_map, const char *space)
{
    int m_idx;

    for (m_idx = 0; m_idx < c_map->m_idx; ++m_idx) {
        if (strcmp(space, c_map->maps[m_idx]->name) == 0) {
            return c_map->maps[m_idx];
        }
    }

    return NULL;
}

code_list *create_code_list()
{
    code_list *cl = (code_list *)malloc(sizeof(code_list));
    memset(cl, 0, sizeof(code_list));

    cl->c_idx = 0;
    cl->c = (code **)malloc(MASP_CODE_MAX * sizeof(code *));

    return cl;
}

func_jump *create_func_jump(char *func_name, int jump_num)
{
    func_jump *jump = (func_jump *)malloc(sizeof(func_jump));
    memset(jump, 0, sizeof(func_jump));

    jump->name = func_name;
    jump->jump_num = jump_num;

    return jump;
}

func_jump_map *create_jump_map()
{
    func_jump_map *jumps = (func_jump_map *)malloc(sizeof(func_jump_map));
    memset(jumps, 0, sizeof(func_jump_map));

    jumps->f_idx = 0;
    jumps->fj = (func_jump **)malloc(MASP_CODE_MAX * sizeof(func_jump *));

    return jumps;
}

void set_func_jump_map(func_jump_map *jumps, char *func_name, int jump_num)
{
    // fixed: need consider func_name conflict.
    jumps->fj[jumps->f_idx++] = create_func_jump(func_name, jump_num);
}

int get_func_jump_num(func_jump_map *jumps, char *name)
{
    int idx;
    for (idx = 0; idx < jumps->f_idx; ++idx) {
        if (strncmp(jumps->fj[idx]->name, name, 
            MIN(strlen(jumps->fj[idx]->name), strlen(name))) == 0) {
            return jumps->fj[idx]->jump_num;
        }
    }

    return 0;
}

code *create_code()
{
    code *c = (code *)malloc(sizeof(code));
    memset(c, 0, sizeof(code));

    return c;
}

code *create_code2(instruction ins, char *offset)
{
    code *c = create_code();

    c->ins = ins;
    strncpy(c->offset, offset, VAR_OFFSET_MAX);

    return c;
}

int code_list_push(code_list *cl, instruction ins, char *offset)
{
    // undo clear offset
    assert(cl != NULL && offset != NULL);

    cl->c[cl->c_idx++] = create_code2(ins, offset);
    
    return cl->c_idx - 1;
}

void code_list_set(code_list *cl, const int idx, char *offset)
{
    assert(cl != NULL && offset != NULL && cl->c_idx >= idx);

    strncpy(cl->c[idx]->offset, offset, VAR_OFFSET_MAX);
}

void code_list_push2(code_list *cl, code* c)
{
    assert(cl != NULL && c != NULL);

    cl->c[cl->c_idx++] = c;
}

void code_list_append(code_list *codes, code_list *extras)
{
    assert(codes != NULL && extras != NULL);
    int idx;

    for (idx = 0; idx < extras->c_idx; ++idx) {
        codes->c[codes->c_idx++] = extras->c[idx];
        // need free ?
    }
}

void code_list_print(code_list *cl)
{
    int idx;
    for (idx = 0; idx < cl->c_idx; ++idx) {
        printf("%d %s\n", cl->c[idx]->ins, cl->c[idx]->offset);
    }
}

void func_jump_map_print(const func_jump_map *jumps)
{
    int idx;
    for (idx = 0; idx < jumps->f_idx; ++idx) {
        printf("%s %d\n", jumps->fj[idx]->name, jumps->fj[idx]->jump_num);
    }
}

code_map *create_code_map()
{
    code_map *cm = (code_map *)malloc(sizeof(code_map));
    memset(cm, 0, sizeof(code_map));
    cm->m_idx = 0;
    cm->maps = (map_list **)malloc(MAP_LIST_MAX * sizeof(map_list *));

    return cm;
} 

void set_code_map(code_map *c_map, const char *cur_space, const code_list *input_cl)
{
    int idx;
    code *c;
    c_map->maps[c_map->m_idx] = create_map_list(cur_space);
    c_map->maps[c_map->m_idx]->cl = create_code_list();

    for (idx = 0; idx < input_cl->c_idx; ++idx) {
        c = create_code2(input_cl->c[idx]->ins, input_cl->c[idx]->offset);
        c_map->maps[c_map->m_idx]->cl->c[c_map->maps[c_map->m_idx]->cl->c_idx] = c;
        ++c_map->maps[c_map->m_idx]->cl->c_idx;
    }

    ++c_map->m_idx;
}

void code_list_clean(code_list *cl)
{
    int idx;

    for (idx = 0; idx < cl->c_idx; ++idx) {
        if (cl->c[idx]) {
            free(cl->c[idx]);
            cl->c[idx] = NULL;
        }
    }
    cl->c_idx = 0;
}

code_segment *create_code_segment()
{
    code_segment *cs = (code_segment *)malloc(sizeof(code_segment));
    memset(cs, 0, sizeof(code_segment));

    cs->capacity = MASP_CODE_MAX;
    cs->size = 0;
    cs->data = (segment **)malloc(cs->capacity * sizeof(segment *));

    return cs;
}

segment *create_segment()
{
    segment *seg = (segment *)malloc(sizeof(segment));
    memset(seg, 0, sizeof(segment));

    return seg;
}

segment *create_segment2(instruction ins, int offset)
{
    segment *seg = create_segment();

    seg->ins = ins;
    seg->offset = offset;

    return seg;
}

void code_segment_push(code_segment *cs, instruction ins, int offset)
{
    assert(cs != NULL);

    if (cs->size >= cs->capacity) {
        return;
    }

    cs->data[cs->size++] = create_segment2(ins, offset);
}

void code_map_print(const code_map *c_map)
{
    assert(c_map != NULL);

    int m_idx;
    for (m_idx = 0; m_idx < c_map->m_idx; ++m_idx) {
        printf("name: %s, code list:\n", c_map->maps[m_idx]->name);
        code_list_print(c_map->maps[m_idx]->cl);
    }
}

virtual_machine *create_virtual_machine()
{
    virtual_machine *vm = (virtual_machine *)malloc(sizeof(virtual_machine));

    memset(vm, 0, sizeof(virtual_machine));

    vm->ax = 0;
    vm->bp = 0;
    vm->ip = 0;
    vm->ss = create_vm_stack();

    return vm;
}

vm_stack *create_vm_stack()
{
    vm_stack *ss = (vm_stack *)malloc(sizeof(vm_stack));
    memset(ss, 0, sizeof(vm_stack));

    ss->capacity = MASP_CODE_MAX;
    ss->size = 0;

    return ss;
}

int vm_stack_pop(vm_stack *vm)
{
    assert(vm != NULL && vm->size >= 0);

    return vm->data[vm->size-- - 1];
}

int vm_stack_back(const vm_stack *vm)
{
    assert(vm != NULL && vm->size < vm->capacity);

    return vm->data[vm->size - 1];
}

int vm_stack_size(const vm_stack *vm)
{
    assert(vm != NULL);

    return vm->size;
}

int vm_stack_get(const vm_stack *vm, const int index)
{
    assert(vm != NULL && index < vm->size);

    return vm->data[index];
}

void vm_stack_push(vm_stack *vm, const int value)
{
    assert(vm != NULL && vm->size < vm->capacity);

    vm->data[vm->size++] = value;
}

void vm_stack_set(vm_stack *vm, const int index, const int value)
{
    assert(vm != NULL && index < vm->size);

    vm->data[index] = value;
}

int file_read_content(const char *file_path, char **str)
{
    FILE *fp;
    size_t file_length, count = 1;

    fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", file_path);
        return CMM_FAILED;
    }

    fseek(fp, 0, SEEK_END);
    file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    *str = (char *)malloc(file_length);
    if ((*str) == NULL) {
        printf("malloc code string failed.\n");
        return CMM_FAILED;
    }

    if (count != fread((*str), file_length, count, fp)) {
        printf("fread file content failed, file path: %s.\n", file_path);
        return CMM_FAILED;
    }

    fclose(fp);
    return CMM_SUCCESS;
}

int file_write_content(code_list *codes, const char *asm_file)
{
    FILE *fp;
    int idx, line_len, data_len;
    char line[VAR_OFFSET_MAX * 2];
    char *data = NULL;

    if (codes == NULL || asm_file == NULL) {
        return CMM_FAILED;
    }

    if (strlen(asm_file) == 0) {
        asm_file = DEFAULT_ASM_PATH;
    }

    data = (char *)malloc(codes->c_idx * (VAR_OFFSET_MAX * 2));
    data_len = 0;

    fp = fopen(asm_file, "w");
    if (fp == NULL) {
        printf("fopen file failed, file path: %s.\n", asm_file);
        return CMM_FAILED;
    }

    for (idx = 0; idx < codes->c_idx; ++idx) {
        switch (codes->c[idx]->ins) {
            case INS_LDC:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "LDC %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_LD:
                strcpy(data + data_len, "LD\n");
                data_len += 3;
                break;
            case INS_ALD:
                strcpy(data + data_len, "ALD\n");
                data_len += 4;
                break;
            case INS_ST:
                strcpy(data + data_len, "ST\n");
                data_len += 3;
                break;
            case INS_AST:
                strcpy(data + data_len, "__AST\n");
                data_len += 6;
                break;
            case INS_PUSH:
                strcpy(data + data_len, "PUSH\n");
                data_len += 5;
                break;
            case INS_POP:
                strcpy(data + data_len, "POP\n");
                data_len += 4;
                break;
            case INS_JMP:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "JMP %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_JZ:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "JZ %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_ADD:
                strcpy(data + data_len, "ADD\n");
                data_len += 4;
                break;
            case INS_SUB:
                strcpy(data + data_len, "SUB\n");
                data_len += 4;
                break;
            case INS_MUL:
                strcpy(data + data_len, "MUL\n");
                data_len += 4;
                break;
            case INS_DIV:
                strcpy(data + data_len, "DIV\n");
                data_len += 4;
                break;
            case INS_LT:
                strcpy(data + data_len, "LT\n");
                data_len += 3;
                break;
            case INS_LE:
                strcpy(data + data_len, "LE\n");
                data_len += 3;
                break;
            case INS_GT:
                strcpy(data + data_len, "GT\n");
                data_len += 3;
                break;
            case INS_GE:
                strcpy(data + data_len, "GE\n");
                data_len += 3;
                break;
            case INS_EQ:
                strcpy(data + data_len, "EQ\n");
                data_len += 3;
                break;
            case INS_NE:
                strcpy(data + data_len, "NE\n");
                data_len += 3;
                break;
            case INS_IN:
                strcpy(data + data_len, "IN\n");
                data_len += 3;
                break;
            case INS_OUT:
                strcpy(data + data_len, "OUT\n");
                data_len += 4;
                break;
            case INS_ADDR:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "ADDR %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_CALL:
                line_len = snprintf(line, VAR_OFFSET_MAX * 2, "CALL %s\n", codes->c[idx]->offset);
                strncpy(data + data_len, line, line_len);
                data_len += line_len;
                break;
            case INS_RET:
                strcpy(data + data_len, "RET\n");
                data_len += 4;
                break;
            default:
                invalid_call("output_asm_file");
        }
    }

    fwrite(data, data_len, 1, fp);
    fclose(fp);
    free(data);
    return CMM_SUCCESS;
}