//
// Created by Wavjaby on 2026/3/26.
//

#include "if.h"

#include <WJCL/string/wjcl_string.h>

#include "lib/code_gen.h"
#include "compiler_util.h"

inline bool code_if(Object* src) {
    compilerLog("> (if)\n");
    // TODO: 實作 if 分支 IR 生成
    //   1. 取得條件運算元字串
    //   2. 推入新 scope，初始化 elseifCount 與 containsElse
    //   3. 輸出條件跳轉 IR，依 scope->id 命名 true/false 兩個 label
    //   4. 輸出 true label（用 buffPrintlnS）
    //   5. 清理 Object，return false
    //   if/else block 結構見 LLVM_IR_CHEATSHEET.md §if / elseif / else 結構
    // get condition result
    char regResultName[MAX_NAME_LENGTH];
    Object boolResult = object_toBoolRegObject(src, regResultName, MAX_NAME_LENGTH);

    // create and init scope
    ScopeData* ifScope = scope_pushType(SCOPE_IF_STMT);
    ifScope->u.ifInfo.elseifCount = 0;
    ifScope->u.ifInfo.containsElse = false;

    // gen Branch
    buffPrintln(&ctx->code, "br i1 %s, label %%if%d.true, label %%if%d.false", 
        regResultName, ifScope->id, ifScope->id);

    // gen Jump true label
    buffPrintlnS(&ctx->code, "if%d.true:", ifScope->id);

    // output
    // object_free(src);  // ITS will cause problem
    object_free(&boolResult);
    return false;
}

inline bool code_elseIfLabel() {
    // TODO: 結束前一個 if/elseif 分支，準備進入下一個 elseif
    //   1. 輸出無條件跳轉到 endif label
    //   2. 輸出前一段的 false label（第一次是 if.false，之後是 elseif<n>.false）
    //   3. 更新 elseifCount
    //   label 命名規則見 LLVM_IR_CHEATSHEET.md §if / elseif / else 結構
    // get previous scope details
    ScopeData* previousScope = scope_peek();
    int scopeID = previousScope->id;
    int elseifCount = previousScope->u.ifInfo.elseifCount;

    // gen Jump to end
    buffPrintln(&ctx->code, "br label %%if%d.endif", scopeID);

    // gen Jump false label
    if (elseifCount == 0) {  // previousScope is IF
        buffPrintlnS(&ctx->code, "if%d.false:", scopeID);
    }
    else {  // previousScope is ELSE_IF
        buffPrintlnS(&ctx->code, "if%d.elseif%d.false:", scopeID, elseifCount);
    }

    return false;
}

inline bool code_elseIf(Object* src) {
    // TODO: 彈出當前 scope，推入同 id 的新 scope，繼續累積 elseifCount
    //   1. 從舊 scope 取出 scopeId 與 elseifCount，scope_dump()
    //   2. scope_pushId 推入同 id 的新 scope，elseifCount + 1
    //   3. 取得條件運算元字串，輸出條件跳轉 IR（true/false label 含 elseifCount）
    //   4. 輸出 elseif true label，清理 Object，return false
    // get previous scope details
    ScopeData* previousScope = scope_peek();
    int scopeID = previousScope->id;
    int elseifCount = previousScope->u.ifInfo.elseifCount;

    // pop previous scope and push new one
    scope_dump();
    compilerLog("> (else if)\n");
    ScopeData* elifScope = scope_pushId(SCOPE_IF_STMT, scopeID);
    elseifCount++;
    elifScope->u.ifInfo.elseifCount = elseifCount;

    // get condition result
    char regResultName[MAX_NAME_LENGTH];
    Object boolResult = object_toBoolRegObject(src, regResultName, MAX_NAME_LENGTH);

    // gen Branch
    buffPrintln(&ctx->code, "br i1 %s, label %%if%d.elseif%d.true, label %%if%d.elseif%d.false", 
        regResultName, scopeID, elseifCount, scopeID, elseifCount);

    // gen Jump true label
    buffPrintlnS(&ctx->code, "if%d.elseif%d.true:", scopeID, elseifCount);

    // output
    // object_free(src);  // ITS will cause problem
    object_free(&boolResult);
    return false;
}

inline bool code_else() {
    // TODO: 切換 scope 並輸出 else 的進入標籤
    //   1. 取出當前 scope 資訊後 scope_dump()
    //   2. 輸出無條件跳轉到 endif，再輸出前一段的 false label
    //   3. scope_pushId 推入同 id 的新 scope，設 containsElse=true
    //   label 命名規則同 code_elseIfLabel，見 LLVM_IR_CHEATSHEET.md
    // get previous scope details
    ScopeData* previousScope = scope_peek();
    int scopeID = previousScope->id;
    int elseifCount = previousScope->u.ifInfo.elseifCount;

    // gen Jump to end
    buffPrintln(&ctx->code, "br label %%if%d.endif", scopeID);

    // gen Jump false label
    if (elseifCount == 0) {  // previousScope is IF
        buffPrintlnS(&ctx->code, "if%d.false:", scopeID);
    }
    else {  // previousScope is ELSE_IF
        buffPrintlnS(&ctx->code, "if%d.elseif%d.false:", scopeID, elseifCount);
    }

    // pop previous scope and push new one
    scope_dump();
    compilerLog("> (else)\n");
    ScopeData* elseScope = scope_pushId(SCOPE_IF_STMT, scopeID);
    elseScope->u.ifInfo.elseifCount = elseifCount + 1;
    elseScope->u.ifInfo.containsElse = true;

    return false;
}

