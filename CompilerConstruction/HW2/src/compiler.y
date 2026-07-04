/* Definition section */
%code requires {
    # define YYLTYPE_IS_DECLARED 1
    # define YYLTYPE_IS_TRIVIAL 1
}

%{
    #include "compiler_util.h"
    #include "main.h"
    #include "expression.h"
    #include "value_data.h"
    #include "scope.h"
    #include "control/for.h"
    #include "control/if.h"
    #include "control/while.h"
    #include "control/function.h"
%}

%define parse.error custom
%locations

/* Variable or self-defined structure */
%union {
    ObjectType var_type;

    bool b_var;
    ScientificNotation n_var;
    char *s_var;

    Object obj_val;
    ValueData val_data;

    FuncCallInfo* func_call;

    bool exp_left;
    ExpOp exp_op;
}
/* Token — quick start 最小集合，實作各規則時依需要自行補充 */
/* ------------------------------------ CONTROL TOKEN ------------------------------------ */
%token IF
%token ELSE_IF
%token ELSE
%token FOR
%token TIMES
%token WHILE_TRUE
%token BREAK
%token RETURN
%token RETURN_ITS
%token END


/* ------------------------------------ OPERATION TOKEN ------------------------------------ */
%token <exp_op> EXP_MATH_OP
%token <exp_op> EXP_MATH_MOD_OP
%token <exp_left> EXP_PREPOSITION
%token <exp_op> EXP_BINARY_LOGIC_OP
%token <exp_op> EXP_LOGIC_OP

%token INDEX
%token PUSH
%token LENGTH
%token THOSE
%token TAKE

%token TO_PERFORM_FUNC
%token REQUIRE_ARGS
%token FUNC_BEGIN
%token FUNC_END_FOR
%token FUNC_END
%token CALL
%token TO_CALL


/* ------------------------------------ VARIABLE TOKEN ------------------------------------ */
%token HERE_IS_A
%token HERE_ARE
%token NAME_IT
%token SAID
%token PAST
%token TOPIC
%token SET
%token <obj_val> ITS
%token IS_THUS


%token <n_var> NUMBER_LIT
%token <b_var> BOOL_LIT
%token <var_type> VAR_TYPE VAR_TYPE_FUNC


/* ------------------------------------ BLOCK TOKEN ------------------------------------ */
%token <s_var> STR_LIT IDENT


/* ------------------------------------ OTHER TOKEN ------------------------------------ */
%token PRINT
%token COMMENT



/* Nonterminal with return — 實作子規則時依需要自行補充 */
%type <obj_val> FunctionDefineStmt
%type <var_type> FunctionParamsStmt
%type <s_var> FunctionEndStmt

%type <val_data> CreateValueDataListStmt
%type <val_data> SingleValueDataListStmt
%type <val_data> MultiValueDataListStmt
%type <val_data> VariableDefineStmt

%type <obj_val> ArrayPushStmt
%type <obj_val> FunctionToCallStmt

/* %type <obj_val> ValueLiteralOrLastStmt */
%type <obj_val> ExpressionChainStmt
%type <obj_val> LogicExpressionStmt
%type <func_call> FunctionCallArgsStmt
%type <func_call> FunctionCallArgsNextStmt

%type <obj_val> ValueStmt
%type <obj_val> LitOrVarStmt
%type <obj_val> ValueLiteralStmt
%type <obj_val> VariableStmt


/* =========== prec =========== */
/* ExpressionChainStmt */
%nonassoc EXP_DONE
%left EXP_MATH_OP THOSE

%nonassoc RETURN
%nonassoc END

%left INDEX
%nonassoc LENGTH

%nonassoc EXP_LOGIC_OP


/* Yacc will start at this nonterminal */
%start Program
%%
/* Grammar section */

/* Scope */
Program
    : GlobalScopeStmt
;

GlobalScopeStmt
    : BodyListStmt
;

/* Scope Body */
BodyListStmt
    : BodyListStmt BodyStmt
    |
;

BodyStmt
    : COMMENT STR_LIT
    | OperationStmt
    | ConditionStmt
    | FunctionStmt
;

