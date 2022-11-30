#include "code_generator.h"

struct tree_node * find_child_node(struct tree_node * node, int type);

//BASIC NODE HANDLING
void print_return_node(struct tree_node * node);
void print_expression_node(struct tree_node * node);
void print_while_node(struct tree_node * node);
void print_if_node(struct tree_node * node);
void print_assaign_node(struct tree_node * node);
void print_first_assign_node(struct tree_node * node);
void print_general_callfunc_node(struct tree_node * node);
void print_callfunc_node(struct tree_node * node);
void print_decfunc_node(struct tree_node * node);
void print_body_node(struct tree_node * body);
void print_defvar_node(struct tree_node * body);

// EXPRESSION NODE HANDLING + everything similar to expression 
void print_string_for_expression(char * string);
void choose_expr_print(struct tree_node * node);
void print_operator(int type);
void postorder(struct tree_node * node);

//CONVERSIONS
void print_codefunc_int2float_conversion();
void implicit_conversion_result(char * type);

void pushs_arguments(struct tree_node * node);

void inbody_scan_node(struct tree_node * node);


//INITIATION OF VARIABLES AT THE START OF THE PROGRAM
void print_init_code();

/*
    FUNCTIONS DEFINITION AT THE START OF THE PROGRAM
    -- definitions of functions, that they can be declared
*/
void print_substring();
void print_ord();
void print_strval();
void print_intval();
void print_floatval();
void print_NLTS_NGTS(char*instruction);
void print_EQS_LTS_GTS(char* instruction);


/*
    ALTERNATIVE FUNCTIONS
    -- functions that are implemented as inline
*/
void print_chr(struct tree_node * node);
void print_strlen(struct tree_node * node);
void print_write(struct tree_node * node);
void print_readf();
void print_readi();
void print_reads();


//////////////////////////////////////////////////////////


struct tree_node * find_child_node(struct tree_node * node, int type){
    if(node == NULL) return NULL;
    for(struct tree_node * child_node = node->head_child;child_node!=NULL;child_node=child_node->next_sibling){
        if((int)child_node->data->type == type){
            return child_node;
        }
    }
    return NULL;
}



void pushs_arguments(struct tree_node * node){
    if(node == NULL) return;

    if (node->next_sibling!=NULL){
        pushs_arguments(node->next_sibling);
    }
    print_expression_node(node);
    

    printf("PUSHS GF@_result\n");
}

void implicit_conversion_result(char * type){
    printf("PUSHS GF@_result\n");
    printf("CALL %sval\n",type);
}

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
        print_first_assign_node(node);
        break;
    case RETURN:
        print_return_node(node);
        break;
    case IF:
        print_if_node(node);
        break;
    case WHILE:
        print_while_node(node);
        break;
    case EXPRESSION:
        print_expression_node(node);
    case VARIABLES: //TODO: DEFVAR
        print_defvar_node(node);

    default:
        break;
    }
}

void print_defvar_node(struct tree_node * defvar_node){
    for(struct tree_node * node = defvar_node->head_child;node!=NULL;node=node->next_sibling){
        printf("DEFVAR LF@%s\n",node->data->value);
    }
}

void print_body_node(struct tree_node * body){
    if(body == NULL) return;
    for(struct tree_node * child_node = body->head_child;child_node!=NULL;child_node=child_node->next_sibling){
        inbody_scan_node(child_node);
    }
}


void parameter_check_jump(int type){
//value of TYPE is in the GF@_tmp
switch (type)
{
case INT_PARAMETER:
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@int\n");
    break;
case FLOAT_PARAMETER:
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@float\n");
    break;
case STR_PARAMETER:
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@string\n");
    break;
case NULL_PARAMETER: 
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@nil\n");
    break;
case INT_NULL_PARAMETER:
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@string\n");
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@float\n");
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@bool\n");
    break;
case FLOAT_NULL_PARAMETER:
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@int\n");
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@string\n");
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@bool\n");
    break;
case STR_NULL_PARAMETER:
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@int\n");
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@float\n");
    printf("JUMPIFEQ !ERROR4 GF@_tmp string@bool\n");
    break;
default:
    break;
}
}

