#include "syntax_tree.h"
/*
TODO:
9) implicitní konverze u aritmetických operací // asi z mat. operací udělat funkce

11)
    function floatval(term) : float
    function intval(term) : int
    function strval(term) : string
    function substring(string $𝑠, int $𝑖, int $𝑗) : ?string
    function ord(string $c) : int //


*/

/*
TODO: LABELS AND NAMES

variable RESULT is special for results:)
variable GF@_op1 and GF@_op2 are used for operations outside stack

MOVE LF@%s GF@_result

function LABEL 
    LABEL %s
function declaration skip  
    LABEL !S%s

if:
(based on static variable counter)
    !IF%d
    !ELSE%d
    !ELSEEND%d

while:
(based on static variable counter)
    !WHILE%d
    !WEND%d
*/


//TODO: remove define
//#define VAR_OPERAND 150
//#define FIRST_ASSIGN 300
//#define RETURN 450

/**
 * @brief generates code based on syntax tree
 * @param struct tree_node * node
*/
void code_generator(struct tree_node * node);


/**
 * @brief determines what print function should be called upon node
 * @param struct tree_node * node
*/
void inbody_scan_node(struct tree_node * node);

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

void print_assaign_first_node(struct tree_node * node);
void print_return_node(struct tree_node * node);


/*
///////////////////////////////////////////////////////////
                 HELP FUNCTIONS
//////////////////////////////////////////////////////////
*/

struct tree_node * find_child_node(struct tree_node * node, int type){
    if(node == NULL) return NULL;
    for(struct tree_node * child_node = node->head_child;child_node!=NULL;child_node=child_node->next_sibling){
        if(child_node->data->type == type){
            return child_node;
        }
    }
    return NULL;
}

void pushs_arguments(struct tree_node * node){
    if (node->next_sibling!=NULL){
        call_arguments(node->next_sibling);
    }
    print_expression_node(node);
    printf("PUSHS GF@_result");
}

///////////////////////////////////////////////////////
//                   MAIN FUNCTIONS
//////////////////////////////////////////////////////

