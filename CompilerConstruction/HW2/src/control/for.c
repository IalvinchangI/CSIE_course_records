//
// Created by WavJaby on 2026/03/26.
//

#include "for.h"

#include <WJCL/string/wjcl_string.h>

#include "lib/code_gen.h"
#include "compiler_util.h"

bool code_forLoop(Object* src) {
    if (src->type == OBJECT_TYPE_UNDEFINED)
        goto FAILED;

    compilerLog("> (for loop, count: %s)\n", object_print(src));

    // TODO: 實作 for 迴圈 IR 生成
    //   1. 推入新 scope，取得計數運算元字串（需升級到 I32）
    //   2. 依 src->type 設定 scope->u.forLoop.symbol（記錄計數器型別）
    //   3. 輸出 entry → header → phi → icmp → 條件跳轉 → body label 的完整 IR 序列
    //   完整 block 結構與各 label 命名規則見 LLVM_IR_CHEATSHEET.md §phi 節點（for 迴圈）
    // create scope
    ScopeData* loopScope = scope_pushType(SCOPE_FOR_LOOP);
    loopScope->u.forLoop.symbol.type = src->type;
    int scopeID = loopScope->id;

    // gen IR
    // gen Jump to entry
    buffPrintln(&ctx->code, "br label %%loop%d.entry", scopeID);
    // entry block
    buffPrintlnS(&ctx->code, "loop%d.entry:", scopeID);
        // get times
        char regTimesName[MAX_NAME_LENGTH];
        Object regTimes = object_loadRegAndPromote(src, OBJECT_TYPE_I32, regTimesName, MAX_NAME_LENGTH);
        if (regTimes.type == OBJECT_TYPE_UNDEFINED) {
            goto FAILED;
        }
        // gen Jump to header
        buffPrintln(&ctx->code, "br label %%loop%d.header", scopeID);
    // header block
    buffPrintlnS(&ctx->code, "loop%d.header:", scopeID);
        // gen phi
        buffPrintln(&ctx->code, "%%loop%d.i = phi %s [ 0, %%loop%d.entry ], "
                                                    "[ %%loop%d.i.next, %%loop%d.update ]", 
            scopeID, objectType2llvmType[OBJECT_TYPE_I32], scopeID, scopeID, scopeID);
        // gen phi.cond
        buffPrintln(&ctx->code, "%%loop%d.cond = %s %s %%loop%d.i, %s", 
            scopeID, opIRIntNames[OP_LT], objectType2llvmType[OBJECT_TYPE_I32], scopeID, regTimesName);
        // gen Branch to body or exit
        buffPrintln(&ctx->code, "br i1 %%loop%d.cond, label %%loop%d.body, label %%loop%d.exit", 
            scopeID, scopeID, scopeID);
    // body block
    buffPrintlnS(&ctx->code, "loop%d.body:", scopeID);
        // body statment
    
    // clean and return
    if (object_getValueType(src) != OBJECT_TYPE_I32) {
        object_free(&regTimes);
    }
    object_free(src);
    return false;

FAILED:
    if (object_getValueType(src) != OBJECT_TYPE_I32) {
        object_free(&regTimes);
    }
    object_free(src);
    return true;
}

bool code_forLoopEnd(Object* obj) {
    // TODO: 輸出迴圈 update/exit IR，彈出 scope
    //   輸出 update label → 計數器遞增 → 跳回 header → exit label
    //   IR 指令與 label 命名規則見 LLVM_IR_CHEATSHEET.md §phi 節點（for 迴圈）
    // get loop scope details
    ScopeData* loopScope = scope_peek();
    int scopeID = loopScope->id;

    // gen IR
    // body block
        // body statment
        buffPrintln(&ctx->code, "br label %%loop%d.update", scopeID);
    // update block
    buffPrintlnS(&ctx->code, "loop%d.update:", scopeID);
        // gen phi.next
        buffPrintln(&ctx->code, "%%loop%d.i.next = add nsw %s %%loop%d.i, 1", 
            scopeID, objectType2llvmType[OBJECT_TYPE_I32], scopeID);
        // gen Jump to header
        buffPrintln(&ctx->code, "br label %%loop%d.header", scopeID);
    // exit
    buffPrintlnS(&ctx->code, "loop%d.exit:", scopeID);

    // pop loop scope
    scope_dump();
    compilerLog("< (for loop end)\n");

    return false;
}