void print_decfunc_node(struct tree_node * node){
    char * func_name = find_child_node(node, NAME)->data->value;
    printf("JUMP !customSkip%s\n",func_name);
     
    //label samotne funkce
    printf("LABEL %s\n",func_name);

    //nový rámec pro proměnné argumentu
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    //for each argument do pop do promenne se jmenem argumentu
    
    for(struct tree_node * argument = find_child_node(node, PARAMETERS)->head_child;argument!=NULL;argument=argument->next_sibling){
        printf("DEFVAR LF@%s\n",argument->data->value);
        printf("POPS LF@%s\n",argument->data->value);
        
        printf("TYPE GF@_tmp LF@%s\n",argument->data->value);
        parameter_check_jump(argument->data->type);
    }

    //do body
    print_body_node(find_child_node(node, BODY));

    printf("MOVE GF@_result nil@nil\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("LABEL !customSkip%s\n",func_name);
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
        printf("TYPE GF@_tmp GF@_result\n"); //check for parameter type
        printf("JUMPIFNEQ !ERROR4 GF@_tmp string@string\n");
    printf("STRLEN GF@_result GF@_result\n");
}
//void ord(struct tree_node * node) is normal function
void print_chr(struct tree_node * node){
    struct tree_node * arguments_node;
    arguments_node = find_child_node(node, ARGUMENTS);
    print_expression_node(arguments_node->head_child);
        printf("TYPE GF@_tmp GF@_result\n"); // check for parameter type
        printf("JUMPIFNEQ !ERROR4 GF@_tmp string@int\n");
    printf("INT2CHAR GF@_result GF@_result\n");
}



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
        print_chr(node);
    }
    else{
        print_general_callfunc_node(node);
    }
}


void check_returned_value(struct tree_node * return_type_node){
    printf("TYPE GF@_tmp GF@_result\n");
    //int
    if(!strcmp("int",return_type_node->data->value)){
        printf("JUMPIFNEQ !ERROR4 string@int GF@_tmp\n");
    }
    //float
    else if(!strcmp("float",return_type_node->data->value)){
        printf("JUMPIFNEQ !ERROR4 string@float GF@_tmp\n");
    }
    //string
    else if(!strcmp("string",return_type_node->data->value)){
        printf("JUMPIFNEQ !ERROR4 string@string GF@_tmp\n");
    }
    //void
    else if(!strcmp("void",return_type_node->data->value)){
        printf("JUMPIFNEQ !ERROR4 string@nil GF@_tmp\n");
    }
    //?int
    else if(!strcmp("?int",return_type_node->data->value)){
        //printf("JUMPIFEQ !ERROR4 string@int GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@float GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@string GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@bool GF@_tmp\n");
    }
    else if(!strcmp("?float",return_type_node->data->value)){
        printf("JUMPIFEQ !ERROR4 string@int GF@_tmp\n");
        //printf("JUMPIFEQ !ERROR4 string@float GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@string GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@bool GF@_tmp\n");
    }
    else if(!strcmp("?string",return_type_node->data->value)){
        printf("JUMPIFEQ !ERROR4 string@int GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@float GF@_tmp\n");
        //printf("JUMPIFEQ !ERROR4 string@string GF@_tmp\n");
        printf("JUMPIFEQ !ERROR4 string@bool GF@_tmp\n");
    }


}

void print_general_callfunc_node(struct tree_node * node){
    char * func_name = find_child_node(node, NAME)->data->value;    
    //move every argumentu into stack
    pushs_arguments(find_child_node(node, ARGUMENTS)->head_child);

    //call function
    printf("CALL %s\n",func_name);
    //TODO: check for return
    struct tree_node * return_type_node = find_child_node(node,(int)TYPE);
    if(return_type_node == NULL)
        return;
    check_returned_value(return_type_node);
}