/* Function */
/* TODO: 函式定義
 * 登錄函式符號、推入 context/scope、逐一登錄參數、結束後彈出。
 * 函式：func_define, func_defineBody, func_defineBodyEnd, func_defineAddParam
 * 注意：參數型別需透過 $<var_type>0 跨規則傳遞；參數列與參數名稱各自是一層規則
 */
FunctionStmt
    : FunctionDefineStmt FunctionParamsListOrBeginStmt BodyListStmt FunctionEndStmt {
        if (func_defineBodyEnd(&$<obj_val>1, $<s_var>4)) YYABORT;
    }
;

FunctionDefineStmt
    : HERE_ARE ValueLiteralStmt VAR_TYPE_FUNC  NAME_IT IDENT {
        if (ObjectType_isInteger($<obj_val>2.type) == false) {
            yyerrorlf("預期為 %s 或 %s 而非 %s\n", &@2, 
                objectType2str[OBJECT_TYPE_I32], objectType2str[OBJECT_TYPE_I64], 
                objectType2str[$<obj_val>2.type]);
            YYABORT;
        }
        $<obj_val>$ = func_define($<obj_val>2.value.number, $<s_var>5);
    }
;

FunctionParamsListOrBeginStmt
    : FunctionParamsListStmt FUNC_BEGIN {
        func_defineBody();
    }
    | FUNC_BEGIN {
        func_defineBody();
    }
;
FunctionParamsListStmt
    : TO_PERFORM_FUNC REQUIRE_ARGS FunctionParamsStmt
    | FunctionParamsListStmt FunctionParamsStmt
;
FunctionParamsStmt
    : ValueLiteralStmt VAR_TYPE SAID IDENT {
        if (func_defineAddParam($<var_type>2, $<s_var>4)) YYABORT;
        $<var_type>$ = $<var_type>2;
    }
    | FunctionParamsStmt SAID IDENT {
        if (func_defineAddParam($<var_type>1, $<s_var>3)) YYABORT;
        $<var_type>$ = $<var_type>1;
    }
;

FunctionEndStmt
    : FUNC_END_FOR IDENT FUNC_END {
        $<s_var>$ = $<s_var>2;
    }
;

/* Condition and Operation */
/* TODO: 控制流（FOR / WHILE / IF-ELSEIF-ELSE）
 * 三種分支，每種都有對應的開始與結束 IR 呼叫。
 * 函式：code_forLoop/End, code_whileLoopStart/End, code_if, code_elseIfLabel, code_elseIf, code_else, code_ifEnd
 * 注意：else-if 與 else 皆為可選；IF 結構由三個子規則組成
 */
ConditionStmt
    : IfChainStmt
    | LoopStmt
    | BREAK {
        if (code_break(&@1)) YYABORT;
    }
;

IfChainStmt
    : IfStmt ElifStmt ElseStmt END {
        code_ifEnd();
    }
;

IfStmt
    : IF LogicExpressionStmt TOPIC {
        code_if(&$<obj_val>2);
    } BodyListStmt
;
ElifStmt
    : ElifStmt ELSE_IF {
        code_elseIfLabel();
    } LogicExpressionStmt TOPIC {
        code_elseIf(&$<obj_val>4);
    } BodyListStmt
    | /* No ELSE_IF */
;
ElseStmt
    : ELSE {
        code_else();
    } BodyListStmt
    | /* No ELSE */
;

LoopStmt
    : FOR ValueStmt TIMES {
        if (code_forLoop(&$<obj_val>2)) YYABORT;
    } BodyListStmt END {              /* for _ in range(n) */
        code_forLoopEnd(NULL);
    }
    | WHILE_TRUE {
        code_whileLoopStart();
    } BodyListStmt END {              /* while true */
        code_whileLoopEnd(NULL);
    }
;

/* TODO: 各種操作語句
 * 涵蓋變數宣告、命名、賦值、函式呼叫、陣列 push、印出、return、break。
 * 函式：object_ValueDataList*, code_createVariable, code_assign, code_stdoutPrint,
 *       code_arrayPush, code_return, code_returnValue, code_break,
 *       func_callInit, func_callArgAdd, func_call, func_takeAndCall
 * 注意：函式呼叫分前置（施）與後置（以施）兩種；mid-rule action 用 $0 傳遞中間值；
 *       呼叫結果後可接命名、return、print 或省略
 * 位置參數：code_return/code_returnValue/code_break 都多一個 tokenLoc 參數，
 *       呼叫時記得帶 RETURN/BREAK 那個 token 自己的 @N（例如 &@1），
 *       不要省略——規則 reduce 前可能已經往後看了一個 token，
 *       全域 yylloc 屆時會指到下一句而非這個 token 自己的位置
 */
