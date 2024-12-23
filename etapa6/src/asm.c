#include "asm.h"

char * concat_str(char * str, char * i){
    int length =  strlen(str) + strlen(i) + 1; 
    char *new_str = malloc(sizeof(char) * (length)); 
    *new_str = '\0'; 
    strcat(new_str, str);
    strcat(new_str, i); 
    return new_str; 
}

void  convert_iloc_to_asm(iloc_op * op, asm_op_list * list, table_symbol * table){
    if (!strcmp(op->mnemonico, "loadI")){
        row_symbol * row = get_or_create_row_from_scope(table, op->arg2); 
        char * str = concat_str("$", op->arg1); 
        char *str_2 = concat_str("-", row->shift); 
        char *str_3 = concat_str(str_2, "(%rbp)"); 
        add_iloc_operation(list, new_iloc_operation("movl", str, str_3, NULL)); 
        free(str); 
        free(str_2);
        free(str_3); 
    }
    if (!strcmp(op->mnemonico, "return")){
        row_symbol * row = get_or_create_row_from_scope(table, op->arg1); 
        char *str_2 = concat_str("-", row->shift); 
        char *str_3 = concat_str(str_2, "(%rbp)"); 
        add_iloc_operation(list, new_iloc_operation("movl", str_3, "%eax", NULL)); 
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
    if (!strcmp(op->mnemonico, "movl")){
        printf("movl %s, %s\n", op->arg1, op->arg2);
    }
}

void print_asm_list(asm_op_list * list){
    print_iloc_op_list(list); 
    printf(".globl	main\n.type	main, @function\nmain:\n.LFB0:\n\tpushq	%%rbp\n\tmovq	%%rsp, %%rbp\n"); 
    while (list != NULL){
        print_asm_op(list->operation);
        list = list->next_operation; 
    }
    printf("\tpopq	%%rbp\n\tret\n"); 
    

}