void print_first_assign_node(struct tree_node * node){
    printf("DEFVAR LF@%s\n",node->head_child->data->value);
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
    static size_t use_counter = 0;
    size_t counter = ++use_counter;
    //VYHODNOTIT EXPRESSION
    print_expression_node(node->head_child);
    //change to bool
    printf("PUSHS GF@_result\n");
    printf("CALL boolval\n");
    //JUMP conditional IF
    printf("JUMPIFNEQ !IF%ld GF@_result bool@false\n",counter);
    //JUMP ELSE
    printf("JUMP !ELSE%ld\n",counter);
    //LABEL IF
    printf("LABEL !IF%ld\n",counter);
    //IF-BODY
    print_body_node(node->head_child->next_sibling);
    //JUMP ELSEEND
    printf("JUMP !ELSEEND%ld\n",counter);
    //LABEL ELSE
    printf("LABEL !ELSE%ld\n",counter);
    //ELSE-BODY
    print_body_node(node->head_child->next_sibling->next_sibling);
    //LABEL ELSEEND
    printf("LABEL !ELSEEND%ld\n",counter);

}

void print_while_node(struct tree_node * node){
    static size_t use_counter = 0;
    use_counter++;
    size_t counter = use_counter;

    //LABEL WHILE
    printf("LABEL !WHILE%ld\n",counter);
    //EVALUEATE EXPRESSION
    print_expression_node(node->head_child);
    
    //change to bool
    printf("PUSHS GF@_result\n");
    printf("CALL boolval\n");
    //JUMP CONDITIONAL END
    printf("JUMPIFEQ !WEND%ld GF@_result bool@false\n",counter);
    //BODY
    print_body_node(node->tail_child);
    //JUMP WHILE
    printf("JUMP !WHILE%ld\n",counter);
    //LABEL END
    printf("LABEL !WEND%ld\n",counter);


    
}

void postorder(struct tree_node * node){
    if(node!=NULL){
        postorder(node->head_child);
        postorder(node->tail_child);
        choose_expr_print(node);
    }
}

void nor_string_bool(){
        printf("POPS GF@_op1\n");
        printf("POPS GF@_op2\n");
        printf("TYPE GF@_tmp GF@_op1\n");
        printf("JUMPIFEQ !ERROR7 GF@_tmp string@string\n");
        printf("JUMPIFEQ !ERROR7 GF@_tmp string@bool\n");
        printf("TYPE GF@_tmp GF@_op1\n");
        printf("JUMPIFEQ !ERROR7 GF@_tmp string@string\n");
        printf("JUMPIFEQ !ERROR7 GF@_tmp string@bool\n");
        printf("PUSHS GF@_op2\n");
        printf("PUSHS GF@_op1\n");

}

void print_operator(int type){
    if(type == PLUS_OPERATOR){
        nor_string_bool();
        printf("CALL !null2int\n");
        printf("CALL !int_to_float\n");
        printf("ADDS\n");
    }
    else if(type == MINUS_OPERATOR){
        nor_string_bool();
        printf("CALL !null2int\n");
        printf("CALL !int_to_float\n");
        printf("SUBS\n");
    }
    else if(type == MULTIPLICATION_OPERATOR){
        //check for type compatibility
        nor_string_bool();
        printf("CALL !null2int\n");
        printf("CALL !int_to_float\n");
        printf("MULS\n");
    }
    else if(type == DIVISION_OPERATOR){
        // both operands to float
        nor_string_bool();

        printf("CALL !null2int\n");
        printf("CALL !int_to_float\n");
        printf("CALL floatval\n");
        printf("MOVE GF@_op2 GF@_result\n");
        printf("CALL floatval\n");
        printf("MOVE GF@_op1 GF@_result\n");
        printf("PUSHS GF@_op1\n");
        printf("PUSHS GF@_op2\n");

        printf("DIVS\n");
    }
    else if(type == CONCATENATION_OPERATOR){
        printf("POPS GF@_op2\n");
        printf("POPS GF@_op1\n");
    //implicit conversion
        printf("PUSHS GF@_op1\n");
        printf("CALL strval\n");
        printf("MOVE GF@_op1 GF@_result \n");

        printf("PUSHS GF@_op2\n");
        printf("CALL strval\n");
        printf("MOVE GF@_op2 GF@_result \n");


        printf("CONCAT GF@_op1 GF@_op1 GF@_op2\n");
        printf("PUSHS GF@_op1\n");
    }
    else if(type == BIGGER_OPERATOR){
        printf("CALL !GTS\n");
    }
    else if(type == SMALLER_OPERATOR){
        printf("CALL !LTS\n");
    }
    else if(type == SMALLER_EQUAL_OPERATOR){
        printf("CALL !NGTS\n");
    }
    else if(type == BIGGER_EQUAL_OPERATOR){
        printf("CALL !NLTS\n");
    }
    else if(type == NOT_EQUAL_OPERATOR){
        printf("CALL !EQS\n");
        printf("NOTS\n");
    }
    else if(type == EQUAL_OPERATOR){
        printf("CALL !EQS\n");
    }
}