OperationStmt
    : CreateValueDataListStmt {
        if (object_ValueDataListAddDefaults(&$<val_data>1, &@1)) YYABORT;
        $<val_data>$ = $<val_data>1;
    } VariableDefineStmt {                               /* Declaration */
        object_ValueDataListFree(&$<val_data>1);
    }
    | CreateValueDataListStmt PRINT {                    /* Print */
        if (code_stdoutPrint(&$<val_data>1, true)) YYABORT;
        object_ValueDataListFree(&$<val_data>1);
    }
    | PAST VariableStmt TOPIC   SET ValueStmt IS_THUS {  /* Assignment */
        if (code_assign(&$<obj_val>2, &$<obj_val>5)) YYABORT;
    }
    | ExpressionChainStmt %prec EXP_DONE                 /* Expression */
    | ExpressionChainStmt %prec EXP_DONE {
        ValueData valData;
        if (object_ValueDataListCreate(($<obj_val>1.value.symbol)->type, NULL, &valData)) YYABORT;
        if (object_ValueDataListAdd(&valData, &$<obj_val>1, &@1)) YYABORT;
        $<val_data>$ = valData;
    } VariableDefineStmt {                               /* Declaration with Expression*/
        object_ValueDataListFree(&$<val_data>3);
    }
    | ExpressionChainStmt %prec EXP_DONE {
        ValueData valData;
        if (object_ValueDataListCreate(($<obj_val>1.value.symbol)->type, NULL, &valData)) YYABORT;
        if (object_ValueDataListAdd(&valData, &$<obj_val>1, &@1)) YYABORT;
        $<val_data>$ = valData;
    } PRINT {                                            /* Print with Expression */
        if (code_stdoutPrint(&$<val_data>2, true)) YYABORT;
        object_ValueDataListFree(&$<val_data>2);
    }
    | ArrayPushStmt                                      /* Push value into Array */
    | RETURN_ITS {                                       /* Return with ITS*/
        if(code_return(&(ctx->last_result), &@1)) YYABORT;
    }
    | RETURN ValueStmt {                                 /* Return */
        if(code_return(&$<obj_val>2, &@1)) YYABORT;
    }
    | FunctionToCallStmt
    | FunctionToCallStmt {
        ValueData valData;
        if (object_ValueDataListCreate(($<obj_val>1.value.symbol)->type, NULL, &valData)) YYABORT;
        if (object_ValueDataListAdd(&valData, &$<obj_val>1, &@1)) YYABORT;
        $<val_data>$ = valData;
    } VariableDefineStmt {                               /* Declaration with FunctionToCall*/
        object_ValueDataListFree(&$<val_data>3);
    }
    | FunctionToCallStmt {
        ValueData valData;
        if (object_ValueDataListCreate(($<obj_val>1.value.symbol)->type, NULL, &valData)) YYABORT;
        if (object_ValueDataListAdd(&valData, &$<obj_val>1, &@1)) YYABORT;
        $<val_data>$ = valData;
    } PRINT {                                            /* Print with FunctionToCall */
        if (code_stdoutPrint(&$<val_data>2, true)) YYABORT;
        object_ValueDataListFree(&$<val_data>2);
    }
;

CreateValueDataListStmt
    : HERE_IS_A SingleValueDataListStmt {
        $<val_data>$ = $<val_data>2;
    }
    | HERE_ARE MultiValueDataListStmt {
        $<val_data>$ = $<val_data>2;
    }
;
SingleValueDataListStmt
    : VAR_TYPE {
        ValueData valData;
        if (object_ValueDataListCreate($<var_type>1, NULL, &valData)) YYABORT;
        $<val_data>$ = valData;
    }
    | SingleValueDataListStmt LitOrVarStmt {
        if (object_ValueDataListAdd(&$<val_data>1, &$<obj_val>2, &@2)) YYABORT;
        $<val_data>$ = $<val_data>1;
    }