inline bool code_ifEnd() {
    // TODO: 根據 scope 狀態輸出 if 結尾 IR，彈出 scope
    //   三種情況各自需要不同的 label 組合（見 LLVM_IR_CHEATSHEET.md §if 結構圖）：
    //   - 有 else：本體結束後跳 endif，endif label 作為匯合點
    //   - 無 else 無 elseif：本體結束後直接落到 false label
    //   - 有 elseif 無 else：最後一個 elseif 的 false label + endif 匯合點
    //   最後 scope_dump()
    // get previous scope details
    ScopeData* previousScope = scope_peek();
    int scopeID = previousScope->id;
    int elseifCount = previousScope->u.ifInfo.elseifCount;

    // end if statement
    if (previousScope->u.ifInfo.containsElse == false) {
        if (elseifCount == 0) {  // previousScope is IF
            // gen Jump to end
            buffPrintln(&ctx->code, "br label %%if%d.false", scopeID);

            // gen Jump false label
            buffPrintlnS(&ctx->code, "if%d.false:", scopeID);
        }
        else {  // previousScope is ELSE_IF
            // gen Jump to end
            buffPrintln(&ctx->code, "br label %%if%d.elseif%d.false", scopeID, elseifCount);

            // gen Jump false label
            buffPrintlnS(&ctx->code, "if%d.elseif%d.false:", scopeID, elseifCount);
        }
    }
    else {  // previousScope is ELSE
        // gen Jump to end
        buffPrintln(&ctx->code, "br label %%if%d.endif", scopeID);
    
        // gen Jump end label
        buffPrintlnS(&ctx->code, "if%d.endif:", scopeID);
    }

    // pop previous scope
    scope_dump();
    compilerLog("< (if end)\n");

    return false;
}

Object object_toBoolRegObject(const Object* src, char* buffer, size_t bufferLen) {
    const ObjectType srcType = object_getValueType(src);
    Object reg = object_nameLiteralOrLoadReg(src, buffer, bufferLen);
    
    // is bool
    if (srcType == OBJECT_TYPE_BOOL) {
        return reg;
    }
    
    // is other type
    SymbolData boolReg = object_createRegisterSymbol(OBJECT_TYPE_BOOL);
    const char* srcTypeLLVM = objectType2llvmType[srcType];
    
    if (ObjectType_isFloat(srcType)) {  // Double/Float -> compare != 0.0
        buffPrintln(&ctx->code, "%%reg%s = %s %s %s, 0.0",
            boolReg.name, opIRFloatNames[OP_NE], srcTypeLLVM, buffer);
    }
    else if (ObjectType_isInteger(srcType)) {  // Integers -> compare != 0
        buffPrintln(&ctx->code, "%%reg%s = %s %s %s, 0",
            boolReg.name, opIRIntNames[OP_NE], srcTypeLLVM, buffer);
    }
    else if (srcType == OBJECT_TYPE_STR) {  // String -> compare.length != 0
        SymbolData lenReg = object_createRegisterSymbol(OBJECT_TYPE_I64);
        buffPrintln(&ctx->code, "%%reg%s = call %s @wy_rt_str_length(ptr %s)", 
            lenReg.name, objectType2llvmType[OBJECT_TYPE_I64], buffer);
        buffPrintln(&ctx->code, "%%reg%s = %s %s %s, 0",
            boolReg.name, opIRIntNames[OP_NE], objectType2llvmType[OBJECT_TYPE_I64], buffer);
    }
    else if (srcType == OBJECT_TYPE_ARRAY) {  // Array -> compare.length != 0
        SymbolData lenReg = object_createRegisterSymbol(OBJECT_TYPE_I64);
        buffPrintln(&ctx->code, "%%reg%s = call %s @wy_rt_array_get_length(ptr %s)", 
            lenReg.name, objectType2llvmType[OBJECT_TYPE_I64], buffer);
        buffPrintln(&ctx->code, "%%reg%s = %s %s %s, 0",
            boolReg.name, opIRIntNames[OP_NE], objectType2llvmType[OBJECT_TYPE_I64], buffer);
    }
    else if (srcType == OBJECT_TYPE_FUNC) {  // Func Pointers -> compare != null
        buffPrintln(&ctx->code, "%%reg%s = %s ptr %s, null",
            boolReg.name, opIRIntNames[OP_NE], buffer);
    }
    else {  // Undefined / Void -> always false (compare null != null)
        buffPrintln(&ctx->code, "%%reg%s = %s ptr null, null",
            boolReg.name, opIRIntNames[OP_NE]);
    }
    
    // output
    snprintf(buffer, bufferLen, "%%reg%s", boolReg.name);
    if (src->type == OBJECT_TYPE_SYMBOL) {
        object_free(&reg);
    }
    return (Object){
        .type = OBJECT_TYPE_REGISTER, 
        .capturedIndex = -1, 
        .value.symbol = cloneStruct(SymbolData, &boolReg)
    };
}
