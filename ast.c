#include "ast.h"
extern char *non_terminal_map[NUM_NT];
extern char *terminal_map[NUM_T]; 

ASTNode* make_node(tree_node* pt_node){

    ASTNode* temp=(ASTNode*)malloc(sizeof(ASTNode));
    temp->parent = temp->forward=temp->left_child=temp->right_child=NULL;
    temp->data.level = pt_node->level;
	temp->data.line_number = pt_node->line_number;
	temp->data.enum_value = pt_node->enum_value;
	strcpy(temp->data.str,pt_node->str);
	temp->data.is_terminal = pt_node -> is_terminal; 
    //Exceptions
    if(temp->data.enum_value == NT_optional && !(temp->data.is_terminal))
    {
        temp->data.enum_value = T_ASSIGNOP;
        temp->data.is_terminal = 1;
    }
    if(temp->data.enum_value == NT_ret && !(temp->data.is_terminal))
    {
        temp->data.enum_value = T_RETURNS;
        temp->data.is_terminal = 1;
    }
    if(temp->data.enum_value == NT_default && !(temp->data.is_terminal))
    {
        temp->data.enum_value = T_DEFAULT;
        temp->data.is_terminal = 1;
    }
    return temp;
}



ASTNode *postorder(tree_node *pTree, ASTNode *inh){
   //ANUBHAV PARTS STARTS
   if(pTree->is_terminal==0 && pTree->enum_value==NT_program){
        //program-> moduleDeclarations otherModules(1) driverModule otherModules(2) 
        tree_node *pt_moduleDeclarations=pTree->child;
        ASTNode *ast_moduleDeclarations=make_node(pt_moduleDeclarations);
        tree_node *pt_otherModules1=pt_moduleDeclarations->next;
        ASTNode *ast_otherModules1=make_node(pt_otherModules1);
        tree_node *pt_driverModule=pt_otherModules1->next;
        ASTNode* driverModule_syn=postorder(pt_driverModule,NULL);
        tree_node *pt_otherModules2=pt_driverModule->next;
        ASTNode *ast_otherModules2=make_node(pt_otherModules2);
        ast_moduleDeclarations->forward=ast_otherModules1;
        ast_otherModules1->forward=driverModule_syn;
        driverModule_syn->forward=ast_otherModules2;
        ast_moduleDeclarations->left_child=postorder(pt_moduleDeclarations,NULL);
        ast_otherModules1->left_child=postorder(pt_otherModules1,NULL);
        ast_otherModules2->left_child=postorder(pt_otherModules2,NULL);
        ASTNode* program_syn=ast_moduleDeclarations;
        return program_syn;
        
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_moduleDeclarations){
        //moduleDeclarations(1) -> moduleDeclaration moduleDeclarations(2)
        if(pTree->branch_no==0){
            tree_node *pt_moduleDeclaration=pTree->child;            
            ASTNode* moduleDeclaration_syn = postorder(pt_moduleDeclaration, NULL);
            ASTNode* moduleDeclarations2_syn = postorder(pt_moduleDeclaration->next,NULL);
            moduleDeclaration_syn->forward=moduleDeclarations2_syn;
            ASTNode* moduleDeclarations1_syn =  moduleDeclaration_syn;
            return moduleDeclarations1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_moduleDeclaration){
        tree_node* pt_Declare=pTree->child;
        tree_node* id=pt_Declare->next->next;
        ASTNode *ast_id=make_node(id);
        ASTNode* moduleDeclaration_syn=ast_id;
        return moduleDeclaration_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_otherModules){
        if(pTree->branch_no==0){
            //otherModules(1) module otherModules(2)
            tree_node *pt_module=pTree->child;
            ASTNode *module_syn=postorder(pt_module,NULL);
            ASTNode *otherModules2_syn=postorder(pt_module->next,NULL);
            module_syn->forward=otherModules2_syn;
            ASTNode *otherModules1_syn=module_syn;
            return otherModules1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }

    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_driverModule){
        //driverModule->DRIVERDEF DRIVER PROGRAM DRIVERENDDEF moduleDef 
        tree_node *pt_PROGRAM=pTree->child->next->next;
        ASTNode *ast_PROGRAM=make_node(pt_PROGRAM);
        tree_node *pt_moduleDef=pt_PROGRAM->next->next;
        ASTNode* moduleDef_syn=postorder(pt_moduleDef,NULL);
        ast_PROGRAM->left_child=moduleDef_syn;
        ASTNode *driverModule_syn=ast_PROGRAM;
        return driverModule_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_module){
        //module->DEF MODULE ID ENDDEF TAKES INPUT SQBO input_plist SQBC SEMICOL ret moduleDef
        tree_node *pt_MODULE=pTree->child->next;
        ASTNode *ast_MODULE=make_node(pt_MODULE);
        tree_node *pt_ID=pt_MODULE->next;
        ASTNode *ast_ID=make_node(pt_ID);
        ast_MODULE->left_child=ast_ID;
        tree_node *pt_input_plist=pt_ID->next->next->next->next->next;
        ASTNode *input_plist_syn=postorder(pt_input_plist,NULL);
        ast_ID->forward=input_plist_syn;
        //
        tree_node *pt_ret=pt_input_plist->next->next->next;
        ASTNode *AST_ret = make_node(pt_ret); //3
        ASTNode *ret_syn=postorder(pt_ret,NULL);
        AST_ret ->left_child = ret_syn; //4
        //
        ASTNode *moduleDef_syn=postorder(pt_ret->next,NULL);
        input_plist_syn->forward=AST_ret; //1 
        AST_ret->forward=moduleDef_syn; //2
        ASTNode *module_syn=ast_MODULE;
        return module_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_ret){
        if(pTree->branch_no==0){
            //ret->RETURNS SQBO output_plist SQBC SEMICOL
            tree_node *pt_RETURNS = pTree->child;
            tree_node *pt_output_plist=pt_RETURNS->next->next;
            ASTNode *output_plist_syn=postorder(pt_output_plist,NULL);
            ASTNode *ret_syn=output_plist_syn;
            return ret_syn;
        }
        else if(pTree->branch_no==1){
            //ret -> EPS
            return NULL;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_input_plist){
        //input_plist->ID COLON dataType N1
        ASTNode *ast_input_plist=make_node(pTree);
        tree_node *pt_ID=pTree->child;
        ASTNode *ast_ID=make_node(pt_ID);
        ast_input_plist->left_child=ast_ID;
        ASTNode *dataType_syn=postorder(pt_ID->next->next,NULL);
        ast_ID->left_child=dataType_syn;
        ASTNode *N1_syn=postorder(pt_ID->next->next->next,NULL);
        ast_ID->forward=N1_syn;
        ASTNode *input_plist_syn=ast_input_plist;
        return input_plist_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_N1){
        if(pTree->branch_no==0){
            //N1(1)->COMMA ID COLON dataType N1(2)
            tree_node *pt_ID=pTree->child->next;
            ASTNode *ast_ID=make_node(pt_ID);
            ASTNode *dataType_syn=postorder(pt_ID->next->next,NULL);
            ast_ID->left_child=dataType_syn;
            ASTNode *N1_2_syn=postorder(pt_ID->next->next->next,NULL);
            ast_ID->forward=N1_2_syn;
            ASTNode *N1_1_syn=ast_ID;
            return N1_1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }    
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_output_plist){
        //output_plist->ID COLON type N2 
        ASTNode *ast_output_plist=make_node(pTree);
        tree_node *pt_ID=pTree->child;
        ASTNode *ast_ID=make_node(pt_ID);
        ast_output_plist->left_child=ast_ID;
        ASTNode *dataType_syn=postorder(pt_ID->next->next,NULL);
        ast_ID->left_child=dataType_syn;
        ASTNode *N2_syn=postorder(pt_ID->next->next->next,NULL);
        ast_ID->forward=N2_syn;
        ASTNode *output_plist_syn=ast_output_plist;
        return output_plist_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_N2){
        if(pTree->branch_no==0){
            //N2(1)->COMMA ID COLON type N2(2)
            tree_node *pt_ID=pTree->child->next;
            ASTNode *ast_ID=make_node(pt_ID);
            ASTNode *type_syn=postorder(pt_ID->next->next,NULL);
            ast_ID->left_child=type_syn;
            ASTNode *N2_2_syn=postorder(pt_ID->next->next->next,NULL);
            ast_ID->forward=N2_2_syn;
            ASTNode *N2_1_syn=ast_ID;
            return N2_1_syn;
        }
        else if(pTree->branch_no==1){
            return NULL;
        }    
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_dataType){
        if(pTree->branch_no==0 || pTree->branch_no==1 || pTree->branch_no==2){
            //dataType->INTEGER | REAL | BOOLEAN 
            tree_node *pt_child=pTree->child;
            ASTNode *ast_child=make_node(pt_child);
            ASTNode *dataType_syn=ast_child;
            return dataType_syn;
        }
        else if(pTree->branch_no==3){
            //dataType->ARRAY SQBO range_arrays SQBC OF type
            tree_node *pt_ARRAY=pTree->child;
            ASTNode *ast_ARRAY=make_node(pt_ARRAY);
            ASTNode *range_arrays_syn=postorder(pt_ARRAY->next->next,NULL);
            ast_ARRAY->left_child=range_arrays_syn;
            tree_node *pt_type=pt_ARRAY->next->next->next->next->next;
            ASTNode *Type_syn=postorder(pt_type,NULL);
            range_arrays_syn->forward = Type_syn;
            //ast_ARRAY->right_child=Type_syn;
            ASTNode *dataType_syn=ast_ARRAY;
            return dataType_syn;
        } 
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_range_arrays){
        //range_arrays->index(1) RANGEOP index(2)
        tree_node *pt_index1=pTree->child;
        ASTNode *AST_rangeop = make_node(pt_index1->next);
        
        ASTNode *index1_syn=postorder(pt_index1,NULL);
        ASTNode *index2_syn=postorder(pt_index1->next->next,NULL);
        AST_rangeop->left_child = index1_syn;
        index1_syn->forward=index2_syn;
        ASTNode *range_arrays_syn=AST_rangeop;
        return range_arrays_syn;

    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_type){
        //type->INTEGER | REAL | BOOLEAN 
        tree_node *type_child=pTree->child;
        ASTNode *ast_type_child=make_node(type_child);
        ASTNode *type_syn=ast_type_child;
        return type_syn; 

    }
    
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_moduleDef){
        //moduleDef->START statements END
        tree_node *pt_statements=pTree->child->next;
        ASTNode * ast_statements=make_node(pt_statements);
        ast_statements->left_child=postorder(pt_statements,NULL);
        ASTNode *moduleDef_syn=ast_statements;
        return moduleDef_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_statements){
        if(pTree->branch_no==0){
            //statements(1)->statement statements(2)
            tree_node *pt_statement=pTree->child;            
            ASTNode* statement_syn = postorder(pt_statement, NULL);
            ASTNode* statements2_syn = postorder(pt_statement->next,NULL);
            statement_syn->forward=statements2_syn;
            ASTNode* statements1_syn = statement_syn;
            return statements1_syn;

        }
        else if(pTree->branch_no==1){
            return NULL;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_statement){
        //statement ioStmt | simpleStmt | declareStmt | conditionalStmt | iterativeStmt
        tree_node *pt_stat_child=pTree->child;
        ASTNode *stat_child_syn=postorder(pt_stat_child,NULL);
        ASTNode *statement_syn=stat_child_syn;
        return statement_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_ioStmt){
        
        if(pTree->branch_no==0){
            //ioStmt->GET_VALUE BO ID BC SEMICOL
            tree_node *pt_GET_VALUE=pTree->child;
            ASTNode *ast_GET_VALUE=make_node(pt_GET_VALUE);
            tree_node *pt_ID=pt_GET_VALUE->next->next;
            ASTNode *ast_ID=make_node(pt_ID);
            ast_GET_VALUE->left_child=ast_ID;
            ASTNode *ioStmt_syn=ast_GET_VALUE;
            return ioStmt_syn;
        }
        else if(pTree->branch_no==1){
            //ioStmt->PRINT BO var BC SEMICOL
            tree_node *pt_PRINT=pTree->child;
            ASTNode *ast_PRINT=make_node(pt_PRINT);
            tree_node *pt_var=pt_PRINT->next->next;
            ASTNode *var_syn=postorder(pt_var,NULL);
            ast_PRINT->left_child=var_syn;
            ASTNode *ioStmt_syn= ast_PRINT;
            return ioStmt_syn;
        }
        
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_var){
        //var->var_id_num | boolconstt
        tree_node *pt_var_child=pTree->child;
        ASTNode *var_child_syn=postorder(pt_var_child,NULL);
        ASTNode *var_syn=var_child_syn;
        return var_syn; 
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_var_id_num){
        if(pTree->branch_no==0){
            //var_id_num->ID whichID 
            tree_node *pt_ID=pTree->child;
            ASTNode *ast_ID=make_node(pt_ID);
            ASTNode *whichID_syn=postorder(pt_ID->next,NULL);
            ast_ID->forward=whichID_syn;
            ASTNode *var_id_num_syn=ast_ID;
            return var_id_num_syn;
        }
        else if(pTree->branch_no==1 || pTree->branch_no==2){
            //var_id_num->NUM | RNUM
            tree_node *pt_child=pTree->child;
            ASTNode *ast_child=make_node(pt_child);
            ASTNode *var_id_num_syn=ast_child;
            return var_id_num_syn;
        }
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_boolconstt){
        //boolconstt->TRUE|FALSE
        //no need of branching in this case;
        tree_node *bool_child=pTree->child;
        ASTNode *ast_bool_child=make_node(bool_child);
        ASTNode *boolconstt_syn=ast_bool_child;
        return boolconstt_syn;
    }
    else if(pTree->is_terminal==0 && pTree->enum_value==NT_whichID){
        if(pTree->branch_no==0){
            //whichID->SQBO index SQBC
            tree_node *pt_index=pTree->child->next;
            ASTNode *index_syn=postorder(pt_index,NULL);
            ASTNode *whichID_syn=index_syn;
            return whichID_syn;
        }
        else if(pTree->branch_no==1){
            //whichID->EPS
            /*ASTNode *whichID_inh=inh;
            ASTNode *whichID_syn=whichID_inh;
            return whichID_syn;*/
            return NULL;
        }    
    }
   //ANUBHAV PART ENDS

   //ASHISH PART-1 STARTS
   if(pTree->is_terminal == 0 && pTree->enum_value == NT_simpleStmt && pTree->branch_no == 0 )
    {
        //simpleStmt -> assignmentStmt
        ASTNode *AST_assignmentStmt , *simpleStmt_syn;
        AST_assignmentStmt = make_node(pTree->child);
        AST_assignmentStmt->left_child = postorder(pTree->child,NULL);
        simpleStmt_syn = AST_assignmentStmt;
        return simpleStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_simpleStmt && pTree->branch_no == 1 )
    {
        //simpleStmt->moduleReuseStmt
        ASTNode *AST_moduleReuseStmt , *simpleStmt_syn;
        AST_moduleReuseStmt = make_node(pTree->child);
        AST_moduleReuseStmt->left_child = postorder(pTree->child,NULL);
        simpleStmt_syn = AST_moduleReuseStmt;
        return simpleStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_assignmentStmt && pTree->branch_no == 0 )
    {
        //assignmentStmt -> ID whichStmt
        ASTNode *AST_ID = make_node(pTree->child);
        ASTNode *whichStmt_inh = AST_ID;
        ASTNode *whichStmt_syn =  postorder(pTree->child->next,whichStmt_inh);
        ASTNode *assignmentStmt_syn = whichStmt_syn;
        return assignmentStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_whichStmt && pTree->branch_no == 0 )
    {
        //whichStmt -> lvalueIDStmt
        ASTNode *lvalueIDStmt_inh = inh;
        ASTNode *whichStmt_syn = postorder(pTree->child,lvalueIDStmt_inh);
        return whichStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_whichStmt && pTree->branch_no == 1)
    {
        //whichStmt -> lvalueARRStmt
        ASTNode *lvalueARRStmt_inh = inh;
        ASTNode *whichStmt_syn = postorder(pTree->child,lvalueARRStmt_inh);
        return whichStmt_syn;
    }
    
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_lvalueIDStmt && pTree->branch_no == 0)
    {
        //lvalueIDStmt -> ASSIGNOP expression SEMICOL
        ASTNode *AST_ASSIGNOP = make_node(pTree->child);
        tree_node *PT_expression = pTree->child->next;
        AST_ASSIGNOP -> left_child = inh;
        AST_ASSIGNOP -> right_child = postorder(PT_expression,NULL);
        return AST_ASSIGNOP;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_lvalueARRStmt && pTree->branch_no == 0)
    {
        //lvalueARRStmt-> SQBO index SQBC ASSIGNOP expression SEMICOL
        ASTNode *index_syn = postorder(pTree->child->next,NULL);
        tree_node *PT_ASSIGNOP = pTree -> child ->next -> next-> next;
        ASTNode *AST_ASSIGNOP = make_node(PT_ASSIGNOP);
        tree_node *PT_expression = PT_ASSIGNOP->next;
        AST_ASSIGNOP -> left_child = inh;
        inh -> forward = index_syn;
        AST_ASSIGNOP -> right_child = postorder(PT_expression,NULL);
        return AST_ASSIGNOP;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_index && pTree->branch_no == 0)
    {
        //index->NUM
        ASTNode * AST_NUM = make_node(pTree -> child);
        return AST_NUM;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_index && pTree->branch_no == 1)
    {
        //index->ID
        ASTNode * AST_ID = make_node(pTree -> child);
        return AST_ID;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_moduleReuseStmt && pTree->branch_no == 0)
    {
        //moduleReuseStmt -> optional USE MODULE ID WITH PARAMETERS idList SEMICOL
        ASTNode *AST_Optional = make_node(pTree->child);
        ASTNode * optional_syn = postorder(pTree->child, NULL);
        tree_node * PT_ID = pTree->child->next->next->next;
        ASTNode *AST_ID = make_node(PT_ID);
        AST_Optional->left_child = optional_syn;
        AST_Optional->right_child = AST_ID;
        tree_node *PT_idList = PT_ID->next->next->next;
        AST_ID->forward = postorder(PT_idList,NULL);
        ASTNode *moduleReuseStmt_syn = AST_Optional;
        return moduleReuseStmt_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_optional && pTree->branch_no == 0)
    {
        //optional -> SQBO idList SQBC ASSIGNOP
        ASTNode * optional_syn = postorder(pTree->child->next,NULL);
        return optional_syn; 
    }
    
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_optional && pTree->branch_no == 1)
    {
        //optional -> EPS
        ASTNode * optional_syn = NULL;
        return optional_syn;
        
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_idList && pTree->branch_no == 0)
    {
        //idList -> ID N3
        ASTNode * AST_idList = make_node(pTree);
        ASTNode * AST_ID = make_node(pTree->child);
        ASTNode * N3_inh = AST_ID;
        AST_idList->left_child = postorder(pTree->child->next,N3_inh);
        return AST_idList;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N3 && pTree->branch_no == 0)
    {
        //N3(1) -> COMMA ID N3(2)
        tree_node *PT_ID = pTree->child->next;
        tree_node *PT_N3 = PT_ID->next;
        ASTNode * AST_ID = make_node(PT_ID);
        ASTNode *N3_2_inh = AST_ID;
        ASTNode *N3_1_inh = inh;
        N3_1_inh -> forward = postorder(PT_N3,N3_2_inh);
        return N3_1_inh;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N3 && pTree->branch_no == 1)
    {
        //N3->EPS
        return inh;
    }
   //ASHISH PART-1 ENDS

    //ANIRUDH PART STARTS    
    //1. Expression -> arithmeticOrBooleanExpr
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_expression){
        if(pTree->branch_no == 0){
            //Expression.syn = arithmeticOrBooleanExpr.syn
            tree_node *PT_arithmeticOrBooleanExpr = pTree->child;
            ASTNode *Expression_syn = postorder(PT_arithmeticOrBooleanExpr, NULL);
            return Expression_syn;

        }
        else if(pTree->branch_no == 1){
            //Expression.syn = u1.syn
            tree_node *PT_u1 = pTree->child;
            ASTNode *Expression_syn = postorder(PT_u1, NULL);
            return Expression_syn;

        }
    }

    //2. u1 -> unary_op new_NT
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_u1){
        if(pTree->branch_no == 0){
            // new_NT.inh=uniary_op.syn
            // U1.syn = new_NT.syn

            tree_node *PT_unary_op = pTree->child;
            ASTNode *unary_op_syn = postorder(PT_unary_op, NULL);
            tree_node *PT_new_NT = PT_unary_op->next;
            //new_NT will inherit AST_unary_op
            ASTNode *new_NT_syn = postorder(PT_new_NT, unary_op_syn);
            ASTNode *u1_syn = new_NT_syn;
            return u1_syn;
        }
    }

    //3. new_NT -> BO arithmeticExpr BC | var_id_num
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_new_NT){
        if(pTree->branch_no == 0){
            //new_NT.inh->left_child = arithmeticExpr.syn
            //new_NT.syn = new_NT.inh
            tree_node *PT_arithmeticExpr = pTree->child->next;
            ASTNode *arithmeticExpr_syn = postorder(PT_arithmeticExpr, NULL);
            //here I inherit the unary op
            ASTNode *New_NT_inh = inh;
            New_NT_inh->left_child = arithmeticExpr_syn;
            ASTNode *New_NT_syn = New_NT_inh;
            return New_NT_syn;
        }
        else if(pTree->branch_no == 1){
            //new_NT.inh->left_child = var_id_num.syn
            //new_NT.syn = new_NT.inh
            tree_node *PT_var_id_num = pTree->child;
            ASTNode *var_id_syn = postorder(PT_var_id_num, NULL);
            //I inherit unary_op
            ASTNode *New_NT_inh = inh;
            New_NT_inh->left_child = var_id_syn;
            ASTNode *New_NT_syn = New_NT_inh;
            return New_NT_syn;

        }
    }

    //4. unary_op -> PLUS | MINUS
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_unary_op){
        if(pTree->branch_no == 0){
            // node=make_node(‘PLUS’,type=’UniaryOp’)
            // Unary_op.syn=node
            tree_node *PT_plus = pTree->child;
            ASTNode *AST_plus = make_node(PT_plus);
            ASTNode *Unary_op_syn = AST_plus;
            return Unary_op_syn;

        }
        else if(pTree->branch_no == 1){
            //node=make_node(‘MINUS’,type=’UniaryOp’)
            //Unary_op.syn=node
            tree_node *PT_minus = pTree->child;
            ASTNode *AST_minus = make_node(PT_minus);
            ASTNode *Unary_op_syn = AST_minus;
            return Unary_op_syn;
        }
    }

    //5. arithmeticOrBooleanExpr AnyTerm N7 ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_arithmeticOrBooleanExpr){
        if(pTree->branch_no == 0){
            //N7.inh = AnyTerm.syn
            //arithmeticOrBooleanExpr.syn = N7.syn
            tree_node *PT_anyterm = pTree->child;
            tree_node *PT_n7 = PT_anyterm->next;
            ASTNode *Anyterm_syn = postorder(PT_anyterm, NULL);
            ASTNode *N7_syn = postorder(PT_n7, Anyterm_syn);
            ASTNode *ArithmeticOrBooleanExpr_syn = N7_syn;
            return ArithmeticOrBooleanExpr_syn;
        }
    }

    //6. N7 logicalOp AnyTerm N7 | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N7){
        if(pTree->branch_no == 0){
            // N7(2).inh = AnyTerm.syn
            // logicalOp.syn-> left_child = N7(1).inh
            // logicalOp.syn -> right_child = N7(2).syn
            // N7(1).syn = logicalOp.syn
            tree_node *PT_logical_op = pTree->child;
            tree_node *PT_anyterm = PT_logical_op->next;
            tree_node *PT_n7_2 = PT_anyterm->next;

            ASTNode *Logical_op_syn = postorder(PT_logical_op, NULL);
            ASTNode *Anyterm_syn = postorder(PT_anyterm, NULL);
            ASTNode *N7_2_syn = postorder(PT_anyterm, Anyterm_syn);

            //filling left and right operands
            ASTNode *N7_1_inh = inh;
            Logical_op_syn->left_child = N7_1_inh;
            Logical_op_syn->right_child = N7_2_syn;

            ASTNode *N7_1_syn = Logical_op_syn;
            return N7_1_syn;
        }
        else if(pTree->branch_no == 1){
            //N7.syn = N7.inh
            ASTNode *N7_inh = inh;
            ASTNode *N7_syn = N7_inh;
            return N7_syn;

        }
    }

    //7. AnyTerm arithmeticExpr N8 | boolconstt ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_AnyTerm){
        if(pTree->branch_no == 0){
            //N8.inh = arithmeticExpr.syn
            //AnyTerm.syn = N8.syn
            tree_node *PT_arithmeticExpr = pTree->child;
            tree_node *PT_n8 = PT_arithmeticExpr->next;

            ASTNode *ArithmeticExpr_syn = postorder(PT_arithmeticExpr, NULL);
            ASTNode *N8_syn = postorder(PT_n8, ArithmeticExpr_syn);

            ASTNode *AnyTerm_syn = N8_syn;
            return AnyTerm_syn;

    
        }
        else if(pTree->branch_no == 1){
            //AnyTerm.syn = boolconstt.syn
            tree_node *PT_boolconstt = pTree->child;
            ASTNode *Boolconstt_syn = postorder(PT_boolconstt, NULL);
            
            ASTNode *Anyterm_syn = Boolconstt_syn;
            return Anyterm_syn;
        }
    }

    //8. N8 relationalOp arithmeticExpr | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N8){
        if(pTree->branch_no == 0){
            //relationalOp.syn->left_child = N8.inh
            //relationalOp.syn->right_child = arithmeticExpr.syn
            //N8.syn = realtionalOp.syn
            tree_node *PT_relationalOp = pTree->child;
            tree_node *PT_arithmeticExpr = PT_relationalOp->next;
            
            ASTNode *Relational_op_syn = postorder(PT_relationalOp, NULL);
            ASTNode *ArithmeticExpr_syn = postorder(PT_arithmeticExpr, NULL);
            //inherited left operand
            ASTNode *N8_inh = inh;
            //filling operand
            Relational_op_syn->left_child = N8_inh;
            Relational_op_syn->right_child = ArithmeticExpr_syn;

            ASTNode *N8_syn = Relational_op_syn;
            return N8_syn;
        }
        else if(pTree->branch_no == 1){
            //N8.syn = N8.inh
            ASTNode *N8_inh = inh;
            ASTNode *N8_syn = N8_inh;
            return N8_syn;
        }
    }

    //9. arithmeticExpr term N4 ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_arithmeticExpr){
        if(pTree->branch_no == 0){
            //N4.inh = term.syn
            //arithmeticExpr.syn = N4.syn
            tree_node *PT_term = pTree->child;
            tree_node *PT_n4 = PT_term->next;

            ASTNode *Term_syn = postorder(PT_term, NULL);
            ASTNode *N4_syn = postorder(PT_n4, Term_syn);

            ASTNode *ArithmeticExpr_syn = N4_syn;
            return ArithmeticExpr_syn;
        }
    }

    //10. N4 op1 term N4 | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N4){
        if(pTree->branch_no == 0){
            // N4(2).inh = term.syn
            // op1.syn->left_child = N4(1).inh
            // op1.syn->right_child = N4(2).syn
            // N4(1).syn = op1.syn
            tree_node *PT_op1 = pTree->child;
            tree_node *PT_term = PT_op1->next;
            tree_node *PT_n4_2 = PT_term->next;

            ASTNode *Op1_syn = postorder(PT_op1, NULL);
            ASTNode *Term_syn = postorder(PT_term, NULL);
            ASTNode *N4_2_syn = postorder(PT_n4_2, Term_syn);

            //left operand inherit
            ASTNode *N4_1_inh = inh;
            //filling operands
            Op1_syn->left_child = N4_1_inh;
            Op1_syn->right_child = N4_2_syn;

            ASTNode *N4_1_syn = Op1_syn;
            return N4_1_syn;
        }
        else if(pTree->branch_no == 1){
            //N4.syn = N4.inh
            ASTNode *N4_inh = inh;
            ASTNode *N4_syn = N4_inh;
            return N4_syn;

        }
    }

    //11. op1 PLUS | MINUS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_op1){
        if(pTree->branch_no == 0){
            //temp = make_node(PLUS)
            //op1.syn = temp
            tree_node *PT_plus = pTree->child;
            ASTNode *AST_plus = make_node(PT_plus);
            ASTNode *Op1_syn = AST_plus;
            return Op1_syn;



        }
        else if(pTree->branch_no == 1){
            //temp = make_node(MINUS)
            //op1.syn = temp
            tree_node *PT_minus = pTree->child;
            ASTNode *AST_minus = make_node(PT_minus);
            ASTNode *Op1_syn = AST_minus;
            return Op1_syn;



        }
    }

    //12. term factor N5 ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_term){
        if(pTree->branch_no == 0){
            //N5.inh = factor.syn
            //term.syn = N5.syn
            tree_node *PT_factor = pTree->child;
            tree_node *PT_n5 = PT_factor->next;

            ASTNode *Factor_syn = postorder(PT_factor, NULL);
            ASTNode *N5_syn = postorder(PT_n5, Factor_syn);

            ASTNode *Term_syn = N5_syn;
            return Term_syn;
        }
    }

    //13. N5 op2 factor N5 | EPS ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N5){
        if(pTree->branch_no == 0){
            // N5(2).inh = factor.syn
            // op2.syn->left_child = N5(1).inh
            // op2.syn->right_child = N5(2).syn
            // N5(1).syn = op2.syn
            tree_node *PT_op2 = pTree->child;
            tree_node *PT_factor = PT_op2->next;
            tree_node *PT_N5_2 = PT_factor->next;

            ASTNode *Op2_syn = postorder(PT_op2, NULL);
            ASTNode *Factor_syn = postorder(PT_factor, NULL);
            ASTNode *N5_2_syn = postorder(PT_N5_2, Factor_syn);

            //getting left operand
            ASTNode *N5_1_inh = inh;
            Op2_syn->left_child = N5_1_inh;
            Op2_syn->right_child = N5_2_syn;

            ASTNode *N5_1_syn = Op2_syn;
            return N5_1_syn;     
        }
        else if(pTree->branch_no == 1){
            //N5.syn = N5.inh
            ASTNode *N5_inh = inh;
            ASTNode *N5_syn = N5_inh;
            return N5_syn;
        }
    }

    //14. op2 -> MUL|DIV
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_op2){
        if(pTree->branch_no == 0){
            // temp = make_node(MUL)
            // Op2.syn = temp

            tree_node *PT_mul = pTree->child;
            ASTNode *AST_mul = make_node(PT_mul);
            ASTNode *Op2_syn = AST_mul;
            return Op2_syn;

        }
        else if(pTree->branch_no == 1){
            // temp = make_node(DIV)
            // Op2.syn = temp

            tree_node *PT_div = pTree->child;
            ASTNode *AST_div = make_node(PT_div);
            ASTNode *Op2_syn = AST_div;
            return Op2_syn;

        }
    }

    //15. factor -> BO arithmeticOrBooleanExpr BC | var_id_num
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_factor){
        if(pTree->branch_no == 0){
            //factor.syn = arithmeticOrBooleanExpr.syn
            tree_node *PT_arithmeticOrBooleanExpr = pTree->child->next;
            ASTNode *ArithmeticOrBooleanExpr_syn = postorder(PT_arithmeticOrBooleanExpr, NULL);
            ASTNode *Factor_syn = ArithmeticOrBooleanExpr_syn;
            return Factor_syn;

        }
        else if(pTree->branch_no == 1){
            //factor.syn = var_id_num.syn
            tree_node *PT_var_id_num = pTree->child;
            ASTNode *Var_id_num_syn = postorder(PT_var_id_num, NULL);
            ASTNode *Factor_syn = Var_id_num_syn;
            return Factor_syn;
        }
    }

    //16. logicalOp AND | OR ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_logicalOp){
        if(pTree->branch_no == 0){
            //temp = make_node(AND)
            //logicalOp.syn = temp
            tree_node *PT_and = pTree->child;
            ASTNode *AST_and = make_node(PT_and);
            ASTNode *Logicalop_syn = AST_and;
            return Logicalop_syn;

        }
        else if(pTree->branch_no == 1){
            //temp = make_node(AND)
            //logicalOp.syn = temp
            tree_node *PT_or = pTree->child;
            ASTNode *AST_or = make_node(PT_or);
            ASTNode *Logicalop_syn = AST_or;
            return Logicalop_syn;
        }

    }

    //17. relationalOp LT | LE | GT | GE | EQ | NE ?
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_relationalOp){
        //temp = make_node(‘the relop’)
        //relationalOp.syn = temp

        if(pTree->branch_no == 0){
            tree_node *PT_lt = pTree->child;
            ASTNode *AST_lt = make_node(PT_lt);
            ASTNode *Relationalop_syn = AST_lt;
            return Relationalop_syn;
        }
        else if(pTree->branch_no == 1){
            tree_node *PT_le = pTree->child;
            ASTNode *AST_le = make_node(PT_le);
            ASTNode *Relationalop_syn = AST_le;
            return Relationalop_syn;

        }
        else if(pTree->branch_no == 2){
            tree_node *PT_gt = pTree->child;
            ASTNode *AST_gt = make_node(PT_gt);
            ASTNode *Relationalop_syn = AST_gt;
            return Relationalop_syn;
            
        }
        else if(pTree->branch_no == 3){
            tree_node *PT_ge = pTree->child;
            ASTNode *AST_ge = make_node(PT_ge);
            ASTNode *Relationalop_syn = AST_ge;
            return Relationalop_syn;
            
        }
        else if(pTree->branch_no == 4){
            tree_node *PT_eq = pTree->child;
            ASTNode *AST_eq = make_node(PT_eq);
            ASTNode *Relationalop_syn = AST_eq;
            return Relationalop_syn;
            
        }
        else if(pTree->branch_no == 5){
            tree_node *PT_ne = pTree->child;
            ASTNode *AST_ne = make_node(PT_ne);
            ASTNode *Relationalop_syn = AST_ne;
            return Relationalop_syn;
            
        }

    }
    //ANIRUDH PART ENDS    

    //ASHISH PART -2 STARTS
     if(pTree->is_terminal == 0 && pTree->enum_value == NT_declareStmt && pTree->branch_no == 0)
    {
        //declareStmt -> DECLARE idList COLON dataType SEMICOL
        ASTNode *AST_Declare = make_node(pTree->child);
        tree_node * PT_idList = pTree->child->next;
        tree_node * PT_dataType = PT_idList->next->next;
        ASTNode *idList_syn = postorder(PT_idList,NULL);
        AST_Declare->left_child = idList_syn;
        idList_syn->forward = postorder(PT_dataType,NULL);
        return AST_Declare;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_conditionalStmt && pTree->branch_no == 0)
    {
        //conditionalStmt -> SWITCH BO ID BC START caseStmts default END
        ASTNode *AST_SWITCH = make_node(pTree->child);
        tree_node * PT_ID = pTree->child->next->next;
        tree_node * PT_caseStmts = PT_ID->next->next->next;
        tree_node * PT_default = PT_caseStmts->next;
        ASTNode *AST_ID = make_node(PT_ID);
        ASTNode *caseStmts_syn = postorder(PT_caseStmts,NULL);
        ASTNode *temp = caseStmts_syn;
        ASTNode *AST_default = make_node(PT_default);//passing a non terminal
        ASTNode *default_syn = postorder(PT_default,NULL); 
        AST_default ->left_child = default_syn;
        AST_SWITCH->left_child = AST_ID;
        AST_ID -> forward = caseStmts_syn;
        while(temp->forward!=NULL)
        {
            temp= temp->forward;
        }
        temp->forward = AST_default;

        return AST_SWITCH;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_caseStmts && pTree->branch_no == 0)
    {
        //caseStmts -> CASE value COLON statements BREAK SEMICOL N9
        tree_node * PT_value = pTree->child->next;
        tree_node * PT_statements = PT_value -> next -> next;
        tree_node * PT_N9 = PT_statements ->next->next->next;
        ASTNode * value_syn = postorder(PT_value,NULL);
        ASTNode * statements_syn = postorder(PT_statements,NULL);
        value_syn->left_child = statements_syn;
        ASTNode * N9_syn = postorder(PT_N9,value_syn);
        return N9_syn;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N9 && pTree->branch_no == 0)
    {
        //N9(1) ->CASE value COLON statements BREAK SEMICOL N9(2)
        tree_node * PT_value = pTree->child->next;
        tree_node * PT_statements = PT_value -> next -> next;
        tree_node * PT_N9 = PT_statements ->next->next->next;
        ASTNode * value_syn = postorder(PT_value,NULL);
        ASTNode * statements_syn = postorder(PT_statements,NULL);
        value_syn->left_child = statements_syn;
        ASTNode * N9_1_inh = inh;
        ASTNode * N9_2_syn = postorder(PT_N9,value_syn);
        N9_1_inh ->forward = N9_2_syn;  
        return N9_1_inh;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_N9 && pTree->branch_no == 1)
    {
        //N9 ->EPS
        return inh;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_value && pTree->branch_no == 0)
    {
        //value -> NUM
        return make_node(pTree->child);
    }
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_value && pTree->branch_no == 1)
    {
        //value -> TRUE
        return make_node(pTree->child);
    }
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_value && pTree->branch_no == 2)
    {
        //value -> FALSE
        return make_node(pTree->child);
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_default && pTree->branch_no == 0)
    {
        //default -> DEFAULT COLON statements BREAK SEMICOL
        tree_node *PT_statements = pTree->child->next->next;
        ASTNode *AST_statements = postorder(PT_statements,NULL);
        return AST_statements;
    }
    
    if(pTree->is_terminal == 0 && pTree->enum_value == NT_default && pTree->branch_no == 1)
    {
        return NULL; // in case of DEFAULT it may be NULL as well.
        //Need to create a Default node in parent is a solution as in ModuleDeclarations or is this fine ??? Change appropriately.
    }


    if(pTree->is_terminal == 0 && pTree->enum_value == NT_iterativeStmt && pTree->branch_no == 0)
    {
        //iterativeStmt -> FOR BO ID IN range BC START statements END
        ASTNode *AST_FOR = make_node(pTree->child);
        tree_node *PT_ID = pTree->child->next->next;
        tree_node *PT_range = PT_ID ->next->next;
        tree_node *PT_statements = PT_range ->next->next->next;
        ASTNode * AST_ID = make_node(PT_ID);
        ASTNode * AST_range = postorder(PT_range,NULL);
        ASTNode * AST_statements = postorder(PT_statements,NULL);
        AST_FOR -> left_child = AST_ID;
        AST_ID->forward = AST_range;
        AST_range -> forward = AST_statements;
        return AST_FOR;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_iterativeStmt && pTree->branch_no == 1)
    {
        //iterativeStmt -> WHILE BO arithmeticOrBooleanExpr BC START statements END
        ASTNode *AST_WHILE = make_node(pTree->child);
        tree_node *PT_AOB = pTree->child->next->next;
        tree_node *PT_statements = PT_AOB ->next->next->next;
        ASTNode * AST_AOB = postorder(PT_AOB,NULL);
        ASTNode * AST_statements = postorder(PT_statements,NULL);
        AST_WHILE -> left_child = AST_AOB;
        AST_AOB -> forward = AST_statements;
        return AST_WHILE;
    }

    if(pTree->is_terminal == 0 && pTree->enum_value == NT_range && pTree->branch_no == 0)
    {
        //range -> NUM(1) RANGEOP NUM(2)
        ASTNode *AST_NUM1 = make_node(pTree->child);
        ASTNode *AST_RANGEOP = make_node(pTree->child->next);
        ASTNode *AST_NUM2 = make_node(pTree->child->next->next);
        AST_RANGEOP->left_child = AST_NUM1;
        AST_NUM1->forward = AST_NUM2;
        return AST_RANGEOP;
        //See yourself why RangeOP is needed and there cant be a forward pointer b/w NUM1 and NUM2 w/o RangeOp as was discussed, otherwise ask.
    }
    // ASHISH PART -2 ENDS
    return NULL;
}