;
MultiValueDataListStmt
    : NUMBER_LIT VAR_TYPE {
        ValueData valData;
        if (object_ValueDataListCreate($<var_type>2, &$<n_var>1, &valData)) YYABORT;
        $<val_data>$ = valData;
    }
    | MultiValueDataListStmt SAID LitOrVarStmt {
        if (object_ValueDataListAdd(&$<val_data>1, &$<obj_val>3, &@3)) YYABORT;
        $<val_data>$ = $<val_data>1;
    }
    | MultiValueDataListStmt SAID ExpressionChainStmt {
        if (object_ValueDataListAdd(&$<val_data>1, &$<obj_val>3, &@3)) YYABORT;
        $<val_data>$ = $<val_data>1;
    }
;

/* Name and Define Variable */
VariableDefineStmt
    : NAME_IT IDENT {
        if (code_createVariable(&$<val_data>0, $<s_var>2)) YYABORT;
        $<val_data>$ = $<val_data>0;
    }
    | VariableDefineStmt SAID IDENT {
        if (code_createVariable(&$<val_data>1, $<s_var>3)) YYABORT;
        $<val_data>$ = $<val_data>1;
    }
;

ArrayPushStmt
    : PUSH ValueStmt EXP_PREPOSITION ValueStmt {
        if ($<exp_left>3 == false) {
            yyerrorlf("「以」為填充列之字詞也\n", &@2);
            YYABORT;
        }
        if (code_arrayPush(&$<obj_val>2, &$<obj_val>4, &@4)) YYABORT;
        $<obj_val>$ = $<obj_val>2;
    }
    | ArrayPushStmt EXP_PREPOSITION ValueStmt {
        if ($<exp_left>2 == false) {
            yyerrorlf("「以」為填充列之字詞也\n", &@2);
            YYABORT;
        }
        if (code_arrayPush(&$<obj_val>1, &$<obj_val>3, &@3)) YYABORT;
        $<obj_val>$ = $<obj_val>1;
    }
;

FunctionToCallStmt
    : CreateValueDataListStmt TAKE ValueLiteralStmt TO_CALL VariableStmt {
        // type check
        if (ObjectType_isInteger($<obj_val>3.type) == false) {
            yyerrorlf("預期為 %s 或 %s 而非 %s\n", &@3, 
                objectType2str[OBJECT_TYPE_I32], objectType2str[OBJECT_TYPE_I64], 
                objectType2str[$<obj_val>3.type]);
            YYABORT;
        }

        int remainValueCount = $<val_data>1.count - sciToInt32($<obj_val>3.value.number);

        // call function
        if (func_takeAndCall($<obj_val>3.value.number, &$<obj_val>5, &$<val_data>1, &@2)) YYABORT;

        // number of return data
        if ($<val_data>1.count > remainValueCount) {
            Object* obj = object_ValueDataListPop(&$<val_data>1);
            ctx->last_result = *obj;
            free(obj);
        }
        else {
            // none
            ctx->last_result = (Object){.type = OBJECT_TYPE_UNDEFINED};
        }
        object_ValueDataListFree(&$<val_data>1);

        $<obj_val>$ = ctx->last_result;
    }
    | ExpressionChainStmt TAKE ValueLiteralStmt TO_CALL VariableStmt {
        // type check
        if (ObjectType_isInteger($<obj_val>3.type) == false) {
            yyerrorlf("預期為 %s 或 %s 而非 %s\n", &@3, 
                objectType2str[OBJECT_TYPE_I32], objectType2str[OBJECT_TYPE_I64], 
                objectType2str[$<obj_val>3.type]);
            YYABORT;
        }
        // create ValueDataList for ExpressionChainStmt output
        ValueData valData;
        if (object_ValueDataListCreate(($<obj_val>1.value.symbol)->type, NULL, &valData)) YYABORT;
        if (object_ValueDataListAdd(&valData, &$<obj_val>1, &@1)) YYABORT;

        // call function
        if (func_takeAndCall($<obj_val>3.value.number, &$<obj_val>5, &valData, &@2)) YYABORT;

        // number of return data
        if (valData.count > 0) {
            Object* obj = object_ValueDataListPop(&valData);
            ctx->last_result = *obj;
            free(obj);
        }
        else {
            // none
            ctx->last_result = (Object){.type = OBJECT_TYPE_UNDEFINED};
        }
        object_ValueDataListFree(&valData);

        $<obj_val>$ = ctx->last_result;
    }
