#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"

token *create_token(char *value, int line, int column)
{

    token *t = (token *)malloc(sizeof(token));

    if (value != NULL)
    {
        t->value = (char *)strdup(value);
    }
    else
    {
        t->value = NULL;
    }

    t->line = line;
    t->column = column;

    return t;
}

node *create_node(char *name, char *value, int line, int column)
{
    node *n = (node *)malloc(sizeof(node));

    n->name = (char *)strdup(name);

    if (value != NULL)
    {
        n->value = (char *)strdup(value);
    }
    else
    {
        n->value = NULL;
    }

    n->annotation = NULL;
    n->brother = NULL;
    n->child = NULL;
    n->line = line;
    n->column = column;

    return n;
}

void add_child(node *dad, node *child)
{
    if (dad == NULL || child == NULL)
    {
        return;
    }
    else
    {
        dad->child = child;
    }
}

void add_brother(node *aux_brother, node *new_brother)
{
    node *aux = aux_brother;
    if (aux_brother == NULL || new_brother == NULL)
    {
        return;
    }
    while (aux->brother != NULL)
    {
        aux = aux->brother;
    }
    aux->brother = new_brother;
}

void check_brothers(node *aux, char *val)
{

    while (aux->brother != NULL)
    {
        aux = aux->brother;
        node *new_child = create_node(val, NULL, 0, 0);
        node *child = aux->child;
        aux->child = new_child;
        add_brother(new_child, child);
    }
}

int count_brothers(node *start_node)
{
    int n = 0;
    node *aux;
    aux = start_node;
    while (aux != NULL)
    {
        aux = aux->brother;
        n++;
    }
    return n;
}

void printAST(node *current, int n)
{
    int i;

    if (current == NULL)
    {
        return;
    }

    if (strcmp(current->name, "NULL") == 0)
    {
        printAST(current->brother, n);
        return;
    }

    if (strcmp(current->name, "NULL") != 0)
    {
        for (i = 0; i < n; i++)
        {
            printf("..");
        }

        if (current->value != NULL)
        {

            if (current->annotation == NULL)
                printf("%s(%s)\n", current->name, current->value);
            else
                printf("%s(%s) - %s\n", current->name, current->value, current->annotation);
        }
        else
        {
            if (current->annotation == NULL)
                printf("%s\n", current->name);
            else
                printf("%s - %s\n", current->name, current->annotation);
        }
    }

    printAST(current->child, n + 1);
    printAST(current->brother, n);
}

// -----------------------------------------------

func_list insert_table(char *table_name, char *table_type, int func_check)
{

    func_list new_node = (func_list)malloc(sizeof(func));
    new_node->table = malloc(sizeof(symb_table));
    new_node->table->table_name = strdup(table_name);
    new_node->table->func_check = func_check;
    if (table_type != NULL)
    {
        new_node->table->table_type = strdup(table_type);
    }
    else
    {
        new_node->table->table_type = NULL;
    }
    new_node->next = NULL;

    if (func_header == NULL)
    {
        func_header = new_node;
        return func_header;
    }

    func_list aux = func_header;
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    aux->next = new_node;
    return new_node;
}

void insert_var(func_list func, char *name, char *type)
{
    vars_list new_node = (vars_list)malloc(sizeof(vars));
    new_node->var_name = strdup(name);
    new_node->var_type = strdup(type);
    new_node->next = NULL;

    if (func->func_vars == NULL)
    {
        func->func_vars = new_node;
        return;
    }

    vars_list aux = func->func_vars;
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    aux->next = new_node;
}

void insert_param(func_list func, char *name, char *type)
{
    param_list new_node = (param_list)malloc(sizeof(params));
    new_node->param_name = strdup(name);
    new_node->param_type = strdup(type);
    new_node->next = NULL;

    if (func->func_param == NULL)
    {
        func->func_param = new_node;
        return;
    }

    param_list aux = func->func_param;
    while (aux->next != NULL)
    {
        aux = aux->next;
    }
    aux->next = new_node;
}