void choose_expr_print(struct tree_node * node){
    //operand but variable
    if(node->data->type == VAR_OPERAND){
        //print push variable
        printf("TYPE GF@_result LF@%s\n");
        printf("JUMPIFEQ !ERROR5 GF@_result string@\n");
        printf("PUSHS LF@%s\n",node->data->value);
    }
    //int
    else if(node->data->type == T_INT){
        printf("PUSHS int@%s\n",node->data->value);
    }
    //float
    else if(node->data->type == T_FLOAT){
        //print push variable
        double float_number = strtod (node->data->value, NULL);
        printf("PUSHS float@%a\n",float_number);
    }
    //string
    else if(node->data->type == T_STRING){
        //print push variable
        //printf("PUSHS string@%s\n",node->data->value); but with added escape sequences
        print_string_for_expression(node->data->value);
    }
    //null
    else if(node->data->type == T_NULL){ 
        //TODO:temporary fix
        if(!strcmp(node->data->value,"===")){
            printf("CALL !EQS\n");
        }
        else{
            printf("PUSHS nil@nil\n");
        }
    }
    //operator
    else{
        print_operator(node->data->type);
    }

}

int isokta(char * string){
    for(int i =0; i<3;i++){
        if(!('0'<=string[i] && string[i]<='7'))
            return 0;
    }
    return 1;
}


int hexa2decimal(char c){
    if(isdigit(c)) return c-'0';
    if(c>='a') return c-'a'+10;
    if(c<='Z') return c-'A'+10;
    return 0;
}

void print_string_for_expression(char * string){
    int dec_num = 0;
    printf("PUSHS string@");
    for(size_t i = 0;string[i]!='\0';i++){
        if(i == 0 || string[i+1]=='\0') {
            continue;
        }
        else if(string[i]=='\\'){
            if(isokta(string+i+1)){
                    dec_num = 0;
                    dec_num =dec_num+ (string[i+1]-'0')*8;
                    dec_num =dec_num+ (string[i+2]-'0')*8;
                    dec_num =dec_num+ (string[i+3]-'0');
                    printf("\\%03d", dec_num);
                    i=i+2;
            }
            else if(string[i+1]=='x' && isxdigit(string[i+2]) && isxdigit(string[i+3])){
                    dec_num = hexa2decimal(string[i+2])*16;
                    dec_num = dec_num + hexa2decimal(string[i+3]);
                    printf("\\%03d", dec_num);
                    i=i+2;
                }

            else {
                switch (string[i+1])
                {
                case 'n':
                    printf("\\%03d", '\n');
                    break;
                case 't':
                    printf("\\%03d", '\t');
                    break;
                case '\"':
                case '$':
                    printf("%c", string[i+1]);
                    break;
                case '\\':
                    printf("\\%03d", '\\');
                    break;
                default:
                    printf("%c", string[i]);
                    i--;
                }
            }
            i++;
            continue;
        }
        else if(string[i]<=32 || string[i]==35 || string[i] == 92){
            printf("\\%03d", string[i]);
        }
        else{
            putchar(string[i]);
        }
    }
    printf("\n");

    return;
}

void print_expression_node(struct tree_node * node){
    //if function has return without value
    if(node->data->type == EXPRESSION) {
        node = node->head_child;
    };
    if(node == NULL){
        printf("MOVE GF@_result nil@nil\n");
        return;
    }
    //postorder tree traversal
    postorder(node);

    printf("POPS GF@_result\n");

}

void print_return_node(struct tree_node * node){
    if(node->head_child == NULL){
        printf("MOVE GF@_result nil@nil\n");
    }
    else{
        print_expression_node(node->head_child);
    }

    //remove frame
    printf("POPFRAME\n");
    //return
    printf("RETURN\n");
}