;

/* Expressions */
/* TODO: 運算式（四則/邏輯，鏈式）
 * 函式：code_expression/Mod, code_expressionChain/Mod
 * 注意：鏈式第一項用 code_expression，後續用 code_expressionChain；需更新 ctx->last_result
 * 位置參數：aLoc 不是單純「左運算元的位置」，是「整個運算式的起點 token」，
 *       log 跟報錯都靠它定位。大部分規則第一個符號就是起點，直接傳 &@1；
 *       但如果文法是「運算子在前」（例如 乘/加/減 開頭、或 凡/兩者 開頭的二元邏輯），
 *       別把 aLoc 設成運算元的位置，要傳那個開頭運算子/關鍵字自己的 &@1，
 *       否則 verbose log 印出來的位置會偏移
 */
ExpressionChainStmt
    : ExpressionStmt {
        $<obj_val>$ = ctx->last_result;
    }
    | ExpressionChainStmt ExpressionNextStmt {
        $<obj_val>$ = ctx->last_result;
    }
;

ExpressionStmt
    : EXP_MATH_OP LitOrVarStmt EXP_PREPOSITION LitOrVarStmt {
        Object result = code_expression($<exp_op>1, $<exp_left>3, &$<obj_val>2, &$<obj_val>4, &@1, &@4);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
    }
    | EXP_MATH_OP LitOrVarStmt EXP_PREPOSITION LitOrVarStmt EXP_MATH_MOD_OP {
        Object result = code_expressionMod($<exp_op>1, $<exp_op>5, $<exp_left>3, &$<obj_val>2, &$<obj_val>4, &@1, &@4);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
    }
    | THOSE LitOrVarStmt LitOrVarStmt EXP_BINARY_LOGIC_OP {
        Object result = code_expression($<exp_op>4, true, &$<obj_val>2, &$<obj_val>3, &@1, &@3);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
    }
    | THOSE LitOrVarStmt {  /* get value */
        ctx->last_result = $<obj_val>2;
    }
    | CALL VariableStmt {
        FuncCallInfo* func_call = func_callInit(&$<obj_val>2);
        if (func_call == NULL) YYABORT;
        $<func_call>$ = func_call;
    } FunctionCallArgsStmt {
        ValueData returnData = {0};
        if ($<func_call>4->func->returnType != OBJECT_TYPE_UNDEFINED) {
            if (object_ValueDataListCreate($<func_call>4->func->returnType, NULL, &returnData)) YYABORT;
        }
        func_call($<func_call>4, &$<obj_val>2, &returnData, &@1);

        // number of return data
        if (returnData.count > 0) {
            Object* obj = object_ValueDataListPop(&returnData);
            ctx->last_result = *obj;
            free(obj);
        }
        else {
            // none
            ctx->last_result = (Object){.type = OBJECT_TYPE_UNDEFINED};
        }
        object_ValueDataListFree(&returnData);
    } 
;

ExpressionNextStmt
    : EXP_MATH_OP ValueStmt EXP_PREPOSITION ValueStmt {
        Object result = code_expressionChain($<exp_op>1, $<exp_left>3, &$<obj_val>2, &$<obj_val>4, &@1, &@4);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
    }
    | EXP_MATH_OP ValueStmt EXP_PREPOSITION ValueStmt EXP_MATH_MOD_OP {
        Object result = code_expressionChainMod($<exp_op>1, $<exp_op>5, $<exp_left>3, &$<obj_val>2, &$<obj_val>4, &@1, &@4);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
        }
    | THOSE ValueStmt ValueStmt EXP_BINARY_LOGIC_OP {
        Object result = code_expressionChain($<exp_op>4, true, &$<obj_val>2, &$<obj_val>3, &@1, &@3);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
    } CALL VariableStmt {
        FuncCallInfo* func_call = func_callInit(&$<obj_val>2);
        if (func_call == NULL) YYABORT;
        $<func_call>$ = func_call;
    } FunctionCallArgsNextStmt {
        ValueData returnData = {0};
        if ($<func_call>4->func->returnType != OBJECT_TYPE_UNDEFINED) {
            if (object_ValueDataListCreate($<func_call>4->func->returnType, NULL, &returnData)) YYABORT;
        }
        func_call($<func_call>4, &$<obj_val>2, &returnData, &@1);

        // number of return data
        if (returnData.count > 0) {
            Object* obj = object_ValueDataListPop(&returnData);
            ctx->last_result = *obj;
            free(obj);
        }
        else {
            // none
            ctx->last_result = (Object){.type = OBJECT_TYPE_UNDEFINED};
        }
        object_ValueDataListFree(&returnData);
    } 