void fill_ast_parent(ASTNode *current){
    if(current == NULL)
        return;
    
    if(current->right_child == NULL){
        if(current->left_child != NULL){
            current->left_child->parent = current;
        }
        fill_ast_parent(current->left_child);

        if(current->forward != NULL){
            current->forward->parent = current->parent;
        }
        fill_ast_parent(current->forward);
    }

    else{
        if(current->left_child != NULL){
            current->left_child->parent = current;
        }
        fill_ast_parent(current->left_child);
        
        if(current->right_child != NULL){
            current->right_child->parent = current;
        }
        fill_ast_parent(current->right_child);

        if(current->forward != NULL){
            current->forward->parent = current->parent;
        }
        fill_ast_parent(current->forward);
    }
}


void printTree(ASTNode *current){
    if(current == NULL){
        return;
    }
    
    if(current->right_child == NULL){
        if(current->data.is_terminal == 1){
            printf(" %s \n",terminal_map[current->data.enum_value] );
        }
        else{
            printf(" %s \n",non_terminal_map[current->data.enum_value] );
        }

        printTree(current->left_child);
        printTree(current->forward);
    }

    else{
        printTree(current->left_child);
        
        if(current->data.is_terminal == 1){
            printf(" %s \n",terminal_map[current->data.enum_value] );
        }
        else{
            printf(" %s \n",non_terminal_map[current->data.enum_value]);
        }

        printTree(current->right_child);
        printTree(current->forward);
    }
}