void print_tables()
{
    // Print Global table
    printf("===== Global Symbol Table =====\n");
    vars_list aux_vars = func_header->func_vars;
    while (aux_vars != NULL)
    {
        printf("%s\t\t%s\n", aux_vars->var_name, aux_vars->var_type);
        aux_vars = aux_vars->next;
    }

    func_list aux = func_header->next;
    while (aux != NULL)
    {
        printf("%s\t(", aux->table->table_name);
        param_list aux_params = aux->func_param;
        while (aux_params != NULL)
        {
            printf("%s", aux_params->param_type);
            aux_params = aux_params->next;
            if (aux_params != NULL)
            {
                printf(",");
            }
        }

        if (aux->table->table_type != NULL)
        {
            printf(")\t%s\n", aux->table->table_type);
        }
        else
        {
            printf(")\tnone\n");
        }

        aux = aux->next;
    }
    printf("\n");

    //Print Local tables
    aux = func_header->next;
    while (aux != NULL)
    {
        printf("===== Function %s(", aux->table->table_name);
        param_list aux_params = aux->func_param;
        while (aux_params != NULL)
        {
            printf("%s", aux_params->param_type);
            aux_params = aux_params->next;
            if (aux_params != NULL)
            {
                printf(",");
            }
        }
        printf(") Symbol Table =====\n");

        if (aux->table->table_type != NULL)
        {
            printf("return\t\t%s\n", aux->table->table_type);
        }
        else
        {
            printf("return\t\tnone\n");
        }

        aux_params = aux->func_param;
        while (aux_params != NULL)
        {
            printf("%s\t\t%s\tparam\n", aux_params->param_name, aux_params->param_type);
            aux_params = aux_params->next;
        }

        vars_list aux_vars = aux->func_vars;
        while (aux_vars != NULL)
        {
            printf("%s\t\t%s\n", aux_vars->var_name, aux_vars->var_type);
            aux_vars = aux_vars->next;
        }

        printf("\n");
        aux = aux->next;
    }
}

char *search_var(func_list func, char *name)
{
    vars_list aux_vars = func->func_vars;
    while (aux_vars != NULL)
    {
        if (strcmp(aux_vars->var_name, name) == 0)
        {
            return aux_vars->var_type;
        }
        aux_vars = aux_vars->next;
    }
    param_list aux_params = func->func_param;
    while (aux_params != NULL)
    {
        if (strcmp(aux_params->param_name, name) == 0)
        {
            return aux_params->param_type;
        }
        aux_params = aux_params->next;
    }
    return NULL;
}

char *search_table(func_list func, char *params, char *name)
{
    func_list aux = func->next;
    while (aux != NULL)
    {
        if (strcmp(aux->table->table_name, name) == 0)
        {

            param_list aux_params = aux->func_param;

            char types[1024];
            strcpy(types, "(");
            while (aux_params != NULL)
            {
                strcat(types, aux_params->param_type);
                aux_params = aux_params->next;
                if (aux_params != NULL)
                {
                    strcat(types, ",");
                }
            }
            strcat(types, ")");

            if (strcmp(types, params) == 0)
            {
                // if (strcmp(types, "()") == 0)
                // {
                //     return "none";
                // }
                if(aux->table->table_type == NULL){
                    return "none";
                }
                return aux->table->table_type;
            }
        }
        aux = aux->next;
    }
    return NULL;
}