;

LogicExpressionStmt
    : ValueStmt
    | ExpressionChainStmt
    | ValueStmt EXP_LOGIC_OP ValueStmt {
        Object result = code_expression($<exp_op>2, true, &$<obj_val>1, &$<obj_val>3, &@1, &@3);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        ctx->last_result = result;
        $<obj_val>$ = result;
    }
;

FunctionCallArgsStmt
    : FunctionCallArgsStmt EXP_PREPOSITION LitOrVarStmt {
        if (func_callArgAdd($<func_call>1, &$<obj_val>3, &@3)) YYABORT;
        $<func_call>$ = $<func_call>1;
    }
    | {
        $<func_call>$ = $<func_call>0;
    }
;
FunctionCallArgsNextStmt
    : FunctionCallArgsNextStmt EXP_PREPOSITION ValueStmt {
        if (func_callArgAdd($<func_call>1, &$<obj_val>3, &@3)) YYABORT;
        $<func_call>$ = $<func_call>1;
    }
    | {
        $<func_call>$ = $<func_call>0;
    }
;

/* Value */
/* TODO: 值、字面值、變數查找
 * 函式：object_createStr/Number/Bool, scope_findSymbol, object_getIndex, code_getLength
 * 注意：ITS 取 ctx->last_result；陣列索引與長度為 ValueStmt 的延伸形式
 */
ValueStmt
    : ValueLiteralStmt {                /* Bool, Number, String */
        $<obj_val>$ = $<obj_val>1;
    }
    | VariableStmt {                    /* Variable */
        $<obj_val>$ = $<obj_val>1;
    }
    | ITS {                             /* 其 */
        $<obj_val>$ = ctx->last_result;
    }
    | ValueStmt INDEX ValueStmt {       /* Element of Array, String */
        $<obj_val>$ = object_getIndex(&$<obj_val>1, &$<obj_val>3, &@1, &@3);
    }
    | ValueStmt LENGTH {                /* Length of Array, String */
        $<obj_val>$ = code_getLength(&$<obj_val>1, &@1);
    }
;

LitOrVarStmt
    : ValueLiteralStmt {                    /* Bool, Number, String */
        $<obj_val>$ = $<obj_val>1;
    }
    | VariableStmt {                        /* Variable */
        $<obj_val>$ = $<obj_val>1;
    }
    | LitOrVarStmt INDEX LitOrVarStmt {     /* Element of Array, String */
        Object result = object_getIndex(&$<obj_val>1, &$<obj_val>3, &@1, &@3);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        $<obj_val>$ = result;
    }
    | LitOrVarStmt LENGTH {                 /* Length of Array, String */
        Object result = code_getLength(&$<obj_val>1, &@1);
        if (result.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        $<obj_val>$ = result;
    }
;

ValueLiteralStmt
    : BOOL_LIT {
        Object obj = object_createBool($<b_var>1);
        $<obj_val>$ = obj;
    }
    | NUMBER_LIT {
        Object obj = object_createNumber(&$<n_var>1);
        if (obj.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        $<obj_val>$ = obj;
    }
    | STR_LIT {
        Object obj = object_createStr($<s_var>1);
        $<obj_val>$ = obj;
    }
;

VariableStmt
    : IDENT {
        Object obj = scope_findSymbol($<s_var>1);
        if (obj.type == OBJECT_TYPE_UNDEFINED) YYABORT;
        $<obj_val>$ = obj;
    }
;

%%

#include "compiler.h"
