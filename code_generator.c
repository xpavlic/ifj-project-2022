#include "syntax_tree.h"
/*
TODO:
2) jak se vraci hodnota z funkce? mysleno jak detekovat return
3) detekce variable v expression jako operand
5) definovani novych promenych
6) poresit special funkce
*/

#define VARIABLE 0
//variable RESULT is special for results:)


/**
 * @brief generates code based on syntax tree
 * @param struct tree_node * node
*/
void code_generator(struct tree_node * node);


/**
 * @brief determines what print function should be called upon node
 * @param struct tree_node * node
*/
void scan_node(struct tree_node * node);

/**
 * @brief prints content of node as a code on stdout
 * @param struct tree_node * node
*/
void print_body_node(struct tree_node * node);
void print_decfunc_node(struct tree_node * node);
void print_callfunc_node(struct tree_node * node);
void print_assaign_node(struct tree_node * node);
void print_if_node(struct tree_node * node);
void print_while_node(struct tree_node * node);
void print_expression_node(struct tree_node * node);

void pushs_arguments(struct tree_node * node);
void print_operation_node(struct tree_node * node);
void postorder(struct tree_node * node);
void print_operator(char * value);

/*
///////////////////////////////////////////////////////////
                 HELP FUNCTIONS
//////////////////////////////////////////////////////////
*/




void pushs_arguments(struct tree_node * node){
    if (node->sibling!=NULL){
        call_arguments(node->sibling);
    }    
    printf("PUSHS %s",node->data->value);
}

///////////////////////////////////////////////////////
//                   MAIN FUNCTIONS
//////////////////////////////////////////////////////

void scan_node(struct tree_node * node){
    switch (node->data->type)
    {
    case BODY:
        print_body_node(node);
        break;
    case FUNC_DEC:
        print_funcdec_node(node);
        break;
    case FUNC_CALL:
        print_funccall_node(node);
        break;
    case ASSAIGN:
        print_assaign_node(node);
        break;
    case IF:
        print_if_node(node);
        break;
    case WHILE:
        print_while_node(node);
        break;
    }
}

void print_body_node(struct tree_node * body){
    if(body == NULL) return;
    for(struct tree_node * child_node = body->head_child;child_node!=NULL;child_node=child_node->sibling){
        scan_node(child_node);
    }
}

void print_decfunc_node(struct tree_node * node){
    char * func_name = node->head_child->data->value;
    printf("JUMP _S%s\n",func_name);
     
    //label samotne funkce
    printf("LABEL _L%s\n",func_name);

    //nový rámec pro proměnné argumentu
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    //for each argument do pop do promenne se jmenem argumentu
    
    for(struct tree_node * argument = node->head_child->sibling->head_child;argument!=NULL;argument=argument->sibling){
        printf("DEFVAR LF@%s\n",argument->data->value);
        printf("POPS LF@%s\n",argument->data->value);
    }


    //do body
    print_body_node(node->head_child->sibling->sibling->sibling);

    //give result to result
    printf("MOVE GF@_result LF@%s\n",node->tail_child->data->value);
    //remove frame
    printf("POPFRAME\n");
    //return
    printf("RETURN\n");

    printf("LABEL _S%s\n",func_name);
}

void print_callfunc_node(struct tree_node * node){
    char * func_name = node->head_child->data->value;

    //move every argumentu into stack
    pushs_arguments(node->tail_child->head_child);

    //call function
    printf("CALL _L%s\n",func_name);
}

void print_assaign_node(struct tree_node * node){
    if(node->tail_child->data->type == FUNC_CALL){
        print_callfunc_node(node->tail_child);
    }
    else{
        print_expression_node(node->tail_child);
    }
    //dat vysledek do promenne

    //TODO: POKUD neni prirazovana promena jeste deklarovana, tak je ji potreba deklarovat
    printf("MOVE LF@%s GF@_result\n",node->head_child->data->value);

}

void print_if_node(struct tree_node * node){
    static size_t counter = 0;
    //VYHODNOTIT EXPRESSION
    print_expression_node(node->head_child);
    //JUMP conditional IF
    printf("JUMPIFNEQ IF%d GF@_result 0\n",counter);
    //JUMP ELSE
    printf("JUMP ELSE%d\n",counter);
    //LABEL IF
    printf("LABEL IF%d\n",counter);
    //IF-BODY
    print_body_node(node->head_child->sibling);
    //JUMP ELSEEND
    printf("JUMP ELSEEND%d\n",counter);
    //LABEL ELSE
    printf("LABEL ELSE%d\n",counter);
    //ELSE-BODY
    print_body_node(node->head_child->sibling);
    //LABEL ELSEEND
    printf("LABEL ELSEEND%d\n",counter);

    counter++;
}

void print_while_node(struct tree_node * node){
    static size_t counter = 0;
    //LABEL WHILE
    printf("LABEL WHILE%d\n",counter);
    //EVALUEATE EXPRESSION
    print_expression_node(node->head_child);
    //JUMP CONDITIONAL END
    printf("JUMPIFNEQ WEND%d GF@_result 0\n",counter);
    //BODY
    print_body_node(node->tail_child);
    //JUMP WHILE
    printf("LABEL WHILE%d\n",counter);
    //LABEL END
    printf("LABEL WEND%d\n",counter);


    counter++;
}

void postorder(struct tree_node * node){
    if(node!=NULL){
        postorder(node->head_child);
        postorder(node->tail_child);
    }
    choose_expr_print(node);
}

void print_operator(char * value){
    if(!strcmp(value,"+")){
        printf("ADDS\n");
    }
    else if(!strcmp(value,"-")){
        printf("SUBS\n");
    }
    else if(!strcmp(value,"*")){
        printf("MULS\n");
    }
    else if(!strcmp(value,"/")){
        printf("DIVS\n");
    }
    else if(!strcmp(value,".")){
        printf("POPS GF@_concat2\n");
        printf("POPS GF@_concat1\n");
        printf("CONCAT GF@_concat1 GF@_concat1 GF@_concat2\n");
        printf("PUSHS GF@_concat1\n");
    }
    else if(!strcmp(value,">")){
        printf("GTS\n");
    }
    else if(!strcmp(value,"<")){
        printf("LTS\n");
    }
    else if(!strcmp(value,"<==")){
        printf("GTS\n");
        printf("NOTS\n");
    }
    else if(!strcmp(value,">==")){
        printf("LTS\n");
        printf("NOTS\n");
    }
    else if(!strcmp(value,"!==")){
        printf("EQS\n");
        printf("NOTS\n");
    }
    else if(!strcmp(value,"===")){
        printf("EQS\n");
    }
}

void choose_expr_print(struct tree_node * node){
    if(node->data->type == OPERATOR){
        print_operator(node->data->value);
    }
    //operand but variable
    //TODO:
    else if(node->data->type == VARIABLE){
        //TODO:
        //print push variable
    }
    else{
        printf("PUSHS %s\n",node->data->value);
    }
}

void print_expression_node(struct tree_node * node){
    
    postorder(node);

    printf("POPS GF@_result\n");

}


void code_generator(struct tree_node * node){
    printf("DEFVAR GF@_result\n");
    printf("DEFVAR GF@_concat1\n");
    printf("DEFVAR GF@_concat2\n");

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    print_body_node(node);

    printf("CLEARS\n");
    //TODO:
    //potom asi to ma delat neco na konci??
}