func_list search_atual_table(func_list func, char *name)
{
    func_list aux = func->next;
    while (aux != NULL)
    {
        if (strcmp(aux->table->table_name, name) == 0)
        {
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

//retorna 0 se encontrar e -1 se nao encontrar
int search_function_exists(func_list func, char *name)
{
    func_list aux = func->next;
    while (aux != NULL)
    {
        if (strcmp(aux->table->table_name, name) == 0)
        {
            return -1;
        }
        aux = aux->next;
    }
    return 0;
}

int search_var_exists(func_list func, char *name)
{
    vars_list aux_vars = func->func_vars;
    while (aux_vars != NULL)
    {
        if (strcmp(aux_vars->var_name, name) == 0)
        {
            return -1;
        }
        aux_vars = aux_vars->next;
    }
    param_list aux_params = func->func_param;
    while (aux_params != NULL)
    {
        if (strcmp(aux_params->param_name, name) == 0)
        {
            return -1;
        }
        aux_params = aux_params->next;
    }
    return 0;
}

char *change_type(char *type)
{
    if (strcmp(type, "Int") == 0)
    {
        return "int";
    }
    else if (strcmp(type, "Float32") == 0)
    {
        return "float32";
    }
    else if (strcmp(type, "Bool") == 0)
    {
        return "bool";
    }
    else if (strcmp(type, "String") == 0)
    {
        return "string";
    }
    else if (strcmp(type, "IntLit") == 0)
    {
        return "intLit";
    }
    else if (strcmp(type, "RealLit") == 0)
    {
        return "realLit";
    }
    else
    {
        return "strLit";
    }
}

void semantic_analysis(node *root)
{

    node *atual = root->child;
    node *aux1, *aux2, *aux3, *aux4, *aux5;
    char *name, *type;

    global_table = insert_table("Global", NULL, 0);
    func_list atual_table = global_table;

    while (atual != NULL)
    {
        if (strcmp(atual->name, "FuncDecl") == 0)
        {
            aux1 = atual->child;

            // FuncHeader
            aux2 = aux1->child; // function ID

            int check = search_function_exists(func_header, aux2->value);
            if (check != 0)
            {
                flag_sem_errors = -1;
                printf("Line %d, column %d: Symbol %s already defined\n", aux2->line, aux2->column, aux2->value);
            }

            aux3 = aux2->brother; // function type or params
            if (strcmp(aux3->name, "FuncParams") == 0)
            { // function is void
                atual_table = insert_table(aux2->value, NULL, 1);
                aux4 = aux3; // params
            }
            else
            {
                atual_table = insert_table(aux2->value, change_type(aux3->name), 1);
                aux4 = aux3->brother; // params
            }

            aux5 = aux4->child;
            while (aux5 != NULL)
            {
                type = change_type(aux5->child->name);
                name = aux5->child->brother->value;
                insert_param(atual_table, name, type);
                aux5 = aux5->brother;
            }

            // FuncBody
            // filho do funcbody
            aux1 = aux1->brother->child;
            while (aux1 != NULL)
            {
                if (strcmp(aux1->name, "VarDecl") == 0)
                {
                    type = change_type(aux1->child->name);
                    name = aux1->child->brother->value;
                    int check = search_var_exists(atual_table, name);
                    if (check == 0)
                    {
                        insert_var(atual_table, name, type);
                    }
                    else
                    {
                        flag_sem_errors = -1;
                        printf("Line %d, column %d: Symbol %s already defined\n", aux1->line, aux1->column, aux1->value);
                    }
                }
                aux1 = aux1->brother;
            }
        }
        else if (strcmp(atual->name, "VarDecl") == 0)
        {
            type = change_type(atual->child->name);
            node *id = atual->child->brother;
            name = id->value;

            int check = search_var_exists(atual_table, name);
            if (check == 0)
            {
                insert_var(atual_table, name, type);
            }
            else
            {
                flag_sem_errors = -1;
                printf("Line %d, column %d: Symbol %s already defined\n", id->line, id->column, id->value);
            }
        }
        atual = atual->brother;
    }
}

void call_ast(node *root)
{

    func_list atual_table = global_table;

    node *atual = root->child;
    node *aux1, *aux2, *aux3, *aux4, *aux5;
    char *name, *type;

    while (atual != NULL)
    {
        if (strcmp(atual->name, "FuncDecl") == 0)
        {
            aux1 = atual->child;

            // FuncHeader
            aux2 = aux1->child;   // function ID
            aux3 = aux2->brother; // function type or params

            if (strcmp(aux3->name, "FuncParams") == 0)
            {                // function is void
                aux4 = aux3; // params
            }
            else
            {
                aux4 = aux3->brother; // params
            }
            aux5 = aux4->child;
            while (aux5 != NULL)
            {
                type = change_type(aux5->child->name);
                name = aux5->child->brother->value;
                aux5 = aux5->brother;
            }
            // FuncBody
            // filho do funcbody
            aux1 = aux1->brother->child;
            atual_table = search_atual_table(func_header, aux2->value);
            while (aux1 != NULL)
            {
                if (strcmp(aux1->name, "VarDecl") != 0)
                {
                    annote_AST(aux1, atual_table);
                }
                aux1 = aux1->brother;
            }
        }
        atual = atual->brother;
    }
}

void annote_AST(node *atual_node, func_list atual_table)
{
    if (atual_node == NULL)
    {
        return;
    }

    //printf("node name: %s\n", atual_node->name);

    if (strcmp(atual_node->name, "Not") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        if (strcmp(atual_node->child->annotation, "bool") != 0)
        {
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Operator ! cannot be applied to type %s\n", atual_node->line, atual_node->column, atual_node->child->annotation);
        }
        else
        {
            atual_node->annotation = strdup("bool");
        }
    }
    else if (strcmp(atual_node->name, "Eq") == 0 || strcmp(atual_node->name, "Ne") == 0 || strcmp(atual_node->name, "Lt") == 0 || strcmp(atual_node->name, "Gt") == 0 || strcmp(atual_node->name, "Le") == 0 || strcmp(atual_node->name, "Ge") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);

        char *operator;
        if (strcmp(atual_node->name, "Eq") == 0)
        {
            operator= "==";
        }
        else if (strcmp(atual_node->name, "Ne") == 0)
        {
            operator= "!=";
        }
        else if (strcmp(atual_node->name, "Lt") == 0)
        {
            operator= "<";
        }
        else if (strcmp(atual_node->name, "Gt") == 0)
        {
            operator= ">";
        }
        else if (strcmp(atual_node->name, "Ge") == 0)
        {
            operator= ">=";
        }
        else if (strcmp(atual_node->name, "Le") == 0)
        {
            operator= "<=";
        }

        if (strcmp(atual_node->child->annotation, atual_node->child->brother->annotation) == 0)
        {
            atual_node->annotation = strdup("bool");
        }
        else
        {
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, operator, atual_node->child->annotation, atual_node->child->brother->annotation);
  
        }
    }
    else if (strcmp(atual_node->name, "Or") == 0 || strcmp(atual_node->name, "And") == 0)
    {

        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);

        if (strcmp(atual_node->child->annotation, atual_node->child->brother->annotation) == 0 &&
            strcmp(atual_node->child->annotation, "bool") == 0)
        {
            atual_node->annotation = atual_node->child->annotation;
        }
        else
        {
            atual_node->annotation = "undef";
            if (strcmp(atual_node->name, "Or") == 0)
            {
                printf("Line %d, column %d: Operator || cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, atual_node->child->annotation, atual_node->child->brother->annotation);
            }
            else
            {
                printf("Line %d, column %d: Operator && cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, atual_node->child->annotation, atual_node->child->brother->annotation);
            }
        }
    }
    else if (strcmp(atual_node->name, "Mod") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);

        if (strcmp(atual_node->child->annotation, atual_node->child->brother->annotation) == 0 &&
            strcmp(atual_node->child->annotation, "int") == 0)
        {
            atual_node->annotation = strdup("int");
        }
        else
        {
            
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Operator %% cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, atual_node->child->annotation, atual_node->child->brother->annotation);

        }
    }

    else if (strcmp(atual_node->name, "IntLit") == 0 || strcmp(atual_node->name, "Int") == 0)
    {
        atual_node->annotation = strdup("int");
    }

    else if (strcmp(atual_node->name, "String") == 0 || strcmp(atual_node->name, "StrLit") == 0)
    {
        atual_node->annotation = strdup("string");
    }

    else if (strcmp(atual_node->name, "Add") == 0 || strcmp(atual_node->name, "Sub") == 0 || strcmp(atual_node->name, "Mul") == 0 || strcmp(atual_node->name, "Div") == 0)
    {

        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);

        char *operator;
        if (strcmp(atual_node->name, "Add") == 0)
        {
            operator= "+";
        }
        else if (strcmp(atual_node->name, "Sub") == 0)
        {
            operator= "-";
        }
        else if (strcmp(atual_node->name, "Mul") == 0)
        {
            operator= "*";
        }
        else if (strcmp(atual_node->name, "Div") == 0)
        {
            operator= "/";
        }

        if (strcmp(atual_node->child->annotation, atual_node->child->brother->annotation) == 0)
        {
            atual_node->annotation = atual_node->child->annotation;
        }
        else
        {
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, operator, atual_node->child->annotation, atual_node->child->brother->annotation);
        }
    }
    else if (strcmp(atual_node->name, "Minus") == 0 || strcmp(atual_node->name, "Plus") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        atual_node->annotation = atual_node->child->annotation;
    }
    else if (strcmp(atual_node->name, "RealLit") == 0 || strcmp(atual_node->name, "Float32") == 0)
    {
        atual_node->annotation = strdup("float32");
    }
    else if (strcmp(atual_node->name, "Bool") == 0)
    {
        atual_node->annotation = strdup("bool");
    }

    else if (strcmp(atual_node->name, "Id") == 0)
    {
        char *type = search_var(atual_table, atual_node->value);
        if (type != NULL)
        {
            atual_node->annotation = type;
        }
        else
        {
            type = search_var(func_header, atual_node->value);
            if (type != NULL)
            {
                atual_node->annotation = type;
            }
            else
            {
                flag_sem_errors = -1;
                printf("Line %d, column %d: Cannot find symbol %s\n", atual_node->line, atual_node->column, atual_node->value);
                atual_node->annotation = "undef";
            }
        }
    }
    else if (strcmp(atual_node->name, "Call") == 0)
    {
        node *func = atual_node->child;

        char types[255];
        strcpy(types, "(");
        node *aux = func->brother;
        while (aux != NULL)
        {
            annote_AST(aux, atual_table);
            strcat(types, aux->annotation);
            aux = aux->brother;
            if (aux != NULL)
            {
                strcat(types, ",");
            }
        }
        strcat(types, ")");
        func->annotation = strdup(types);

        char *type = search_table(func_header, func->annotation, func->value);

        if (type != NULL)
        {
            if(strcmp(type, "none") != 0){
                atual_node->annotation = type;
            }
        }
        else
        {
            //error
            flag_sem_errors = -1;
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Cannot find symbol %s%s\n", func->line, func->column, func->value, func->annotation);
        }
    }

    else if (strcmp(atual_node->name, "Assign") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);

        if (strcmp(atual_node->child->annotation, atual_node->child->brother->annotation) == 0)
        {
            atual_node->annotation = atual_node->child->annotation;
        }
        else
        {
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Operator = cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, atual_node->child->annotation, atual_node->child->brother->annotation);
        }
    }

    else if (strcmp(atual_node->name, "ParseArgs") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);

        if (strcmp(atual_node->child->annotation, atual_node->child->brother->annotation) == 0 &&
            strcmp(atual_node->child->annotation, "int") == 0)
        {
            atual_node->annotation = atual_node->child->annotation;
        }
        else
        {
            atual_node->annotation = "undef";
            printf("Line %d, column %d: Operator strconv.Atoi cannot be applied to types %s, %s\n", atual_node->line, atual_node->column, atual_node->child->annotation, atual_node->child->brother->annotation);
        }

        atual_node->annotation = atual_node->child->annotation;
    }
    else if (strcmp(atual_node->name, "Print") == 0)
    {
        annote_AST(atual_node->child, atual_table);
    }
    else if (strcmp(atual_node->name, "If") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);
        annote_AST(atual_node->child->brother->brother, atual_table);
    }
    else if (strcmp(atual_node->name, "Block") == 0)
    {

        node *aux = atual_node->child;
        while (aux != NULL)
        {
            annote_AST(aux, atual_table);

            aux = aux->brother;
        }
    }

    else if (strcmp(atual_node->name, "Return") == 0)
    {
        if (atual_node->child != NULL)
        {
            annote_AST(atual_node->child, atual_table);
        }
    }

    else if (strcmp(atual_node->name, "For") == 0)
    {
        annote_AST(atual_node->child, atual_table);
        annote_AST(atual_node->child->brother, atual_table);
    }
}