void print_init_code(){
    //global variables for functional usage

    //frame for main body local variables

    printf(".IFJcode22\n");

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

    printf("TYPE GF@_result GF@_op1\n");
    printf("JUMPIFEQ !int2float_conversion GF@_result string@float\n");
    //if is same as float jump to conversion
    printf("TYPE GF@_result GF@_op2\n");
    //if is same as float jump to conversion 
    printf("JUMPIFEQ !int2float_conversion GF@_result string@float\n");
    //both are int
    printf("PUSHS GF@_op1\n");
    printf("PUSHS GF@_op2\n");
    printf("RETURN\n");

    //conversion here
    //at least one of them is float
    printf("LABEL !int2float_conversion\n");
    
    printf("TYPE GF@_result GF@_op1\n");

    printf("TYPE GF@_result GF@_op1\n");
    printf("JUMPIFEQ !int2float_skip_op1 GF@_result string@float\n");
    printf("INT2FLOAT GF@_op1 GF@_op1\n");
    printf("LABEL !int2float_skip_op1\n");

    printf("TYPE GF@_result GF@_op2\n");
    printf("JUMPIFEQ !int2float_skip_op2 GF@_result string@float\n");
    printf("INT2FLOAT GF@_op2 GF@_op2\n");
    printf("LABEL !int2float_skip_op2\n");


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


    printf("TYPE GF@_op1 GF@_op1\n");
    printf("TYPE GF@_op2 GF@_op2\n");

    printf("JUMPIFNEQ !else%s GF@_op1 GF@_op2\n",instruction);


    printf("%s\n",instruction);
    printf("RETURN\n");

    printf("LABEL !else%s\n",instruction);

    printf("POPS GF@_op2\n");
    printf("POPS GF@_op1\n");

    printf("PUSHS bool@false\n");

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


    printf("TYPE GF@_result GF@_op1\n"); //type
    printf("MOVE GF@_tmp GF@_op1\n"); //value
    printf("JUMPIFEQ !isnull%s GF@_result string@nil\n",instruction);
    printf("JUMPIFEQ !isstring%s GF@_result string@string\n",instruction);
    printf("JUMPIFEQ !isint%s GF@_result string@int\n",instruction);

    printf("TYPE GF@_result GF@_op2\n"); //type
    printf("MOVE GF@_tmp GF@_op2\n"); //value
    printf("JUMPIFEQ !isnull%s GF@_result string@nil\n",instruction);
    printf("JUMPIFEQ !isstring%s GF@_result string@string\n",instruction);
    printf("JUMPIFEQ !isint%s GF@_result string@int\n",instruction);

        printf("JUMP !skip_scpecial_condition%s\n",instruction);
        //is type null
        printf("LABEL !isnull%s\n",instruction);
            printf("JUMPIFEQ !true%s GF@_tmp nil@nil\n",instruction);
            printf("JUMP !skip_scpecial_condition%s\n",instruction);
        printf("LABEL !isstring%s\n",instruction);
            printf("JUMPIFEQ !true%s GF@_tmp string@\n",instruction);
            printf("JUMP !skip_scpecial_condition%s\n",instruction);
        printf("LABEL !isint%s\n",instruction);
            printf("JUMPIFEQ !true%s GF@_tmp int@0\n",instruction);
            printf("JUMP !skip_scpecial_condition%s\n",instruction);


        printf("LABEL !skip_scpecial_condition%s\n",instruction);
    
    
    //end of checking for null "" 0
    printf("TYPE GF@_op1 GF@_op1\n");
    printf("TYPE GF@_op2 GF@_op2\n");

    printf("JUMPIFNEQ !false%s GF@_op1 GF@_op2\n",instruction);

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

    printf("PUSHS bool@false\n");

    printf("RETURN\n");
//end
    printf("LABEL !skip%s\n",instruction);
}