void inbody_scan_node(struct tree_node * node){
    switch (node->data->type)
    {
    case BODY:
        print_body_node(node);
        break;
    case FUNC_DEC:
        print_decfunc_node(node);
        break;
    case FUNC_CALL:
        print_callfunc_node(node);
        break;
    case ASSIGN:
        print_assaign_node(node);
        break;
    case FIRST_ASSIGN:
        print_first_assaign_node(node);
        break;
    case RETURN:
        print_return(node);
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
    for(struct tree_node * child_node = body->head_child;child_node!=NULL;child_node=child_node->next_sibling){
        inbody_scan_node(child_node);
    }
}


void print_decfunc_node(struct tree_node * node){
    char * func_name = find_child_node(node, NAME)->data->value;
    printf("JUMP !S%s\n",func_name);
     
    //label samotne funkce
    printf("LABEL %s\n",func_name);

    //nový rámec pro proměnné argumentu
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    //for each argument do pop do promenne se jmenem argumentu
    
    for(struct tree_node * argument = find_child_node(node, ARGUMENTS)->head_child;argument!=NULL;argument=argument->next_sibling){
        printf("DEFVAR LF@%s\n",argument->data->value);
        printf("POPS LF@%s\n",argument->data->value);
    }


    //do body
    print_body_node(find_child_node(node, BODY));

    printf("MOVE GF@_result null@null");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL !S%s\n",func_name);
}


void print_reads(){
    printf("READ GF@_result string\n");
}
void print_readi(){
    printf("READ GF@_result int\n");
}
void print_readf(){
    printf("READ GF@_result float\n");
}
void print_write(struct tree_node * node){
    struct tree_node * arguments_node;
    arguments_node = find_child_node(node, ARGUMENTS);
    for(struct tree_node * child_node = arguments_node->head_child;child_node!=NULL;child_node=child_node->next_sibling){
        print_expression_node(child_node);
        printf("WRITE GF@_result\n");
    }

}
void print_strlen(struct tree_node * node){
    struct tree_node * arguments_node;
    arguments_node = find_child_node(node, ARGUMENTS);
    print_expression_node(arguments_node->head_child);
    printf("STRLEN GF@_result GF@_result");
}
//void ord(struct tree_node * node) is normal function
void chr(struct tree_node * node){
    struct tree_node * arguments_node;
    arguments_node = find_child_node(node, ARGUMENTS);
    print_expression_node(arguments_node->head_child);
    printf("INT2CHAR GF@_result GF@_result");
}



//TODO: for inbuild functions that doesnt work normally
void print_callfunc_node(struct tree_node * node){
    char * func_name = find_child_node(node, NAME)->data->value;
    if(!strcmp(func_name,"write")){
        print_write(node);
    }
    else if(!strcmp(func_name,"reads")){
        print_reads();
    }
    else if(!strcmp(func_name,"readi")){
        print_readi();
    }
    else if(!strcmp(func_name,"readf")){
        print_readf();
    }
    else if(!strcmp(func_name,"strlen")){
        print_strlen(node);
    }
    else if(!strcmp(func_name,"chr")){
        print_reads(node);
    }
    else{
        print_general_callfunc_node(node);
    }
}


void print_general_callfunc_node(struct tree_node * node){
    char * func_name = find_child_node(node, NAME)->data->value;    
    //move every argumentu into stack
    pushs_arguments(find_child_node(node, ARGUMENTS)->head_child);

    //call function
    printf("CALL %s\n",func_name);

}


void print_first_assaign_node(struct tree_node * node){
    printf("DEFVAR LF@%s\n");
    print_assaign_node(node);
}

void print_assaign_node(struct tree_node * node){
    if(node->tail_child->data->type == FUNC_CALL){
        print_callfunc_node(node->tail_child);
    }
    else{
        print_expression_node(node->tail_child);
    }
    //dat vysledek do promenne

    printf("MOVE LF@%s GF@_result\n",node->head_child->data->value);

}

void print_if_node(struct tree_node * node){
    static size_t counter = 0;
    //VYHODNOTIT EXPRESSION
    print_expression_node(node->head_child);
    //JUMP conditional IF
    printf("JUMPIFNEQ !IF%d GF@_result 0\n",counter);
    //JUMP ELSE
    printf("JUMP !ELSE%d\n",counter);
    //LABEL IF
    printf("LABEL !IF%d\n",counter);
    //IF-BODY
    print_body_node(node->head_child->next_sibling);
    //JUMP ELSEEND
    printf("JUMP !ELSEEND%d\n",counter);
    //LABEL ELSE
    printf("LABEL !ELSE%d\n",counter);
    //ELSE-BODY
    print_body_node(node->head_child->next_sibling->next_sibling);
    //LABEL ELSEEND
    printf("LABEL !ELSEEND%d\n",counter);

    counter++;
}

void print_while_node(struct tree_node * node){
    static size_t counter = 0;
    //LABEL WHILE
    printf("LABEL !WHILE%d\n",counter);
    //EVALUEATE EXPRESSION
    print_expression_node(node->head_child);
    //JUMP CONDITIONAL END
    printf("JUMPIFNEQ !WEND%d GF@_result 0\n",counter);
    //BODY
    print_body_node(node->tail_child);
    //JUMP WHILE
    printf("LABEL !WHILE%d\n",counter);
    //LABEL END
    printf("LABEL !WEND%d\n",counter);


    counter++;
}

void postorder(struct tree_node * node){
    if(node!=NULL){
        postorder(node->head_child);
        postorder(node->tail_child);
        choose_expr_print(node);
    }
}

void print_operator(char * value){
    if(!strcmp(value,"+")){
        printf("CALL !int_to_float\n");
        printf("ADDS\n");
    }
    else if(!strcmp(value,"-")){
        printf("CALL !int_to_float\n");
        printf("SUBS\n");
    }
    else if(!strcmp(value,"*")){
        printf("CALL !int_to_float\n");
        printf("MULS\n");
    }
    else if(!strcmp(value,"/")){
        printf("CALL !int_to_float\n");
        printf("DIVS\n");
    }
    else if(!strcmp(value,".")){
        printf("POPS GF@_op2\n");
        printf("POPS GF@_op1\n");
        printf("CONCAT GF@_op1 GF@_op1 GF@_op2\n");
        printf("PUSHS GF@_op1\n");
    }
    else if(!strcmp(value,">")){
        printf("CALL !GTS\n");
    }
    else if(!strcmp(value,"<")){
        printf("CALL !LTS\n");
    }
    else if(!strcmp(value,"<==")){
        printf("CALL !NGTS\n");
    }
    else if(!strcmp(value,">==")){
        printf("CALL !NLTS\n");
    }
    else if(!strcmp(value,"!==")){
        printf("CALL !EQS\n");
        printf("NOTS\n");
    }
    else if(!strcmp(value,"===")){
        printf("CALL !EQS\n");
    }
}

void choose_expr_print(struct tree_node * node){
    //operator
    if(node->data->type == OPERATOR){
        print_operator(node->data->value);
    }
    //operand but variable
    else if(node->data->type == VAR_OPERAND){
        //print push variable
        printf("PUSHS LF@%s\n",node->data->value);
    }
    //int
    else if(node->data->type == T_INT){
        printf("PUSHS int@%s\n",node->data->value);
    }
    //float
    else if(node->data->type == T_FLOAT){
        //print push variable
        float float_number = strtof (node->data->value, NULL);
        printf("PUSHS float@%a\n",float_number);
    }
    //string
    else if(node->data->type == T_STRING){
        //print push variable
        //printf("PUSHS string@%s\n",node->data->value); but with added escape sequences
        print_string_for_expression(node->data->value);
    }
    //null
    else{ 
        printf("PUSHS null@%s\n",node->data->value);
    }
}

print_string_for_expression(char * string){
    printf("PUSHS string@");
    for(size_t i = 0;string[i]!="\0";i++){
        if(string[i]<32 || string[i]==35 || string[i] == 92){
            printf("%03d", string[i]);
        }
        else{
            putchar(string[i]);
        }
    }
    printf("\n");
}

void print_expression_node(struct tree_node * node){
    //if function has return without value
    if(node == NULL){
        printf("MOVE GF@_result null@null");
        return;
    }
    //postorder tree traversal
    postorder(node);

    printf("POPS GF@_result\n");

}

void print_return_node(struct tree_node * node){
    print_expression_node(node->head_child);
    //remove frame
    printf("POPFRAME\n");
    //return
    printf("RETURN\n");
}



void print_init_code(){
    //global variables for functional usage

    //frame for main body local variables

    printf("DEFVAR GF@_result\n");
    printf("DEFVAR GF@_tmp\n");
    
    printf("DEFVAR GF@_op1\n");
    printf("DEFVAR GF@_op2\n");

}

void print_codefunc_int2float_conversion(){
    printf("JUMP !skip_int_to_float\n");
    printf("LABEL !int_to_float\n");

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("TYPE GF@_result GF@op1\n");
    printf("JUMPIFEQ !int2float_conversion GF@_result string@float");
    //if is same as float jump to conversion
    printf("TYPE GF@_result GF@op2\n");
    //if is same as float jump to conversion 
    printf("JUMPIFEQ !int2float_conversion GF@_result string@float");
    //both are int
    printf("PUSHS GF@_op1\n");
    printf("PUSHS GF@_op2\n");
    printf("RETURN\n");

    //conversion here
    //at least one of them is float
    printf("LABEL !int2float_conversion\n");
    printf("INT2FLOAT GF@_op1\n");
    printf("INT2FLOAT GF@_op2\n");
    printf("PUSHS GF@_op1\n");
    printf("PUSHS GF@_op2\n");
    printf("RETURN\n");

    printf("LABEL !skip_int_to_float\n");

}

void print_EQS_LTS_GTS(char* instruction){
    printf("JUMP !skip%s\n",instruction);
    printf("LABEL !%s\n", instruction);

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("PUSHS GF@_op1\n");
    printf("PUSHS GF@_op2\n");


    printf("TYPE GF@op1 GF@op1\n");
    printf("TYPE GF@op2 GF@op2\n");

    printf("JUMPIFNEQ !else%s GF@op1 GF@op2\n",instruction);


    printf("%s\n",instruction);
    printf("RETURN\n");

    printf("LABEL !else%s\n",instruction);

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("PUSHS int@0\n");

    printf("RETURN\n");

    printf("LABEL !skip%s\n",instruction);
}

void print_NLTS_NGTS(char*instruction){
    printf("JUMP !skip%s\n",instruction);
    printf("LABEL !%s\n", instruction);

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("PUSHS GF@_op1\n");
    printf("PUSHS GF@_op2\n");
    //check if one of them is null ""  0


    printf("TYPE GF@_result GF_op1\n"); //type
    printf("MOVE GF@_tmp GF@_op1"); //value
    printf("JUMPIFEQ !isnull%s GF@_result string@null\n",instruction);
    printf("JUMPIFEQ !isstring%s GF@_result string@string\n",instruction);
    printf("JUMPIFEQ !isint%s GF@_result string@int\n",instruction);

    printf("TYPE GF@_result GF_op2\n"); //type
    printf("MOVE GF@_tmp GF@_op2"); //value
    printf("JUMPIFEQ !isnull%s GF@_result string@null\n",instruction);
    printf("JUMPIFEQ !isstring%s GF@_result string@string\n",instruction);
    printf("JUMPIFEQ !isint%s GF@_result string@int\n",instruction);

        printf("JUMP !skip_scpecial_condition%s\n",instruction);
        //is type null
        printf("LABEL !isnull%s\n",instruction);
            printf("JUMPIFEQ !true%s GF@_tmp null@null\n",instruction);
            printf("JUMP !skip_scpecial_condition%s",instruction);
        printf("LABEL !isstring%s\n",instruction);
            printf("JUMPIFEQ !true%s GF@_tmp string@\n",instruction);
            printf("JUMP !skip_scpecial_condition%s",instruction);
        printf("LABEL !isint%s\n",instruction);
            printf("JUMPIFEQ !true%s GF@_tmp int@0\n",instruction);
            printf("JUMP !skip_scpecial_condition%s",instruction);


        printf("LABEL !skip_scpecial_condition%s",instruction);
    
    
    //end of checking for null "" 0
    printf("TYPE GF@_op1 GF@_op1\n");
    printf("TYPE GF@_op2 GF@_op2\n");

    printf("JUMPINFEQ !false%s GF@op1 GF@op2\n",instruction);

//if
    printf("%s\n",&(instruction[1]));
    printf("NOTS\n");
    printf("RETURN\n");

//if else
    printf("LABEL !true%s\n",instruction);

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("PUSHS int@1\n");

    printf("RETURN\n");

//else
    printf("LABEL !false%s\n",instruction);

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("PUSHS int@0\n");

    printf("RETURN\n");
//end
    printf("LABEL !skip%s\n",instruction);
}




void code_generator(struct tree_node * node){

    print_init_code();

    //TODO: inbuld functions here

    //TODO: implicit coversion INTtoFLOAT
    print_codefunc_int2float_conversion();

    //TODO: comparison
    print_EQS_LTS_GTS("EQS");
    print_EQS_LTS_GTS("LTS");
    print_EQS_LTS_GTS("GTS");
    print_NLTS_NGTS("NLTS");
    print_NLTS_NGTS("NGTS");
    //TODO: zbytek

    /*
    TODO:
    function floatval(term) : float
    function intval(term) : int
    function strval(term) : string
    function substring(string $𝑠, int $𝑖, int $𝑗) : ?string
    function ord(string $c) : int //
    */


    printf("CALL !MAIN\n");
    printf("JUMP !SKIP_MAIN\n");
    printf("LABEL !MAIN\n");

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    print_body_node(node);
    //clear thing after usage
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL !SKIP_MAIN\n");

    printf("CLEARS\n");
    printf("EXIT int@0\n");
    //TODO:
    //potom asi to ma delat neco na konci?? asi nějaký return
}
