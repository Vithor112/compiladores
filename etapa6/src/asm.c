#include "asm.h"

char * concat_str(char * str, char * i){
    int length =  strlen(str) + strlen(i) + 1; 
    char *new_str = malloc(sizeof(char) * (length)); 
    *new_str = '\0'; 
    strcat(new_str, str);
    strcat(new_str, i); 
    return new_str; 
}

char *get_mem_str(char * shift){
    char *str_2 = concat_str("-", shift);
    char *str_3 = concat_str(str_2, "(%rbp)"); 
    free(str_2); 
    return str_3; 
}

char * get_asm_op_code(char * mnemonico){
    if (!strcmp(mnemonico, "add")){
        return "addl"; 
    }
    if (!strcmp(mnemonico, "sub")){
        return "subl"; 
    }
    if (!strcmp(mnemonico, "mult")){
        return "imul"; 
    }
    fprintf(stderr, "Failed to find asm code for iloc instruction %s", mnemonico);
    exit(1); 
}

void  convert_iloc_to_asm(iloc_op * op, asm_op_list * list, table_symbol * table){
    if (!strcmp(op->mnemonico, "loadI")){
        row_symbol * row = get_or_create_row_from_scope(table, op->arg2); 
        char * str = concat_str("$", op->arg1); 
        char * str_2 = get_mem_str(row->shift); 
        add_iloc_operation(list, new_iloc_operation("movl", str, str_2, NULL)); 
        free(str); 
        free(str_2);
    }
    if (!strcmp(op->mnemonico, "return")){
        row_symbol * row = get_row_from_scope_or_throw(table, op->arg1); 
        char *str = get_mem_str(row->shift); 
        add_iloc_operation(list, new_iloc_operation("movl", str, "%eax", NULL)); 
        free(str);
    }
    if (!strcmp(op->mnemonico, "add") || !strcmp(op->mnemonico, "sub") || !strcmp(op->mnemonico, "mult")){
        row_symbol * row = get_row_from_scope_or_throw(table, op->arg1); 
        char *str = get_mem_str(row->shift); 
        row_symbol * row_2 = get_row_from_scope_or_throw(table, op->arg2); 
        char *str_2 = get_mem_str(row_2->shift); 
        row_symbol * row_3 = get_or_create_row_from_scope(table, op->arg3); 
        char *str_3 = get_mem_str(row_3->shift); 
        add_iloc_operation(list, new_iloc_operation("movl", str,  "%ebx", NULL)); 
        add_iloc_operation(list, new_iloc_operation(get_asm_op_code(op->mnemonico), str_2, "%ebx", NULL)); 
        add_iloc_operation(list, new_iloc_operation("movl", "%ebx",  str_3, NULL));
        free(str);
        free(str_2);
        free(str_3);
    }
    if (!strcmp(op->mnemonico, "div")){
        row_symbol * row = get_row_from_scope_or_throw(table, op->arg1); 
        char *str = get_mem_str(row->shift); 
        row_symbol * row_2 = get_row_from_scope_or_throw(table, op->arg2); 
        char *str_2 = get_mem_str(row_2->shift); 
        row_symbol * row_3 = get_or_create_row_from_scope(table, op->arg3); 
        char *str_3 = get_mem_str(row_3->shift); 
        add_iloc_operation(list, new_iloc_operation("movl", str,  "%eax", NULL));
        add_iloc_operation(list, new_iloc_operation("cltd", NULL, NULL, NULL)); 
        add_iloc_operation(list, new_iloc_operation("idivl", str_2, NULL, NULL)); 
        add_iloc_operation(list, new_iloc_operation("movl", "%eax",  str_3, NULL));
        free(str);
        free(str_2);
        free(str_3);
    }
}

asm_op_list * generate_asm(iloc_op_list * list, table_symbol * table){
    asm_op_list * list_asm = create_iloc_list(); 
    while (list != NULL){
        convert_iloc_to_asm(list->operation,list_asm, table);
        list = list->next_operation; 
    }
    return list_asm; 
}

void print_asm_op(asm_op * op){
    printf("\t"); 
    if (!strcmp(op->mnemonico, "cltd")){
        printf("%s", op->mnemonico);
    }
    if (!strcmp(op->mnemonico, "idivl")){
        printf("%s %s", op->mnemonico,  op->arg1);
    }
    if (!strcmp(op->mnemonico, "movl") || !strcmp(op->mnemonico, "addl") || !strcmp(op->mnemonico, "subl") || !strcmp(op->mnemonico, "imul")){
        printf("%s %s, %s", op->mnemonico,  op->arg1, op->arg2);
    }
    printf("\n");
}

void print_asm_list(asm_op_list * list){
    printf(".globl	main\n.type	main, @function\nmain:\n.LFB0:\n\tpushq	%%rbp\n\tmovq	%%rsp, %%rbp\n"); 
    while (list != NULL){
        print_asm_op(list->operation);
        list = list->next_operation; 
    }
    printf("\tpopq	%%rbp\n\tret\n"); 
    

}