void print_floatval(){
    printf("JUMP !skipfloat_val\n");
    printf("LABEL floatval\n");
    printf("POPS GF@_result\n");
    printf("TYPE GF@_tmp GF@_result\n"); // in tmp is type

    printf("JUMPIFEQ !float_val_int GF@_tmp string@int\n");
    printf("JUMPIFEQ !float_val_float GF@_tmp string@float\n");
    printf("JUMPIFEQ !float_val_string GF@_tmp string@string\n");
    printf("JUMPIFEQ !float_val_null GF@_tmp string@nil\n");
    printf("JUMPIFEQ !float_val_bool GF@_tmp string@bool\n");

    printf("LABEL !float_val_float \n");
    printf("RETURN\n");

    printf("LABEL !float_val_null \n");
    printf("INT2FLOAT GF@_result int@0\n");
    printf("RETURN\n");

    printf("LABEL !float_val_string \n");
    printf("JUMP !ERROR7\n");
    printf("RETURN\n");

    printf("LABEL !float_val_bool \n");
    printf("JUMP !ERROR7\n");
    printf("RETURN\n");


    printf("LABEL !float_val_int \n");
    printf("INT2FLOAT GF@_result GF@_result \n");
    printf("RETURN\n");

    printf("LABEL !float_val_end\n");
    printf("LABEL !skipfloat_val\n");
}

void print_boolval(){
    printf("JUMP !skipbool_val\n");
    printf("LABEL boolval\n");
    printf("POPS GF@_result\n");
    printf("TYPE GF@_tmp GF@_result\n"); // in tmp is type

    printf("JUMPIFEQ !bool_val_int GF@_tmp string@int\n");
    printf("JUMPIFEQ !bool_val_float GF@_tmp string@float\n");
    printf("JUMPIFEQ !bool_val_string GF@_tmp string@string\n");
    printf("JUMPIFEQ !bool_val_null GF@_tmp string@nil\n");
    //is bool
    printf("RETURN\n");

    printf("LABEL !bool_val_null \n");
    printf("MOVE GF@_result bool@false\n");
    printf("RETURN\n");

    printf("LABEL !bool_val_string \n");
    printf("EQ GF@_tmp GF@_result string@\n"); //is ""
    printf("EQ GF@_result GF@_result string@0\n"); // is "0"
    printf("OR GF@_result GF@_result GF@_tmp\n");
    printf("NOT GF@_result GF@_result\n");
    printf("RETURN\n");

    printf("LABEL !bool_val_int \n");
    printf("EQ GF@_result GF@_result int@0\n");
    printf("NOT GF@_result GF@_result\n");
    printf("RETURN\n");

    printf("LABEL !bool_val_float \n");
    printf("INT2FLOAT GF@_tmp int@0\n");
    printf("EQ GF@_result GF@_result GF@_tmp\n");
    printf("NOT GF@_result GF@_result\n");
    printf("RETURN\n");


    printf("LABEL !bool_val_end\n");
    printf("LABEL !skipbool_val\n");
}


void print_intval(){
    printf("JUMP !skipint_val\n");
    printf("LABEL intval\n");
    printf("POPS GF@_result\n");
    printf("TYPE GF@_tmp GF@_result\n"); // in tmp is type

    printf("JUMPIFEQ !int_val_int GF@_tmp string@int\n");
    printf("JUMPIFEQ !int_val_float GF@_tmp string@float\n");
    printf("JUMPIFEQ !int_val_string GF@_tmp string@string\n");
    printf("JUMPIFEQ !int_val_null GF@_tmp string@nil\n");
    printf("JUMPIFEQ !int_val_bool GF@_tmp string@bool\n");


    printf("LABEL !int_val_int \n");
    printf("RETURN\n");

    printf("LABEL !int_val_null \n");
    printf("MOVE GF@_result int@0\n");
    printf("RETURN\n");

    printf("LABEL !int_val_string \n");
    printf("JUMP !ERROR7\n");
    printf("RETURN\n");

    printf("LABEL !int_val_bool \n");
    printf("JUMP !ERROR7\n");
    printf("RETURN\n");


    printf("LABEL !int_val_float \n");
    printf("FLOAT2INT GF@_result GF@_result \n");
    printf("RETURN\n");

    printf("LABEL !int_val_end\n");
    printf("LABEL !skipint_val\n");
}


void print_strval(){
    printf("JUMP !skipstr_val\n");
    printf("LABEL strval\n");
    printf("POPS GF@_result\n");
    printf("TYPE GF@_tmp GF@_result\n"); // in tmp is type

    printf("JUMPIFEQ !str_val_int GF@_tmp string@int\n");
    printf("JUMPIFEQ !str_val_float GF@_tmp string@float\n");
    printf("JUMPIFEQ !str_val_string GF@_tmp string@string\n");
    printf("JUMPIFEQ !str_val_null GF@_tmp string@nil\n");
    printf("JUMPIFEQ !str_val_bool GF@_tmp string@bool\n");


    printf("LABEL !str_val_string \n");
    printf("RETURN\n");

    printf("LABEL !str_val_null \n");
    printf("MOVE GF@_result string@\n");
    printf("RETURN\n");
/* zbytek je rozsireni*/
    printf("LABEL !str_val_int\n");
    printf("JUMP !ERROR7\n");
    printf("RETURN\n");

    printf("LABEL !str_val_float\n");
    printf("JUMP !ERROR7\n"); 
    printf("RETURN\n");

    printf("LABEL !str_val_bool\n");
    printf("JUMP !ERROR7\n"); 
    printf("RETURN\n");

/**/
    printf("LABEL !str_val_end\n");
    printf("LABEL !skipstr_val\n");
}

void print_ord(){
    printf("JUMP !skipord\n");
    printf("LABEL ord\n");
    printf("POPS GF@_result\n");

    printf("TYPE GF@_tmp GF@_result\n");
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@string\n");


    printf("JUMPIFEQ !ord_empty GF@_result string@\n"); //check if string is empty
    printf("STRI2INT GF@_result GF@_result int@0\n"); //returns first character of string
    printf("RETURN\n");
    
    printf("LABEL !ord_empty\n");
    printf("MOVE GF@_result int@0\n");
    printf("RETURN\n");

    printf("LABEL !ord_end\n");
    printf("LABEL !skipord\n");


}


void print_substring(){
    printf("JUMP !skipsubstring\n");
    printf("LABEL substring\n");

    printf("CREATEFRAME\n");

    printf("DEFVAR TF@%s\n","string");
    printf("POPS TF@%s\n","string");
    printf("TYPE GF@_tmp TF@string\n");//check for type
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@string\n");


    printf("DEFVAR TF@%s\n","start");
    printf("POPS TF@%s\n","start");
    printf("TYPE GF@_tmp TF@start\n"); // check for type
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@int\n");


    printf("DEFVAR TF@%s\n","end");
    printf("POPS TF@%s\n","end");
    printf("TYPE GF@_tmp TF@end\n"); // check for type
    printf("JUMPIFNEQ !ERROR4 GF@_tmp string@int\n");


    printf("DEFVAR TF@%s\n","counter");
    printf("SUB TF@counter TF@end TF@start \n ");


    //CHECKS
    // $i < 0
    printf("LT GF@_tmp TF@start int@0 \n");
    printf("JUMPIFNEQ !substring_return_null GF@_tmp  bool@false\n");
    // $j < 0
    printf("LT GF@_tmp TF@end int@0 \n");
    printf("JUMPIFNEQ !substring_return_null GF@_tmp bool@false\n");
    //$i > $j
    printf("GT GF@_tmp TF@start TF@end \n");
    printf("JUMPIFNEQ !substring_return_null GF@_tmp bool@false\n");
    //$i > strlen
    printf("STRLEN GF@_tmp TF@string\n");
    printf("GT GF@_tmp TF@start GF@_tmp \n");
    printf("JUMPIFNEQ !substring_return_null GF@_tmp bool@false\n");
    //$i==strlen
    printf("STRLEN GF@_tmp TF@string\n");
    printf("EQ GF@_tmp TF@start GF@_tmp \n");
    printf("JUMPIFNEQ !substring_return_null GF@_tmp bool@false\n");
    //$j>strelen
    printf("STRLEN GF@_tmp TF@string\n");
    printf("GT GF@_tmp TF@end GF@_tmp \n");
    printf("JUMPIFNEQ !substring_return_null GF@_tmp bool@false\n");

    /*
    result = "";
    do{result=result . string[counter+start];}while(counter!=0);
    */
    printf("MOVE GF@_result string@\n");

    printf("JUMPIFEQ !substring_end TF@counter int@0\n"); // counter != 0

    printf("LABEL !substring_dowhile\n"); //do while
    printf("SUB TF@counter TF@counter int@1\n");
    printf("ADD GF@_tmp TF@start TF@counter\n");
    printf("GETCHAR GF@_tmp TF@string GF@_tmp\n");
    printf("CONCAT GF@_result GF@_tmp GF@_result \n");

    printf("JUMPIFNEQ !substring_dowhile TF@counter int@0\n"); // counter != 0
    printf("LABEL !substring_end\n");

    printf("RETURN\n");

    //return null
    printf("LABEL !substring_return_null\n");
    printf("MOVE GF@_result nil@nil\n");
    printf("RETURN\n");

    printf("LABEL !skipsubstring\n");



}

void definition_null2int(){
    printf("JUMP !skipnull2int\n");
    printf("LABEL !null2int\n");
    printf("POPS GF@_op1\n");
    printf("POPS GF@_op2\n");

    printf("TYPE GF@_tmp GF@_op1\n");
    printf("JUMPIFNEQ !notnull1 GF@_tmp string@nil\n"); //check if string is empty
    printf("MOVE GF@_op1 int@0\n"); //returns first character of string
    printf("LABEL !notnull1\n");

    printf("TYPE GF@_tmp GF@_op2\n");
    printf("JUMPIFNEQ !notnull2 GF@_tmp string@nil\n"); //check if string is empty
    printf("MOVE GF@_op2 int@0\n"); //returns first character of string
    printf("LABEL !notnull2\n");

    printf("PUSHS GF@_op2\n");
    printf("PUSHS GF@_op1\n");


    printf("RETURN\n");

    printf("LABEL !skipnull2int\n");


}

//TODO: UGLY FIX part1

void move_assign(struct tree_node * original_node, int count){
    original_node->data->type=ASSIGN;
    struct tree_node * iterator_node = NULL;
    for(iterator_node = original_node->parent;count!=0;iterator_node=iterator_node->parent){
        if(iterator_node->data!=NULL && iterator_node->data->type==WHILE){
            count--;
        }
    }
    //checking for duplicate defvar
    for(struct tree_node * node = iterator_node->head_child;node->data->type==FIRST_ASSIGN;node=node->next_sibling){
        if(!strcmp(node->head_child->data->value,original_node->head_child->data->value))return;
    }

    struct tree_node *new_node = init_tree_node();
    new_node->parent = iterator_node;
    add_tn_data(new_node, FIRST_ASSIGN, "");

    new_node->next_sibling=iterator_node->head_child;
    iterator_node->head_child = new_node;

    add_tree_node(new_node,NAME,original_node->head_child->data->value);
    add_tree_node(new_node,T_INT,"0");
}

void tree_traversal(struct tree_node * node, int count){
    if(node == NULL){
        return;
    }
    else{
        if(node->data!=NULL && node->data->type==FIRST_ASSIGN && count !=0){
            move_assign(node, count);
        }
        if(node->data!=NULL && node->data->type==WHILE){
            count++;
        }
        for(struct tree_node * child_node = node->head_child;child_node!=NULL;child_node=child_node->next_sibling){
            tree_traversal(child_node, count);
        }

    }
}





void code_generator(struct tree_node * node){
    //TODO: UGLY FIX part2
    tree_traversal(node,0);
    print_init_code();


    //implicit coversion INTtoFLOAT
    print_codefunc_int2float_conversion();

    //comparison
    print_EQS_LTS_GTS("EQS");
    print_EQS_LTS_GTS("LTS");
    print_EQS_LTS_GTS("GTS");
    print_NLTS_NGTS("NLTS");
    print_NLTS_NGTS("NGTS");

    print_boolval();
    print_floatval();
    print_intval();
    print_strval();
    print_substring();
    print_ord();

    definition_null2int();

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
    

    printf("LABEL !ERROR4\n");
    printf("EXIT int@4\n");

    printf("LABEL !ERROR5\n");
    printf("EXIT int@5\n");

    printf("LABEL !ERROR7\n");
    printf("EXIT int@5\n");


    //TODO:
    //potom asi to ma delat neco na konci?? asi nějaký return
}

